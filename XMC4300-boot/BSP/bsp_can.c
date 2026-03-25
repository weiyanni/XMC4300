/* ============================================================================
 * 文件名: bsp_can.c
 * 描述: CAN 通信控制实现
 * 
 * 硬件逻辑:
 *   - CAN_STB: LOW=正常模式, HIGH=待机模式
 *   - CTRLB_H_A + CTRLA_L_B: 都HIGH=CAN模式
 *   - CTRL_120R: LOW=开启终端电阻, HIGH=关闭终端电阻
 *   - 默认: CAN模式 + 终端电阻开启
 * ============================================================================ */
#include "DAVE.h"
#include "bsp_can.h"
#include "can_protocol.h"

/* DAVE 生成的 IO 对象 */
extern const DIGITAL_IO_t CAN_STB;
extern const DIGITAL_IO_t CTRLB_H_A;
extern const DIGITAL_IO_t CTRLA_L_B;
extern const DIGITAL_IO_t CTRL_120R;

void BSP_CAN_Init(void)
{
    /* 切换到 CAN 模式 */
    BSP_CAN_ModeSelect();
    
    /* 正常模式 (非待机) */
    BSP_CAN_Normal();
    
    /* CAN 模式下开启终端电阻 */
    BSP_120R_Enable();
    
    /* 注意: 中断配置由 BSP_CAN_ApplyConfig() 统一处理 */
}


void BSP_CAN_ModeSelect(void)
{
    /* CAN 模式: CTRLB_H_A=HIGH, CTRLA_L_B=HIGH */
    DIGITAL_IO_SetOutputHigh(&CTRLB_H_A);
    DIGITAL_IO_SetOutputHigh(&CTRLA_L_B);
}

void BSP_CAN_Normal(void)
{
    DIGITAL_IO_SetOutputLow(&CAN_STB);   /* STB LOW = 正常模式 */
}

void BSP_CAN_Standby(void)
{
    DIGITAL_IO_SetOutputHigh(&CAN_STB);  /* STB HIGH = 待机模式 */
}

void BSP_120R_Enable(void)
{
    DIGITAL_IO_SetOutputLow(&CTRL_120R);   /* LOW = 开启终端电阻 */
}

void BSP_120R_Disable(void)
{
    DIGITAL_IO_SetOutputHigh(&CTRL_120R);  /* HIGH = 关闭终端电阻 */
}

/* ==============================================================
 * CAN ID 和接收过滤器配置
 * 
 * 消息对象布局 (根据 DAVE 配置):
 *   - LMO_01 (lmobj_ptr[0]): 发送 MO (TRANSMSGOBJ)
 *   - LMO_02 (lmobj_ptr[1]): 接收 MO (RECMSGOBJ, rx_event_enable=true)
 * ============================================================== */

/* 外部 DAVE 对象 */
extern const CAN_NODE_t CAN_NODE_0;

uint16_t BSP_CAN_ParseDevID(const char *dev_id_str)
{
    uint16_t dev_id = 254;  /* 默认值 */
    
    if (dev_id_str != NULL && dev_id_str[0] != '\0')
    {
        int val = 0;
        for (int i = 0; i < 6 && dev_id_str[i] >= '0' && dev_id_str[i] <= '9'; i++)
        {
            val = val * 10 + (dev_id_str[i] - '0');
        }
        /* 发送 ID 范围: 0-512 */
        if (val >= 0 && val <= 512)
        {
            dev_id = (uint16_t)val;
        }
    }
    
    return dev_id;
}

void BSP_CAN_SetTxID(uint32_t tx_id)
{
    /* 更新发送 MO 的 ID (LMO_01 = lmobj_ptr[0]) */
    if (CAN_NODE_0.lmobj_ptr[0] != NULL)
    {
        CAN_NODE_MO_UpdateID(CAN_NODE_0.lmobj_ptr[0], tx_id);
    }
}

void BSP_CAN_SetRxFilter(uint8_t max_id)
{
    /*
     * 接收过滤器配置:
     * 标准 CAN ID 是 11 位 (0x000 - 0x7FF)
     * 要接收 0 - 255: 掩码 = 0x700 (只检查高3位是否为0)
     */
    (void)max_id;  /* 硬件过滤 0-255 */
    
    /* 接收 MO 是 LMO_02 = lmobj_ptr[1] */
    if (CAN_NODE_0.lmobj_ptr[1] != NULL)
    {
        CAN_NODE_MO_UpdateID(CAN_NODE_0.lmobj_ptr[1], 0x000);
        XMC_CAN_MO_SetAcceptanceMask(CAN_NODE_0.lmobj_ptr[1]->mo_ptr, 0x700);
    }
}

