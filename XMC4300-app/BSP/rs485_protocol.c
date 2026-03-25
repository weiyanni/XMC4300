/* ============================================================================
 * 文件名: RS485_protocol.c
 * 描述: RS485 协议处理实现 (Boot 版本)
 *
 * 参考 User_FoE_Impl.c 的 Flash 操作逻辑
 * ============================================================================ */
#include <rs485_protocol.h>
#include "bsp_485.h"
#include "shared_conf.h"
#include "Dave/Generated/E_EEPROM_XMC4/e_eeprom_xmc4.h"
#include "SSC/Src/ecat_def.h"        /* 必须在 ryhandObjects.h 之前，提供 UINT16 等类型定义 */
#include "SSC/Src/ryhandObjects.h"
#include <string.h>
#include <xmc_flash.h>
#include "DAVE.h"

/* 外部 SSC 对象 */
extern TOBJ8000 DevInfo0x8000;

/* 升级上下文 (全局) */
extern UpgradeContext_t g_UpgradeCtx;

/* Flash 写入 (参考 FoE 实现) */
static uint32_t s_FlashWriteAddr;       /* 当前 Flash 写入地址 */
static uint8_t  s_PageBuf[256] __attribute__((aligned(4)));
static uint16_t s_PageBufOffset;

/* ==============================================================
 * 密码计算 (基于设备ID)
 * ============================================================== */
uint16_t RS485_Protocol_CalcPassword(uint16_t dev_id)
{
    return (uint16_t)((dev_id * 0x1234) ^ 0xABCD);
}

/* ==============================================================
 * CRC32 计算 (多项式 0x04C11DB7，与上位机一致)
 *
 * 上位机算法特点：
 * 1. 按 32 位字处理数据
 * 2. MSB first
 * ============================================================== */
static uint32_t s_CRC = 0xFFFFFFFF;
static const uint32_t CRC_POLYNOMIAL = 0x04C11DB7;

static void CRC_Reset(void)
{
    s_CRC = 0xFFFFFFFF;
}

/* 按 32 位字更新 CRC (与上位机 CalCRC 函数一致) */
static void CRC_UpdateWord(uint32_t data)
{
    uint32_t xbit = 1U << 31;

    for (int bits = 0; bits < 32; bits++)
    {
        if (s_CRC & 0x80000000)
        {
            s_CRC <<= 1;
            s_CRC ^= CRC_POLYNOMIAL;
        }
        else
        {
            s_CRC <<= 1;
        }
        if (data & xbit)
        {
            s_CRC ^= CRC_POLYNOMIAL;
        }
        xbit >>= 1;
    }
}

static uint32_t CRC_GetValue(void)
{
    return s_CRC;
}

uint32_t RS485_Protocol_CalcCRC32(const uint8_t *data, uint32_t len)
{
    CRC_Reset();
    /* 按 32 位字处理 */
    uint32_t word_count = len / 4;
    const uint32_t *words = (const uint32_t *)data;
    for (uint32_t i = 0; i < word_count; i++)
    {
        CRC_UpdateWord(words[i]);
    }
    return CRC_GetValue();
}

/* ==============================================================
 * Flash 操作 (参考 FoE 实现)
 * ============================================================== */

/* 刷新页缓冲区到 Flash */
static uint8_t FlushPageBuffer(void)
{
    if (s_PageBufOffset == 0) return 0;

    /* 填充剩余部分为 0xFF */
    if (s_PageBufOffset < 256)
    {
        memset(&s_PageBuf[s_PageBufOffset], 0xFF, 256 - s_PageBufOffset);
    }

    /* 写入 Flash */
    XMC_FLASH_ProgramPage((uint32_t*)s_FlashWriteAddr, (uint32_t*)s_PageBuf);

    s_FlashWriteAddr += 256;
    s_PageBufOffset = 0;

    return 0;
}

/* 写入数据到页缓冲区 (参考 FoE 的 BufferData) */
static uint8_t BufferData(const uint8_t *data, uint16_t size)
{
    for (uint16_t i = 0; i < size; i++)
    {
        /* 注意: CRC 在升级结束时从 Flash 统一计算 */
        s_PageBuf[s_PageBufOffset++] = data[i];

        if (s_PageBufOffset >= 256)
        {
            if (FlushPageBuffer()) return 1;
        }
    }
    return 0;
}

