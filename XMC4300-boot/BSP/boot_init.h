/* ================= boot_init.h ================= */
#ifndef BOOT_INIT_H_
#define BOOT_INIT_H_

#include <stdint.h>

void Jump_To_App(void);
int Check_Can_Jump_To_App(void);
uint32_t Calc_App_Flash_CRC32(uint32_t dataSize);
void ENABLE_ESC_INT_USER(void);

/**
 * @brief 从 EEPROM 加载 CAN 和 RS485 波特率配置并应用到硬件
 */
void BSP_LoadBaudrateConfig(void);

#endif /* BOOT_INIT_H_ */

