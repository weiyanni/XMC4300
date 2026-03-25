/**
 * @file    app.h
 * @brief   应用层接口定义
 * @details 本文件定义了应用层的公共接口，包括硬件初始化和 EtherCAT 数据处理。
 *          业务逻辑与协议栈分离，便于维护和移植。
 * 
 * @author  
 * @date    2024
 * @version 1.0
 */

#ifndef APP_H_
#define APP_H_

#include <stdint.h>
#include <stdbool.h>

/*============================================================================*/
/* 全局变量声明                                                               */
/*============================================================================*/

/** @brief TMC6460 芯片 ID，初始化时读取 */
extern volatile uint32_t g_chip_id;

/** @brief 硬件初始化状态标志 */
extern volatile bool g_init_success;

/** @brief LED 状态翻转计数器 */
extern uint32_t g_led_active_counter;

/*============================================================================*/
/* 函数声明                                                                   */
/*============================================================================*/

/**
 * @brief   硬件初始化
 * @details 初始化 TMC6460 电机驱动芯片，通过 SPI 读取芯片 ID 验证通信正常。
 *          - 成功：g_chip_id = 0x36343630, g_init_success = true
 *          - 失败：g_chip_id = 0xFFFFFFFF (若读回0), g_init_success = false
 * 
 * @return  true: 初始化成功, false: 初始化失败
 */
bool APP_HardwareInit(void);

/**
 * @brief   EtherCAT 应用层数据处理
 * @details 处理 EtherCAT PDO 数据交换：
 *          - 输入处理：解析主站下发的命令（如 LED 控制）
 *          - 输出准备：填充返回给主站的数据（如芯片 ID、计数器）
 * 
 * @param[in]  pRxData  RxPDO 数据指针 (主站 -> 从站)，对应 0x7000 对象
 * @param[out] pTxData  TxPDO 数据指针 (从站 -> 主站)，对应 0x6000 对象
 * 
 * @note    本函数由 ryhand.c 中的 APPL_Application() 调用，每个 EtherCAT 周期执行一次
 */
void APP_EtherCATProcess(void *pRxData, void *pTxData);

/**
 * @brief   处理参数操作命令 (0x9001 ParamAction)
 * @details 操作成功后，值自动变成 0x8000 + 指令
 *          - 0x0001: 保存参数到 Flash → 成功返回 0x8001
 *          - 0x00FF: 软件重启
 *          - 其它: 不操作，值保持写入值
 * 
 * @param[in]  action   操作码
 * @param[out] pResult  操作结果值（写回 ParamAction）
 * @return     0: 成功
 */
uint8_t APP_HandleParamAction(uint16_t action, uint16_t *pResult);

/**
 * @brief  App 身份自注册函数 (Self-Identity Registration)
 * * @details
 * 1. 状态识别：检查 EEPROM 是否处于非有效状态（如刚烧录 HEX 后的全空状态）。
 * 2. 特征计算：利用链接脚本符号 `__app_code_end` 获取当前固件的精确物理长度，
 * 并基于此长度计算整个 App 区域的物理 CRC32 校验值。
 * 3. 身份固化：将 MagicHeader (SHARED_MAGIC_VALID)、实际长度及 CRC32 写入 EEPROM。
 * * @note
 * - 执行前：Bootloader 依靠“盲跳”逻辑进入 App（仅检查栈指针）。
 * - 执行后：Bootloader 将切换为“严格校验模式”，必须物理 CRC 匹配才允许跳转，防止固件损坏或被篡改。
 * @return 无 (结果直接写入物理 EEPROM 模拟区)
 */
void App_Self_Identity_Registration(void);
#endif /* APP_H_ */
