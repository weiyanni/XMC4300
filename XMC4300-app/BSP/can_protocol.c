/* ============================================================================
 * 文件名: can_protocol.c
 * 描述: CAN 协议处理实现 (App 版本)
 * 
 * App 职责:
 *   - 0xF0: 返回软件版本
 *   - 0xF1: 验证密码，密码正确后跳转到 Boot
 *   - 0xF2/0xF3: 不处理（由 Boot 处理）
 * 
 * 多字节数据使用小端格式传输
 * ============================================================================ */
#include "can_protocol.h"
#include "bsp_can.h"
#include "app_init.h"
#include "SSC/Src/ecat_def.h"        /* 必须在 ryhandObjects.h 之前，提供 UINT16 等类型定义 */
#include "SSC/Src/ryhandObjects.h"
#include <string.h>

/* 外部 SSC 对象 */
extern TOBJ8000 DevInfo0x8000;

/* ==============================================================
 * 密码计算 (基于设备ID)
 * 算法: password = (dev_id * 0x1234) ^ 0xABCD
 * ============================================================== */
uint16_t CAN_Protocol_CalcPassword(uint8_t dev_id)
{
    return (uint16_t)((dev_id * 0x1234) ^ 0xABCD);
}

/* ==============================================================
 * CRC32 计算 (多项式 0x04C11DB7)
 * ============================================================== */
static uint32_t s_CRC = 0xFFFFFFFF;
static const uint32_t CRC_POLYNOMIAL = 0x04C11DB7;

