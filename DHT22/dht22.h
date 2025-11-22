#ifndef __DHT22_H
#define __DHT22_H

#include "stm32f1xx_hal.h"  // 根据实际使用的HAL库修改

// DHT22数据结构体
typedef struct {
    float temperature;  // 温度值(℃)
    float humidity;     // 湿度值(%RH)
    uint8_t is_valid;   // 数据是否有效(1:有效, 0:无效)
} DHT22_Data_t;

// 初始化DHT22传感器
void DHT22_Init(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

// 从DHT22读取温湿度数据
void DHT22_ReadData(DHT22_Data_t* data);

#endif /* __DHT22_H */
