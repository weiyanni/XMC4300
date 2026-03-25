/* ================= systick.h ================= */
#ifndef SYSTICK_H_
#define SYSTICK_H_

#include <stdint.h>

void     SysTick_Init(void);
uint32_t Get_Time_Ms(void);
void     Delay_Ms(uint32_t ms);

#endif /* SYSTICK_H_ */