static void CRC_Update(uint32_t data)
{
    uint32_t xbit = 1UL << 31;
    
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

uint32_t CAN_Protocol_CalcCRC32(const uint8_t *data, uint32_t len)
{
    s_CRC = 0xFFFFFFFF;
    
    /* 按 4 字节处理 */
    uint32_t words = len / 4;
    for (uint32_t i = 0; i < words; i++)
    {
        /* 小端读取 */
        uint32_t word = (uint32_t)data[i*4] |
                        ((uint32_t)data[i*4+1] << 8) |
                        ((uint32_t)data[i*4+2] << 16) |
                        ((uint32_t)data[i*4+3] << 24);
        CRC_Update(word);
    }
    
    /* 处理剩余字节 */
    uint32_t remain = len % 4;
    if (remain > 0)
    {
        uint32_t word = 0;
        for (uint32_t i = 0; i < remain; i++)
        {
            word |= ((uint32_t)data[words*4 + i] << (i * 8));
        }
        CRC_Update(word);
    }
    
    return s_CRC;
}

/* ==============================================================
 * 协议命令分发
 * ============================================================== */
void CAN_Protocol_HandleCommand(const uint8_t *rx_data, uint8_t rx_len)
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
        case CAN_CMD_GET_SW_VERSION:
            CAN_Protocol_HandleGetSwVersion();
            break;
            
        case CAN_CMD_START_UPGRADE:
            CAN_Protocol_HandleStartUpgrade(rx_data, rx_len);
            break;
            
        case CAN_CMD_UPGRADE_DATA:
            /* App 不处理升级数据，由 Boot 处理 */
            break;
            
        case CAN_CMD_UPGRADE_END:
            /* App 不处理升级结束，由 Boot 处理 */
            break;
        case CAN_CMD_SET_CAN_BAUD:
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
        case CAN_CMD_SET_RS485_BAUD:
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
void CAN_Protocol_HandleGetSwVersion(void)
{
    const char *version = DevInfo0x8000.Sv;
    uint8_t version_len = (uint8_t)strlen(version);
    
    if (version_len == 0)
    {
        version_len = 1;
    }
    
    uint8_t total_frames = (version_len + CAN_FRAME_PAYLOAD_SIZE - 1) / CAN_FRAME_PAYLOAD_SIZE;
    if (total_frames == 0)
    {
        total_frames = 1;
    }
    
    for (uint8_t frame_num = 0; frame_num < total_frames; frame_num++)
    {
        uint8_t tx_data[8];
        
        tx_data[0] = CAN_CMD_GET_SW_VERSION;
        
        uint8_t is_tail = (frame_num == total_frames - 1) ? 1 : 0;
        tx_data[1] = (is_tail << 7) | (frame_num & CAN_FRAME_INFO_NUM_MASK);
        
        uint8_t offset = frame_num * CAN_FRAME_PAYLOAD_SIZE;
        for (uint8_t i = 0; i < CAN_FRAME_PAYLOAD_SIZE; i++)
        {
            if (offset + i < version_len)
            {
                tx_data[2 + i] = (uint8_t)version[offset + i];
            }
            else
            {
                tx_data[2 + i] = 0x00;
            }
        }
        
        BSP_CAN_Transmit(tx_data, 8);
    }
}

/* ==============================================================
 * 0xF1 命令 - 开始升级 (App 版本)
 * 
 * App 职责:
 *   1. 解析请求中的密码
 *   2. 计算正确密码并比较
 *   3. 密码错误: 返回状态=0 + 正确密码
 *   4. 密码正确: 返回状态=1 (App进入升级)，然后跳转到 Boot
 * ============================================================== */
void CAN_Protocol_HandleStartUpgrade(const uint8_t *rx_data, uint8_t rx_len)
{
    if (rx_len < 7)
    {
        return;
    }
    
    /* 解析请求 (小端) */
    /* uint32_t data_len = (uint32_t)rx_data[1] | 
                        ((uint32_t)rx_data[2] << 8) | 
                        ((uint32_t)rx_data[3] << 16); */
    uint16_t recv_password = (uint16_t)rx_data[4] | ((uint16_t)rx_data[5] << 8);
    
    /* 帧长度: 主机发送1字节，这里强制转换为 uint16_t */
    uint16_t recv_frame_len = (uint16_t)rx_data[6]; 
    (void)recv_frame_len;  /* 暂不使用 */
    
    /* 获取设备ID并计算正确密码 */
    uint8_t dev_id = BSP_CAN_ParseDevID(DevInfo0x8000.DevID);
    uint16_t correct_password = CAN_Protocol_CalcPassword(dev_id);
    
    /* 准备应答 */
    uint8_t tx_data[8] = {0};
    tx_data[0] = CAN_CMD_START_UPGRADE;
    
    /* 密码 (小端) */
    tx_data[2] = (uint8_t)(correct_password & 0xFF);
    tx_data[3] = (uint8_t)((correct_password >> 8) & 0xFF);
    
    /* 设备支持的帧长度 (小端) - 改为 1 字节以匹配 5 字节总长 */
    tx_data[4] = (uint8_t)(CAN_MAX_FRAME_LENGTH & 0xFF);
    /* tx_data[5] = (uint8_t)((CAN_MAX_FRAME_LENGTH >> 8) & 0xFF); 高字节不需要发 */
    
    if (recv_password != correct_password)
    {
        /* 密码错误 */
        tx_data[1] = UPGRADE_STATUS_PASSWORD_FAIL;
        BSP_CAN_Transmit(tx_data, 5);  /* 发送 5 字节 */
    }
    else
    {
        /* 密码正确，从 App 进入升级模式 */
        tx_data[1] = UPGRADE_STATUS_APP_TO_BOOT;
        BSP_CAN_Transmit(tx_data, 5);  /* 发送 5 字节 */
        
        /* 等待发送完成 */
        for (volatile int i = 0; i < 50000; i++);
        
        /* 跳转到 Bootloader */
        Handle_Bootstrap_Request();
    }
}

/* ==============================================================
 * 0xF2/0xF3 命令 - App 不处理
 * ============================================================== */
void CAN_Protocol_HandleUpgradeData(const uint8_t *rx_data, uint8_t rx_len)
{
    (void)rx_data;
    (void)rx_len;
    /* App 不处理 */
}

void CAN_Protocol_HandleUpgradeEnd(const uint8_t *rx_data, uint8_t rx_len)
{
    (void)rx_data;
    (void)rx_len;
    /* App 不处理 */
}
