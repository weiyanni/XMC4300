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
#include "dev_config.h"
#include "SSC/Src/ryhandObjects.h"

EcatRxMap_t g_RxConfigTable[] = {
    // 规则1: 0x200~0x2FF -> 0x6000
    // 限制: 只有当 pData[0] == 0xA1 时才转发 (比如 0xA1 代表 "Write Data")
	{ 0x01, &Txpdo160x6000, 16, 0},//{ 0x01, &Txpdo160x6000, 16, CMD_LIST( 0xA1, 0xA2, 0xFF )},

    // 规则2: 0x300~0x35F -> 0x6010
    // 限制: 无 (enableFilter = 0)，收到任何数据都转发
    { 0x017, &Txpdo60x6010,  6,  0 },

    // 规则3: 0x400~0x45F -> 0x6020
    // 限制: 只有当 pData[0] == 0xB6 时才转发
    { 0x01, &Txpdo_TS0x6020, 6, CMD_LIST( 0xB6 )}
};

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
 * CAN 波特率切换实现
 * ============================================================== */

/* 外部 DAVE 对象 */
extern const CAN_NODE_t CAN_NODE_0;

/* 外部 SSC 对象 */
extern TOBJ8000 DevInfo0x8000;

/* 支持的 CAN 波特率表 (16种) - 导出供外部使用 */
const uint32_t CAN_SUPPORTED_BAUDRATES[] = {
    5000, 10000, 20000, 25000, 40000, 50000, 62500, 80000,
    100000, 125000, 200000, 250000, 400000, 500000, 800000, 1000000
};
const uint8_t CAN_BAUDRATE_COUNT = sizeof(CAN_SUPPORTED_BAUDRATES) / sizeof(CAN_SUPPORTED_BAUDRATES[0]);

uint8_t BSP_CAN_IsBaudrateValid(uint32_t baudrate)
{
    for (uint8_t i = 0; i < CAN_BAUDRATE_COUNT; i++)
    {
        if (CAN_SUPPORTED_BAUDRATES[i] == baudrate)
        {
            return 1;
        }
    }
    return 0;
}

BSP_CAN_STATUS_t BSP_CAN_SetBaudrate(uint32_t baudrate, uint8_t save_to_eeprom)
{
    /* 验证波特率 */
    if (!BSP_CAN_IsBaudrateValid(baudrate))
    {
        return BSP_CAN_ERROR_INVALID_BAUDRATE;
    }
    
    /* 配置硬件波特率 */
    CAN_NODE_ConfigBaudrate(&CAN_NODE_0, baudrate, 
                            CAN_DEFAULT_SAMPLE_POINT, CAN_DEFAULT_SJW);
    
    /* 更新 SSC 对象 (控制域和数据域使用相同波特率) */
    DevInfo0x8000.BR_canfd_ctrl = baudrate;
    DevInfo0x8000.BR_canfd_data = baudrate;
    
    /* 保存到 EEPROM */
    if (save_to_eeprom)
    {
        if (DevConfig_SaveToEEPROM() != 0)
        {
            return BSP_CAN_ERROR_HW_FAILED;
        }
    }
    
    return BSP_CAN_OK;
}

BSP_CAN_STATUS_t BSP_CANFD_UpdateBaudrate(uint32_t baudrate_ctrl,uint32_t baudrate_data)
{
    /* 验证波特率 */
    if (!BSP_CAN_IsBaudrateValid(baudrate_data))
    {
        return BSP_CAN_ERROR_INVALID_BAUDRATE;
    }

    /* 配置硬件波特率 */
    CAN_NODE_ConfigBaudrate(&CAN_NODE_0, baudrate_data,
                            CAN_DEFAULT_SAMPLE_POINT, CAN_DEFAULT_SJW);
}

/* ==============================================================
 * 发送设置can波特率消息
 * ============================================================== */
