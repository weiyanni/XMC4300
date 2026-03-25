#include "ecat_callback.h"
#include "ryhandObjects.h"      /* 包含 DevInfo0x8000 定义 */
#include "BSP/bsp_485.h"        /* 包含 485 驱动接口 */
#include "Dave/Generated/E_EEPROM_XMC4/e_eeprom_xmc4.h" /* */
#include <string.h>
#include "shared_conf.h"

/* 设备配置 (从 EEPROM 读取) */
DeviceConfig_t s_DevConfig;
uint8_t s_DevConfigValid = 0;
/* 假设在 E_EEPROM 中的存储块索引为 0 */
#define STORAGE_BLOCK_DEV_INFO  0

uint8_t FWUPDATE_InvalidateAppFlag(void)
{
    SharedConfig_t cfg;
    E_EEPROM_XMC4_STATUS_t status;

    // 1. 读取当前 EEPROM 中的配置
    E_EEPROM_XMC4_ReadArray(SHARED_CONFIG_EEPROM_OFFSET, (uint8_t*)&cfg, sizeof(SharedConfig_t));

    // 2. 将 Header 设为无效值 (例如 e)
    cfg.MagicHeader = SHARED_MAGIC_UPDATING;

    // 3. 重新计算校验和，否则下次读取时 SharedConfig_IsValid 会报错
    cfg.Checksum = SharedConfig_CalcChecksum(&cfg);

    // 4. 写回 EEPROM
    status = E_EEPROM_XMC4_WriteArray(SHARED_CONFIG_EEPROM_OFFSET, (uint8_t*)&cfg, sizeof(SharedConfig_t));
    if (status != E_EEPROM_XMC4_STATUS_OK) {
        return 2;
    }

    // 5. 强制更新物理 Flash 存储
    E_EEPROM_XMC4_UpdateFlashContents();

    return 0;
}



/**
 * @brief 从 EEPROM 加载配置并在上电时初始化
 */
void LoadDeviceConfig(void)
{
    if (!s_DevConfigValid)
    {
        E_EEPROM_XMC4_ReadArray(DEVICE_CONFIG_EEPROM_OFFSET,
                                (uint8_t*)&s_DevConfig, sizeof(DeviceConfig_t));
        s_DevConfigValid = DeviceConfig_IsValid(&s_DevConfig);
    }
}

/**
 * @brief 执行持久化保存
 */
static void SaveDeviceConfig(void)
{
    E_EEPROM_XMC4_WriteArray(DEVICE_CONFIG_EEPROM_OFFSET,
                            (uint8_t*)&s_DevConfig, sizeof(DeviceConfig_t));
}

