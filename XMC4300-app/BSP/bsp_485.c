/* ============================================================================
 * 文件名: bsp_485.c
 * 描述: RS485 通信控制实现
 *
 * 硬件逻辑:
 *   - UART1_RS485_DE: LOW=接收, HIGH=发送
 *   - CTRLB_H_A + CTRLA_L_B: 都LOW=485模式, 都HIGH=CAN模式
 * ============================================================================ */
#include "DAVE.h"
#include "bsp_485.h"
#include "can_protocol.h"

/* DAVE 生成的 IO 对象 */
extern const DIGITAL_IO_t UART1_RS485_DE;
extern const DIGITAL_IO_t CTRLB_H_A;
extern const DIGITAL_IO_t CTRLA_L_B;

// 接收上下文变量
static RS485_RxState_t g_rx_state = STATE_WAIT_HEADER;
static uint8_t g_rx_id = 0;
static uint8_t g_rx_len = 0;
static uint8_t g_rx_data_buf[RS485_MAX_DATA_LEN];
static uint8_t g_rx_data_cnt = 0;
static uint8_t g_rx_checksum = 0; // 用于计算累加和

/* DMA 发送专用缓冲区 (必须是全局变量以保证 DMA 访问安全) */
static uint8_t g_rs485_dma_buf[256];

uint8_t g_rs485_rx_buf[RS485_RX_BUF_SIZE];
volatile uint32_t g_last_dma_count = RS485_RX_BUF_SIZE;
volatile uint32_t g_idle_ms = 0;
volatile bool g_is_processing = false; // 防止定时器和回调同时处理数据


CanMsg RxMessage[BUS_NUM][RxMessLength];
volatile uint8_t RxMessRead[BUS_NUM];
volatile uint8_t RxMessWrite[BUS_NUM];
uint8_t LastActionBus = BUS_NONE;
unsigned char CanRxLength;
uint16_t  RxCanId;

void BSP_485_Init(void)
{
    /* 切换到 485 模式 */
    BSP_485_ModeSelect();

    /* 默认接收模式 */
    BSP_485_RxEnable();
}

//void RS485_Start_Receive(void)
//{
//	g_idle_ms = 0;
//	g_last_dma_count = RS485_RX_BUF_SIZE;
//	g_is_processing = false;
//
//	BSP_485_RxEnable(); // 物理层切到接收模式
//
//	// 启动 DAVE UART DMA 接收
//	UART_Receive(&UART_485, g_rs485_rx_buf, RS485_RX_BUF_SIZE);
//}

void BSP_485_ModeSelect(void)
{
    /* 485 模式: CTRLB_H_A=LOW, CTRLA_L_B=LOW */
    DIGITAL_IO_SetOutputLow(&CTRLB_H_A);
    DIGITAL_IO_SetOutputLow(&CTRLA_L_B);
}

void BSP_485_TxEnable(void)
{
    DIGITAL_IO_SetOutputHigh(&UART1_RS485_DE);  /* DE HIGH = 发送 */
}

void BSP_485_RxEnable(void)
{
    DIGITAL_IO_SetOutputLow(&UART1_RS485_DE);   /* DE LOW = 接收 */
}

/* ==============================================================
 * RS485 波特率切换实现
 * ============================================================== */
#include "dev_config.h"
#include "SSC/Src/ryhandObjects.h"

/* 外部 DAVE 对象 */
extern UART_t UART_485;

/* 外部 SSC 对象 */
extern TOBJ8000 DevInfo0x8000;

/* 支持的 RS485 波特率表 (18种) */
static const uint32_t s_485Baudrates[] = {
    1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200,
    230400, 460800, 921600, 1000000, 2000000, 2500000,
    3000000, 4000000, 5000000, 6000000, 8000000, 10000000
};
#define RS485_BAUDRATE_COUNT  (sizeof(s_485Baudrates) / sizeof(s_485Baudrates[0]))

/* 默认过采样率 */
#define RS485_DEFAULT_OVERSAMPLING    16

uint8_t BSP_485_IsBaudrateValid(uint32_t baudrate)
{
    for (uint8_t i = 0; i < RS485_BAUDRATE_COUNT; i++)
    {
        if (s_485Baudrates[i] == baudrate)
        {
            return 1;
        }
    }
    return 0;
}

