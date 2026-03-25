#include <string.h>
#include "xmc_flash.h"
#include "xmc_scu.h"
#include "foeappl.h" // 包含 ECAT 错误码定义
#include "firmwareupdate_SSC.h"
#include "shared_conf.h"
#include <DAVE.h>
#include "objdef.h"
#include "ryhandObjects.h"

/* ================= 配置区域 ================= */
#define APP_START_ADDR      0x0C020000UL    // App 起始地址 (S8)
#define FLASH_PAGE_SIZE     256             // XMC4000 编程页大小
#define SLAVE_MAX_FRAME_LEN 64               // 从机支持的最大 SDO 数据长度 (RxData大小)
#define FW_UPDATE_PASSWORD  0x1234           // 【配置】升级密码 (2字节)

/* ================= 变量定义 ================= */
static uint32_t g_current_write_addr = APP_START_ADDR;
static uint8_t  g_page_buffer[FLASH_PAGE_SIZE]; // 256字节缓存
static uint32_t g_buffer_index = 0;             // 缓存当前填充位置

/* 内部状态变量 */
static uint32_t g_fw_total_size = 0;         // 固件总大小
static uint32_t g_current_offset = 0;        // 当前已写入的总偏移量
static uint16_t g_next_expected_index = 0;   // 连包检测计数器
static uint8_t  g_negotiated_len = 64;       // 协商后的最大帧长

/* Offset 写入模式专用缓存 */
static uint32_t g_offset_page_addr = 0xFFFFFFFF;
static uint8_t  g_offset_buffer[FLASH_PAGE_SIZE];
static uint8_t  g_offset_buffer_dirty = 0;

/* ================= 全局变量 ================= */
#define FLASH_PAGE_SIZE 256

// 256字节 蓄水池
static uint8_t  g_fifo_buffer[FLASH_PAGE_SIZE];
// 水位线 (当前存了多少字节)
static uint32_t g_fifo_fill_len = 0;
// 当前 Flash 写入地址指针
static uint32_t g_fifo_current_addr = APP_START_ADDR;

extern TOBJ9000 SimulateCANFD0x9000;
/* ================= 辅助函数声明 ================= */
uint32_t CalcCrc32(const uint8_t *p_data, uint32_t size); // 需确保工程中有CRC实现
uint32_t ExtractUint32(uint8_t *pData, uint8_t len);
void PackUint32(uint8_t *pData, uint32_t val, uint8_t len);

/* ================= 函数实现 ================= */

/**
 * @brief  开始下载：擦除 Flash S8 扇区
 * @param  pName 文件名指针 (可选检查)
 * @param  password 密码 (可选检查)
 */
/* 确保定义了 Uncached 地址宏，防止 Cache 干扰时序 */
#ifndef APP_START_ADDR_UNCACHED
#define APP_START_ADDR_UNCACHED  (APP_START_ADDR | 0x04000000UL)
#endif

void FWUPDATE_StartDownload(void)
{
    /* 1. 初始化变量 */
    g_current_write_addr = APP_START_ADDR;
    g_buffer_index = 0;

    /* 填充 Buffer 为 0xFF */
    memset(g_page_buffer, 0xFF, FLASH_PAGE_SIZE);

    /* -----------------------------------------------------------------------
     * 核心修复 A：强制复位 Flash 状态机 & 关中断
     * -----------------------------------------------------------------------
     */

    /* 1. 关中断：这是最关键的一步！防止 Watchdog/PDI 中断打断命令序列 */
    __disable_irq();

    /* 2. 清除状态 & 复位 */
    /* 必须在关中断后做，确保复位命令本身也不被打断 */
    XMC_FLASH_ClearStatus();
    XMC_FLASH_Reset();

    /* 3. 执行擦除 (使用 Uncached 地址) */
    /* 这一步必须是原子的，不能插入任何读取 Flash 的操作 */
    XMC_FLASH_EraseSector((uint32_t *)APP_START_ADDR_UNCACHED);

    uint32_t fsr = FLASH0->FSR;

    /* 4. 等待擦除完成 (PBUSY) */
    /* 注意：XMC_FLASH_EraseSector 内部有 while 等待，
       但在等待期间 CPU 是 Stall (暂停) 的，或者在空转。
       只有擦除彻底完成后，才允许恢复中断。*/

    /* 5. 恢复中断 */
    __enable_irq();

    /* -----------------------------------------------------------------------
     * 核心修复 B：清除指令缓存
     * -----------------------------------------------------------------------
     * 告诉 CPU 刚才 Flash 内容变了，不要用缓存里的旧数据
     */
    __DSB();
    __ISB();
    XMC_PREFETCH_InvalidateInstructionBuffer();

    /* -----------------------------------------------------------
	 * 【错误判断】
	 * ----------------------------------------------------------- */
    if (fsr & (XMC_FLASH_STATUS_COMMAND_SEQUENCE_ERROR | XMC_FLASH_STATUS_PROTECTION_ERROR))
	{
		/* 如果断点停在这里，说明擦除失败了！
		   fsr & 0x02 (SQER) -> 序列错误 (说明取指冲突了，需要把代码搬到 RAM)
		   fsr & 0x10 (PROER)-> 保护错误 (说明扇区被锁了)
		*/
		volatile int error_trap = 1;
		while(error_trap); // 死循环方便调试
	}
}

