
#ifndef BMP280_H
#define BMP280_H


#include "i2c.h"
#include <stdint.h>

#define BMP280_I2C &hi2c1

HAL_StatusTypeDef BMP280_Init();
HAL_StatusTypeDef BMP280_Read(int32_t* temp, int32_t* press);


#endif //BMP280_H