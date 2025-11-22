# STM32 轻量级延时库（支持 us / ms）

> ⏱️ 极简、零依赖的阻塞式延时实现，使用 `delay_us()` / `delay_ms()`，适用于 STM32 项目中的精准时序控制。 

## ✨ 核心特性

- ✅ **无需调用 `delay_init()`** — 开箱即用
- ✅ 提供 `delay_us(uint32_t us)` 和 `delay_ms(uint32_t ms)` 两个函数
- ✅ 基于 **循环计数（for-loop + NOP）** 实现，依赖 `SystemCoreClock`
- ✅ 自动从 `SystemCoreClock`（HAL 库定义）获取主频，自动计算循环次数
- ✅ 适用于 **所有 STM32 系列**（F1/F4/G0/L4/H7 等）
- ✅ 特别适合 **外设初始化时序**（如 DHT22、OLED、DS18B20 等）

## 📁 文件说明

|           |                                                              |
| --------- | ------------------------------------------------------------ |
| `delay.h` | 声明`delay_us()`和`delay_ms()`函数                           |
| `delay.c` | 使用`for`循环 + 编译器屏障（如`__IO`或`volatile`）实现精确延时 |

## ⚙️ 使用方式

\#include "delay.h"

int main(void)

{

​    HAL_Init();

​    SystemClock_Config(); // 确保 SystemCoreClock 已正确设置

​    // 直接使用！无需 delay_init()

​    delay_ms(1000);   // 延时 1 秒

​    delay_us(50);     // 精确延时 50 微秒（用于 DHT22 启动信号等）

}

> ✅ **关键前提**：`SystemCoreClock` 必须已被正确初始化（通常由 `SystemClock_Config()` 设置）。 

## 🔍 实现原理（典型代码结构）

在 `delay.c` 中，你可能会看到类似以下逻辑：

\#include "stm32f4xx_hal.h"  // 或对应系列

void delay_us(uint32_t us)

{

​    uint32_t start = HAL_GetTick();

​    // 为避免长时间 us 导致溢出，通常只用于 < 10ms 的场景

​    // 或更常见的是：基于 CPU 周期计算

​    uint32_t cycles = (SystemCoreClock / 1000000) * us / 7; // 估算每循环耗时（含 NOP）

​    while(cycles--);

}

但更**常见且高效的做法**是使用 **内联汇编或 volatile 循环**，例如：

void delay_us(uint32_t us)

{

​    volatile uint32_t i;

​    for(i = 0; i < us * (SystemCoreClock / 3000000); i++); // 经验系数

}

> 📌 这种方式**不需要任何外设初始化**（如 SysTick 或 DWT），但精度依赖于： 
>
> - 编译器优化等级（建议 `-O0` 或使用 `volatile` 防止优化）
> - CPU 主频是否准确
> - 指令周期估算是否合理

## ⚠️ 注意事项

- **精度有限**：受编译器优化、主频误差、中断影响，**适合 1~500 µs 的粗略高精度场景**
- **禁止高优化**：若使用 `-O2`/`-O3`，循环可能被优化掉！务必用 `volatile` 保护
- **不适用于长时间延时**：`delay_ms(1000)` 会消耗大量 CPU，建议长延时用 `HAL_Delay()`
- **中断会增加实际延时**：延时期间若发生中断，总时间会变长

## 🆚 与 HAL_Delay 对比

|                |                                  |                  |
| -------------- | -------------------------------- | ---------------- |
| 最小单位       | 1 ms                             | **1 µs**         |
| 是否需要初始化 | 否（SysTick 已由 HAL_Init 启用） | **否**           |
| 是否阻塞 CPU   | 是                               | 是               |
| 是否受中断影响 | 否（基于 tick 计数）             | **是**（纯循环） |
| 适合长时间延时 | ✅ 是                             | ❌ 否（浪费 CPU） |
| 适合微秒级时序 | ❌ 否                             | ✅ 是             |

## 🎯 典型用途

- DHT22 启动信号（拉低 1ms，再拉高 20~40µs）
- 单总线器件复位时序
- OLED 指令间的短延时（如 100µs）
- 红外信号脉冲生成

## 📄 许可证

GPL 3.0