void can_set_can_baud(uint32_t BR_canfd_ctrl,uint32_t BR_canfd_data)
{
	uint8_t tx_data[8] = {0};
	tx_data[0] = CAN_CMD_SET_CAN_BAUD;

	// --- 控制域波特率  ---
	tx_data[1] = (uint8_t)(BR_canfd_ctrl & 0xFF);         // 低位
	tx_data[2] = (uint8_t)((BR_canfd_ctrl >> 8) & 0xFF);  // 中位
	tx_data[3] = (uint8_t)((BR_canfd_ctrl >> 16) & 0xFF); // 高位

	// --- 数据域波特率  ---
	tx_data[4] = (uint8_t)(BR_canfd_data & 0xFF);         // 低位
	tx_data[5] = (uint8_t)((BR_canfd_data >> 8) & 0xFF);  // 中位
	tx_data[6] = (uint8_t)((BR_canfd_data >> 16) & 0xFF); // 高位

	for(int i = 1; i < 17; i += 3)
	{
		uint32_t start_tick = Get_Time_Ms(); // 假设你有一个获取毫秒计数的函数
		while (XMC_CAN_MO_GetStatus(CAN_NODE_0.lmobj_ptr[0]->mo_ptr) & XMC_CAN_MO_STATUS_TX_PENDING)
		{
		    if ((Get_Time_Ms() - start_tick) > 10) // 超过10ms则判定为超时
		    {
		        break;
		    }
		}
		CAN_NODE_MO_UpdateID(CAN_NODE_0.lmobj_ptr[0], i);
		BSP_CAN_Transmit(tx_data, 8);
	}

}

/* ==============================================================
 * 发送设置rs485波特率消息
 * ============================================================== */
void can_set_rs485_baud(uint32_t BR_485)
{
	uint8_t tx_data[8] = {0};
	tx_data[0] = CAN_CMD_SET_RS485_BAUD;

	//波特率
	tx_data[1] = (uint8_t)(BR_485 & 0xFF);
	tx_data[2] = (uint8_t)((BR_485 >> 8) & 0xFF);
	tx_data[3] = (uint8_t)((BR_485 >> 16) & 0xFF);
	tx_data[4] = (uint8_t)((BR_485 >> 24) & 0xFF);

	for(int i = 1; i < 17; i += 3)
	{
		uint32_t start_tick = Get_Time_Ms(); // 假设你有一个获取毫秒计数的函数
		while (XMC_CAN_MO_GetStatus(CAN_NODE_0.lmobj_ptr[0]->mo_ptr) & XMC_CAN_MO_STATUS_TX_PENDING)
		{
		    if ((Get_Time_Ms() - start_tick) > 10) // 超过10ms则判定为超时
		    {
		        break;
		    }
		}
		CAN_NODE_MO_UpdateID(CAN_NODE_0.lmobj_ptr[0], i);
		BSP_CAN_Transmit(tx_data, 5);
	}

}

/* ==============================================================
 * CAN ID 和接收过滤器配置
 * 
 * 消息对象布局 (根据 DAVE 配置):
 *   - LMO_01 (lmobj_ptr[0]): 发送 MO (TRANSMSGOBJ)
 *   - LMO_02 (lmobj_ptr[1]): 接收 MO (RECMSGOBJ, rx_event_enable=true)
 * 
 * ID 过滤策略:
 *   - 发送 ID = DevID (设备ID, 默认254)
 *   - 接收过滤: 只接收 0-254 (掩码 0x700, ID=0x000)
 * ============================================================== */

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
     * 
     * 标准 CAN ID 是 11 位 (0x000 - 0x7FF)
     * 
     * 要接收 0 - max_id (如 0-254):
     *   - 设置接收 MO 的 ID = 0x000
     *   - 设置掩码 = 0x700 (只检查高3位是否为0)
     *   
     * 这样可以接收 0x000 - 0x0FF (0-255) 范围的 ID
     * 如果需要精确到 254，需要软件层面过滤
     */
    (void)max_id;  /* 暂时不用精确值，硬件过滤 0-255 */
    
    /* 接收 MO 是 LMO_02 = lmobj_ptr[1] */
    if (CAN_NODE_0.lmobj_ptr[1] != NULL)
    {
        /* 设置接收 MO 的 ID 为 0 */
        CAN_NODE_MO_UpdateID(CAN_NODE_0.lmobj_ptr[1], 0x000);
        
        /* 设置接收掩码: 0x700 表示只检查 bit8-10 是否为 0 */
        /* 这样接收 0x000 - 0x0FF (0-255) */
        XMC_CAN_MO_SetAcceptanceMask(CAN_NODE_0.lmobj_ptr[1]->mo_ptr, 0x700);
    }
}