/**
 * @brief  接收数据并写入 Flash
 * @param  pData 数据指针
 * @param  Size  数据长度 (字节)
 * @return 0=成功, 其他=错误码
 */
uint16_t FWUPDATE_Data(uint16_t *pData, uint16_t Size)
{
    uint8_t *pBytes = (uint8_t *)pData;
    uint32_t i;

    /* 遍历接收到的每一个字节 */
    for (i = 0; i < Size; i++)
    {
        /* 1. 填入缓存 */
        g_page_buffer[g_buffer_index++] = pBytes[i];

        /* 2. 如果缓存满了 (256字节)，执行一次页写入 */
        if (g_buffer_index >= FLASH_PAGE_SIZE)
        {
            /* 安全检查：防止溢出覆盖 Bootloader 或 EEPROM */
            if (g_current_write_addr >= APP_END_ADDR)
            {
                return ECAT_FOE_ERRCODE_DISKFULL;
            }

            /* 执行页编程 */
            XMC_FLASH_ProgramPage((uint32_t *)g_current_write_addr, (uint32_t *)g_page_buffer);

            /* 更新地址，重置缓存 */
            g_current_write_addr += FLASH_PAGE_SIZE;
            g_buffer_index = 0;
            memset(g_page_buffer, 0xFF, FLASH_PAGE_SIZE); // 清空缓存
        }
    }

    return 0; // 成功
}

/* 这是一个简单的软件 CRC32 算法，或者你可以使用 XMC4300 的 FCE 硬件模块 */
uint32_t CalcCrc32(const uint8_t *p_data, uint32_t size)
{
    // 标准 CRC-32 (IEEE 802.3)
    uint32_t crc = 0xFFFFFFFF;
    for (uint32_t i = 0; i < size; i++) {
        crc ^= p_data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xEDB88320; // 这里的多项式是 0x04C11DB7 的位反转形式
            } else {
                crc >>= 1;
            }
        }
    }
    return ~crc; // 关键：标准算法最后需要取反
}

extern uint32_t g_foe_total_received;
extern volatile uint8_t  g_bResetRequest;


/**
 * @brief  SDO下载结束：刷入残留数据、校验CRC、设置结束标志
 * (放在 firmwareupdate_SSC.c 中以访问静态 buffer 变量)
 * @return 0 = 成功, 其他 = SDO Abort Code (用于 SDO_WriteCallback 返回)
 */