/* 擦除 App 区域 (参考 FoE 的 FOE_Write) */
static uint8_t Flash_EraseAppArea(void)
{
    /* 清除状态 (重要!) */
    XMC_FLASH_ClearStatus();

    /* 擦除 App 扇区 */
    XMC_FLASH_EraseSector((uint32_t*)APP_START_ADDR);

    /* 检查错误 */
    if (FLASH0->FSR & (XMC_FLASH_STATUS_VERIFY_ERROR |
                       XMC_FLASH_STATUS_PROTECTION_ERROR |
                       XMC_FLASH_STATUS_OPERATION_ERROR))
    {
        return 1;
    }

    return 0;
}

/* ==============================================================
 * 协议命令分发
 * ============================================================== */
void RS485_Protocol_HandleCommand(uint16_t  Id,const uint8_t *rx_data, uint8_t rx_len)
{
	uint32_t BR_canfd_ctrl = 0;
	uint32_t BR_canfd_data = 0;
	uint32_t BR_485 = 0;
    if (rx_data == NULL || rx_len == 0)
    {
        return;
    }

    uint8_t cmd = rx_data[0];

    switch (cmd)
    {
        case RS485_CMD_GET_SW_VERSION:
        	if(Id == 254)
        	{
        		RS485_Protocol_HandleGetSwVersion();
        	}
            break;

        case RS485_CMD_START_UPGRADE:
        	if(Id == 254)
        	{
        		RS485_Protocol_HandleStartUpgrade(rx_data, rx_len);
        	}
            break;

        case RS485_CMD_UPGRADE_DATA:
        	if(Id == 254)
        	{
        		RS485_Protocol_HandleUpgradeData(rx_data, rx_len);
        	}
            break;
        case RS485_CMD_UPGRADE_END:
        	if(Id == 254)
        	{
        		RS485_Protocol_HandleUpgradeEnd(rx_data, rx_len);
        	}
            break;
        case RS485_CMD_SET_CAN_BAUD:
        	BR_canfd_ctrl = ((uint32_t)rx_data[1]) |
						   ((uint32_t)rx_data[2] << 8) |
						   ((uint32_t)rx_data[3] << 16);
        	BR_canfd_data = ((uint32_t)rx_data[4]) |
						   ((uint32_t)rx_data[5] << 8) |
						   ((uint32_t)rx_data[6] << 16) |
						   ((uint32_t)rx_data[7] << 24);
        	DevInfo0x8000.BR_canfd_ctrl = BR_canfd_ctrl;
        	DevInfo0x8000.BR_canfd_data = BR_canfd_data;
        	DevConfig_SaveToEEPROM();
			break;
        case RS485_CMD_SET_RS485_BAUD:
        	BR_485 = ((uint32_t)rx_data[1]) |
				   ((uint32_t)rx_data[2] << 8) |
				   ((uint32_t)rx_data[3] << 16) |
				   ((uint32_t)rx_data[4] << 24);
            DevInfo0x8000.BR_485  = BR_485;
            DevConfig_SaveToEEPROM();
			break;
        default:
            break;
    }
}


/* ==============================================================
 * 0xF0 命令 - 读取软件版本
 * ============================================================== */
void RS485_Protocol_HandleGetSwVersion(void)
{
	const char *version = DevInfo0x8000.Sv; // 例如 "1.0.0"
	uint8_t version_len = (uint8_t)strlen(version);
	if (version_len == 0) version_len = 1;

	// 分包发送逻辑 (如果版本号较长)
	const uint8_t PIECE_SIZE = 6;
	uint8_t total_frames = (version_len + PIECE_SIZE - 1) / PIECE_SIZE;

	for (uint8_t frame_num = 0; frame_num < total_frames; frame_num++)
	{
		uint8_t payload[8]; // 协议中的 DATA 部分

		// 构造 DATA 内容
		payload[0] = 0xF0; // 命令识别码 (回复 GetVersion)

		// 帧信息: Bit7=是否尾帧, Bit0-6=帧序号
		uint8_t is_tail = (frame_num == total_frames - 1) ? 1 : 0;
		payload[1] = (is_tail << 7) | (frame_num & 0x7F);

		// 填充版本字符串片段
		uint8_t offset = frame_num * PIECE_SIZE;
		for (uint8_t j = 0; j < 6; j++) {
			payload[2 + j] = (offset + j < version_len) ? (uint8_t)version[offset + j] : 0x00;
		}

		// --- 核心：调用刚才写的组包发送函数 ---
		RS485_SendProtocolFrame(254,payload, 8);

		// 如果是连续多帧发送，建议加一点微小的软件延时，
		// 确保 DMA 有机会启动（虽然有 Busy 检测，但太快可能会占满带宽）
		for(volatile int d = 0; d < 500; d++);
	}
}

