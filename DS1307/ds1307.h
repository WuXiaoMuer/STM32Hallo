// DS1307.h
#ifndef DS1307_H
#define DS1307_H

#include "main.h"

// DS1307 I2C��ַ
#define DS1307_ADDR                  0xD0

// DS1307�Ĵ�����ַ
#define DS1307_REG_SECONDS           0x00
#define DS1307_REG_MINUTES           0x01
#define DS1307_REG_HOURS             0x02
#define DS1307_REG_DAY               0x03
#define DS1307_REG_DATE              0x04
#define DS1307_REG_MONTH             0x05
#define DS1307_REG_YEAR              0x06
#define DS1307_REG_CONTROL           0x07

// ʱ��ṹ��
typedef struct {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t day;      // ���ڼ� (1-7)
    uint8_t date;     // ���� (1-31)
    uint8_t month;    // �·� (1-12)
    uint8_t year;     // ��� (0-99)
} DS1307_Time_t;

// ��������
HAL_StatusTypeDef DS1307_Init(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef DS1307_ReadTime(I2C_HandleTypeDef *hi2c, DS1307_Time_t *time);
HAL_StatusTypeDef DS1307_WriteTime(I2C_HandleTypeDef *hi2c, DS1307_Time_t *time);
uint8_t DS1307_BCDToDec(uint8_t bcd);
uint8_t DS1307_DecToBCD(uint8_t dec);

#endif /* DS1307_H */