uint8_t FWUPDATE_SDO_DownloadFinished(void)
{
    /* 1. ====== 处理缓存中的残留数据 (Flush Buffer) ====== */
    /* FWUPDATE_Data 只是把数据填入 g_page_buffer，满 256 字节才写 Flash。
     * 因此文件末尾通常会有剩余不足一页的数据留在 g_buffer_index 中。
     */
    if (g_buffer_index > 0)
    {
        /* A. 将剩余空间填充为 0xFF (Flash 擦除后的默认状态) */
        /* 防止写入随机垃圾数据，虽然后面的数据一般不读，但保持整洁 */
        memset(&g_page_buffer[g_buffer_index], 0xFF, FLASH_PAGE_SIZE - g_buffer_index);

        /* B. 安全检查：防止地址溢出 */
        if (g_current_write_addr >= APP_END_ADDR)
        {
             return ABORTIDX_DATA_CANNOT_BE_READ_OR_STORED;
        }

        /* C. 将这一页写入 Flash */
        XMC_FLASH_ProgramPage((uint32_t *)g_current_write_addr, (uint32_t *)g_page_buffer);

        /* D. 更新地址和索引 (虽然结束了，但为了状态一致性) */
        g_current_write_addr += FLASH_PAGE_SIZE;
        g_buffer_index = 0;
    }

    /* 2. ====== 确保 Flash 操作物理完成 (Barriers) ====== */
    /* 数据同步屏障 & 指令同步屏障，防止 CPU 预取指读到旧数据 */
    __DSB();
    __ISB();
    XMC_PREFETCH_InvalidateInstructionBuffer();
    __DSB();
    __ISB();

    /* 3. ====== 长度基本检查 ====== */
    /* 至少要包含 4字节 CRC */
    if (g_sdo_total_received < 4)
    {
        return ABORTIDX_DATA_CANNOT_BE_READ_OR_STORED;
    }

    /* 4. ====== CRC 校验逻辑 ====== */
    /* 前提：上位机发送的 bin 文件末尾已经附加了 4字节 CRC32 */

    // A. 计算 Flash 中 bin 文件的结束位置
    uint32_t file_end_addr = APP_START_ADDR + g_sdo_total_received;
    uint32_t crc_addr = file_end_addr - 4;

    // B. 从 Flash 末尾读取“预期”的 CRC
    uint32_t expected_crc = *(uint32_t *)crc_addr;

    // C. 计算有效代码长度 (总长 - 4字节CRC)
    uint32_t code_size = g_sdo_total_received - 4;

    // D. 计算 Flash 中实际写入数据的 CRC
    //    从 APP_START_ADDR 开始计算，长度为 code_size
    uint32_t calc_crc = CalcCrc32((const uint8_t *)APP_START_ADDR, code_size);

    /* 5. ====== 比对结果 ====== */
    if (calc_crc != expected_crc)
    {
    	/* 1. 【必加】清除之前的写入错误标志 */
		/* 如果不加这一行，因为之前数据写错产生的 PVER 标志会阻止本次擦除 */
		XMC_FLASH_ClearStatus();

		/* 2. 关中断（防止命令被打断） */
		__disable_irq();

		/* 3. 执行擦除 */
		XMC_FLASH_EraseSector((uint32_t *)APP_START_ADDR);

		/* 4. 恢复中断 */
		__enable_irq();

		/* 5. 清除 CPU 指令缓存，确保看到的是擦除后的 0xFF */
		XMC_PREFETCH_InvalidateInstructionBuffer();

        // 2. 重置接收计数，允许用户重新尝试
        g_sdo_total_received = 0;

        // 3. 返回错误码
        return ABORTIDX_DATA_CANNOT_BE_READ_OR_STORED; // 或自定义错误码
    }

    /* 6. ====== 校验成功 ====== */
    g_bResetRequest = 1;

    return 0; // 成功
}


void FWUPDATE_FIFO_Init(void)
{
    // 重置所有指针
    g_fifo_fill_len = 0;
    g_fifo_current_addr = APP_START_ADDR;

    // 把缓存填满 FF (好习惯)
    memset(g_fifo_buffer, 0xFF, FLASH_PAGE_SIZE);
}

uint8_t FWUPDATE_FIFO_Write(uint8_t *pData, uint32_t len)
{
    uint32_t processed = 0;

    while (processed < len)
    {
        // 1. 计算 buffer 还能塞多少？
        uint32_t space_left = FLASH_PAGE_SIZE - g_fifo_fill_len;

        // 2. 计算本次搬运量
        uint32_t chunk = len - processed;
        if (chunk > space_left) chunk = space_left;

        // 3. 搬运数据
        memcpy(&g_fifo_buffer[g_fifo_fill_len], &pData[processed], chunk);
        g_fifo_fill_len += chunk;
        processed += chunk;

        // 4. 如果满了 256，写入 Flash
        if (g_fifo_fill_len == FLASH_PAGE_SIZE)
        {
            // 安全检查：地址是否越界
            if (g_fifo_current_addr < APP_START_ADDR || g_fifo_current_addr >= APP_END_ADDR)
            {
                return 1; // 错误：地址越界
            }

            // 写入 Flash
            XMC_FLASH_ProgramPage((uint32_t *)g_fifo_current_addr, (uint32_t *)g_fifo_buffer);

            // 指针后移
            g_fifo_current_addr += FLASH_PAGE_SIZE;

            // 清空水位 (重置 buffer)
            g_fifo_fill_len = 0;
            // 重新填为 FF，防止下一页只写一半时有残留
            memset(g_fifo_buffer, 0xFF, FLASH_PAGE_SIZE);
        }
    }
    return 0; // 成功
}

