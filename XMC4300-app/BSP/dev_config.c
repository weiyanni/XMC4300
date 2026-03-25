/* ============================================================================ * 文件名: dev_config.c * 描述: 设备配置管理 (0x8000 对象) - EEPROM 存储和手动保存 *
 * 使用方法: *   - 0x8000 对象：直接读写 SSC 对象，不自动保存
 *   - 0x9001 ParamAction：手动控制保存/重启
 *     - 写 0x01 → 保存参数到 EEPROM，成功后值变为 0x8001 *     - 写 0xFF → 软件重启，成功后值变为 0x80FF *     - 其他值 → 不操作，保持写入值 * ============================================================================ */#include "DAVE.h"#include "bsp_can.h"#include "bsp_485.h"#include "dev_config.h"
#include "shared_conf.h"#include "Dave/Generated/E_EEPROM_XMC4/e_eeprom_xmc4.h"#include "SSC/Src/ryhandObjects.h"
#include <string.h>
#include <stdint.h>
/* ==============================================================
 * 外部对象 (来自 SSC) * ============================================================== */
extern TOBJ8000 DevInfo0x8000;extern UINT16 ParamAction0x9001;
/* ============================================================== * 默认配置值 * ============================================================== */static const DeviceConfig_t g_DefaultConfig = {    .Magic          = DEVICE_CONFIG_MAGIC,    .Name           = "RyHand Device",    .Hv             = "HW V1.0",    .Sv             = "SW V1.0",    .Sn             = "SN000000",    .BuildTime      = 0x20260114,    .BR_canfd_ctrl  = 0x000F4240,  // 1,000,000 (1Mbps)    .BR_canfd_data  = 0x004C4B40,  // 1,000,000 (1Mbps)    .BR_485         = 0x004C4B40,  // 5,000,000 (5Mbps)    .Use120         = 0,    .Use485         = 1,    .UseModbus      = 0,    .Reserved       = 0,    .DevID          = "254",       // 设备ID: 254    .Checksum       = 0  // 会在保存时计算};/* ============================================================== * 内部函数 * ============================================================== */
/* 同步 DeviceConfig_t -> TOBJ8000 (EEPROM -> SSC 对象) */void SyncConfig_ToSSC(const DeviceConfig_t *cfg){    memcpy(DevInfo0x8000.Name, cfg->Name, 32);
    memcpy(DevInfo0x8000.Hv, cfg->Hv, 32);
    memcpy(DevInfo0x8000.Sv, cfg->Sv, 32);
    memcpy(DevInfo0x8000.Sn, cfg->Sn, 32);
    DevInfo0x8000.BuildTime      = cfg->BuildTime;    DevInfo0x8000.BR_canfd_ctrl  = cfg->BR_canfd_ctrl;    DevInfo0x8000.BR_canfd_data  = cfg->BR_canfd_data;    DevInfo0x8000.BR_485         = cfg->BR_485;    DevInfo0x8000.Use120         = cfg->Use120;
    DevInfo0x8000.Use485         = cfg->Use485;    DevInfo0x8000.UseModbus      = cfg->UseModbus;    DevInfo0x8000.SubIndex012    = cfg->Reserved;    memcpy(DevInfo0x8000.DevID, cfg->DevID, 6);}
/* 同步 TOBJ8000 -> DeviceConfig_t (SSC 对象 -> EEPROM) */void SyncConfig_FromSSC(DeviceConfig_t *cfg)
{
    cfg->Magic = DEVICE_CONFIG_MAGIC;
    memcpy(cfg->Name, DevInfo0x8000.Name, 32);
    memcpy(cfg->Hv, DevInfo0x8000.Hv, 32);
    memcpy(cfg->Sv, DevInfo0x8000.Sv, 32);    memcpy(cfg->Sn, DevInfo0x8000.Sn, 32);    cfg->BuildTime      = DevInfo0x8000.BuildTime;
    cfg->BR_canfd_ctrl  = DevInfo0x8000.BR_canfd_ctrl;    cfg->BR_canfd_data  = DevInfo0x8000.BR_canfd_data;    cfg->BR_485         = DevInfo0x8000.BR_485;
    cfg->Use120         = DevInfo0x8000.Use120;
    cfg->Use485         = DevInfo0x8000.Use485;    cfg->UseModbus      = DevInfo0x8000.UseModbus;
    cfg->Reserved       = DevInfo0x8000.SubIndex012;    memcpy(cfg->DevID, DevInfo0x8000.DevID, 6);
    cfg->DevID[6] = 0;    cfg->DevID[7] = 0;
    cfg->Checksum = DeviceConfig_CalcChecksum(cfg);}/* ============================================================== * 接口实现
 * ============================================================== */
void DevConfig_Init(void){
    DeviceConfig_t cfg;    /* 从 EEPROM 读取配置 */
    E_EEPROM_XMC4_ReadArray(DEVICE_CONFIG_EEPROM_OFFSET,
                            (uint8_t*)&cfg, sizeof(DeviceConfig_t));

    if (DeviceConfig_IsValid(&cfg))    {
        /* 配置有效，同步到 SSC 对象 */        SyncConfig_ToSSC(&cfg);
    }    else    {        /* 配置无效，使用默认值并保存 */        DevConfig_RestoreDefaults();    }    /* 应用硬件配置 (CAN/485模式, 波特率等) */    DevConfig_ApplyHardware();}
uint8_t DevConfig_SaveToEEPROM(void){    DeviceConfig_t cfg;
    /* 从 SSC 对象读取当前值 */    SyncConfig_FromSSC(&cfg);    /* 写入 EEPROM */    E_EEPROM_XMC4_WriteArray(DEVICE_CONFIG_EEPROM_OFFSET,                              (uint8_t*)&cfg, sizeof(DeviceConfig_t));    /* 刷新到 Flash */    E_EEPROM_XMC4_STATUS_t status = E_EEPROM_XMC4_UpdateFlashContents();    return (status == E_EEPROM_XMC4_STATUS_OK) ? 0 : 1;}
/* ============================================================== * 设置主板波特率 * ============================================================== */void set_master_baud(DeviceConfig_t cfg){	 if(cfg.Use485)	{		if (BSP_485_IsBaudrateValid(DevInfo0x8000.BR_485))		{			rs485_set_rs485_baud(DevInfo0x8000.BR_485);		}		if (BSP_CAN_IsBaudrateValid(DevInfo0x8000.BR_canfd_ctrl) && BSP_CAN_IsBaudrateValid(DevInfo0x8000.BR_canfd_data))		{			rs485_set_can_baud(DevInfo0x8000.BR_canfd_ctrl,DevInfo0x8000.BR_canfd_data);		}	}	else	{		if (BSP_485_IsBaudrateValid(DevInfo0x8000.BR_485))		{			can_set_rs485_baud(DevInfo0x8000.BR_485);		}		if (BSP_CAN_IsBaudrateValid(DevInfo0x8000.BR_canfd_ctrl) && BSP_CAN_IsBaudrateValid(DevInfo0x8000.BR_canfd_data))		{			can_set_can_baud(DevInfo0x8000.BR_canfd_ctrl,DevInfo0x8000.BR_canfd_data);		}	}}void DevConfig_OnCoeWrite(uint16_t index, uint8_t subindex){    /* 只处理 0x8000 对象的写入 (ParamAction) */    if (index == 0x9001)    {        uint16_t action = ParamAction0x9001;    	DeviceConfig_t cfg;
    	/* 从 EEPROM 读取配置 */    	E_EEPROM_XMC4_ReadArray(DEVICE_CONFIG_EEPROM_OFFSET,    							(uint8_t*)&cfg, sizeof(DeviceConfig_t));
        switch (action)        {            case 0x01:  /* 保存参数到 EEPROM */
                if (DevConfig_SaveToEEPROM() == 0)                {                	set_master_baud(cfg);                    /* 成功：设置为 0x8000 + 原始指令 */                    ParamAction0x9001 = 0x8001;                }                else                {                    /* 失败：设置错误码 */                    ParamAction0x9001 = 0xFFFF;                }                break;            case 0xFF:  /* 软件重启 */                ParamAction0x9001 = 0x80FF;                /* 延迟一点时间确保响应发送 */                for (volatile int i = 0; i < 100000; i++);                NVIC_SystemReset();                break;
            default:                /* 其他值：不操作，保持写入值 */                break;        }    }}void DevConfig_RestoreDefaults(void){    DeviceConfig_t cfg = g_DefaultConfig;    cfg.Checksum = DeviceConfig_CalcChecksum(&cfg);
    /* 同步到 SSC 对象 */    SyncConfig_ToSSC(&cfg);    /* 保存到 EEPROM */    E_EEPROM_XMC4_WriteArray(DEVICE_CONFIG_EEPROM_OFFSET,                              (uint8_t*)&cfg, sizeof(DeviceConfig_t));    E_EEPROM_XMC4_UpdateFlashContents();}/* ============================================================== * 应用硬件配置 * ============================================================== */#include "bsp_can.h"#include "bsp_485.h"uint8_t rec_data[10];void DevConfig_ApplyHardware(void){    /*     * CAN/485 互斥逻辑:     *   Use485=1 → 485模式, CAN待机, 120R关闭     *   Use485=0 → CAN模式, 根据Use120控制终端电阻     */    if (DevInfo0x8000.Use485)    {        /* ===== 485 模式 ===== */        /* 关闭 CAN (待机模式) */        BSP_CAN_Standby();
        /* 切换到 485 模式 */        BSP_485_ModeSelect();        BSP_485_RxEnable();
        BSP_RS485_ApplyConfig();        /* 手动开启底层标准接收中断使能 (Standard Receive Interrupt) */        /* 这样硬件只要收到字节就会向 NVIC 触发信号，无需调用 UART_Receive */        XMC_UART_CH_EnableEvent(UART_485.channel, XMC_UART_CH_EVENT_STANDARD_RECEIVE);        /* 如果您开启了 FIFO，则需要开启 FIFO 接收中断 */        XMC_USIC_CH_RXFIFO_EnableEvent(UART_485.channel, XMC_USIC_CH_RXFIFO_EVENT_CONF_STANDARD);        /* 核心修复：彻底禁止 SR1 (Node 1) 的 CPU 中断跳转 */		/* 这样做不会屏蔽硬件信号流向 DMA，但能防止 CPU 跳进空的或误删标志位的 ISR *///		NVIC_DisableIRQ(USIC1_1_IRQn);//		NVIC_ClearPendingIRQ(USIC1_1_IRQn);        NVIC_SetPriority(USIC1_1_IRQn, 3); // 设置优先级        NVIC_EnableIRQ(USIC1_1_IRQn);      // 启用中断    }    else    {        /* ===== CAN 模式 (或都不启用) ===== */
        /* 切换到 CAN 模式 */        BSP_CAN_ModeSelect();        BSP_CAN_Normal();        /* 使用统一的 CAN 配置接口 */        CAN_Config_t can_cfg = {            .baudrate = DevInfo0x8000.BR_canfd_ctrl,            .dev_id   = BSP_CAN_ParseDevID(DevInfo0x8000.DevID),            .use_120r = DevInfo0x8000.Use120        };        BSP_CAN_ApplyConfig(&can_cfg);    }}

