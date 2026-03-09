# STM32 HAL 驱动 GY302 (BH1750) 数字光照传感器

## 项目概述

本项目提供基于 STM32 HAL 库的 GY302 光照传感器驱动代码，可快速集成到 STM32 工程中，实现光照强度（单位：lux）的稳定读取。

- **GY302**：是搭载 BH1750FVI 芯片的模块化产品，本驱动完全兼容 BH1750 芯片；
- **通信方式**：I2C 标准模式（100kHz），适配 STM32 全系列芯片；
- **核心特性**：高分辨率测量（1lx 精度）、鲁棒的错误处理、极简的调用接口。

## 硬件说明

### 1. 引脚定义与接线

GY302 模块共 4 个核心引脚，推荐接线如下（以 STM32F103 为例）：

| GY302 引脚 | STM32 推荐引脚 |            功能说明             |
| :--------: | :------------: | :-----------------------------: |
|    VCC     |      3.3V      | 供电（禁止接 5V，避免芯片损坏） |
|    GND     |      GND       |              接地               |
|    SDA     | PB7 (I2C1_SDA) |     I2C 数据引脚（需上拉）      |
|    SCL     | PB6 (I2C1_SCL) |     I2C 时钟引脚（需上拉）      |
|    ADDR    |      GND       | 地址选择（推荐接地，禁止悬空）  |

> 注意：若 STM32 引脚无内部上拉，需在 SDA/SCL 与 3.3V 之间接 4.7KΩ 上拉电阻。

### 2. I2C 地址说明

ADDR 引脚接法决定传感器的 I2C 通信地址，本驱动默认适配 ADDR 接地的场景：

| ADDR 引脚接法  | 7 位地址 | 写地址（代码中） | 读地址（代码中） |
| :------------: | :------: | :--------------: | :--------------: |
| 接 GND（推荐） |   0x23   |       0x46       |       0x47       |
|     接 VCC     |   0x5C   |       0x98       |       0x99       |
|     接 SDA     |   0x39   |       0x72       |       0x73       |

## 快速开始

### 1. CubeMX 配置

1. 打开 STM32CubeMX，选择目标芯片；

2. 配置 RCC：选择外部晶振或内部时钟；

3. 配置 I2C1：

   - 模式：`I2C`（非 SMBus）；
   - 速率：`Standard Mode (100kHz)`；
   - 引脚：PB6 (SCL)、PB7 (SDA)；

   

4. （可选）配置 USART：用于打印光照数据；

5. 生成 HAL 库工程（MDK-ARM/STM32CubeIDE）。

### 2. 代码集成

将以下文件添加到工程中：

#### bh1750.h

```
#ifndef __BH1750_H__
#define __BH1750_H__

#include "main.h"

// ADDR接GND：写地址0x46 / 读地址0x47
// ADDR接VCC：写地址0x98 / 读地址0x99
#define BH1750_ADDR_WRITE 0x46
#define BH1750_ADDR_READ  0x47

void BH1750_Init(void);
float BH1750_ReadLux(void);

#endif
```

#### bh1750.c

```
#include "bh1750.h"

extern I2C_HandleTypeDef hi2c1;

void BH1750_Init(void)
{
    uint8_t cmd = 0x01;  // 上电指令
    HAL_I2C_Master_Transmit(&hi2c1, BH1750_ADDR_WRITE, &cmd, 1, HAL_MAX_DELAY);
    HAL_Delay(10);
}

float BH1750_ReadLux(void)
{
    uint8_t buf[2] = {0};
    uint8_t cmd = 0x10;  // 连续高分辨率模式

    if (HAL_I2C_Master_Transmit(&hi2c1, BH1750_ADDR_WRITE, &cmd, 1, 100) != HAL_OK)
    {
        return -1;
    }

    HAL_Delay(180);
    if (HAL_I2C_Master_Receive(&hi2c1, BH1750_ADDR_READ, buf, 2, HAL_MAX_DELAY) == HAL_OK)
    {
        uint16_t raw_data = (buf[0] << 8) | buf[1];
        return (float)raw_data / 1.2f;
    }

    return -1;
}
```

### 3. 调用示例（main.c）

```
#include "bh1750.h"
#include "stdio.h"

// 重定向printf到USART1（需提前配置串口）
int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_I2C1_Init();       // I2C初始化（CubeMX生成）
    MX_USART1_UART_Init();// 串口初始化（可选）

    BH1750_Init();        // BH1750传感器初始化

    while (1)
    {
        float lux = BH1750_ReadLux();
        if (lux >= 0)
        {
            printf("当前光照强度：%.1f lux\r\n", lux);
        }
        else
        {
            printf("BH1750读取失败！\r\n");
        }
        HAL_Delay(500); // 500ms刷新一次
    }
}
```

## 函数说明



|       函数名       |               功能描述               | 参数 |                返回值                |
| :----------------: | :----------------------------------: | :--: | :----------------------------------: |
|  `BH1750_Init()`   | 初始化 BH1750 传感器（发送上电指令） |  无  |                  无                  |
| `BH1750_ReadLux()` |          读取当前光照强度值          |  无  | 成功：光照强度（lux）；失败：返回 -1 |

## 关键注意事项

1. **I2C 地址**：读操作必须使用带读位的地址（如 0x47），写操作使用写地址（如 0x46），地址错误会导致通信失败；
2. **采样延时**：高分辨率模式需至少 120ms 采样时间，代码中设 180ms 留冗余；
3. **ADDR 引脚**：禁止悬空，优先接 GND，否