uint8_t FWUPDATE_FIFO_Flush(void)
{
    // 检查是否有没写完的尾巴
    if (g_fifo_fill_len > 0)
    {
        // 因为 Init 和 Write 里都有 memset 0xFF，
        // 所以 buffer 后半部分已经是 FF 了，直接写即可。

        if (g_fifo_current_addr < APP_START_ADDR || g_fifo_current_addr >= APP_END_ADDR)
        {
            return 1;
        }

        XMC_FLASH_ProgramPage((uint32_t *)g_fifo_current_addr, (uint32_t *)g_fifo_buffer);

        // 稍微延时，确保写入完成
        for(volatile int i=0; i<5000; i++);
    }
    return 0;
}

extern uint8_t UpdateSharedConfig(uint32_t appSize, uint32_t crc);
/**
 * @brief 结束时刷新缓存并校验
 * @param masterCRC 主站发来的预期 CRC
 * @return 0=成功, 1=CRC错误, 2=硬件不匹配, 0xFF=Flash写入失败
 */
uint8_t FWUPDATE_SDO_Finish_New(uint32_t masterCRC)
{
	// 1. 强制写入最后残留的数据
	if (FWUPDATE_FIFO_Flush() != 0)
	{
		return 1; // 写入失败
	}

	// 2. 指令屏障 & CRC 校验 (代码保持不变)
	__DSB(); __ISB();
	XMC_PREFETCH_InvalidateInstructionBuffer();

    // 3. 计算 Flash CRC
    // 计算范围：从 APP_START_ADDR 开始，长度为 g_fw_total_size
    uint32_t calcCrc = Calc_App_Flash_CRC32(g_fw_total_size);

    // 4. 对比 CRC
    if (calcCrc != masterCRC)
    {
        return 1; // 错误码 1: CRC 校验失败
    }

	if (UpdateSharedConfig(g_fw_total_size, calcCrc))
	{
		return ECAT_FOE_ERRCODE_DISKFULL;
	}

    // 5. (可选) 硬件版本检查
    // 假设版本号存在 Flash 偏移 0x100 处
    // uint8_t fwHwVer = *((uint8_t*)(APP_START_ADDR + 0x100));
    // if (fwHwVer != MY_CURRENT_HW_VER) return 2; // 错误码 2: 版本不匹配

    return 0; // Success
}

/* ================= SDO 协议处理回调函数 ================= */

#define CMD_START 0xF1
#define CMD_DATA  0xF2
#define CMD_END   0xF3
volatile uint8_t g_bFirmwareEraseChip = 0;
volatile uint8_t g_UpdateStatus = 0;

// 定义一个足够大的数组，专门用来存放最近一次收到的完整原始包
uint8_t g_Debug_RxRawPacket[300];

// 再定义一个变量记录长度，方便你看收到了多少个字节
uint32_t g_Debug_RxLen = 0;

