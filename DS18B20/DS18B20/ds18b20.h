#ifndef __DS18B20_H
#define __DS18B20_H
#include "stm32f1xx_hal.h"
#define DS18B20_PORT GPIOA
#define DS18B20_PIN  GPIO_PIN_7
void DS18B20_Init(void);
float DS18B20_ReadTemp(void);
int8_t DS18B20_Diag(void);
#endif