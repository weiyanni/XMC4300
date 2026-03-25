/* ============================================================================
 * 文件名: dev_config.h
 * 描述: 设备配置管理 (0x8000 对象) - EEPROM 存储和 CoE 同步
 * ============================================================================ */
#ifndef DEV_CONFIG_H_
#define DEV_CONFIG_H_

#include <stdint.h>
#include "shared_conf.h"

/* ==============================================================
 * 接口函数
 * ============================================================== */

/**
 * @brief 初始化设备配置模块
 *        从 EEPROM 加载配置到 SSC 对象 DevInfo0x8000
 */
void DevConfig_Init(void);

/**
 * @brief 保存当前 SSC 对象到 EEPROM
 * @return 0=成功, 非0=失败
 */
uint8_t DevConfig_SaveToEEPROM(void);

/**
 * @brief CoE 写入回调 (在 pAPPL_CoeWriteInd 中调用)
 *        检测 0x8000 对象写入，自动保存到 EEPROM
 * @param index 对象索引
 * @param subindex 子索引
 */
void DevConfig_OnCoeWrite(uint16_t index, uint8_t subindex);

/**
 * @brief 恢复默认配置
 */
void DevConfig_RestoreDefaults(void);

/**
 * @brief 根据当前 SSC 对象配置应用到硬件
 *        - 配置 CAN/485 模式 (互斥)
 *        - 配置 120R 电阻 (仅 CAN 模式有效)
 *        - 应用波特率
 */
void DevConfig_ApplyHardware(void);

/* 同步 DeviceConfig_t -> TOBJ8000 (EEPROM -> SSC 对象) */
void SyncConfig_ToSSC(const DeviceConfig_t *cfg);

/* 同步 TOBJ8000 -> DeviceConfig_t (SSC 对象 -> EEPROM) */
void SyncConfig_FromSSC(DeviceConfig_t *cfg);

#endif /* DEV_CONFIG_H_ */

