/* ================= systick.c ================= */
#include "DAVE.h"
#include "systick.h"

/* 全局时间戳 (volatile 防止编译器优化) */
volatile uint32_t g_u32TickCount = 0;

/* SysTick 中断处理函数 */
void SysTick_Handler(void)
{
    g_u32TickCount++;
}

/* 初始化 SysTick (1ms 中断一次) */
void SysTick_Init(void)
{
    SysTick_Config(SystemCoreClock / 1000);
}

/* 获取当前时间 (ms) */
uint32_t Get_Time_Ms(void)
{
    return g_u32TickCount;
}

/* 非阻塞式延时 (ms) */
void Delay_Ms(uint32_t ms)
{
    uint32_t start = g_u32TickCount;
    while ((g_u32TickCount - start) < ms)
    {
        /* 等待 */
    }
}
