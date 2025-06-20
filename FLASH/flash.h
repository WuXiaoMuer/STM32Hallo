#ifndef __FLASH_H
#define __FLASH_H

#include "main.h"
#include <stdint.h>

// Flash操作函数声明
void WriteFlash(uint32_t addr, uint32_t *Data, uint32_t L);
void Flash_Read(uint32_t address, uint32_t *data, uint16_t length);

// 使用芯片最后1KB空间存储阈值 (根据具体芯片调整)
#define FLASH_SAVE_ADDR  (0x0801FC00) // STM32F103C8T6 64KB Flash的最后1KB

// 阈值数据结构体 (保证4字节对齐)
typedef struct __attribute__((packed, aligned(4))) {
    uint8_t TemperatureThreshold;
    uint8_t HumidityThreshold;
    uint16_t crc; // 添加CRC校验
} ThresholdData;

extern ThresholdData thresholdData; // 全局阈值数据

// 函数声明
void SaveThresholdsToFlash(void);
void LoadThresholdsFromFlash(void);
void SetDefaultThresholds(void);


#endif
