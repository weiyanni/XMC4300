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
    nProgress       : UINT;             
    bBusy           : BOOL;
    bError          : BOOL;
    nErrId          : UDINT;

    (* ================= 内部变量 ================= *)
    nState          : INT := 0;         
    fbFileOpen      : FB_FileOpen;
    fbFileRead      : FB_FileRead;
    fbFileClose     : FB_FileClose;
    hFile           : UINT := 0;        
    aFileBuffer     : ARRAY[0..1000000] OF BYTE; 
    nFileSize       : UDINT;            

    fbSdoWrite      : FB_EcCoESdoWrite;
    fbSdoRead       : FB_EcCoESdoRead;
    nObjIndex       : WORD := 16#9000;  
    nRxIndex        : UINT := 0;
    nOffset         : UDINT;
    aTxBuffer       : ARRAY[0..63] OF BYTE; 
    
    MAX_PAYLOAD     : UDINT := 59;
    CMD_START       : BYTE := 16#F1;
    CMD_DATA        : BYTE := 16#F2;
    CMD_END         : BYTE := 16#F3;
    MAX_RETRY       : INT := 3;
    
    nCrc32          : UDINT;
    
    rtStart         : R_TRIG;
    fbTimer         : TON;
    nSlaveState AT %I* : UINT;
    
    nChunkSize      : UDINT;
    nTxLen          : UDINT;
    nRetry          : INT := 0;
    nRetryState     : INT := 0;
END_VAR

rtStart(CLK := bStartUpdate);

IF bReset THEN
    nState := 0; bError := FALSE; nErrId := 0; bBusy := FALSE; nRetry := 0;
    sStatus := 'Reset'; bReset := FALSE;
END_IF