void BSP_CAN_ConfigID(uint16_t dev_id)
{
    /* 发送 ID = DevID + 256 */
    BSP_CAN_SetTxID((uint32_t)(dev_id + 256));
    /* 接收过滤器保持 0-254 */
    BSP_CAN_SetRxFilter(254);
}

void BSP_CAN_SetBaudrate(uint32_t baudrate)
{
    if (baudrate >= 5000 && baudrate <= 1000000)
    {
        CAN_NODE_ConfigBaudrate(&CAN_NODE_0, baudrate, 
                                CAN_DEFAULT_SAMPLE_POINT, CAN_DEFAULT_SJW);
    }
}

/* ==============================================================
 * CAN 收发功能
 * ============================================================== */

uint8_t BSP_CAN_Transmit(const uint8_t *data, uint8_t len)
{
    if (data == NULL || len == 0 || len > 8)
    {
        return 1;
    }
    
    /* 使用发送 MO (LMO_01 = lmobj_ptr[0]) */
    CAN_NODE_LMO_t *tx_mo = CAN_NODE_0.lmobj_ptr[0];
    if (tx_mo == NULL)
    {
        return 1;
    }
    
    /* 更新数据长度 */
    tx_mo->mo_ptr->can_data_length = len;
    
    /* 更新数据并发送 */
    CAN_NODE_MO_UpdateData(tx_mo, (uint8_t*)data);
    CAN_NODE_MO_Transmit(tx_mo);
    
    return 0;
}

void BSP_CAN_RxHandler(void)
{
    /* 获取接收 MO (LMO_02 = lmobj_ptr[1]) */
    CAN_NODE_LMO_t *rx_mo = CAN_NODE_0.lmobj_ptr[1];
    if (rx_mo == NULL)
    {
        return;
    }
    
    /* 检查是否有接收数据 */
    uint32_t status = CAN_NODE_MO_GetStatus(rx_mo);
    if (!(status & XMC_CAN_MO_STATUS_RX_PENDING))
    {
        return;
    }
    
    /* 清除接收标志 */
    CAN_NODE_MO_ClearStatus(rx_mo, XMC_CAN_MO_RESET_STATUS_RX_PENDING);
    
    /* 读取接收数据 */
    CAN_NODE_MO_Receive(rx_mo);
    
    /* 获取接收到的数据 */
    uint8_t rx_data[8];
    rx_data[0] = (uint8_t)(rx_mo->mo_ptr->can_data[0] & 0xFF);
    rx_data[1] = (uint8_t)((rx_mo->mo_ptr->can_data[0] >> 8) & 0xFF);
    rx_data[2] = (uint8_t)((rx_mo->mo_ptr->can_data[0] >> 16) & 0xFF);
    rx_data[3] = (uint8_t)((rx_mo->mo_ptr->can_data[0] >> 24) & 0xFF);
    rx_data[4] = (uint8_t)(rx_mo->mo_ptr->can_data[1] & 0xFF);
    rx_data[5] = (uint8_t)((rx_mo->mo_ptr->can_data[1] >> 8) & 0xFF);
    rx_data[6] = (uint8_t)((rx_mo->mo_ptr->can_data[1] >> 16) & 0xFF);
    rx_data[7] = (uint8_t)((rx_mo->mo_ptr->can_data[1] >> 24) & 0xFF);
    
    /* 获取数据长度 */
    uint8_t rx_len = rx_mo->mo_ptr->can_data_length;
    
    /* 调用协议处理模块 */
    CAN_Protocol_HandleCommand(rx_data, rx_len);
}

/* ==============================================================
 * CAN 接收中断处理函数
 * LMO_02 配置了 rx_sr=0，对应 CAN0_0_IRQHandler
 * ============================================================== */
void CAN0_0_IRQHandler(void)
{
    BSP_CAN_RxHandler();
}

/* ==============================================================
 * CAN 统一配置接口
 * ============================================================== */
void BSP_CAN_ApplyConfig(const CAN_Config_t *cfg)
{
    if (cfg == NULL)
    {
        return;
    }
    
    /* 1. 配置波特率 */
    if (cfg->baudrate >= 5000 && cfg->baudrate <= 1000000)
    {
        BSP_CAN_SetBaudrate(cfg->baudrate);
    }
    
    /* 2. 配置设备 ID 和接收过滤器 */
    BSP_CAN_ConfigID(cfg->dev_id);
    
    /* 3. 配置终端电阻 */
    if (cfg->use_120r)
    {
        BSP_120R_Enable();
    }
    else
    {
        BSP_120R_Disable();
    }
    
    /* 4. 启用 CAN 接收中断 */
    NVIC_SetPriority(CAN0_0_IRQn, CAN_IRQ_PRIORITY);
    NVIC_EnableIRQ(CAN0_0_IRQn);
}