/* ==============================================================
 * 0xF1 命令 - 开始升级 (Boot 版本)
 * ============================================================== */
void RS485_Protocol_HandleStartUpgrade(const uint8_t *rx_data, uint8_t rx_len)
{
	/* 1. 基本长度校验 (根据原逻辑 rx_data 包含 Payload，需确保长度足够) */
	if (rx_len < 7)
	{
		return;
	}

	/* 2. 解析请求 (小端) */
	/* rx_data[1..3] 为数据总长度 */
	uint16_t recv_password = (uint16_t)rx_data[4] | ((uint16_t)rx_data[5] << 8);

	/* 帧长度: 主机发送1字节 */
	uint16_t recv_frame_len = (uint16_t)rx_data[6];
	(void)recv_frame_len;

	/* 3. 获取设备ID并计算正确密码 (复用 RS485 协议层的密码算法) */
	/* 假设 DevInfo0x8000 是全局对象 */
	uint16_t dev_id = DevInfo0x8000.DevID;
	uint16_t correct_password = RS485_Protocol_CalcPassword(dev_id);

	/* 4. 准备应答数据 */
	uint8_t tx_payload[8] = {0};
	tx_payload[0] = RS485_CMD_START_UPGRADE; /* 对应 0xF1 */

	/* 填充应答内容 (保持小端格式) */
	tx_payload[2] = (uint8_t)(correct_password & 0xFF);
	tx_payload[3] = (uint8_t)((correct_password >> 8) & 0xFF);

	/* 设备支持的最大数据包长度 (RS485_MAX_DATA_LEN 为 8) */
	tx_payload[4] = (uint8_t)(RS485_MAX_DATA_LEN & 0xFF);

	if (recv_password != correct_password)
	{
		/* 密码错误状态 */
		tx_payload[1] = 0x00; /* 假设 UPGRADE_STATUS_PASSWORD_FAIL 为 2 */

		/* 调用 485 组包发送接口: 会自动添加 A5, ID, LEN, Checksum */
		RS485_SendProtocolFrame(254,tx_payload, 5);
	}
	else
	{
		/* 密码正确，准备跳转 */
		tx_payload[1] = 0x01; /* 假设 UPGRADE_STATUS_APP_TO_BOOT 为 1 */

		/* 发送应答帧 */
		RS485_SendProtocolFrame(254,tx_payload, 5);

		/* 5. 关键：等待 485 硬件彻底发完 (因为 485 是半双工) */
		/* 由于使用了 DMA + IDLE 轮询，这里等待 tx_busy 清零即可 */
		uint32_t timeout = 0xFFFFF;
		extern UART_t UART_485;
		while (UART_485.runtime->tx_busy && --timeout);

		/* 增加物理链路排空延时 (5Mbps 虽快，但需确保停止位发完) */
		for (volatile int i = 0; i < 50000; i++);

		Handle_Bootstrap_Request();
	}
}

/* ==============================================================
 * 0xF2 命令 - 升级数据
 *
 * 注意: 数据按顺序接收，地址用于校验（可选）
 * ============================================================== */
void RS485_Protocol_HandleUpgradeData(const uint8_t *rx_data, uint8_t rx_len)
{

}

/* ==============================================================
 * 0xF3 命令 - 升级结束
 * ============================================================== */
void RS485_Protocol_HandleUpgradeEnd(const uint8_t *rx_data, uint8_t rx_len)
{

}
