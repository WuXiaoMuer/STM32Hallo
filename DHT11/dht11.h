#ifndef __DHT11_H
#define __DHT11_H

#include "stm32f1xx_hal.h"  // ����ʵ��оƬ�ͺŵ�������stm32f4xx_hal.h��

// DHT11���ݽṹ
typedef struct {
    GPIO_TypeDef *GPIOx;    // GPIO�˿ڣ���GPIOA��
    uint16_t GPIO_Pin;      // GPIO���ţ���GPIO_PIN_0��
    uint8_t Temperature;    // �¶ȣ��������֣���C��
    uint8_t Humidity;       // ʪ�ȣ��������֣�%��
} DHT11_TypeDef;

// ��������
void DHT11_Init(DHT11_TypeDef *DHT11, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
uint8_t DHT11_ReadData(DHT11_TypeDef *DHT11);

// ΢�뼶��ʱ���������û�ʵ�֣���ʹ���·�dht11.c�е�ģ�壩
void delay_us(uint16_t us);

#endif /* __DHT11_H */
