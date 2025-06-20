#include "flash.h"
#include "stm32f1xx_hal_flash.h"
#include "string.h"

ThresholdData thresholdData; // ����ȫ����ֵ����

// Flashд�뺯��
void WriteFlash(uint32_t addr, uint32_t *Data, uint32_t L)
{
    uint32_t i = 0;
    
    // 1. ����FLASH
    HAL_FLASH_Unlock();
    
    // 2. ����FLASH
    FLASH_EraseInitTypeDef FlashSet;
    FlashSet.TypeErase = FLASH_TYPEERASE_PAGES;
    FlashSet.PageAddress = addr;
    FlashSet.NbPages = 1;
    
    uint32_t PageError = 0;
    HAL_FLASHEx_Erase(&FlashSet, &PageError);
    
    // 3. д��FLASH
    for(i = 0; i < L; i++)
    {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr + 4 * i, Data[i]);
    }
    
    // 4. ����FLASH
    HAL_FLASH_Lock();
}

// Flash��ȡ����
void Flash_Read(uint32_t address, uint32_t *data, uint16_t length)
{
    uint16_t i;
    for(i = 0; i < length; i++)
    {
        data[i] = *(__IO uint32_t*)(address + (i * 4));
    }
}

// ������ֵ��Flash
void SaveThresholdsToFlash(void)
{
    // ������ת��Ϊuint32_t�����Ա�д��Flash
    uint32_t flash_data[sizeof(ThresholdData) / sizeof(uint32_t)];
    memcpy(flash_data, &thresholdData, sizeof(ThresholdData));
    
    // д��Flash
    WriteFlash(FLASH_SAVE_ADDR, flash_data, sizeof(ThresholdData) / sizeof(uint32_t));
}

// ��Flash������ֵ
void LoadThresholdsFromFlash(void)
{
    uint32_t flash_data[sizeof(ThresholdData) / sizeof(uint32_t)];
    Flash_Read(FLASH_SAVE_ADDR, flash_data, sizeof(ThresholdData) / sizeof(uint32_t));
    memcpy(&thresholdData, flash_data, sizeof(ThresholdData));
    
    // ��֤������Ч��
    if(thresholdData.TemperatureThreshold > 100 || thresholdData.HumidityThreshold > 100)
    {
        SetDefaultThresholds(); // ���������Ч������ΪĬ��ֵ
    }
}

// ����Ĭ����ֵ
void SetDefaultThresholds(void)
{
    thresholdData.TemperatureThreshold = 30; // Ĭ���¶���ֵ30��C
    thresholdData.HumidityThreshold = 70;    // Ĭ��ʪ����ֵ70%
    
}
