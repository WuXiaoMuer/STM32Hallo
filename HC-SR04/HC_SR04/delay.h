#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f1xx_hal.h"  // 根据芯片系列修改（如f4xx、h7xx）

// 微秒级延时（基于SysTick，兼容HAL_Delay）
void DelayUs(uint32_t us);

#endif /* __DELAY_H */