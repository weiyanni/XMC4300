/**
 * @file    app.c
 * @brief   应用层实现
 * @details 本文件实现了应用层的核心业务逻辑，包括：
 *          - TMC6460 电机驱动芯片初始化
 *          - EtherCAT PDO 数据处理（LED 控制、状态反馈）
 * 
 *          数据流向：
 *          ┌─────────┐    RxPDO (0x7000)    ┌─────────┐
 *          │  主站   │ ──────────────────> │  从站   │
 *          │ Master  │ <────────────────── │  Slave  │
 *          └─────────┘    TxPDO (0x6000)    └─────────┘
 * 
 * @author  
 * @date    2024
 * @version 1.0
 */

#include "app.h"
#include "DAVE.h"
#include <string.h>
#include "shared_conf.h"

/*============================================================================*/
/* 全局变量定义                                                               */
/*============================================================================*/

volatile uint32_t g_chip_id = 0;          /**< TMC6460 芯片 ID */
volatile bool g_init_success = false;     /**< 初始化成功标志 */
uint32_t g_led_active_counter = 0;        /**< LED 翻转计数器 */

/*============================================================================*/
/* 私有变量                                                                   */
/*============================================================================*/

static uint8_t s_prev_led_state = 0xFF;   /**< LED 上一次状态 (0xFF=未初始化) */
extern uint32_t __app_code_end;
#define APP_START_ADDR_RD 0x08020000UL

/*============================================================================*/
/* 私有宏定义                                                                 */
/*============================================================================*/

#define TMC6460_CHIP_ID         0x00            /**< TMC6460 芯片 ID 寄存器地址 */
#define TMC6460_CHIP_ID_VALUE   0x36343630      /**< TMC6460 预期芯片 ID 值 "6460" */

/*============================================================================*/
/* 私有函数                                                                   */
/*============================================================================*/


/**
 * @brief   简单延时函数
 * @details 阻塞式延时，仅用于初始化阶段，主循环中禁止使用！
 * 
 * @param[in] us  延时微秒数（近似值）
 */
static void Delay_us(uint32_t us)
{
    volatile uint32_t count = us * 50;
    while (count--);
}


/*============================================================================*/
/* 公共函数实现                                                               */
/*============================================================================*/

//#include "param_storage.h"

bool APP_HardwareInit(void)
{
    /* 等待 TMC6460 上电稳定 */
    Delay_us(4000);
    
    /* 读取芯片 ID 验证 SPI 通信 */
    g_chip_id = TMC6460_SPI_Read(TMC6460_CHIP_ID);
    g_init_success = (g_chip_id == TMC6460_CHIP_ID_VALUE);
    
    /* 如果读回 0，设置错误码便于调试 */
    if (!g_init_success && g_chip_id == 0) {
        g_chip_id = 0xFFFFFFFF;
    }
    
    return g_init_success;
}


/**
 * @brief   EtherCAT 应用层数据处理
 * @details PDO 数据结构（6 通道，每通道 26 字节）：
 * 
 *          RxPDO 0x7000 (主站->从站):
 *          ┌────────────┬──────────────┐
 *          │ Txlen_n    │ TxData_n[24] │  × 6 通道
 *          │ (2 bytes)  │ (24 bytes)   │
 *          └────────────┴──────────────┘
 * 
 *          TxPDO 0x6000 (从站->主站):
 *          ┌────────────┬──────────────┐
 *          │ Rxlen_n    │ RxData_n[24] │  × 6 通道
 *          │ (2 bytes)  │ (24 bytes)   │
 *          └────────────┴──────────────┘
 * 
 *          当前实现：
 *          - 通道1 输入：LED 控制命令 (0x01=开, 其他=关)
 *          - 通道1 输出：TMC6460 芯片 ID (大端)
 *          - 通道2 输出：LED 翻转计数器 (小端)
 */
void APP_EtherCATProcess(void *pRxData, void *pTxData)
{
    /*
     * PDO 通道数据结构定义
     * 与 ryhandObjects.h 中的 TOBJ6000/TOBJ7000 结构对应
     */
    typedef struct {
        uint16_t len;       /**< 有效数据长度 */
        char data[24];      /**< 数据缓冲区 */
    } PdoChannel_t;
    
    typedef struct {
        uint16_t subIndex0; /**< 子索引数量 (固定为12) */
        PdoChannel_t ch[6]; /**< 6 个数据通道 */
    } PdoData_t;
    
    PdoData_t *pRx = (PdoData_t *)pRxData;  /* 主站->从站 */
    PdoData_t *pTx = (PdoData_t *)pTxData;  /* 从站->主站 */
    
    if (pRx == NULL || pTx == NULL) return;

    /* ===================================================================== */
    /* 1. 处理输入命令 (Master -> Slave)                                     */
    /* ===================================================================== */
    
    /*
     * 通道1：LED 控制命令
     * - len > 0 且 data[0] == 0x01：点亮 LED
     * - 其他情况：熄灭 LED
     * - 仅在状态变化时执行硬件操作，避免频繁 IO
     */
    uint16_t cmdLen = pRx->ch[0].len;
    uint8_t *pCmdData = (uint8_t *)pRx->ch[0].data;
    uint8_t current_led_state = (cmdLen > 0 && pCmdData[0] == 0x01) ? 1 : 0;

    if (current_led_state != s_prev_led_state)
    {
        /* 执行 LED 硬件操作 (低电平点亮) */
        if (current_led_state == 1) {
            DIGITAL_IO_SetOutputLow(&LED);
        } else {
            DIGITAL_IO_SetOutputHigh(&LED);
        }
        
        /* 状态变化计数 */
        g_led_active_counter++;
        s_prev_led_state = current_led_state;
    }

    /* ===================================================================== */
    /* 2. 准备输出数据 (Slave -> Master)                                     */
    /* ===================================================================== */
    
    /*
     * 通道1：TMC6460 芯片 ID
     * - 长度：4 字节
     * - 格式：大端 (MSB first)
     * - 正常值：0x36343630 ("6460")
     * - 错误值：0xFFFFFFFF (初始化失败)
     */
    pTx->ch[0].len = 4;
    uint8_t *pData1 = (uint8_t *)pTx->ch[0].data;
    uint32_t val_id = g_chip_id;
    pData1[0] = (uint8_t)((val_id >> 24) & 0xFF);
    pData1[1] = (uint8_t)((val_id >> 16) & 0xFF);
    pData1[2] = (uint8_t)((val_id >> 8)  & 0xFF);
    pData1[3] = (uint8_t)((val_id)       & 0xFF);

    /*
     * 通道2：LED 翻转计数器
     * - 长度：4 字节
     * - 格式：小端 (LSB first)
     * - 每次 LED 状态变化时 +1
     */
    pTx->ch[1].len = 4;
    uint8_t *pData2 = (uint8_t *)pTx->ch[1].data;
    uint32_t val_cnt = g_led_active_counter;
    pData2[0] = (uint8_t)((val_cnt)       & 0xFF);
    pData2[1] = (uint8_t)((val_cnt >> 8)  & 0xFF);
    pData2[2] = (uint8_t)((val_cnt >> 16) & 0xFF);
    pData2[3] = (uint8_t)((val_cnt >> 24) & 0xFF);
}

