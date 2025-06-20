// DS1307.h
#ifndef DS1307_H
#define DS1307_H

#include "main.h"

// DS1307 I2C地址
#define DS1307_ADDR                  0xD0

// DS1307寄存器地址
#define DS1307_REG_SECONDS           0x00
#define DS1307_REG_MINUTES           0x01
#define DS1307_REG_HOURS             0x02
#define DS1307_REG_DAY               0x03
#define DS1307_REG_DATE              0x04
#define DS1307_REG_MONTH             0x05
#define DS1307_REG_YEAR              0x06
#define DS1307_REG_CONTROL           0x07

// 时间结构体
typedef struct {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t day;      // 星期几 (1-7)
    uint8_t date;     // 日期 (1-31)
    uint8_t month;    // 月份 (1-12)
    uint8_t year;     // 年份 (0-99)
} DS1307_Time_t;

// 函数声明
HAL_StatusTypeDef DS1307_Init(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef DS1307_ReadTime(I2C_HandleTypeDef *hi2c, DS1307_Time_t *time);
HAL_StatusTypeDef DS1307_WriteTime(I2C_HandleTypeDef *hi2c, DS1307_Time_t *time);
uint8_t DS1307_BCDToDec(uint8_t bcd);
uint8_t DS1307_DecToBCD(uint8_t dec);

#endif /* DS1307_H */
