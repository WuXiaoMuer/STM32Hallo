#ifndef __DHT11_H
#define __DHT11_H

#include "stm32f1xx_hal.h"  // 根据实际芯片型号调整（如stm32f4xx_hal.h）

// DHT11数据结构
typedef struct {
    GPIO_TypeDef *GPIOx;    // GPIO端口（如GPIOA）
    uint16_t GPIO_Pin;      // GPIO引脚（如GPIO_PIN_0）
    uint8_t Temperature;    // 温度（整数部分，°C）
    uint8_t Humidity;       // 湿度（整数部分，%）
} DHT11_TypeDef;

// 函数声明
void DHT11_Init(DHT11_TypeDef *DHT11, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
uint8_t DHT11_ReadData(DHT11_TypeDef *DHT11);

// 微秒级延时函数（需用户实现，或使用下方dht11.c中的模板）
void delay_us(uint16_t us);

#endif /* __DHT11_H */