CASE nState OF
    (* ================= 初始化 ================= *)
    0: 
        IF rtStart.Q THEN
            bBusy := TRUE; bError := FALSE; nErrId := 0; nRetry := 0;
            bStartUpdate := FALSE; nProgress := 0; nOffset := 0; nRxIndex := 0;
            nState := 5; sStatus := 'Wait Slave...';
        END_IF
        
    5: (* 等待从站 *)
        fbTimer(IN := TRUE, PT := T#10S);
        IF (nSlaveState AND 16#000F) >= 2 THEN 
            fbTimer(IN := FALSE); nState := 10;
        ELSIF fbTimer.Q THEN 
            nState := 999; nErrId := 16#FF01; sStatus := 'Slave Timeout';
        END_IF

    (* ================= 文件读取 ================= *)
    10: 
        fbFileOpen(bExecute := TRUE, sNetId := '', sPathName := sFileName, nMode := FOPEN_MODEREAD OR FOPEN_MODEBINARY);
        IF NOT fbFileOpen.bBusy THEN
            IF fbFileOpen.bError THEN nState := 999; nErrId := fbFileOpen.nErrId; sStatus := 'File Open Err';
            ELSE hFile := fbFileOpen.hFile; nState := 20; fbFileOpen(bExecute := FALSE); END_IF
        END_IF

    20: 
        fbFileRead(bExecute := TRUE, sNetId := '', hFile := hFile, pReadBuff := ADR(aFileBuffer), cbReadLen := SIZEOF(aFileBuffer));
        IF NOT fbFileRead.bBusy THEN
            IF fbFileRead.bError THEN nState := 999; nErrId := fbFileRead.nErrId; sStatus := 'File Read Err';
            ELSE nFileSize := fbFileRead.cbRead;
                IF nFileSize = 0 THEN nState := 999; sStatus := 'Empty File';
                ELSE nState := 30; END_IF
                fbFileRead(bExecute := FALSE);
            END_IF
        END_IF

    30: 
        fbFileClose(bExecute := TRUE, sNetId := '', hFile := hFile);
        IF NOT fbFileClose.bBusy THEN nState := 50; fbFileClose(bExecute := FALSE); END_IF

    (* ================= 第一次握手 (App) ================= *)
    50: (* RxID *)
        aTxBuffer[0] := 254;
        fbSdoWrite(sNetId := sMasterNetId, nSlaveAddr := nSlaveAddr, nIndex := nObjIndex, nSubIndex := 6, pSrcBuf := ADR(aTxBuffer[0]), cbBufLen := 2, bExecute := TRUE);
        IF NOT fbSdoWrite.bBusy THEN
            IF fbSdoWrite.bError THEN 
                IF nRetry < MAX_RETRY THEN nRetry := nRetry + 1; nRetryState := 50; nState := 900;
                ELSE nState := 999; nErrId := fbSdoWrite.nErrId; sStatus := 'RxID Err'; END_IF
            ELSE nRetry := 0; nState := 51; fbSdoWrite(bExecute := FALSE); END_IF
        END_IF

    51: (* RxLen *)
        aTxBuffer[0] := 7;
        fbSdoWrite(sNetId := sMasterNetId, nSlaveAddr := nSlaveAddr, nIndex := nObjIndex, nSubIndex := 7, pSrcBuf := ADR(aTxBuffer[0]), cbBufLen := 2, bExecute := TRUE);
        IF NOT fbSdoWrite.bBusy THEN
            IF fbSdoWrite.bError THEN 
                IF nRetry < MAX_RETRY THEN nRetry := nRetry + 1; nRetryState := 51; nState := 900;
                ELSE nState := 999; nErrId := fbSdoWrite.nErrId; sStatus := 'RxLen Err'; END_IF
            ELSE nRetry := 0; nState := 52; fbSdoWrite(bExecute := FALSE); END_IF
        END_IF

    52: (* RxIndex *)
        nRxIndex := 1;
        fbSdoWrite(sNetId := sMasterNetId, nSlaveAddr := nSlaveAddr, nIndex := nObjIndex, nSubIndex := 5, pSrcBuf := ADR(nRxIndex), cbBufLen := 2, bExecute := TRUE);
        IF NOT fbSdoWrite.bBusy THEN
            IF fbSdoWrite.bError THEN 
                IF nRetry < MAX_RETRY THEN nRetry := nRetry + 1; nRetryState := 52; nState := 900;
                ELSE nState := 999; nErrId := fbSdoWrite.nErrId; END_IF
            ELSE nRetry := 0; nState := 53; fbSdoWrite(bExecute := FALSE); END_IF
        END_IF

    53: (* 发送 F1 *)
        MEMSET(ADR(aTxBuffer), 0, SIZEOF(aTxBuffer));
        aTxBuffer[0] := CMD_START;
        aTxBuffer[1] := UDINT_TO_BYTE(nFileSize AND 16#FF);
        aTxBuffer[2] := UDINT_TO_BYTE(SHR(nFileSize, 8) AND 16#FF);
        aTxBuffer[3] := UDINT_TO_BYTE(SHR(nFileSize, 16) AND 16#FF);
        aTxBuffer[4] := 16#34; aTxBuffer[5] := 16#12; aTxBuffer[6] := 64;
        
        fbSdoWrite(sNetId := sMasterNetId, nSlaveAddr := nSlaveAddr, nIndex := nObjIndex, nSubIndex := 8, pSrcBuf := ADR(aTxBuffer), cbBufLen := 7, bExecute := TRUE);
        IF NOT fbSdoWrite.bBusy THEN
            IF fbSdoWrite.bError THEN 
                IF nRetry < MAX_RETRY THEN nRetry := nRetry + 1; nRetryState := 53; nState := 900;
                ELSE nState := 999; nErrId := fbSdoWrite.nErrId; sStatus := 'F1 Err'; END_IF
            ELSE nRetry := 0; fbTimer(IN := FALSE); nState := 54; fbSdoWrite(bExecute := FALSE); END_IF
        END_IF

    54: (* 检查 App 状态 *)
        fbTimer(IN := TRUE, PT := T#10S);
        IF fbTimer.Q THEN nState := 999; sStatus := 'App Timeout'; fbTimer(IN := FALSE);
        ELSE
            fbSdoRead(sNetId := sMasterNetId, nSlaveAddr := nSlaveAddr, nIndex := nObjIndex, nSubIndex := 4, pDstBuf := ADR(aTxBuffer), cbBufLen := 64, bExecute := TRUE);
            IF NOT fbSdoRead.bBusy THEN
                IF fbSdoRead.bError THEN 
                    IF nRetry < MAX_RETRY THEN nRetry := nRetry + 1; nRetryState := 54; nState := 900;
                    ELSE nState := 999; nErrId := fbSdoRead.nErrId; sStatus := 'Read Err'; END_IF
                ELSIF aTxBuffer[0] = 16#F1 THEN
                    nRetry := 0;
                    CASE aTxBuffer[1] OF
                        0: nState := 999; sStatus := 'Pwd Err';
                        1: nState := 60; sStatus := 'Jumping...'; fbTimer(IN := FALSE);
                        2, 5: nState := 100; sStatus := 'Boot Ready'; fbTimer(IN := FALSE);
                        3: nState := 999; sStatus := 'Mem Full';
                        4: sStatus := 'Erasing...';
                        ELSE nState := 999; sStatus := 'Unknown Status';
                    END_CASE
                END_IF
                fbSdoRead(bExecute := FALSE);
            END_IF
        END_IF

    60: (* 等待重启 *)
        fbTimer(IN := TRUE, PT := T#10S); 
        IF fbTimer.Q THEN nState := 100; fbTimer(IN := FALSE); END_IF

    (* ================= 第二次握手 (Boot) ================= *)
    100: 
        nOffset := 0; nRxIndex := 0; nProgress := 0; nRetry := 0; nState := 101;

    101: (* RxLen=0 *)
        aTxBuffer[0] := 0;
        fbSdoWrite(sNetId := sMasterNetId, nSlaveAddr := nSlaveAddr, nIndex := nObjIndex, nSubIndex := 7, pSrcBuf := ADR(aTxBuffer[0]), cbBufLen := 2, bExecute := TRUE);
        IF NOT fbSdoWrite.bBusy THEN
            IF fbSdoWrite.bError THEN 
                IF nRetry < MAX_RETRY THEN nRetry := nRetry + 1; nRetryState := 101; nState := 900;
                ELSE nState := 999; nErrId := fbSdoWrite.nErrId; END_IF
            ELSE nRetry := 0; nState := 110; fbSdoWrite(bExecute := FALSE); END_IF
        END_IF

    110: (* RxIndex *)
        nRxIndex := 0;
        fbSdoWrite(sNetId := sMasterNetId, nSlaveAddr := nSlaveAddr, nIndex := nObjIndex, nSubIndex := 5, pSrcBuf := ADR(nRxIndex), cbBufLen := 2, bExecute := TRUE);
        IF NOT fbSdoWrite.bBusy THEN
            IF fbSdoWrite.bError THEN 
                IF nRetry < MAX_RETRY THEN nRetry := nRetry + 1; nRetryState := 110; nState := 900;
                ELSE nState := 999; nErrId := fbSdoWrite.nErrId; END_IF
            ELSE nRetry := 0; nState := 111; fbSdoWrite(bExecute := FALSE); END_IF
        END_IF

    111: (* 发送 F1 到 Boot *)
        MEMSET(ADR(aTxBuffer), 0, SIZEOF(aTxBuffer));
        aTxBuffer[0] := CMD_START;
        aTxBuffer[1] := UDINT_TO_BYTE(nFileSize AND 16#FF);
        aTxBuffer[2] := UDINT_TO_BYTE(SHR(nFileSize, 8) AND 16#FF);
        aTxBuffer[3] := UDINT_TO_BYTE(SHR(nFileSize, 16) AND 16#FF);
        aTxBuffer[4] := 16#34; aTxBuffer[5] := 16#12; aTxBuffer[6] := 64;
        
        fbSdoWrite(sNetId := sMasterNetId, nSlaveAddr := nSlaveAddr, nIndex := nObjIndex, nSubIndex := 8, pSrcBuf := ADR(aTxBuffer), cbBufLen := 7, bExecute := TRUE);
        IF NOT fbSdoWrite.bBusy THEN
            IF fbSdoWrite.bError THEN 
                IF nRetry < MAX_RETRY THEN nRetry := nRetry + 1; nRetryState := 111; nState := 900;
                ELSE nState := 999; nErrId := fbSdoWrite.nErrId; END_IF
            ELSE nRetry := 0; fbTimer(IN := FALSE); nState := 112; fbSdoWrite(bExecute := FALSE); END_IF
        END_IF

    112: (* 轮询 Boot 状态 *)
        fbTimer(IN := TRUE, PT := T#60S);
        IF fbTimer.Q THEN nState := 999; sStatus := 'Boot Timeout'; fbTimer(IN := FALSE);
        ELSE
            fbSdoRead(sNetId := sMasterNetId, nSlaveAddr := nSlaveAddr, nIndex := nObjIndex, nSubIndex := 4, pDstBuf := ADR(aTxBuffer), cbBufLen := 64, bExecute := TRUE);
            IF NOT fbSdoRead.bBusy THEN
                IF fbSdoRead.bError THEN 
                    IF nRetry < MAX_RETRY THEN nRetry := nRetry + 1; nRetryState := 112; nState := 900;
                    ELSE nState := 999; nErrId := fbSdoRead.nErrId; END_IF
                ELSIF aTxBuffer[0] = 16#F1 THEN
                    nRetry := 0;
                    CASE aTxBuffer[1] OF
                        0: nState := 999; sStatus := 'Pwd Err';
                        1: nState := 60; sStatus := 'Jumping...'; fbTimer(IN := FALSE);
                        2, 5: nRxIndex := 1; nState := 120; sStatus := 'Uploading...'; fbTimer(IN := FALSE);
                        3: nState := 999; sStatus := 'Mem Full';
                        4: sStatus := 'Erasing...';
                        ELSE nState := 999; sStatus := 'Unknown';
                    END_CASE
                END_IF
                fbSdoRead(bExecute := FALSE);
            END_IF
        END_IF

    (* ================= 数据传输 ================= *)
    120: 
        IF nOffset >= nFileSize THEN nState := 130;
        ELSE
            nChunkSize := nFileSize - nOffset;
            IF nChunkSize > MAX_PAYLOAD THEN nChunkSize := MAX_PAYLOAD; END_IF
            
            MEMSET(ADR(aTxBuffer), 0, SIZEOF(aTxBuffer));
            aTxBuffer[0] := CMD_DATA;
            MEMCPY(ADR(aTxBuffer[1]), ADR(nOffset), 4);
            MEMCPY(ADR(aTxBuffer[5]), ADR(aFileBuffer) + nOffset, UDINT_TO_UINT(nChunkSize));
            nTxLen := 5 + nChunkSize;
            nState := 121;
        END_IF

    121: (* RxIndex *)
        fbSdoWrite(sNetId := sMasterNetId, nSlaveAddr := nSlaveAddr, nIndex := nObjIndex, nSubIndex := 5, pSrcBuf := ADR(nRxIndex), cbBufLen := 2, bExecute := TRUE);
        IF NOT fbSdoWrite.bBusy THEN
            IF fbSdoWrite.bError THEN 
                IF nRetry < MAX_RETRY THEN nRetry := nRetry + 1; nRetryState := 121; nState := 900;
                ELSE nState := 999; nErrId := fbSdoWrite.nErrId; END_IF
            ELSE nRetry := 0; nState := 122; fbSdoWrite(bExecute := FALSE); END_IF
        END_IF

    122: (* RxData *)
        fbSdoWrite(sNetId := sMasterNetId, nSlaveAddr := nSlaveAddr, nIndex := nObjIndex, nSubIndex := 8, pSrcBuf := ADR(aTxBuffer), cbBufLen := nTxLen, bExecute := TRUE);
        IF NOT fbSdoWrite.bBusy THEN
            IF fbSdoWrite.bError THEN 
                IF nRetry < MAX_RETRY THEN nRetry := nRetry + 1; nRetryState := 122; nState := 900;
                ELSE nState := 999; nErrId := fbSdoWrite.nErrId; END_IF
            ELSE 
                nRetry := 0;
                nOffset := nOffset + nChunkSize;
                nRxIndex := nRxIndex + 1;
                nProgress := UDINT_TO_UINT((nOffset * 100) / nFileSize);
                nState := 120;
                fbSdoWrite(bExecute := FALSE);
            END_IF
        END_IF

    (* ================= 结束校验 ================= *)
    130: 
        nCrc32 := F_DATA_CRC32(ADR(aFileBuffer), nFileSize);
        MEMSET(ADR(aTxBuffer), 0, SIZEOF(aTxBuffer));
        aTxBuffer[0] := CMD_END;
        MEMCPY(ADR(aTxBuffer[1]), ADR(nCrc32), 4);
        nState := 131;

    131: (* RxIndex *)
        fbSdoWrite(sNetId := sMasterNetId, nSlaveAddr := nSlaveAddr, nIndex := nObjIndex, nSubIndex := 5, pSrcBuf := ADR(nRxIndex), cbBufLen := 2, bExecute := TRUE);
        IF NOT fbSdoWrite.bBusy THEN
            IF fbSdoWrite.bError THEN 
                IF nRetry < MAX_RETRY THEN nRetry := nRetry + 1; nRetryState := 131; nState := 900;
                ELSE nState := 999; nErrId := fbSdoWrite.nErrId; END_IF
            ELSE nRetry := 0; nState := 132; fbSdoWrite(bExecute := FALSE); END_IF
        END_IF

    132: (* RxData *)
        fbSdoWrite(sNetId := sMasterNetId, nSlaveAddr := nSlaveAddr, nIndex := nObjIndex, nSubIndex := 8, pSrcBuf := ADR(aTxBuffer), cbBufLen := 5, bExecute := TRUE);
        IF NOT fbSdoWrite.bBusy THEN
            IF fbSdoWrite.bError THEN 
                IF nRetry < MAX_RETRY THEN nRetry := nRetry + 1; nRetryState := 132; nState := 900;
                ELSE nState := 999; nErrId := fbSdoWrite.nErrId; END_IF
            ELSE nRetry := 0; fbTimer(IN := FALSE); nState := 133; fbSdoWrite(bExecute := FALSE); END_IF
        END_IF

    133: (* 等待结果 *)
        fbTimer(IN := TRUE, PT := T#30S);
        IF fbTimer.Q THEN nState := 999; sStatus := 'Final Timeout'; fbTimer(IN := FALSE);
        ELSE
            fbSdoRead(sNetId := sMasterNetId, nSlaveAddr := nSlaveAddr, nIndex := nObjIndex, nSubIndex := 4, pDstBuf := ADR(aTxBuffer), cbBufLen := 64, bExecute := TRUE);
            IF NOT fbSdoRead.bBusy THEN
                IF fbSdoRead.bError THEN 
                    IF nRetry < MAX_RETRY THEN nRetry := nRetry + 1; nRetryState := 133; nState := 900;
                    ELSE nState := 999; nErrId := fbSdoRead.nErrId; fbTimer(IN := FALSE); END_IF
                ELSIF aTxBuffer[0] = 16#F3 THEN
                    nRetry := 0;
                    IF aTxBuffer[1] = 0 THEN nState := 200; sStatus := 'Success!'; fbTimer(IN := FALSE);
                    ELSE nState := 999; sStatus := 'CRC Err'; nErrId := 16#E000 + aTxBuffer[1]; fbTimer(IN := FALSE); END_IF
                END_IF
                fbSdoRead(bExecute := FALSE);
            END_IF
        END_IF

    200: (* 完成 *)
        bBusy := FALSE;
        IF NOT bStartUpdate THEN nState := 0; END_IF

    900: (* 重试延迟 *)
        fbTimer(IN := TRUE, PT := T#500MS);
        IF fbTimer.Q THEN 
            fbTimer(IN := FALSE); 
            nState := nRetryState; 
            sStatus := CONCAT('Retry ', INT_TO_STRING(nRetry));
        END_IF

    999: (* 错误 *)
        bError := TRUE; bBusy := FALSE;
        IF hFile <> 0 THEN fbFileClose(bExecute := TRUE, sNetId := '', hFile := hFile); hFile := 0; END_IF
        IF NOT bStartUpdate THEN nState := 0; END_IF
END_CASE