void BSP_CAN_ConfigID(uint16_t dev_id)
{
    /* 发送 ID = DevID + 256 */
    BSP_CAN_SetTxID((uint32_t)(dev_id + 256));
    
    /* 配置接收过滤器只接收 0-254 */
    BSP_CAN_SetRxFilter(254);
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

/**
 * @brief  CAN 接收数据处理函数
 *         根据 CAN ID 将数据填入对应的 EtherCAT TxPDO 通道
 *
 * @param  canId    接收到的 CAN ID
 * @param  pData    指向数据缓冲区的指针
 * @param  dlc      数据长度 (0-8)
 */
void Process_Can_Rx_Frame(uint32_t canId, uint8_t* pData, uint8_t dlc)
{
    for (int i = 0; i < RX_MAP_COUNT; i++)
    {
        EcatRxMap_t* pMap = &g_RxConfigTable[i];

        // 1. 检查 ID 范围
        if (canId >= pMap->baseCanId && canId <= (pMap->baseCanId + pMap->channelCount))
        {
            // 如果指针不为空，说明开启了过滤
            if (pMap->pCmdList != NULL && pMap->cmdListSize > 0)
            {
                // 没数据无法判断命令字，丢弃
                if (dlc == 0) return;

                uint8_t receivedCmd = pData[0];
                uint8_t isMatch = 0; // 0:未匹配, 1:匹配

                // 遍历允许的命令列表
                for (uint8_t k = 0; k < pMap->cmdListSize; k++)
                {
                    if (receivedCmd == pMap->pCmdList[k])
                    {
                        isMatch = 1;
                        break; // 找到了！退出内层循环
                    }
                }

                // 如果遍历完都没找到匹配的，说明是非法命令，直接丢弃
                if (isMatch == 0) return;
            }

            uint8_t channelIndex = canId - pMap->baseCanId;

			// --- 4. 寻址与拷贝 ---
			EcatChannel_t* pHead = (EcatChannel_t*)( (uint8_t*)pMap->pEcatObject + sizeof(UINT16) );
			EcatChannel_t* pTargetCh = &pHead[channelIndex];

			// 既然是直连，直接从头覆盖写入即可
			// 如果保留了命令字，直接 copy
			// 如果 dlc > 24 (虽然CAN只有8)，做个保护
			uint8_t copyLen = dlc;
			if (copyLen > 24) copyLen = 24;

			memcpy(pTargetCh->data, pData, copyLen);

			// 更新长度 (直接等于本次接收长度，或者保持最大长度，看需求)
			// 通常 CAN 直连模式下，收到多少就是多少
			pTargetCh->len = copyLen;

        }
    }
	return;
}

/**
 * @brief  通用的EtherCAT PDO到CAN的转发函数
 * @brief  此函数将一个内存块（PDO数据）通过CAN总线分包发送出去。
 *
 * @param  baseCanId  用于此数据序列的基础CAN ID。每个包的ID将是 baseCanId + 包序号。
 * @param  pData      指向要发送的数据源的指针 (void* 类型以保证通用性)。
 * @param  dataSize   要发送数据的总字节数。
 *
 * @return EcatCanFwdStatus_t  返回函数执行的状态。
 */
EcatCanFwdStatus_t EcatCan_ForwardPdo_Modified(uint32_t baseCanId, const void* pData, uint16_t dataSize)
{
    // --- 1. 参数有效性检查 ---
    if (pData == NULL || dataSize == 0)
    {
        return ECAT_CAN_FWD_ERROR_INVALID_PARAMS;
    }

    // --- 直接获取要使用的发送MO ---
    CAN_NODE_LMO_t* tx_mo = CAN_NODE_0.lmobj_ptr[0]; // 假设总是使用第一个MO来发送
    if (tx_mo == NULL)
    {
        return ECAT_CAN_FWD_ERROR_INVALID_PARAMS;
    }

    // --- 2. 初始化变量 ---
    const uint8_t* pDataSource = (const uint8_t*)pData;
    uint16_t bytes_sent = 0;
    uint8_t packet_sequence = 0;

    // --- 3. 循环分包发送 ---
    while (bytes_sent < dataSize)
    {
        uint8_t bytes_to_send = (dataSize - bytes_sent > 8) ? 8 : (dataSize - bytes_sent);

        // a. 在发送前，修改MO的CAN ID !!!
        CAN_NODE_MO_UpdateID(tx_mo, baseCanId + packet_sequence);

        // b. 更新数据长度
        tx_mo->mo_ptr->can_data_length = bytes_to_send;

        // c. 更新数据并发送
        CAN_NODE_MO_UpdateData(tx_mo, (uint8_t*)(pDataSource + bytes_sent));
        if (CAN_NODE_MO_Transmit(tx_mo) != CAN_NODE_STATUS_SUCCESS)
        {
            return ECAT_CAN_FWD_ERROR_CAN_TX;
        }

        // d. 更新计数器
        bytes_sent += bytes_to_send;
        packet_sequence++;

        // e. 总线友好性延时
        for (volatile uint32_t i = 0; i < 500; ++i) { __NOP(); }
    }

    return ECAT_CAN_FWD_SUCCESS;
}

/**
 * @brief  通道级转发函数
 *         只转发真正发生变化的通道，避免重复发送和误触发
 *
 * @param  pCurrentObj  指向当前 EtherCAT 对象结构体的指针 (如 &Rxpdo160x7000)
 * @param  pLastObj     指向用于保存历史状态的结构体指针 (如 &last_7000)
 * @param  baseCanId    该对象的基础 CAN ID
 */
void Forward_MultiChannel_Obj_Auto(void* pCurrentObj, void* pLastObj, uint32_t baseCanId)
{
    // 1. 获取通道数量 (通过 SubIndex0)
    uint16_t subIndexCount = *((uint16_t*)pCurrentObj);
    uint8_t channelCount = subIndexCount / 2;

    // 2. 计算当前数据和历史数据的起始地址 (跳过 u16SubIndex0)
    EcatChannel_t* pCurrCh = (EcatChannel_t*)( (uint8_t*)pCurrentObj + sizeof(UINT16) );
    EcatChannel_t* pLastCh = (EcatChannel_t*)( (uint8_t*)pLastObj + sizeof(UINT16) );

    // 3. 遍历所有通道
    for (uint8_t i = 0; i < channelCount; i++)
    {
        // --- 核心改进：通道级差异检测 ---

        // 只有当 (当前长度 > 0) 且 (当前通道数据 != 历史通道数据) 时才处理
        if (pCurrCh[i].len > 0 &&
            memcmp(&pCurrCh[i], &pLastCh[i], sizeof(EcatChannel_t)) != 0)
        {
            uint16_t sendLen = pCurrCh[i].len;
            if (sendLen > 24) sendLen = 24; // 安全截断

            // 发送 CAN 报文
            EcatCan_ForwardPdo_Modified(baseCanId, pCurrCh[i].data, sendLen);

            // --- 关键：仅更新当前通道的历史快照 ---
            // 这样下次循环时，如果这个通道没变，memcmp 就会返回 0，不会再次发送
            memcpy(&pLastCh[i], &pCurrCh[i], sizeof(EcatChannel_t));
        }

        // 特殊情况处理：如果 Master 把 Len 改为 0，我们也需要更新快照，
        // 否则下次 Master 把 Len 改回非 0 但数据没变时，可能无法触发发送。
        else if (pCurrCh[i].len == 0 && pLastCh[i].len != 0)
        {
             memcpy(&pLastCh[i], &pCurrCh[i], sizeof(EcatChannel_t));
        }
    }
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
    
    /* --- 新增：获取 CAN ID --- */
    uint32_t rx_can_id;

    // 1. 读取硬件仲裁寄存器 (MOAR) 的原始值
    // 注意路径：通过 can_mo_ptr 访问硬件寄存器
    uint32_t moar_value = rx_mo->mo_ptr->can_mo_ptr->MOAR;

    // 2. 判断是否为扩展帧 (Bit 29 IDE)
    // CAN_MO_MOAR_IDE_Msk 通常定义为 0x20000000
    if (moar_value & 0x20000000UL)
    {
        // 扩展帧 (29-bit): ID 在 [28:0]
        rx_can_id = moar_value & 0x1FFFFFFFUL;
    }
    else
    {
        // 标准帧 (11-bit): ID 在 [28:18]
        // 必须右移 18 位才能得到正确的值
        rx_can_id = (moar_value >> 18) & 0x7FF;
    }

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
    /* can 转发 ethercat 模块 */
    Process_Can_Rx_Frame(rx_can_id, rx_data, rx_len);
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
    if (BSP_CAN_IsBaudrateValid(cfg->baudrate))
    {
        CAN_NODE_ConfigBaudrate(&CAN_NODE_0, cfg->baudrate, 
                                CAN_DEFAULT_SAMPLE_POINT, CAN_DEFAULT_SJW);
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
    
    /* 4. 启用 CAN 接收中断 (DAVE 的 CAN_NODE APP 不启用 NVIC) */
    NVIC_SetPriority(CAN0_0_IRQn, CAN_IRQ_PRIORITY);
    NVIC_EnableIRQ(CAN0_0_IRQn);
}


