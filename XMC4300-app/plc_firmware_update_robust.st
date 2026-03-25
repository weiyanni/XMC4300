// -----------------------------------------------------------------------------
// PLC Firmware Update over EtherCAT CoE SDO (0x9000)
// 优化点：
// 1) 先扫描文件得到大小，再二次打开边读边发（避免 1MB 大数组）
// 2) RxID/RxLen 用 WORD 发送 2 字节（避免高字节不确定导致 Boot 端校验失败）
// 3) 关键步骤增加重试（SDO 写/读错误 & ACK 超时）
// 4) 上传过程中增量计算 CRC32（IEEE/ZIP，reflected poly 0xEDB88320）
// -----------------------------------------------------------------------------

FUNCTION F_CRC32_Append : UDINT
VAR_INPUT
    nCrcIn  : UDINT;            // 初始值通常为 16#FFFFFFFF
    pData   : POINTER TO BYTE;  // 数据指针
    cbData  : UDINT;            // 数据长度（字节）
END_VAR
VAR
    nCrc    : UDINT;
    i       : UDINT;
    j       : INT;
    b       : BYTE;
    pCur    : POINTER TO BYTE;
END_VAR

nCrc := nCrcIn;
pCur := pData;

FOR i := 1 TO cbData DO
    b := pCur^;
    nCrc := nCrc XOR BYTE_TO_UDINT(b);
    FOR j := 0 TO 7 DO
        IF (nCrc AND 1) <> 0 THEN
            nCrc := SHR(nCrc, 1) XOR 16#EDB88320;
        ELSE
            nCrc := SHR(nCrc, 1);
        END_IF
    END_FOR
    pCur := pCur + 1;
END_FOR

F_CRC32_Append := nCrc;

END_FUNCTION


PROGRAM MAIN
VAR
    (* ================= 控制变量 ================= *)
    bStartUpdate    : BOOL;
    bReset          : BOOL;
    sFileName       : T_MaxString := 'D:\work\Firmware\firmware_final1.bin';
    sMasterNetId    : T_AmsNetId := '192.168.110.52.2.1';
    nSlaveAddr      : UINT := 1001;

    (* ================= 状态指示 ================= *)
    sStatus         : STRING := 'Ready';
    sFullErrorMsg   : STRING(255);
    nProgress       : UINT;
    bBusy           : BOOL;
    bError          : BOOL;
    nErrId          : UDINT;
    nErrStep        : INT := 0;

    (* ================= 内部变量 ================= *)
    nState          : INT := 0;

    // ---- 文件操作（扫描大小 -> 上传流式读取） ----
    fbFileOpen      : FB_FileOpen;
    fbFileRead      : FB_FileRead;
    fbFileClose     : FB_FileClose;
    hFile           : UINT := 0;
    aFileBuf        : ARRAY[0..4095] OF BYTE;
    nFileSize       : UDINT := 0;
    nBufValid       : UDINT := 0;
    nBufPos         : UDINT := 0;

    // ---- SDO ----
    fbSdoWrite      : FB_EcCoESdoWrite;
    fbSdoRead       : FB_EcCoESdoRead;
    nObjIndex       : WORD := 16#9000;
    wRxId           : WORD := 254;
    wRxLen          : WORD := 0;
    nRxIndex        : UINT := 0;
    nTxIndexRead    : UINT := 0;

    // Tx / Rx 分离
    aSdoTx          : ARRAY[0..63] OF BYTE;
    aSdoRx          : ARRAY[0..63] OF BYTE;
    nTxLen          : UDINT := 0;

    // ---- 传输 ----
    nOffset         : UDINT := 0;
    nChunkSize      : UDINT := 0;

    // ---- 协议常量 ----
    MAX_PAYLOAD     : UDINT := 59;
    CMD_START       : BYTE := 16#F1;
    CMD_DATA        : BYTE := 16#F2;
    CMD_END         : BYTE := 16#F3;

    // ---- CRC ----
    nCrcWork        : UDINT := 16#FFFFFFFF;
    nCrc32          : UDINT := 0;

    // ---- 触发/超时/重试 ----
    rtStart         : R_TRIG;
    fbTimer         : TON;
    nRetryCount     : DINT := 0;
    MAX_RETRY       : DINT := 3;

    // ---- 从站状态 ----
    nSlaveState AT %I* : UINT;
END_VAR

rtStart(CLK := bStartUpdate);

