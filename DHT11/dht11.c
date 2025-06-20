#include "dht11.h"
#include <stdint.h>

// ˽�к�������
static uint8_t DHT11_Start(DHT11_TypeDef *DHT11);
static uint8_t DHT11_ReadByte(DHT11_TypeDef *DHT11);
static void Set_Pin_Output(DHT11_TypeDef *DHT11);
static void Set_Pin_Input(DHT11_TypeDef *DHT11);

// ��ʼ��DHT11
void DHT11_Init(DHT11_TypeDef *DHT11, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
    DHT11->GPIOx = GPIOx;
    DHT11->GPIO_Pin = GPIO_Pin;
    DHT11->Temperature = 0;
    DHT11->Humidity = 0;
}

// ��ȡ��ʪ�����ݣ�����0�ɹ���1ʧ�ܣ�
uint8_t DHT11_ReadData(DHT11_TypeDef *DHT11) {
    uint8_t data[5] = {0};
    
    if (DHT11_Start(DHT11) != 0) return 1; // ����ʧ��
    
    // ��ȡ40λ���ݣ�5�ֽڣ�
    for (int i = 0; i < 5; i++) {
        data[i] = DHT11_ReadByte(DHT11);
    }
    
    // У�����֤
    if ((data[0] + data[1] + data[2] + data[3]) != data[4]) {
        return 1; // У��ʧ��
    }
    
    DHT11->Humidity = data[0];
    DHT11->Temperature = data[2];
    return 0;
}

// ����DHT11�����Ϳ�ʼ�źţ�
static uint8_t DHT11_Start(DHT11_TypeDef *DHT11) {
    Set_Pin_Output(DHT11);
    
    // ������������18ms
    HAL_GPIO_WritePin(DHT11->GPIOx, DHT11->GPIO_Pin, GPIO_PIN_RESET);
    delay_us(18000);
    
    // ��������20-40��s
    HAL_GPIO_WritePin(DHT11->GPIOx, DHT11->GPIO_Pin, GPIO_PIN_SET);
    delay_us(30);
    
    // �л�Ϊ����ģʽ���ȴ�DHT11��Ӧ
    Set_Pin_Input(DHT11);
    
    // ���DHT11�ĵ͵�ƽ��Ӧ�źţ���ʱ��⣩
    uint32_t timeout = 10000; // 10ms��ʱ
    while (HAL_GPIO_ReadPin(DHT11->GPIOx, DHT11->GPIO_Pin) == GPIO_PIN_RESET) {
        if (--timeout == 0) return 1;
        delay_us(1);
    }
    
    // ���DHT11�ĸߵ�ƽ�źţ���ʱ��⣩
    timeout = 10000;
    while (HAL_GPIO_ReadPin(DHT11->GPIOx, DHT11->GPIO_Pin) == GPIO_PIN_SET) {
        if (--timeout == 0) return 1;
        delay_us(1);
    }
    
    return 0; // �����ɹ�
}

// ��ȡһ���ֽڵ�����
static uint8_t DHT11_ReadByte(DHT11_TypeDef *DHT11) {
    uint8_t byte = 0;
    
    for (int i = 0; i < 8; i++) {
        // �ȴ��͵�ƽ����������λ��ʼ��
        while (HAL_GPIO_ReadPin(DHT11->GPIOx, DHT11->GPIO_Pin) == GPIO_PIN_RESET);
        
        // ��ʱ40��s�жϸߵ͵�ƽ��0: 26-28��s, 1: 70��s��
        delay_us(40);
        if (HAL_GPIO_ReadPin(DHT11->GPIOx, DHT11->GPIO_Pin) == GPIO_PIN_SET) {
            byte |= (1 << (7 - i)); // ��λ��ǰ
        }
        
        // �ȴ��ߵ�ƽ����
        while (HAL_GPIO_ReadPin(DHT11->GPIOx, DHT11->GPIO_Pin) == GPIO_PIN_SET);
    }
    
    return byte;
}

// ��������Ϊ���ģʽ
static void Set_Pin_Output(DHT11_TypeDef *DHT11) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DHT11->GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(DHT11->GPIOx, &GPIO_InitStruct);
}

// ��������Ϊ����ģʽ
static void Set_Pin_Input(DHT11_TypeDef *DHT11) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DHT11->GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(DHT11->GPIOx, &GPIO_InitStruct);
}

// ΢�뼶��ʱ���������û�����ʵ�ʶ�ʱ��ʵ�֣�
__weak void delay_us(uint16_t us) {
    /* Ĭ��ʵ�֣����滻Ϊ����ʵ����ʱ������ */
    for (uint16_t i = 0; i < us; i++) {
        for (uint16_t j = 0; j < 10; j++) { // ������ʱ����У׼��
            __NOP();
        }
    }
}
