# STM32 Flash 阈值存储模块

该项目提供了基于 STM32F103C8T6 芯片的 Flash 存储功能实现，主要用于存储和管理温度、湿度阈值数据，并通过 CRC 校验确保数据完整性。

## 功能特点

- 提供 Flash 读写封装函数，简化 STM32 Flash 操作流程
- 实现温度和湿度阈值数据的存储与加载
- 包含数据有效性校验机制
- 支持默认阈值设置功能
- 针对 STM32F103C8T6 64KB Flash 进行优化

## 核心数据结构



```c
// 阈值数据结构(确保4字节对齐)
typedef struct __attribute__((packed, aligned(4))) {
    uint8_t TemperatureThreshold;  // 温度阈值
    uint8_t HumidityThreshold;     // 湿度阈值
    uint16_t crc;                  // CRC校验值
} ThresholdData;
```

## 主要 API

| 函数名                    | 功能描述                          |
| ------------------------- | --------------------------------- |
| `WriteFlash`              | 向指定 Flash 地址写入数据         |
| `Flash_Read`              | 从指定 Flash 地址读取数据         |
| `SaveThresholdsToFlash`   | 将阈值数据保存到 Flash            |
| `LoadThresholdsFromFlash` | 从 Flash 加载阈值数据             |
| `SetDefaultThresholds`    | 设置默认阈值 (温度 30℃，湿度 70%) |

## 存储地址

默认使用 STM32F103C8T6 的最后 1KB Flash 空间进行存储：

```c
#define FLASH_SAVE_ADDR  (0x0801FC00)
```

## 使用方法

1. 初始化时调用`LoadThresholdsFromFlash()`加载存储的阈值
2. 需要修改阈值时，直接更新`thresholdData`结构体成员
3. 调用`SaveThresholdsToFlash()`保存修改后的阈值
4. 如需恢复默认值，调用`SetDefaultThresholds()`

## 注意事项

- Flash 操作会进行扇区擦除，请注意数据备份
- 阈值数据会进行范围校验 (0-100)，超出范围将自动使用默认值
- 该实现针对 STM32F103C8T6 芯片，其他型号可能需要调整 Flash 地址和页大小

## 依赖

- STM32 HAL 库
- stdint 标准库
- string 库 (用于 memcpy 操作)