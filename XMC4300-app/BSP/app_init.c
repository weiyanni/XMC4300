/* ================= app_init.c ================= */
#include "DAVE.h"
#include "shared_conf.h"
#include "Dave/Generated/E_EEPROM_XMC4/e_eeprom_xmc4.h"
#include <string.h>
#include <xmc_flash.h>

/* .no_init 段变量：软件复位时不清零，断电后丢失 */
/* 注意：段名是 .no_init（带下划线），与链接脚本一致 */
uint32_t g_RamMagicFlag __attribute__((section(".no_init")));

void SYNC0IRQHandler(void)
{
    Sync0_Isr();
}

void SYNC1IRQHandler(void)
{
    Sync1_Isr();
}

void ENABLE_ESC_INT_USER(void)
{
    INTERRUPT_Enable(&INT_SYNC0);
    INTERRUPT_Enable(&INT_SYNC1);
}

void DISABLE_ESC_INT_USER(void)
{
    INTERRUPT_Disable(&INT_SYNC0);
    INTERRUPT_Disable(&INT_SYNC1);
}

void App_Init(void)
{
    /* 中断向量表重定向 (使用 cached 地址) */
    /* 注意: 必须在 DAVE_Init 之前设置，否则中断会跳转到错误的地址 */
    SCB->VTOR = APP_START_ADDR_RD;
    __DSB();
    __ISB();
    
    /* 强制重置 E_EEPROM 状态 */
    /* Boot 跳转到 App 时，SRAM 内容不会被清除。
     * E_EEPROM 驱动检查 initialized 标志来决定是否重新初始化。
     * 我们必须强制重置此标志，否则 DAVE_Init 会跳过 E_EEPROM 初始化。 */
    extern E_EEPROM_XMC4_t E_EEPROM_XMC4_0;
    E_EEPROM_XMC4_0.initialized = false;
}

/* 请求进入 Bootloader 升级模式 */
void Handle_Bootstrap_Request(void)
{
	/* 6. 执行跳转逻辑 (Handle_Bootstrap_Request 通常定义在 bootmode.c) */
    XMC_SCU_RESET_AssertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_CCU41);
    for (volatile int i = 0; i < 100; i++); // 短延时
    XMC_SCU_RESET_DeassertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_CCU41);
    XMC_UART_CH_Stop(UART_485.channel);
    XMC_DMA_CH_Disable(UART_485.config->global_dma->dma, UART_485.config->transmit_dma_config->dma_channel);
    /* 1. 关中断 */
    __disable_irq();
    
    /* 2. 写 RAM 标志（软件复位后 RAM 数据保留） */
    g_RamMagicFlag = RAM_MAGIC_VAL;
    
    /* 3. 系统复位，重启进入 Bootloader */
    NVIC_SystemReset();
    
    /* 不会到达这里 */
}
