/* ============================================================================
 * 文件名: can_protocol.c
 * 描述: CAN 协议处理实现 (Boot 版本)
 * 
 * 参考 User_FoE_Impl.c 的 Flash 操作逻辑
 * ============================================================================ */
#include "can_protocol.h"
#include "bsp_can.h"
#include "shared_conf.h"
#include "Dave/Generated/E_EEPROM_XMC4/e_eeprom_xmc4.h"
#include <string.h>
#include <xmc_flash.h>
#include "ecat_callback.h"

/* 升级上下文 (全局) */
UpgradeContext_t g_UpgradeCtx = {0};

/* Flash 写入 (参考 FoE 实现) */
static uint32_t s_FlashWriteAddr;       /* 当前 Flash 写入地址 */
static uint8_t  s_PageBuf[256] __attribute__((aligned(4)));
static uint16_t s_PageBufOffset;


/* ==============================================================
 * 密码计算 (基于设备ID)
 * ============================================================== */
uint16_t CAN_Protocol_CalcPassword(uint16_t dev_id)
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

uint32_t CAN_Protocol_CalcCRC32(const uint8_t *data, uint32_t len)
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
void CAN_Protocol_HandleCommand(const uint8_t *rx_data, uint8_t rx_len)
{
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
            CAN_Protocol_HandleUpgradeData(rx_data, rx_len);
            break;
            
        case CAN_CMD_UPGRADE_END:
            CAN_Protocol_HandleUpgradeEnd(rx_data, rx_len);
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
    LoadDeviceConfig();
    
    const char *version = s_DevConfigValid ? s_DevConfig.Sv : "Boot V1.0";
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
 * 0xF1 命令 - 开始升级 (Boot 版本)
 * ============================================================== */
void CAN_Protocol_HandleStartUpgrade(const uint8_t *rx_data, uint8_t rx_len)
{
    if (rx_len < 7)
    {
        return;
    }
    
    LoadDeviceConfig();
    
    /* 解析请求 (小端) */
    uint32_t data_len = (uint32_t)rx_data[1] | 
                        ((uint32_t)rx_data[2] << 8) | 
                        ((uint32_t)rx_data[3] << 16);
    uint16_t recv_password = (uint16_t)rx_data[4] | ((uint16_t)rx_data[5] << 8);
    
    /* 获取设备ID并计算正确密码 */
    uint16_t dev_id = s_DevConfigValid ? 
                     BSP_CAN_ParseDevID(s_DevConfig.DevID) : CAN_DEFAULT_DEV_ID;
    uint16_t correct_password = CAN_Protocol_CalcPassword(dev_id);
    
    /* 准备应答 */
    uint8_t tx_data[8] = {0};
    tx_data[0] = CAN_CMD_START_UPGRADE;
    
    /* 密码 (小端) */
    tx_data[2] = (uint8_t)(correct_password & 0xFF);
    tx_data[3] = (uint8_t)((correct_password >> 8) & 0xFF);
    
    /* 设备支持的帧长度 (小端) */
    tx_data[4] = (uint8_t)(CAN_MAX_FRAME_LENGTH & 0xFF);
    /* tx_data[5] = (uint8_t)((CAN_MAX_FRAME_LENGTH >> 8) & 0xFF); 高字节不需要发 */
    
    if (recv_password != correct_password)
    {
        /* 密码错误 */
        tx_data[1] = UPGRADE_STATUS_PASSWORD_FAIL;
        BSP_CAN_Transmit(tx_data, 5); /* 发送 5 字节 */
        return;
    }
    
    /* 密码正确，检查空间 */
    if (data_len > APP_MAX_SIZE)
    {
        tx_data[1] = UPGRADE_STATUS_NO_MEMORY;
        BSP_CAN_Transmit(tx_data, 5); /* 发送 5 字节 */
        return;
    }
    
    /* 发送"擦除开始"状态 */
    tx_data[1] = UPGRADE_STATUS_ERASE_START;
    BSP_CAN_Transmit(tx_data, 5); /* 发送 5 字节 */
    
    /* 初始化 Flash 写入状态 (参考 FoE) */
    s_FlashWriteAddr = APP_START_ADDR;
    s_PageBufOffset = 0;
    memset(s_PageBuf, 0xFF, 256);
    
    /* 擦除 Flash */
    if (Flash_EraseAppArea())
    {
        tx_data[1] = UPGRADE_STATUS_NO_MEMORY;
        BSP_CAN_Transmit(tx_data, 5); /* 发送 5 字节 */
        return;
    }
    
    /* 初始化升级上下文 */
    g_UpgradeCtx.state = UPGRADE_STATE_READY;
    g_UpgradeCtx.total_size = data_len;
    g_UpgradeCtx.received_size = 0;
    g_UpgradeCtx.next_addr = 0;
    CRC_Reset();
    
    /* 发送"擦除完成"状态 */
    tx_data[1] = UPGRADE_STATUS_ERASE_OK;
    BSP_CAN_Transmit(tx_data, 5); /* 发送 5 字节 */
}

/* ==============================================================
 * 0xF2 命令 - 升级数据
 * 
 * 注意: 数据按顺序接收，地址用于校验（可选）
 * ============================================================== */
void CAN_Protocol_HandleUpgradeData(const uint8_t *rx_data, uint8_t rx_len)
{
    if (rx_len < 5)
    {
        return;
    }
    
    if (g_UpgradeCtx.state != UPGRADE_STATE_READY && 
        g_UpgradeCtx.state != UPGRADE_STATE_RECEIVING)
    {
        return;
    }
    
    g_UpgradeCtx.state = UPGRADE_STATE_RECEIVING;
    
    /* 解析地址 (小端) - 用于校验 */
    uint32_t addr = (uint32_t)rx_data[1] | 
                    ((uint32_t)rx_data[2] << 8) | 
                    ((uint32_t)rx_data[3] << 16);
    
    /* 验证地址是否符合预期 */
    if (addr != g_UpgradeCtx.next_addr)
    {
        /* 地址不连续，可能丢包，但继续处理 */
    }
    
    /* 数据长度 = 总帧长 - 4 (命令 + 地址) */
    uint8_t data_len = rx_len - 4;
    const uint8_t *data = &rx_data[4];
    
    /* 写入数据到页缓冲区 (同时更新 CRC) */
    BufferData(data, data_len);
    
    /* 更新进度 */
    g_UpgradeCtx.received_size += data_len;
    g_UpgradeCtx.next_addr = addr + data_len;
    
    /* 发送应答 (只发 4 字节: CMD + 3字节地址) */
    uint8_t tx_data[4];
    tx_data[0] = CAN_CMD_UPGRADE_DATA;
    tx_data[1] = (uint8_t)(g_UpgradeCtx.next_addr & 0xFF);
    tx_data[2] = (uint8_t)((g_UpgradeCtx.next_addr >> 8) & 0xFF);
    tx_data[3] = (uint8_t)((g_UpgradeCtx.next_addr >> 16) & 0xFF);
    
    BSP_CAN_Transmit(tx_data, 4);
}

/* ==============================================================
 * 0xF3 命令 - 升级结束
 * ============================================================== */
void CAN_Protocol_HandleUpgradeEnd(const uint8_t *rx_data, uint8_t rx_len)
{
    if (rx_len < 5)
    {
        return;
    }
    
    if (g_UpgradeCtx.state != UPGRADE_STATE_RECEIVING)
    {
        return;
    }
    
    /* 刷新最后一页 */
    FlushPageBuffer();
    
    g_UpgradeCtx.state = UPGRADE_STATE_VERIFYING;
    
    /* 解析主机发送的 CRC32 (小端) */
    uint32_t host_crc = (uint32_t)rx_data[1] | 
                        ((uint32_t)rx_data[2] << 8) | 
                        ((uint32_t)rx_data[3] << 16) |
                        ((uint32_t)rx_data[4] << 24);
    
    /* 从 Flash 读取数据并计算 CRC32 (按 32 位字处理，与上位机一致) */
    CRC_Reset();
    uint32_t word_count = g_UpgradeCtx.received_size / 4;
    const uint32_t *flash_data = (const uint32_t *)APP_START_ADDR;
    for (uint32_t i = 0; i < word_count; i++)
    {
        CRC_UpdateWord(flash_data[i]);
    }
    uint32_t device_crc = CRC_GetValue();
    
    /* 准备应答 */
    uint8_t tx_data[8] = {0};
    tx_data[0] = CAN_CMD_UPGRADE_END;
    
    /* CRC32 (小端) */
    tx_data[2] = (uint8_t)(device_crc & 0xFF);
    tx_data[3] = (uint8_t)((device_crc >> 8) & 0xFF);
    tx_data[4] = (uint8_t)((device_crc >> 16) & 0xFF);
    tx_data[5] = (uint8_t)((device_crc >> 24) & 0xFF);
    
    if (host_crc != device_crc)
    {
        /* CRC 校验失败 */
        tx_data[1] = UPGRADE_RESULT_CRC_FAIL;
        BSP_CAN_Transmit(tx_data, 6);
        
        g_UpgradeCtx.state = UPGRADE_STATE_IDLE;
        return;
    }
    
    /* 更新 SharedConfig (参考 FoE) */
    SharedConfig_t cfg;
    cfg.MagicHeader = SHARED_MAGIC_VALID;
    cfg.AppActualSize = g_UpgradeCtx.received_size;
    cfg.AppCRC32 = device_crc;
    cfg.Checksum = SharedConfig_CalcChecksum(&cfg);
    
    E_EEPROM_XMC4_WriteArray(SHARED_CONFIG_EEPROM_OFFSET, 
                             (uint8_t*)&cfg, sizeof(SharedConfig_t));
    E_EEPROM_XMC4_UpdateFlashContents();
    
    /* 升级成功 */
    tx_data[1] = UPGRADE_RESULT_SUCCESS;
    BSP_CAN_Transmit(tx_data, 6);
    
    g_UpgradeCtx.state = UPGRADE_STATE_DONE;
    
    /* 等待发送完成后重启 */
    for (volatile int i = 0; i < 100000; i++);
    
    NVIC_SystemReset();
}