BSP_485_STATUS_t BSP_485_SetBaudrate(uint32_t baudrate, uint8_t save_to_eeprom)
{
    /* 验证波特率 */
    if (!BSP_485_IsBaudrateValid(baudrate))
    {
        return BSP_485_ERROR_INVALID_BAUDRATE;
    }

    /* 配置硬件波特率 */
    UART_STATUS_t status = UART_SetBaudrate(&UART_485, baudrate, RS485_DEFAULT_OVERSAMPLING);
    if (status != UART_STATUS_SUCCESS)
    {
        return BSP_485_ERROR_HW_FAILED;
    }

    /* 更新 SSC 对象 */
    DevInfo0x8000.BR_485 = baudrate;

    /* 保存到 EEPROM */
    if (save_to_eeprom)
    {
        if (DevConfig_SaveToEEPROM() != 0)
        {
            return BSP_485_ERROR_HW_FAILED;
        }
    }

    return BSP_485_OK;
}

/* ==============================================================
 * 发送设置rs485波特率消息
 * ============================================================== */
void rs485_set_rs485_baud(uint32_t BR_485)
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
		RS485_SendProtocolFrame(i,tx_data, 5);
	}
}

/* ==============================================================
 * 发送设置can波特率消息
 * ============================================================== */
void rs485_set_can_baud(uint32_t BR_canfd_ctrl,uint32_t BR_canfd_data)
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
		RS485_SendProtocolFrame(i,tx_data, 8);
	}
}

BSP_485_STATUS_t BSP_RS485_UpdateBaudrate(uint32_t baudrate)
{
	/* 验证波特率 */
	if (!BSP_485_IsBaudrateValid(baudrate))
	{
		return BSP_485_ERROR_INVALID_BAUDRATE;
	}

	/* 配置硬件波特率 */
	UART_STATUS_t status = UART_SetBaudrate(&UART_485, baudrate, RS485_DEFAULT_OVERSAMPLING);
	if (status != UART_STATUS_SUCCESS)
	{
		return BSP_485_ERROR_HW_FAILED;
	}
}

uint16_t BSP_RS485_ParseDevID(const char *dev_id_str)
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

/* ==============================================================
 * rs485 统一配置接口
 * ============================================================== */
void BSP_RS485_ApplyConfig()
{
    if (DevInfo0x8000.Use120)
    {
    	BSP_120R_Enable();
    }
    else
    {
    	BSP_120R_Disable();
    }

	/* 应用 RS485 波特率 (仅硬件,不保存) */
	if (BSP_485_IsBaudrateValid(DevInfo0x8000.BR_485))
	{
		BSP_485_SetBaudrate(DevInfo0x8000.BR_485, 0);
	}
}

/*
/*
 * DMA 发送启动函数 (非阻塞，供业务层调用)
 */
static void RS485_Send_DMA(uint8_t *data, uint16_t len)
{
	/* 1. 不要直接 return，改为等待上一次发送结束 */
	    /* 5Mbps 下这里的等待时间极短 (微秒级) */
	    uint32_t timeout = 0xFFFFF;
	    while (UART_485.runtime->tx_busy && --timeout);

	    /* 2. 如果超时（硬件异常），强制释放 */
	    if (timeout == 0) UART_485.runtime->tx_busy = false;

	    if (len > sizeof(g_rs485_dma_buf)) len = sizeof(g_rs485_dma_buf);
	    memcpy(g_rs485_dma_buf, data, len);

	    BSP_485_TxEnable();

	    /* 3. 启动 DMA 传输并检查状态 */
	    UART_STATUS_t status = UART_Transmit(&UART_485, g_rs485_dma_buf, len);

	    /* 如果启动失败，确保清除 busy 标志，否则系统会死锁 */
	    if (status != UART_STATUS_SUCCESS) {
	        UART_485.runtime->tx_busy = false;
	    }
}

/**
 * @brief 发送 RS485 协议帧
 * @param id       目标设备 ID (16位)
 * @param cmd_data 数据负载指针
 * @param cmd_len  数据负载长度
 */