stu_upgrade upgrade_info;
uint8_t SDO_WriteCallback(uint16_t index, uint8_t subindex, uint32_t dataSize, uint16_t *pData, uint8_t bCompleteAccess)
{
    uint8_t *pByteData = (uint8_t *)pData;

    // ----------------------------------------------------------------
    // Sub 6: RxID (保持不变)
    // ----------------------------------------------------------------
    if (subindex == 0x06) {
        if (pData[0]!= 254) return ABORTIDX_VALUE_EXCEEDED;
        upgrade_info.RxID = 254;
    }
    // ----------------------------------------------------------------
    // Sub 7: RxLen (保持不变)
    // ----------------------------------------------------------------
    else if (subindex == 0x07) {
        // Bootloader 必须接受变长数据，不要加严格的 == 1 限制
    	upgrade_info.RxLen = pData[0];
    }
    // ----------------------------------------------------------------
    // Sub 5: RxIndex (保持不变)
    // ----------------------------------------------------------------
    else if (subindex == 0x05) {
    	upgrade_info.RxIndex = pData[0];
    }
    // ----------------------------------------------------------------
    // Sub 8: RxData (核心逻辑修改)
    // ----------------------------------------------------------------
	else if (subindex == 0x08)
	{
		uint8_t cmd = pByteData[0];
		memset(SimulateCANFD0x9000.TxData, 0, 64);

		// ============================================================
		// 阶段 1: Start (F1) -> 回复状态 4 (开始擦除)
		// ============================================================
		if (cmd == CMD_START)
		{
			uint32_t nNewSize = (uint32_t)pByteData[1] | ((uint32_t)pByteData[2] << 8) | ((uint32_t)pByteData[3] << 16);
			uint8_t pwdChk = pByteData[6]; // 简化的密码检查，请保留您的完整检查

			// 1. 检查大小
			if (nNewSize > APP_MAX_SIZE) {
				g_UpdateStatus = 3; // 内存不足
			}
			// 2. 检查密码 (这里仅示例)
			else if (pwdChk != 64) {
				g_UpdateStatus = 0; // 密码错误
			}
			else {
				if (FWUPDATE_InvalidateAppFlag() != 0)
				{
					// 如果 EEPROM 修改失败，可以考虑中止升级以防万一
					g_UpdateStatus = 0;
				}
				else
				{
					// 3. 校验通过
					FWUPDATE_FIFO_Init();
					if (g_UpdateStatus != 5 && g_bFirmwareEraseChip == 0)
					{
						g_fw_total_size = nNewSize;

						g_UpdateStatus = 4; // 【状态 4】: 开始擦除 (Busy)
						g_bFirmwareEraseChip = 1;    // 通知 MainLoop 干活
					}
				}
			}

			// 4. 立即回复当前状态 (4)
			SimulateCANFD0x9000.TxData[0] = 0xF1;
			SimulateCANFD0x9000.TxData[1] = g_UpdateStatus; // 这里回复 4
			SimulateCANFD0x9000.TxData[2] = 0x34;
			SimulateCANFD0x9000.TxData[3] = 0x12;
			SimulateCANFD0x9000.TxData[4] = pwdChk;
			SimulateCANFD0x9000.TxLen = 5;
			SimulateCANFD0x9000.TxIndex = upgrade_info.RxIndex;
			SimulateCANFD0x9000.TxID = 254 + 256;
		}
		// ============================================================
		// 阶段 2: Data (F2) -> 必须状态 5 才能接收
		// ============================================================
		else if (cmd == CMD_DATA)
		{
		    if (g_UpdateStatus != 5) return ABORTIDX_DATA_CANNOT_BE_READ_OR_STORED;

		    // 1. 计算 Payload 长度
		    uint32_t payloadLen = 0;
		    if (dataSize > 5) payloadLen = dataSize - 5;

		    // 2. 调用写入函数
		    if (payloadLen > 0)
		    {
		        // 注意：这里传的是 &pByteData[5]，即纯数据部分
		        if (FWUPDATE_FIFO_Write(&pByteData[5], payloadLen) != 0)
		        {
		            return ABORTIDX_DATA_CANNOT_BE_READ_OR_STORED; // 写入失败
		        }
		    }

		    // 3. 回复
		    SimulateCANFD0x9000.TxData[0] = 0xF2;
		    SimulateCANFD0x9000.TxLen = 1;
		    SimulateCANFD0x9000.TxIndex = upgrade_info.RxIndex;
		}
        // ============================================================
        // 阶段 3: End (F3) - 校验
        // ============================================================
        else if (cmd == CMD_END)
        {
            uint32_t masterCRC;
            memcpy(&masterCRC, &pByteData[1], 4);

            // 执行最终校验
            uint8_t updateResult = FWUPDATE_SDO_Finish_New(masterCRC);

            // 构造回复
            SimulateCANFD0x9000.TxData[0] = 0xF3;
            SimulateCANFD0x9000.TxData[1] = updateResult; // 0=成功
            SimulateCANFD0x9000.TxLen = 2;

            if (updateResult == 0)
            {
            	g_bResetRequest = 1; // 标记准备重启
            }
        }
        else
        {
            return ABORTIDX_DATA_CANNOT_BE_READ_OR_STORED;
        }

        // 统一回复 Index (用于握手)
        SimulateCANFD0x9000.TxIndex = upgrade_info.RxIndex;
        SimulateCANFD0x9000.TxID = 254 + 256;

    }

    return 0;
}