IF bReset THEN
    nState := 0;
    bBusy := FALSE;
    bError := FALSE;
    nErrId := 0;
    nErrStep := 0;
    sStatus := 'Reset Done';
    sFullErrorMsg := '';
    nProgress := 0;
    nRetryCount := 0;
    nFileSize := 0;
    nOffset := 0;
    nRxIndex := 0;
    nBufValid := 0;
    nBufPos := 0;
    nCrcWork := 16#FFFFFFFF;
    nCrc32 := 0;

    fbTimer(IN := FALSE);
    fbSdoWrite(bExecute := FALSE);
    fbSdoRead(bExecute := FALSE);
    fbFileOpen(bExecute := FALSE);
    fbFileRead(bExecute := FALSE);
    fbFileClose(bExecute := FALSE);

    IF hFile <> 0 THEN
        fbFileClose(bExecute := TRUE, sNetId := '', hFile := hFile);
        hFile := 0;
        fbFileClose(bExecute := FALSE);
    END_IF

    bReset := FALSE;
END_IF

CASE nState OF
    0: (* 等待开始 *)
        IF rtStart.Q THEN
            bBusy := TRUE;
            bError := FALSE;
            nErrId := 0;
            nErrStep := 0;
            sFullErrorMsg := '';
            bStartUpdate := FALSE;

            nProgress := 0;
            nOffset := 0;
            nRxIndex := 0;
            nTxIndexRead := 0;
            nRetryCount := 0;

            nFileSize := 0;
            nBufValid := 0;
            nBufPos := 0;
            nCrcWork := 16#FFFFFFFF;
            nCrc32 := 0;

            nState := 5;
            sStatus := 'Waiting for Slave (Pre-Op)...';
        END_IF

    5: (* 等待从站在线 *)
        fbTimer(IN := TRUE, PT := T#10S);
        IF (nSlaveState AND 16#000F) >= 2 THEN
            fbTimer(IN := FALSE);
            nState := 10;
            sStatus := 'Opening File (Scan Size)...';
        ELSIF fbTimer.Q THEN
            nErrStep := nState;
            nErrId := 16#FF01;
            sStatus := 'Error: Slave Offline (Timeout)';
            fbTimer(IN := FALSE);
            nState := 999;
        END_IF

    // ================= 文件扫描：得到文件大小 =================
    10: (* 打开文件用于扫描 *)
        fbFileOpen(bExecute := TRUE, sNetId := '', sPathName := sFileName, nMode := FOPEN_MODEREAD OR FOPEN_MODEBINARY);
        IF NOT fbFileOpen.bBusy THEN
            IF fbFileOpen.bError THEN
                nErrStep := nState;
                nErrId := fbFileOpen.nErrId;
                sStatus := 'File Open Error (Scan)';
                nState := 999;
            ELSE
                hFile := fbFileOpen.hFile;
                nFileSize := 0;
                fbFileOpen(bExecute := FALSE);
                nState := 20;
                sStatus := 'Scanning File Size...';
            END_IF
        END_IF

    20: (* 读取直到 EOF，用于累计大小 *)
        fbFileRead(bExecute := TRUE, sNetId := '', hFile := hFile, pReadBuff := ADR(aFileBuf), cbReadLen := SIZEOF(aFileBuf));
        IF NOT fbFileRead.bBusy THEN
            IF fbFileRead.bError THEN
                nErrStep := nState;
                nErrId := fbFileRead.nErrId;
                sStatus := 'File Read Error (Scan)';
                nState := 999;
            ELSE
                IF fbFileRead.cbRead = 0 THEN
                    fbFileRead(bExecute := FALSE);
                    nState := 30;
                    sStatus := 'Closing File (Scan)...';
                ELSE
                    nFileSize := nFileSize + fbFileRead.cbRead;
                    fbFileRead(bExecute := FALSE);

                    IF nFileSize > 16#00FFFFFF THEN
                        nErrStep := nState;
                        nErrId := 16#FF10;
                        sStatus := 'Error: File Too Large (>16MB for 3-byte size)';
                        nState := 999;
                    END_IF
                END_IF
            END_IF
        END_IF

    30: (* 关闭扫描用句柄 *)
        fbFileClose(bExecute := TRUE, sNetId := '', hFile := hFile);
        IF NOT fbFileClose.bBusy THEN
            hFile := 0;
            fbFileClose(bExecute := FALSE);

            IF nFileSize = 0 THEN
                nErrStep := nState;
                nErrId := 16#FF11;
                sStatus := 'Error: File is Empty';
                nState := 999;
            ELSE
                nState := 50;
                sStatus := 'File Ready. Handshaking (App/Boot)...';
            END_IF
        END_IF

    // ================= 第一次握手（可能在 App 或 Boot） =================
    50: (* RxID = 254 *)
        wRxId := 254;
        fbSdoWrite(sNetId := sMasterNetId, nSlaveAddr := nSlaveAddr, nIndex := nObjIndex, nSubIndex := 6, pSrcBuf := ADR(wRxId), cbBufLen := SIZEOF(wRxId), bExecute := TRUE);
        IF NOT fbSdoWrite.bBusy THEN
            IF fbSdoWrite.bError THEN
                fbSdoWrite(bExecute := FALSE);
                IF nRetryCount < MAX_RETRY THEN
                    nRetryCount := nRetryCount + 1;
                    sStatus := 'Set RxID Fail - Retry...';
                ELSE
                    nErrStep := nState;
                    nErrId := fbSdoWrite.nErrId;
                    sStatus := 'Set RxID Fail';
                    nState := 999;
                END_IF
            ELSE
                nRetryCount := 0;
                fbSdoWrite(bExecute := FALSE);
                nState := 51;
            END_IF
        END_IF

    51: (* RxLen = 7（App 端 START 长度） *)
        wRxLen := 7;
        fbSdoWrite(sNetId := sMasterNetId, nSlaveAddr := nSlaveAddr, nIndex := nObjIndex, nSubIndex := 7, pSrcBuf := ADR(wRxLen), cbBufLen := SIZEOF(wRxLen), bExecute := TRUE);
        IF NOT fbSdoWrite.bBusy THEN
            IF fbSdoWrite.bError THEN
                fbSdoWrite(bExecute := FALSE);
                IF nRetryCount < MAX_RETRY THEN
                    nRetryCount := nRetryCount + 1;
                    sStatus := 'Set RxLen Fail - Retry...';
                ELSE
                    nErrStep := nState;
                    nErrId := fbSdoWrite.nErrId;
                    sStatus := 'Set RxLen Fail';
                    nState := 999;
                END_IF
            ELSE
                nRetryCount := 0;
                fbSdoWrite(bExecute := FALSE);
                nState := 52;
            END_IF
        END_IF

    52: (* RxIndex = 1 *)
        nRxIndex := 1;
        fbSdoWrite(sNetId := sMasterNetId, nSlaveAddr := nSlaveAddr, nIndex := nObjIndex, nSubIndex := 5, pSrcBuf := ADR(nRxIndex), cbBufLen := SIZEOF(nRxIndex), bExecute := TRUE);
        IF NOT fbSdoWrite.bBusy THEN
            IF fbSdoWrite.bError THEN
                fbSdoWrite(bExecute := FALSE);
                IF nRetryCount < MAX_RETRY THEN
                    nRetryCount := nRetryCount + 1;
                    sStatus := 'Set RxIndex Fail - Retry...';
                ELSE
                    nErrStep := nState;
                    nErrId := fbSdoWrite.nErrId;
                    sStatus := 'Set RxIndex Fail';
                    nState := 999;
                END_IF
            ELSE
                nRetryCount := 0;
                fbSdoWrite(bExecute := FALSE);
                nState := 53;
            END_IF
        END_IF

    53: (* 发送 START *)
        sStatus := 'Sending START...';
        aSdoTx[0] := CMD_START;
        aSdoTx[1] := UDINT_TO_BYTE(nFileSize AND 16#FF);
        aSdoTx[2] := UDINT_TO_BYTE(SHR(nFileSize, 8) AND 16#FF);
        aSdoTx[3] := UDINT_TO_BYTE(SHR(nFileSize, 16) AND 16#FF);
        aSdoTx[4] := 16#34;
        aSdoTx[5] := 16#12;
        aSdoTx[6] := 64;
        nTxLen := 7;

        fbSdoWrite(sNetId := sMasterNetId, nSlaveAddr := nSlaveAddr, nIndex := nObjIndex, nSubIndex := 8, pSrcBuf := ADR(aSdoTx), cbBufLen := nTxLen, bExecute := TRUE);
        IF NOT fbSdoWrite.bBusy THEN
            IF fbSdoWrite.bError THEN
                fbSdoWrite(bExecute := FALSE);
                IF nRetryCount < MAX_RETRY THEN
                    nRetryCount := nRetryCount + 1;
                    sStatus := 'Send START Fail - Retry...';
                ELSE
                    nErrStep := nState;
                    nErrId := fbSdoWrite.nErrId;
                    sStatus := 'Send START Fail';
                    nState := 999;
                END_IF
            ELSE
                nRetryCount := 0;
                fbSdoWrite(bExecute := FALSE);
                fbTimer(IN := FALSE);
                nState := 54;
            END_IF
        END_IF

    54: (* 读取状态 Sub4（超时 10s） *)
        sStatus := 'Checking Status (App/Boot)...';
        fbTimer(IN := TRUE, PT := T#10S);

        IF fbTimer.Q THEN
            fbTimer(IN := FALSE);
            fbSdoRead(bExecute := FALSE);
            IF nRetryCount < MAX_RETRY THEN
                nRetryCount := nRetryCount + 1;
                sStatus := 'Status Timeout - Retry START...';
                nState := 53;
            ELSE
                nErrStep := nState;
                nErrId := 16#FF20;
                sStatus := 'Handshake Timeout';
                nState := 999;
            END_IF
        ELSE
            fbSdoRead(sNetId := sMasterNetId, nSlaveAddr := nSlaveAddr, nIndex := nObjIndex, nSubIndex := 4, pDstBuf := ADR(aSdoRx), cbBufLen := SIZEOF(aSdoRx), bExecute := TRUE);
            IF NOT fbSdoRead.bBusy THEN
                IF fbSdoRead.bError THEN
                    fbSdoRead(bExecute := FALSE);
                    IF nRetryCount < MAX_RETRY THEN
                        nRetryCount := nRetryCount + 1;
                        sStatus := 'Read Status Error - Retry...';
                    ELSE
                        nErrStep := nState;
                        nErrId := fbSdoRead.nErrId;
                        sStatus := 'Read Status Error';
                        nState := 999;
                    END_IF
                ELSE
                    IF aSdoRx[0] = CMD_START THEN
                        fbTimer(IN := FALSE);
                        nRetryCount := 0;
                        CASE aSdoRx[1] OF
                            0:
                                nErrStep := nState; nErrId := 16#FF21; sStatus := 'Error: Password Wrong'; nState := 999;
                            1:
                                sStatus := 'In App. Waiting Reboot...'; nState := 60;
                            2, 5:
                                sStatus := 'In Boot. Starting Boot Handshake...'; nState := 100;
                            4:
                                sStatus := 'In Boot (Erasing). Wait...'; nState := 113;
                            3:
                                nErrStep := nState; nErrId := 16#FF23; sStatus := 'Error: Memory Full'; nState := 999;
                            ELSE
                                nErrStep := nState; nErrId := 16#FF24; sStatus := 'Unknown Status Code'; nState := 999;
                        END_CASE
                    END_IF
                    fbSdoRead(bExecute := FALSE);
                END_IF
            END_IF
        END_IF

    60: (* 等待 App 重启 *)
        fbTimer(IN := TRUE, PT := T#10S);
        IF fbTimer.Q THEN
            fbTimer(IN := FALSE);
            sStatus := 'Connecting to Bootloader...';
            nState := 100;
        END_IF

    // ================= Boot 握手：START + 等待 Ready/Done =================
    100:
        nOffset := 0;
        nProgress := 0;
        nBufValid := 0;
        nBufPos := 0;
        nCrcWork := 16#FFFFFFFF;
        nCrc32 := 0;
        nRxIndex := 0;
        nRetryCount := 0;
        fbTimer(IN := FALSE);
        nState := 101;

    101: (* RxLen = 0 *)
        wRxLen := 0;
        fbSdoWrite(sNetId := sMasterNetId, nSlaveAddr := nSlaveAddr, nIndex := nObjIndex, nSubIndex := 7, pSrcBuf := ADR(wRxLen), cbBufLen := SIZEOF(wRxLen), bExecute := TRUE);
        IF NOT fbSdoWrite.bBusy THEN
            IF fbSdoWrite.bError THEN
                fbSdoWrite(bExecute := FALSE);
                IF nRetryCount < MAX_RETRY THEN nRetryCount := nRetryCount + 1; sStatus := 'Boot: Set RxLen Fail - Retry...';
                ELSE nErrStep := nState; nErrId := fbSdoWrite.nErrId; sStatus := 'Boot: Set RxLen Fail'; nState := 999; END_IF
            ELSE
                nRetryCount := 0;
                fbSdoWrite(bExecute := FALSE);
                nState := 111;
            END_IF
        END_IF

    111: (* RxIndex = 0 *)
        nRxIndex := 0;
        fbSdoWrite(sNetId := sMasterNetId, nSlaveAddr := nSlaveAddr, nIndex := nObjIndex, nSubIndex := 5, pSrcBuf := ADR(nRxIndex), cbBufLen := SIZEOF(nRxIndex), bExecute := TRUE);
        IF NOT fbSdoWrite.bBusy THEN
            IF fbSdoWrite.bError THEN
                fbSdoWrite(bExecute := FALSE);
                IF nRetryCount < MAX_RETRY THEN nRetryCount := nRetryCount + 1; sStatus := 'Boot: Write RxIndex Fail - Retry...';
                ELSE nErrStep := nState; nErrId := fbSdoWrite.nErrId; sStatus := 'Boot: Write RxIndex Fail'; nState := 999; END_IF
            ELSE
                nRetryCount := 0;
                fbSdoWrite(bExecute := FALSE);
                nState := 112;
            END_IF
        END_IF

    112: (* 发送 START 到 Boot *)
        sStatus := 'Sending START to Boot...';
        aSdoTx[0] := CMD_START;
        aSdoTx[1] := UDINT_TO_BYTE(nFileSize AND 16#FF);
        aSdoTx[2] := UDINT_TO_BYTE(SHR(nFileSize, 8) AND 16#FF);
        aSdoTx[3] := UDINT_TO_BYTE(SHR(nFileSize, 16) AND 16#FF);
        aSdoTx[4] := 16#34; aSdoTx[5] := 16#12; aSdoTx[6] := 64;
        nTxLen := 7;
        fbSdoWrite(sNetId := sMasterNetId, nSlaveAddr := nSlaveAddr, nIndex := nObjIndex, nSubIndex := 8, pSrcBuf := ADR(aSdoTx), cbBufLen := nTxLen, bExecute := TRUE);
        IF NOT fbSdoWrite.bBusy THEN
            IF fbSdoWrite.bError THEN
                fbSdoWrite(bExecute := FALSE);
                IF nRetryCount < MAX_RETRY THEN nRetryCount := nRetryCount + 1; sStatus := 'Boot: Send START Fail - Retry...';
                ELSE nErrStep := nState; nErrId := fbSdoWrite.nErrId; sStatus := 'Boot: Send START Fail'; nState := 999; END_IF
            ELSE
                nRetryCount := 0;
                fbSdoWrite(bExecute := FALSE);
                fbTimer(IN := FALSE);
                nState := 113;
            END_IF
        END_IF

    113: (* 轮询 Boot 状态（超时 60s） *)
        sStatus := 'Waiting Boot Response...';
        fbTimer(IN := TRUE, PT := T#60S);

        IF fbTimer.Q THEN
            fbTimer(IN := FALSE);
            fbSdoRead(bExecute := FALSE);
            IF nRetryCount < MAX_RETRY THEN nRetryCount := nRetryCount + 1; sStatus := 'Boot Status Timeout - Retry START...'; nState := 112;
            ELSE nErrStep := nState; nErrId := 16#FF30; sStatus := 'Boot Status Timeout'; nState := 999; END_IF
        ELSE
            fbSdoRead(sNetId := sMasterNetId, nSlaveAddr := nSlaveAddr, nIndex := nObjIndex, nSubIndex := 4, pDstBuf := ADR(aSdoRx), cbBufLen := SIZEOF(aSdoRx), bExecute := TRUE);
            IF NOT fbSdoRead.bBusy THEN
                IF fbSdoRead.bError THEN
                    fbSdoRead(bExecute := FALSE);
                    IF nRetryCount < MAX_RETRY THEN nRetryCount := nRetryCount + 1; sStatus := 'Boot Poll Read Error - Retry...';
                    ELSE nErrStep := nState; nErrId := fbSdoRead.nErrId; sStatus := 'Boot Poll Read Error'; nState := 999; END_IF
                ELSE
                    IF aSdoRx[0] = CMD_START THEN
                        CASE aSdoRx[1] OF
                            0: nErrStep := nState; nErrId := 16#FF31; sStatus := 'Error: Password Wrong'; fbTimer(IN := FALSE); nState := 999;
                            1: sStatus := 'In App. Jumping...'; fbTimer(IN := FALSE); nState := 60;
                            2, 5:
                                sStatus := 'Boot Ready. Opening File...'; fbTimer(IN := FALSE);
                                nRxIndex := 1; nRetryCount := 0; nState := 115;
                            3: nErrStep := nState; nErrId := 16#FF33; sStatus := 'Error: Memory Full'; fbTimer(IN := FALSE); nState := 999;
                            4: sStatus := 'Erasing Flash...'; (* stay *)
                            ELSE nErrStep := nState; nErrId := 16#FF34; sStatus := 'Unknown Boot Status'; fbTimer(IN := FALSE); nState := 999;
                        END_CASE
                    END_IF
                    fbSdoRead(bExecute := FALSE);
                END_IF
            END_IF
        END_IF

    // ================= 上传：二次打开文件，边读边发 =================
    115: (* 打开文件用于上传 *)
        fbFileOpen(bExecute := TRUE, sNetId := '', sPathName := sFileName, nMode := FOPEN_MODEREAD OR FOPEN_MODEBINARY);
        IF NOT fbFileOpen.bBusy THEN
            IF fbFileOpen.bError THEN
                nErrStep := nState;
                nErrId := fbFileOpen.nErrId;
                sStatus := 'File Open Error (Upload)';
                nState := 999;
            ELSE
                hFile := fbFileOpen.hFile;
                nBufValid := 0;
                nBufPos := 0;
                nOffset := 0;
                nCrcWork := 16#FFFFFFFF;
                fbFileOpen(bExecute := FALSE);
                nState := 118;
            END_IF
        END_IF

    118: (* 填充文件缓冲区 *)
        sStatus := 'Reading File Buffer...';
        fbFileRead(bExecute := TRUE, sNetId := '', hFile := hFile, pReadBuff := ADR(aFileBuf), cbReadLen := SIZEOF(aFileBuf));
        IF NOT fbFileRead.bBusy THEN
            IF fbFileRead.bError THEN
                nErrStep := nState;
                nErrId := fbFileRead.nErrId;
                sStatus := 'File Read Error (Upload)';
                nState := 999;
            ELSE
                nBufValid := fbFileRead.cbRead;
                nBufPos := 0;
                fbFileRead(bExecute := FALSE);
                IF nBufValid = 0 THEN
                    nErrStep := nState; nErrId := 16#FF40; sStatus := 'Error: Unexpected EOF'; nState := 999;
                ELSE
                    nState := 120;
                END_IF
            END_IF
        END_IF

    120: (* 组包 *)
        IF nOffset >= nFileSize THEN
            nProgress := 100;
            nState := 129;
            sStatus := 'Upload Done. Closing File...';
        ELSE
            IF nBufPos >= nBufValid THEN
                nState := 118;
            ELSE
                nChunkSize := nFileSize - nOffset;
                IF nChunkSize > MAX_PAYLOAD THEN nChunkSize := MAX_PAYLOAD; END_IF
                IF nChunkSize > (nBufValid - nBufPos) THEN nChunkSize := nBufValid - nBufPos; END_IF

                aSdoTx[0] := CMD_DATA;
                aSdoTx[1] := UDINT_TO_BYTE(nOffset AND 16#FF);
                aSdoTx[2] := UDINT_TO_BYTE(SHR(nOffset, 8) AND 16#FF);
                aSdoTx[3] := UDINT_TO_BYTE(SHR(nOffset, 16) AND 16#FF);
                aSdoTx[4] := UDINT_TO_BYTE(SHR(nOffset, 24) AND 16#FF);
                MEMCPY(ADR(aSdoTx[5]), ADR(aFileBuf) + nBufPos, UDINT_TO_UINT(nChunkSize));

                nTxLen := 5 + nChunkSize;
                nState := 121;
                sStatus := 'Uploading...';
            END_IF
        END_IF

    121: (* 写 RxIndex *)
        fbSdoWrite(sNetId := sMasterNetId, nSlaveAddr := nSlaveAddr, nIndex := nObjIndex, nSubIndex := 5, pSrcBuf := ADR(nRxIndex), cbBufLen := SIZEOF(nRxIndex), bExecute := TRUE);
        IF NOT fbSdoWrite.bBusy THEN
            IF fbSdoWrite.bError THEN
                fbSdoWrite(bExecute := FALSE);
                IF nRetryCount < MAX_RETRY THEN nRetryCount := nRetryCount + 1; sStatus := 'Data: Write RxIndex Fail - Retry...';
                ELSE nErrStep := nState; nErrId := fbSdoWrite.nErrId; sStatus := 'Data: Write RxIndex Fail'; nState := 999; END_IF
            ELSE
                nRetryCount := 0;
                fbSdoWrite(bExecute := FALSE);
                nState := 122;
            END_IF
        END_IF

    122: (* 写 RxData *)
        fbSdoWrite(sNetId := sMasterNetId, nSlaveAddr := nSlaveAddr, nIndex := nObjIndex, nSubIndex := 8, pSrcBuf := ADR(aSdoTx), cbBufLen := nTxLen, bExecute := TRUE);
        IF NOT fbSdoWrite.bBusy THEN
            IF fbSdoWrite.bError THEN
                fbSdoWrite(bExecute := FALSE);
                IF nRetryCount < MAX_RETRY THEN nRetryCount := nRetryCount + 1; sStatus := 'Data: Write RxData Fail - Retry...'; nState := 121;
                ELSE nErrStep := nState; nErrId := fbSdoWrite.nErrId; sStatus := 'Data: Write RxData Fail'; nState := 999; END_IF
            ELSE
                nRetryCount := 0;
                fbSdoWrite(bExecute := FALSE);
                fbTimer(IN := FALSE);
                nState := 123;
            END_IF
        END_IF

    123: (* 等待 ACK：读 TxIndex（超时 5s） *)
        fbTimer(IN := TRUE, PT := T#5S);
        IF fbTimer.Q THEN
            fbTimer(IN := FALSE);
            fbSdoRead(bExecute := FALSE);
            IF nRetryCount < MAX_RETRY THEN nRetryCount := nRetryCount + 1; sStatus := 'Data ACK Timeout - Resend...'; nState := 121;
            ELSE nErrStep := nState; nErrId := 16#FF50; sStatus := 'Data ACK Timeout'; nState := 999; END_IF
        ELSE
            fbSdoRead(sNetId := sMasterNetId, nSlaveAddr := nSlaveAddr, nIndex := nObjIndex, nSubIndex := 1, pDstBuf := ADR(nTxIndexRead), cbBufLen := SIZEOF(nTxIndexRead), bExecute := TRUE);
            IF NOT fbSdoRead.bBusy THEN
                IF fbSdoRead.bError THEN
                    fbSdoRead(bExecute := FALSE);
                    IF nRetryCount < MAX_RETRY THEN nRetryCount := nRetryCount + 1; sStatus := 'Data ACK Read Error - Retry...';
                    ELSE nErrStep := nState; nErrId := fbSdoRead.nErrId; sStatus := 'Data ACK Read Error'; nState := 999; END_IF
                ELSE
                    IF nTxIndexRead = nRxIndex THEN
                        nCrcWork := F_CRC32_Append(nCrcWork, ADR(aSdoTx[5]), nChunkSize);
                        nOffset := nOffset + nChunkSize;
                        nBufPos := nBufPos + nChunkSize;
                        nRxIndex := nRxIndex + 1;
                        nProgress := UDINT_TO_UINT((nOffset * 100) / nFileSize);
                        fbTimer(IN := FALSE);
                        nRetryCount := 0;
                        nState := 120;
                    END_IF
                    fbSdoRead(bExecute := FALSE);
                END_IF
            END_IF
        END_IF

    129: (* 关闭上传文件句柄 *)
        fbFileClose(bExecute := TRUE, sNetId := '', hFile := hFile);
        IF NOT fbFileClose.bBusy THEN
            hFile := 0;
            fbFileClose(bExecute := FALSE);
            nState := 130;
        END_IF

    130: (* 准备 END (F3 + CRC32) *)
        sStatus := 'Verifying CRC...';
        nCrc32 := nCrcWork XOR 16#FFFFFFFF;
        aSdoTx[0] := CMD_END;
        aSdoTx[1] := UDINT_TO_BYTE(nCrc32 AND 16#FF);
        aSdoTx[2] := UDINT_TO_BYTE(SHR(nCrc32, 8) AND 16#FF);
        aSdoTx[3] := UDINT_TO_BYTE(SHR(nCrc32, 16) AND 16#FF);
        aSdoTx[4] := UDINT_TO_BYTE(SHR(nCrc32, 24) AND 16#FF);
        nTxLen := 5;
        nState := 131;

    131: (* 写 RxIndex *)
        fbSdoWrite(sNetId := sMasterNetId, nSlaveAddr := nSlaveAddr, nIndex := nObjIndex, nSubIndex := 5, pSrcBuf := ADR(nRxIndex), cbBufLen := SIZEOF(nRxIndex), bExecute := TRUE);
        IF NOT fbSdoWrite.bBusy THEN
            IF fbSdoWrite.bError THEN
                fbSdoWrite(bExecute := FALSE);
                IF nRetryCount < MAX_RETRY THEN nRetryCount := nRetryCount + 1; sStatus := 'End: Write RxIndex Fail - Retry...';
                ELSE nErrStep := nState; nErrId := fbSdoWrite.nErrId; sStatus := 'End: Write RxIndex Fail'; nState := 999; END_IF
            ELSE
                nRetryCount := 0;
                fbSdoWrite(bExecute := FALSE);
                nState := 132;
            END_IF
        END_IF

    132: (* 写 END 包 *)
        fbSdoWrite(sNetId := sMasterNetId, nSlaveAddr := nSlaveAddr, nIndex := nObjIndex, nSubIndex := 8, pSrcBuf := ADR(aSdoTx), cbBufLen := nTxLen, bExecute := TRUE);
        IF NOT fbSdoWrite.bBusy THEN
            IF fbSdoWrite.bError THEN
                fbSdoWrite(bExecute := FALSE);
                IF nRetryCount < MAX_RETRY THEN nRetryCount := nRetryCount + 1; sStatus := 'End: Write RxData Fail - Retry...';
                ELSE nErrStep := nState; nErrId := fbSdoWrite.nErrId; sStatus := 'End: Write RxData Fail'; nState := 999; END_IF
            ELSE
                nRetryCount := 0;
                fbSdoWrite(bExecute := FALSE);
                fbTimer(IN := FALSE);
                nState := 133;
            END_IF
        END_IF

    133: (* 等待最终结果（超时 30s） *)
        sStatus := 'Waiting Final Result...';
        fbTimer(IN := TRUE, PT := T#30S);
        IF fbTimer.Q THEN
            fbTimer(IN := FALSE);
            fbSdoRead(bExecute := FALSE);
            IF nRetryCount < MAX_RETRY THEN nRetryCount := nRetryCount + 1; sStatus := 'Final Timeout - Retry Read...';
            ELSE nErrStep := nState; nErrId := 16#FF60; sStatus := 'Final Result Timeout'; nState := 999; END_IF
        ELSE
            fbSdoRead(sNetId := sMasterNetId, nSlaveAddr := nSlaveAddr, nIndex := nObjIndex, nSubIndex := 4, pDstBuf := ADR(aSdoRx), cbBufLen := SIZEOF(aSdoRx), bExecute := TRUE);
            IF NOT fbSdoRead.bBusy THEN
                IF fbSdoRead.bError THEN
                    fbSdoRead(bExecute := FALSE);
                    IF nRetryCount < MAX_RETRY THEN nRetryCount := nRetryCount + 1; sStatus := 'Final Read Error - Retry...';
                    ELSE nErrStep := nState; nErrId := fbSdoRead.nErrId; sStatus := 'Final Read Error'; nState := 999; END_IF
                ELSE
                    IF aSdoRx[0] = CMD_END THEN
                        fbTimer(IN := FALSE);
                        IF aSdoRx[1] = 0 THEN
                            sStatus := 'Update Success!';
                            nState := 200;
                        ELSE
                            nErrStep := nState;
                            nErrId := 16#E000 + aSdoRx[1];
                            sStatus := 'CRC/Flash Error';
                            nState := 999;
                        END_IF
                    END_IF
                    fbSdoRead(bExecute := FALSE);
                END_IF
            END_IF
        END_IF

    200: (* 完成 *)
        bBusy := FALSE;
        nState := 0;

    999: (* 错误处理 *)
        bError := TRUE;
        bBusy := FALSE;

        IF sFullErrorMsg = '' THEN
            sFullErrorMsg := CONCAT('Step: ', INT_TO_STRING(nErrStep));
            sFullErrorMsg := CONCAT(sFullErrorMsg, ', ErrId: 16#');
            sFullErrorMsg := CONCAT(sFullErrorMsg, DWORD_TO_HEXSTR(UDINT_TO_DWORD(nErrId), 8, FALSE));
            sFullErrorMsg := CONCAT(sFullErrorMsg, ' | ');
            sFullErrorMsg := CONCAT(sFullErrorMsg, sStatus);
        END_IF

        IF hFile <> 0 THEN
            fbFileClose(bExecute := TRUE, sNetId := '', hFile := hFile);
            hFile := 0;
            fbFileClose(bExecute := FALSE);
        END_IF

        nState := 0;
END_CASE
