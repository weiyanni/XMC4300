#ifndef APP_ECAT_CALLBACK_H
#define APP_ECAT_CALLBACK_H

#include "ecat_def.h"
#include "objdef.h"
#include "shared_conf.h"
#include "bsp_485.h"

#ifdef __cplusplus
extern "C" {
#endif

extern DeviceConfig_t s_DevConfig;
extern uint8_t s_DevConfigValid;

/**
 * @brief 应用层参数初始化
 * 从模拟 EEPROM 加载已保存的配置，并应用到对象字典
 */
void App_Param_Init(void);

/**
 * @brief 手动触发配置保存
 * 将当前的 DevInfo0x8000 结构体保存到 Flash
 */
static void SaveDeviceConfig(void);

void LoadDeviceConfig(void);

void App_Param_SyncFromSystem(void);

/**
 * @brief 在升级开始前，将 EEPROM 中的 App 标志设为无效
 * @return 0: 成功, 其他: 失败
 */
uint8_t FWUPDATE_InvalidateAppFlag(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_ECAT_CALLBACK_H */
