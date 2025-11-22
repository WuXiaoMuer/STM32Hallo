# STM32 DHT22 温湿度传感器驱动库

> 🌡️💧 轻量级单总线驱动，用于读取 DHT22 / AM2302 数字温湿度传感器数据，适用于 STM32 系列微控制器。 

## ✨ 核心特性

- ✅ 支持 **DHT22** 和 **AM2302**（完全兼容）
- ✅ 精确测量范围：
  - **湿度**：0 ~ 100% RH（±2% 精度）
  - **温度**：-40 ~ +80°C（±0.5°C 精度）
- ✅ 单总线（1-Wire）通信，仅需 **1 个 GPIO 引脚**
- ✅ 提供结构化数据输出（温度 float，湿度 float）
- ✅ 内置 **校验和验证**，确保数据可靠性
- ✅ 支持 **数据读取状态返回**（成功 / 超时 / 校验错误）
- ✅ 基于 **HAL 库** 实现，易于移植到 STM32F1/F4/H7 等系列

## 📁 文件结构

|           |                                                  |
| --------- | ------------------------------------------------ |
| `dht22.h` | 驱动头文件：函数声明、错误码定义、数据结构       |
| `dht22.c` | 驱动实现：初始化、启动信号、数据接收、校验、解析 |

## ⚙️ 使用说明

### 1. 硬件连接

|      |                |                                           |
| ---- | -------------- | ----------------------------------------- |
| VDD  | 3.3V 或 5V     | 支持宽电压（推荐 3.3V 与 STM32 电平匹配） |
| DATA | GPIO（如 PA0） | **单总线，需上拉电阻（4.7kΩ）**           |
| GND  | GND            | 接地                                      |

> ⚠️ **必须** 在 DATA 线上接一个 **4.7kΩ 上拉电阻** 到 VDD，否则通信可能失败。 

### 2. 软件配置

在 `dht22.h` 中定义使用的 GPIO 端口和引脚（示例）：

\#define DHT22_GPIO_PORT     GPIOA

\#define DHT22_GPIO_PIN      GPIO_PIN_0

确保该 GPIO 已由 STM32CubeMX 或手动初始化为 **输出/输入模式（动态切换）**。

### 3. 初始化与读取

```c
#include "dht22.h"

DHT22_Data_t sensorData;

uint8_t result;

// 无需复杂初始化，直接读取

result = DHT22_ReadData(&sensorData);

if (result == DHT22_OK) {

    float temperature = sensorData.temperature;  // 单位：°C

    float humidity    = sensorData.humidity;     // 单位：%

    printf("Temp: %.1f°C, Humi: %.1f%%\n", temperature, humidity);

} else {

    // 处理错误（超时、校验失败等）

    if (result == DHT22_ERROR_CHECKSUM) {

        printf("Checksum error!\n");

    } else if (result == DHT22_ERROR_TIMEOUT) {

        printf("Sensor timeout!\n");

    }

}
```

> ⏱️ **注意**：DHT22 **两次读取间隔 ≥ 2 秒**，否则可能返回错误或旧数据。 

## 📌 关键设计说明

- **单总线时序**：通过精确延时（微秒级）模拟 DHT22 通信协议

- GPIO 模式切换

  ：

  - 发送启动信号时：GPIO 配置为 **推挽输出**
  - 接收数据时：切换为 **浮空输入**

- 延时实现

  ：通常使用 

  ```
  HAL_Delay()
  ```

  （毫秒）和 

  ```
  HAL_DelayMicroseconds()
  ```

  （微秒）或 DWT 延时

  > 若项目中无微秒延时函数，需自行实现（如基于 DWT 或 TIM） 

## 📦 依赖

- STM32 HAL 库（`HAL_GPIO`, `HAL_Delay`）
- 微秒级延时支持（建议实现 `delay_us(uint32_t us)`）
- 已初始化对应 GPIO 时钟（如 `__HAL_RCC_GPIOA_CLK_ENABLE()`）

## 📄 返回状态码

|                        |                          |
| ---------------------- | ------------------------ |
| `DHT22_OK`             | 读取成功                 |
| `DHT22_ERROR_TIMEOUT`  | 通信超时（传感器无响应） |
| `DHT22_ERROR_CHECKSUM` | 校验和错误（数据损坏）   |
| `DHT22_ERROR_PIN`      | 引脚配置错误（部分实现） |

## 🎯 典型应用场景

- 环境监测（温湿度记录）
- 智能家居（自动加湿/除湿控制）
- 农业大棚监控
- 嵌入式调试信息显示（配合 OLED / LCD）

## 📝 注意事项

1. **读取频率限制**：DHT22 最快每 **2 秒** 读取一次，频繁读取会导致错误。
2. **电源稳定性**：建议在 VDD 和 GND 之间加 100nF 电容，减少干扰。
3. **线缆长度**：DATA 线建议 **< 20cm**，过长需降低速率或加驱动。
4. **避免中断干扰**：读取过程中应 **关闭全局中断** 或确保延时不受干扰（关键微秒时序）。

## 📄 许可证

GPL 3.0

> 💡 **提示**：此驱动已在 STM32F103C8T6、STM32F407 等开发板上验证，配合 OLED 显示温湿度效果极佳。 