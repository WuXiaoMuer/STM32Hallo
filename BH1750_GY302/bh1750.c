#include "bh1750.h"

extern I2C_HandleTypeDef hi2c1;


void BH1750_Init(void){
    uint8_t cmd = 0x01;//上电
    HAL_I2C_Master_Transmit(&hi2c1, BH1750_ADDR, &cmd, 1, HAL_MAX_DELAY);
    HAL_Delay(10);
}

float BH1750_ReadLux(void){
    uint8_t buf[2];
    uint8_t cmd = 0x10; //连续模式
    HAL_I2C_Master_Transmit(&hi2c1, BH1750_ADDR, &cmd, 1, 100);
    HAL_Delay(180);
    if(HAL_I2C_Master_Receive(&hi2c1, BH1750_ADDR, buf, 2, HAL_MAX_DELAY) == HAL_OK){
        uint16_t raw = (buf[0] << 8) | buf[1];
        return (float)raw / 1.2;
    }
    return -1;
}