/*============================================================================*/
/* 参数操作处理                                                               */
/*============================================================================*/

/** 参数操作码定义 (0x9001 ParamAction) */
#define PARAM_ACTION_SAVE           0x0001  /**< 保存参数到 Flash */
#define PARAM_ACTION_REBOOT         0x00FF  /**< 软件重启 */
#define PARAM_ACTION_SUCCESS_MASK   0x8000  /**< 操作成功标志 */

uint8_t APP_HandleParamAction(uint16_t action, uint16_t *pResult)
{
    uint16_t result = action;  /* 默认保持原值 */
    
    switch (action)
    {
        case PARAM_ACTION_SAVE:
            /* 从对象字典同步到 RAM，再写入 Flash */
            PARAM_SyncFromObjDict();
            if (PARAM_Save()) {
                result = PARAM_ACTION_SUCCESS_MASK | action;  /* 0x8001 */
            }
            break;
            
        case PARAM_ACTION_REBOOT:
            /* 先标记成功，再重启 */
            result = PARAM_ACTION_SUCCESS_MASK | action;  /* 0x80FF */
            *pResult = result;
            /* 软件重启 */
            NVIC_SystemReset();
            /* 不会执行到这里 */
            break;
            
        default:
            /* 其它指令不操作，值保持写入值 */
            break;
    }
    
    *pResult = result;
    return 0;
}

/* ==============================================================
 * CRC32 计算 (与上位机/CAN 协议一致的算法)
 *
 * 特点: 按 32 位字处理，多项式 0x04C11DB7
 * ============================================================== */
uint32_t Calc_App_Flash_CRC32(uint32_t dataSize)
{
    uint32_t crc = 0xFFFFFFFF;
    const uint32_t CRC_POLY = 0x04C11DB7;
    const uint32_t *pData = (const uint32_t *)APP_START_ADDR_RD;

    if (dataSize > APP_MAX_SIZE) {
        dataSize = APP_MAX_SIZE;
    }

    /* 按 32 位字处理 (与上位机 CalCRC 函数一致) */
    uint32_t wordCount = dataSize / 4;
    for (uint32_t i = 0; i < wordCount; i++)
    {
        uint32_t data = pData[i];
        uint32_t xbit = 1U << 31;

        for (int bits = 0; bits < 32; bits++)
        {
            if (crc & 0x80000000)
            {
                crc <<= 1;
                crc ^= CRC_POLY;
            }
            else
            {
                crc <<= 1;
            }
            if (data & xbit)
            {
                crc ^= CRC_POLY;
            }
            xbit >>= 1;
        }
    }

    return crc;
}

void App_Self_Identity_Registration(void)
{
    SharedConfig_t cfg;
    E_EEPROM_XMC4_ReadArray(SHARED_CONFIG_EEPROM_OFFSET, (uint8_t*)&cfg, sizeof(SharedConfig_t));

    /* 如果当前不是有效状态，说明是烧录 HEX 后第一次运行 */
    if (cfg.MagicHeader != SHARED_MAGIC_VALID)
    {
        /* 计算实际物理长度 */
        uint32_t actualSize = (uint32_t)&__app_code_end - APP_START_ADDR_RD;

        /* 填充身份证信息 */
        cfg.MagicHeader = SHARED_MAGIC_VALID;
        cfg.AppActualSize = actualSize;

        /* 计算 CRC32：范围从起始地址到实际结束位置 */
        cfg.AppCRC32 = Calc_App_Flash_CRC32(actualSize);

        /* 写入 EEPROM */
        cfg.Checksum = SharedConfig_CalcChecksum(&cfg);
        E_EEPROM_XMC4_WriteArray(SHARED_CONFIG_EEPROM_OFFSET, (uint8_t*)&cfg, sizeof(SharedConfig_t));
        E_EEPROM_XMC4_UpdateFlashContents();
    }
}