void RS485_SendProtocolFrame(int id, uint8_t *cmd_data, uint8_t cmd_len)
{
    uint8_t tx_buf[64];
    uint8_t i = 0;
    uint8_t checksum = 0;

    // 1. 组装协议头部
    tx_buf[i++] = 0xA5;                 // Header 帧头

    tx_buf[i++] = (uint8_t)(id & 0xFF);         // ID Low (取低8位)
    tx_buf[i++] = (uint8_t)((id >> 8) & 0xFF);  // ID High (取高8位)

    tx_buf[i++] = cmd_len;              // Length 负载长度

    // 2. 填充 Payload (负载数据)
    for (uint8_t j = 0; j < cmd_len; j++) {
        tx_buf[i++] = cmd_data[j];
    }

    // 3. 计算校验和 (从 Header 累加到最后一个 Data)
    for (uint8_t k = 0; k < i; k++) {
        checksum += tx_buf[k];
    }
    tx_buf[i++] = checksum;             // Checksum 校验位

    // 4. 调用 BSP 层接口发送
    RS485_Send_DMA(tx_buf, i);
}

///*********************************************************************
//*@ Description: 用户命令接收解析函数
//* @param      :
//		*dat - 源数据首地址
//		len  - 有效数据长度，单位字节
//* @return     : 无
//*********************************************************************/
//void UcmdRecv(uint8_t* dat, uint8_t len, uint8_t bus)
//{
//  static uint8_t rcvsta = CMD_HEADER;
//  static uint8_t RxIndex = 0;
//  static uint8_t check = 0;
//  static Frame_t tcmd;
//  static uint8_t* pdata = (uint8_t*)(&tcmd);
//
//  while(len)
//  {
//    if(RxIndex == 0) check = *dat;
//    else check += *dat;
//
//    pdata[RxIndex++] = *dat++;
//
//    switch(rcvsta)
//    {
//    case CMD_HEADER :
//      if(tcmd.header != 0xA5) RxIndex = 0;
//      else rcvsta = CMD_ID;
//
//      break;
//
//    case CMD_ID   :
//      if(RxIndex >= 3) rcvsta = CMD_LEN;
//
//      break;
//
//    case CMD_LEN   :
//      if(tcmd.len > 64)
//      {
//        rcvsta = CMD_HEADER;
//      }
//      else if(tcmd.len == 0)
//      {
//        tcmd.check = check;
//        rcvsta = CMD_CRC;
//      }
//      else rcvsta = CMD_DATA;
//
//      break;
//
//    case CMD_DATA   :
//      if(RxIndex >= (tcmd.len + 4))
//      {
//        tcmd.check = check;
//        rcvsta = CMD_CRC;
//      }
//      break;
//
//    case CMD_CRC   :
//      if((tcmd.check == pdata[RxIndex - 1]) && ((tcmd.id == 0) || (tcmd.id == 254)))
//      {
//        if(((RxMessWrite[bus] + 1) % RxMessLength) != RxMessRead[bus])
//        {
//          RxMessage[bus][RxMessWrite[bus]].id  = tcmd.id;
//          RxMessage[bus][RxMessWrite[bus]].len = tcmd.len;
//          memcpy(RxMessage[bus][RxMessWrite[bus]].Data, tcmd.Data, tcmd.len);
//          RxMessWrite[bus] = (RxMessWrite[bus] + 1) % RxMessLength;
//        }
//      }
//      uint8_t  	chec=pdata[RxIndex - 1];
//      // 清除缓存内容,可以不清，为了安全还是清
//      memset(&tcmd, 0, sizeof(Frame_t));
//      RxIndex = 0;
//      rcvsta = CMD_HEADER;
//      break;
//
//    default:
//      RxIndex = 0;
//      rcvsta = CMD_HEADER;
//      break;
//
//    }
//
//    len--;
//  }
//
//}

