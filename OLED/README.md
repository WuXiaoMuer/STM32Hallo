# OLED
STM32 Hal lib drive -github STM32Hallo WuXiaoMuer



# OLED I2C 驱动库

## 概述

这是一个用于STM32F1系列微控制器的OLED显示屏I2C驱动库。该库提供了基本的OLED显示控制功能，支持通过I2C接口与OLED屏幕通信。

## 特性

- 支持I2C通信协议
- 提供基本的显示控制功能
- 支持字符和字符串显示
- 简洁易用的API接口

## 硬件连接

默认使用以下引脚连接：
- SCL: PB8
- SDA: PB9

如需修改引脚定义，请编辑`OLED_W_SCL`和`OLED_W_SDA`宏定义。

## API 说明

### 初始化函数
- `OLED_I2C_Init()`: 初始化I2C通信
- `OLED_Init()`: 初始化OLED显示屏

### 基本I2C操作
- `OLED_I2C_Start()`: 发送I2C起始信号
- `OLED_I2C_Stop()`: 发送I2C停止信号
- `OLED_I2C_SendByte(uint8_t Byte)`: 发送一个字节数据

### 显示控制
- `OLED_WriteCommand(uint8_t Command)`: 写入命令到OLED
- `OLED_WriteData(uint8_t Data)`: 写入数据到OLED
- `OLED_SetCursor(uint8_t Y, uint8_t X)`: 设置光标位置
- `OLED_Clear()`: 清空屏幕

### 显示功能
- `OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)`: 显示单个字符
- `OLED_ShowString(uint8_t Line, uint8_t Column, char *String)`: 显示字符串

## 使用示例

```c
#include "oled.h"

int main(void) {
    // 初始化
    OLED_Init();
    
    // 显示内容
    OLED_ShowString(1, 1, "Hello World!");
    OLED_ShowChar(2, 1, 'A');
    
    while(1) {
        // 主循环
    }
}
```

## 依赖

- STM32 HAL库
- STM32F1xx系列微控制器

## 注意事项

1. 使用前请确保正确配置了STM32的时钟和GPIO
2. 默认使用PB8和PB9作为I2C引脚，如需修改请更改宏定义
3. 确保OLED屏幕的I2C地址与硬件匹配

