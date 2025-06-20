#ifndef __FLASH_H
#define __FLASH_H

#include "main.h"
#include <stdint.h>

// Flash������������
void WriteFlash(uint32_t addr, uint32_t *Data, uint32_t L);
void Flash_Read(uint32_t address, uint32_t *data, uint16_t length);

// ʹ��оƬ���1KB�ռ�洢��ֵ (���ݾ���оƬ����)
#define FLASH_SAVE_ADDR  (0x0801FC00) // STM32F103C8T6 64KB Flash�����1KB

// ��ֵ���ݽṹ�� (��֤4�ֽڶ���)
typedef struct __attribute__((packed, aligned(4))) {
    uint8_t TemperatureThreshold;
    uint8_t HumidityThreshold;
    uint16_t crc; // ���CRCУ��
} ThresholdData;

extern ThresholdData thresholdData; // ȫ����ֵ����

// ��������
void SaveThresholdsToFlash(void);
void LoadThresholdsFromFlash(void);
void SetDefaultThresholds(void);


#endif
