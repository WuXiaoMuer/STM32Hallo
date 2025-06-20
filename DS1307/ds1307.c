// DS1307.c
#include "ds1307.h"
#include "main.h"

// BCD码转十进制
uint8_t DS1307_BCDToDec(uint8_t bcd) {
    return ((bcd / 16) * 10) + (bcd % 16);
}

// 十进制转BCD码
uint8_t DS1307_DecToBCD(uint8_t dec) {
    return ((dec / 10) * 16) + (dec % 10);
}

// 初始化DS1307
HAL_StatusTypeDef DS1307_Init(I2C_HandleTypeDef *hi2c) {
    uint8_t control = 0x00; // 基本配置，无方波输出
    
    // 写入控制寄存器
    return HAL_I2C_Mem_Write(hi2c, DS1307_ADDR, DS1307_REG_CONTROL, 
                             I2C_MEMADD_SIZE_8BIT, &control, 1, 100);
}

// 读取DS1307时间
HAL_StatusTypeDef DS1307_ReadTime(I2C_HandleTypeDef *hi2c, DS1307_Time_t *time) {
    uint8_t buffer[7];
    
    // 从秒寄存器开始读取7个字节
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(hi2c, DS1307_ADDR, DS1307_REG_SECONDS, 
                                              I2C_MEMADD_SIZE_8BIT, buffer, 7, 100);
    
    if (status == HAL_OK) {
        // 转换数据格式
        time->seconds = DS1307_BCDToDec(buffer[0] & 0x7F); // 忽略CH位
        time->minutes = DS1307_BCDToDec(buffer[1]);
        time->hours = DS1307_BCDToDec(buffer[2] & 0x3F);  // 24小时制
        time->day = DS1307_BCDToDec(buffer[3]);
        time->date = DS1307_BCDToDec(buffer[4]);
        time->month = DS1307_BCDToDec(buffer[5]);
        time->year = DS1307_BCDToDec(buffer[6]);
    }
    
    return status;
}

// 设置DS1307时间
HAL_StatusTypeDef DS1307_WriteTime(I2C_HandleTypeDef *hi2c, DS1307_Time_t *time) {
    uint8_t buffer[8];
    
    // 准备数据，注意BCD格式
    buffer[0] = DS1307_REG_SECONDS; // 起始寄存器地址
    buffer[1] = DS1307_DecToBCD(time->seconds);
    buffer[2] = DS1307_DecToBCD(time->minutes);
    buffer[3] = DS1307_DecToBCD(time->hours);
    buffer[4] = DS1307_DecToBCD(time->day);
    buffer[5] = DS1307_DecToBCD(time->date);
    buffer[6] = DS1307_DecToBCD(time->month);
    buffer[7] = DS1307_DecToBCD(time->year);
    
    // 写入数据
    return HAL_I2C_Master_Transmit(hi2c, DS1307_ADDR, buffer, 8, 100);
}
