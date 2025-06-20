#include "flash.h"
#include "stm32f1xx_hal_flash.h"
#include "string.h"

ThresholdData thresholdData; // 定义全局阈值数据

// Flash写入函数
void WriteFlash(uint32_t addr, uint32_t *Data, uint32_t L)
{
    uint32_t i = 0;
    
    // 1. 解锁FLASH
    HAL_FLASH_Unlock();
    
    // 2. 擦除FLASH
    FLASH_EraseInitTypeDef FlashSet;
    FlashSet.TypeErase = FLASH_TYPEERASE_PAGES;
    FlashSet.PageAddress = addr;
    FlashSet.NbPages = 1;
    
    uint32_t PageError = 0;
    HAL_FLASHEx_Erase(&FlashSet, &PageError);
    
    // 3. 写入FLASH
    for(i = 0; i < L; i++)
    {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr + 4 * i, Data[i]);
    }
    
    // 4. 锁定FLASH
    HAL_FLASH_Lock();
}

// Flash读取函数
void Flash_Read(uint32_t address, uint32_t *data, uint16_t length)
{
    uint16_t i;
    for(i = 0; i < length; i++)
    {
        data[i] = *(__IO uint32_t*)(address + (i * 4));
    }
}

// 保存阈值到Flash
void SaveThresholdsToFlash(void)
{
    // 将数据转换为uint32_t数组以便写入Flash
    uint32_t flash_data[sizeof(ThresholdData) / sizeof(uint32_t)];
    memcpy(flash_data, &thresholdData, sizeof(ThresholdData));
    
    // 写入Flash
    WriteFlash(FLASH_SAVE_ADDR, flash_data, sizeof(ThresholdData) / sizeof(uint32_t));
}

// 从Flash加载阈值
void LoadThresholdsFromFlash(void)
{
    uint32_t flash_data[sizeof(ThresholdData) / sizeof(uint32_t)];
    Flash_Read(FLASH_SAVE_ADDR, flash_data, sizeof(ThresholdData) / sizeof(uint32_t));
    memcpy(&thresholdData, flash_data, sizeof(ThresholdData));
    
    // 验证数据有效性
    if(thresholdData.TemperatureThreshold > 100 || thresholdData.HumidityThreshold > 100)
    {
        SetDefaultThresholds(); // 如果数据无效，设置为默认值
    }
}

// 设置默认阈值
void SetDefaultThresholds(void)
{
    thresholdData.TemperatureThreshold = 30; // 默认温度阈值30°C
    thresholdData.HumidityThreshold = 70;    // 默认湿度阈值70%
    
}
