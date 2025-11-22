# 张大头电机驱动库 (ZhangDaTou Motor Driver Library)

一个用于控制张大头系列步进电机驱动器的 STM32 库，提供了丰富的电机控制功能和示例代码，方便快速实现电机的速度控制、位置控制、同步控制等多种操作模式。

## 功能特点

- 支持多种运动模式：速度模式、扭矩模式、直接位置模式、限速位置模式、梯形加减速位置模式
- 提供完整的电机状态读取功能：位置、速度、电流、温度、电压等
- 支持参数配置：PID 参数、细分设置、地址修改、模式切换等
- 支持同步控制，可实现多电机协同运动
- 包含失速保护、限位保护等安全机制
- 提供丰富的示例代码，快速上手

## 硬件支持

- 张大头系列步进电机驱动器
- STM32 微控制器（通过 UART 通信）
- 支持的电机类型：0.9° 和 1.8° 步进电机

## 接口说明

库中主要函数分类：

1. **基础配置函数**：编码器校准、复位、恢复出厂设置等
2. **运动控制函数**：使能、停止、各种运动模式设置
3. **状态读取函数**：位置、速度、温度、电流等参数读取
4. **参数配置函数**：细分设置、地址修改、模式切换等

详细接口定义请参考 ZhangDaTou.h 头文件。

## 使用示例

```c
// 速度模式示例：顺时针以1000 RPM旋转
void Example_SpeedMode(void)
{
    ZHDT_Motor_Enable(1, 1, SYNC_DISABLE);
    HAL_Delay(100);
    // 2000 RPM/s 加速度，1000.0 RPM 速度
    ZHDT_Motor_SpeedMode(1, DIR_CW, 2000, 10000, SYNC_DISABLE);
}

// 位置模式示例：旋转360度
void Example_TrapezoidalRotation(void)
{
    ZHDT_Motor_Enable(1, 1, SYNC_DISABLE);
    HAL_Delay(100);
    // 梯形加减速模式，旋转360.0度
    ZHDT_Motor_TrapezoidalPosMode(1, DIR_CW, 1000, 1000, 20000, 3600, POS_RELATIVE, SYNC_DISABLE);
}
```

更多示例请参考 ZhangDaTouDemo.h 文件，包含同步控制、位置校准、往复运动等多种场景。

## 安装与使用

1. 将库文件添加到你的 STM32 项目中
2. 包含头文件 `#include "ZhangDaTou.h"`
3. 初始化 UART 通信（默认使用 USART1）
4. 调用相应的 API 函数控制电机

## 注意事项

- 确保 UART 通信波特率与驱动器匹配（默认 115200）
- 电机使能后需要适当延时再发送控制指令
- 多电机同步控制时，使用 SYNC_ENABLE 标记并调用 SyncStart 函数

## 许可证

本项目采用 GPL 3.0 许可证。