/**
 * @brief 处理 FoE 固件升级后的系统复位请求
 * @note  应在主循环 (while(1)) 中持续调用
 */
void FOE_CheckAndPerformReset(void)
{
    /* 使用 static 保持状态跨调用 */
    static uint32_t reset_timeout_counter = 0;
    const uint32_t RESET_TIMEOUT_MAX = 70000;

    static volatile uint8_t *const pAlStatus = (volatile uint8_t *)(0x60000130);

    if (!g_bResetRequest)
    {
        reset_timeout_counter = 0;  // 清除计数器
        return;
    }

    /* 指示灯反馈 */
    DIGITAL_IO_SetOutputHigh(&LED);

    /* 计数器递增 */
    reset_timeout_counter++;

    uint8_t current_state = *pAlStatus & 0x0F;

    /* 4. 检查复位触发条件：
     * - 状态切换至 INIT (0x01)
     * - 或者达到设定的最大超时时间
     */
    if ((current_state == 0x01) || (reset_timeout_counter > RESET_TIMEOUT_MAX))
    {
        /* 6. 执行处理器复位，重启并跳转至新固件 */
        NVIC_SystemReset();
    }
}

/**
 * @brief 处理固件擦除请求并启动下载任务
 * @note  此函数包含阻塞延时，但内部维持了协议栈轮询以防止 EtherCAT 掉线
 */
void FOE_ProcessEraseState(void)
{
    // 只有当擦除标志位置位时才执行
    if (!g_bFirmwareEraseChip)
    {
        return;
    }

    /* 1. 执行带协议栈维护的延时 */
    const uint32_t DELAY_CYCLES = 50000;
    for (volatile uint32_t delay_counter = 0; delay_counter < DELAY_CYCLES; delay_counter++)
    {
        /* 关键：在延时期间持续调用协议栈主循环，保持从站在线 */
        MainLoop();

        /* 微小延时，平衡轮询频率 */
        for (volatile int k = 0; k < 100; k++);
    }

    /* 2. 状态转换与清理 */
    g_bFirmwareEraseChip = 0;   // 清除触发标志
    FWUPDATE_StartDownload();   // 启动下载逻辑
    g_UpdateStatus = 5;         // 更新内部升级状态机

    /* 3. 反馈结果 (通过模拟 CANFD 接口) */
    FOE_SendStatusFeedback(0xF1, 5);
}

/* ================= 通用辅助函数实现 ================= */

/**
 * @brief 内部辅助函数：发送升级状态反馈
 */
static void FOE_SendStatusFeedback(uint8_t cmd, uint8_t status)
{
    memset(SimulateCANFD0x9000.TxData, 0, 64);
    SimulateCANFD0x9000.TxData[0] = cmd;
    SimulateCANFD0x9000.TxData[1] = status;
    SimulateCANFD0x9000.TxData[2] = 0x34;
    SimulateCANFD0x9000.TxData[3] = 0x12;
    SimulateCANFD0x9000.TxData[4] = 64;
    // 填充其他辅助信息 (可选)
    SimulateCANFD0x9000.TxLen = 5;
    SimulateCANFD0x9000.TxIndex = SimulateCANFD0x9000.RxIndex;
    SimulateCANFD0x9000.TxID = 254 + 256;

    // 此处可根据需要添加实际的发送触发函数，如 CAN_Transmit(...)
}

uint32_t ExtractUint32(uint8_t *pData, uint8_t len)
{
    uint32_t val = 0;
    for(int i=0; i<len; i++) val |= ((uint32_t)pData[i] << (8*i));
    return val;
}

void PackUint32(uint8_t *pData, uint32_t val, uint8_t len)
{
    for(int i=0; i<len; i++) pData[i] = (uint8_t)(val >> (8*i));
}