static uint16_t g_rx_id_full = 0;
static uint8_t  g_id_cnt = 0;
///**
// * @brief 解析从 DMA 缓冲区提取出的原始数据流
// * @param buffer 指向收到的原始数据
// * @param length 数据长度
// */
//void RS485_Parse_Stream(const uint8_t *buffer, uint16_t length)
//{
//    for (uint16_t i = 0; i < length; i++)
//    {
//        uint8_t byte = buffer[i];
//
//        switch (g_rx_state)
//        {
//            case STATE_WAIT_HEADER:
//                if (byte == RS485_FRAME_HEADER) {
//                    g_rx_checksum = byte;
//                    g_id_cnt = 0;
//                    g_rx_state = STATE_WAIT_ID;
//                }
//                break;
//
//            case STATE_WAIT_ID:
//                g_rx_checksum += byte;
//                if (g_id_cnt == 0) {
//                    g_rx_id_full = (uint16_t)byte;
//                    g_id_cnt++;
//                } else {
//                    g_rx_id_full |= (uint16_t)(byte << 8);
//                    g_rx_state = STATE_WAIT_LEN;
//                }
//                break;
//
//            case STATE_WAIT_LEN:
//                g_rx_len = byte;
//                g_rx_checksum += byte;
//                g_rx_data_cnt = 0;
//                if (g_rx_len <= RS485_MAX_DATA_LEN) {
//                    g_rx_state = (g_rx_len > 0) ? STATE_WAIT_DATA : STATE_WAIT_CHECK;
//                } else {
//                    g_rx_state = STATE_WAIT_HEADER;
//                }
//                break;
//
//            case STATE_WAIT_DATA:
//                g_rx_data_buf[g_rx_data_cnt++] = byte;
//                g_rx_checksum += byte;
//                if (g_rx_data_cnt >= g_rx_len) {
//                    g_rx_state = STATE_WAIT_CHECK;
//                }
//                break;
//
//            case STATE_WAIT_CHECK:
//                if (byte == g_rx_checksum) {
//                    // 校验通过，处理指令
//                    RS485_Protocol_HandleCommand(g_rx_data_buf, g_rx_len);
//
//                }
//                g_rx_state = STATE_WAIT_HEADER;
//                break;
//        }
//    }
//}

void RS485_Rx_Handler()
{
    while (!XMC_USIC_CH_RXFIFO_IsEmpty(UART_485.channel))
    {
        uint8_t byte = (uint8_t)XMC_UART_CH_GetReceivedData(UART_485.channel);

        switch (g_rx_state)
        {
            case STATE_WAIT_HEADER:
                if (byte == RS485_FRAME_HEADER ) {
                    g_rx_checksum = byte;
                    g_id_cnt = 0;
                    g_rx_state = STATE_WAIT_ID;
                }
                break;

            case STATE_WAIT_ID:
                g_rx_checksum += byte;
                if (g_id_cnt == 0) {
                    g_rx_id_full = (uint16_t)byte; // 低字节
                    g_id_cnt++;
                } else {
                    g_rx_id_full |= (uint16_t)(byte << 8); // 高字节
                    g_rx_state = STATE_WAIT_LEN;
                }
                break;

            case STATE_WAIT_LEN:
                g_rx_len = byte;
                g_rx_checksum += byte;
                g_rx_data_cnt = 0;
                if (g_rx_len <= RS485_MAX_DATA_LEN) {
                    g_rx_state = (g_rx_len > 0) ? STATE_WAIT_DATA : STATE_WAIT_CHECK;
                } else {
                    g_rx_state = STATE_WAIT_HEADER;
                }
                break;

            case STATE_WAIT_DATA:
                g_rx_data_buf[g_rx_data_cnt++] = byte;
                g_rx_checksum += byte;
                if (g_rx_data_cnt >= g_rx_len) {
                    g_rx_state = STATE_WAIT_CHECK;
                }
                break;

            case STATE_WAIT_CHECK:
                // 1. 首先进行校验和验证
                if (byte == g_rx_checksum)
                {
                    // 2. 校验通过后，判断 ID 是否为 254 (0x00FE)
                    if (g_rx_id_full == 254)
                    {
						if(((RxMessWrite[BUS_485] + 1) % RxMessLength) != RxMessRead[BUS_485])
						{
						  RxMessage[BUS_485][RxMessWrite[BUS_485]].id  = g_rx_id_full;
						  RxMessage[BUS_485][RxMessWrite[BUS_485]].len = g_rx_len;
						  memcpy(RxMessage[BUS_485][RxMessWrite[BUS_485]].Data, g_rx_data_buf, g_rx_len);
						  RxMessWrite[BUS_485] = (RxMessWrite[BUS_485] + 1) % RxMessLength;
						}
//                        // 只有 ID 匹配时才调用处理函数并触发回复逻辑
//                        RS485_Protocol_HandleCommand(g_rx_data_buf, g_rx_len);
                    }
                    // 如果 ID 不匹配，静默丢弃，不执行任何操作
                }else
                {
                	memset(g_rx_data_buf, 0, sizeof(g_rx_data_buf));
                }

                // 无论校验是否通过或 ID 是否匹配，都要回到起始状态准备接收下一帧
                g_rx_state = STATE_WAIT_HEADER;
                break;
        }
    }
}

