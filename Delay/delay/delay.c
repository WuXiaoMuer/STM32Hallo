#include "delay.h"

// 基于SysTick的微秒延时（兼容HAL_Delay，不影响1ms中断）
void DelayUs(uint32_t us)
{
    uint32_t tick_start = 0;
    uint32_t reload_val = SysTick->LOAD;  // SysTick重载值（HAL库配置为SystemCoreClock/1000，即1ms）
    uint32_t us_ticks = (reload_val + 1) / 1000;  // 1μs对应的SysTick计数值（如72MHz时，reload_val=71999，us_ticks=72）
    
    // 计算需要的总计数值（向下计数，需用重载值差值计算）
    uint32_t total_ticks = us * us_ticks;
    if (total_ticks == 0) return;

    tick_start = SysTick->VAL;  // 获取当前SysTick计数值
    while (1)
    {
        uint32_t tick_current = SysTick->VAL;
        uint32_t tick_diff = 0;

        // 处理SysTick向下计数的溢出情况
        if (tick_current < tick_start)
        {
            tick_diff = tick_start - tick_current;
        }
        else
        {
            tick_diff = (reload_val - tick_current) + tick_start + 1;
        }

        if (tick_diff >= total_ticks)
        {
            break;  // 延时时间到，退出循环
        }
    }
}