void RS485_Tx_DMA_Callback()
{
	/* 1. 此时 tx_busy 已经被 uart_conf.c 里的 handler 清除了 */

	    /* 2. 等待硬件移位完成 (5Mbps 速率下，进到这里可能已经完成了) */
	    while (!(XMC_UART_CH_GetStatusFlag(UART_485.channel) & XMC_UART_CH_STATUS_FLAG_TRANSMISSION_IDLE));

	    g_rx_state = STATE_WAIT_HEADER;
	    /* 3. 切换回接收 */
	    BSP_485_RxEnable();

	    /* 4. 清除协议标志，准备下一次 */
	    XMC_UART_CH_ClearStatusFlag(UART_485.channel, XMC_UART_CH_STATUS_FLAG_TRANSMISSION_IDLE);
}

//void RS485_Rx_DMA_Callback(void)
//{
//	// 如果定时器已经正在处理了，直接退出
//	if (g_is_processing) return;
//
//	g_is_processing = true;
//
//	// 既然进入了回调，说明 256 字节全部填满了
//	// 这里不需要调用 Abort，因为 DAVE 驱动在 DMA 结束时会自动清理 rx_busy
//
////	RS485_Parse_Stream(g_rs485_rx_buf, RS485_RX_BUF_SIZE);
//	UcmdRecv(g_rs485_rx_buf, RS485_RX_BUF_SIZE,BUS_485);
//
//	// 重新开启下一轮监听
//	RS485_Start_Receive();
//}

void USIC1_1_IRQHandler(void)
{
    // 简单清除标志位即可，不做任何业务逻辑
    //XMC_UART_CH_ClearStatusFlag(XMC_UART1_CH1, XMC_UART_CH_STATUS_FLAG_TRANSMIT_BUFFER_INDICATION);
}


//// 此函数每 1ms 被调用一次
//void RS485_Rx_Timeout_Task(void)
//{
//	// 如果没有在接收 busy 状态，或者正在处理数据，则跳过
//	if (UART_485.runtime->rx_busy == false || g_is_processing) return;
//
//	// 获取当前 DMA 剩余计数 (BLOCK_TS)
//	// 根据你的 xmc_dma.h，该值从 256 往 0 递减
//	uint32_t current_count = XMC_DMA_CH_GetTransferredData(UART_485.config->global_dma->dma,
//														   UART_485.config->receive_dma_config->dma_channel);
//
//	if (current_count > 0) // 说明已经收到至少 1 个字节
//	{
//		if (current_count == g_last_dma_count) // 如果 1ms 内计数值没变 -> 线路可能空闲
//		{
//			g_idle_ms++;
//			if (g_idle_ms >= RS485_IDLE_THRESHOLD)
//			{
//				// --- 判定为一帧结束 (超时截断) ---
//				g_is_processing = true;
//				uint32_t len = RS485_RX_BUF_SIZE - current_count;
//
//				UART_AbortReceive(&UART_485); // 停止 DMA，清理 rx_busy 标志
//
//				if (current_count > 0)
//				{
//					UcmdRecv(g_rs485_rx_buf, current_count,BUS_485);
////					RS485_Parse_Stream(g_rs485_rx_buf, (uint8_t)len);
//				}
//				// 3. 重启前清空缓冲区（关键：防止下次读到旧数据误以为是错的）
//				memset(g_rs485_rx_buf, 0, current_count);
//				RS485_Start_Receive(); // 重新开启监听
//			}
//		}
//		else // 计数值变了，说明正在接收
//		{
//			g_idle_ms = 0;
//			g_last_dma_count = current_count;
//		}
//	}
//}
