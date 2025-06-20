#include "dht11.h"
#include <stdint.h>

// 私有函数声明
static uint8_t DHT11_Start(DHT11_TypeDef *DHT11);
static uint8_t DHT11_ReadByte(DHT11_TypeDef *DHT11);
static void Set_Pin_Output(DHT11_TypeDef *DHT11);
static void Set_Pin_Input(DHT11_TypeDef *DHT11);

// 初始化DHT11
void DHT11_Init(DHT11_TypeDef *DHT11, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
    DHT11->GPIOx = GPIOx;
    DHT11->GPIO_Pin = GPIO_Pin;
    DHT11->Temperature = 0;
    DHT11->Humidity = 0;
}

// 读取温湿度数据（返回0成功，1失败）
uint8_t DHT11_ReadData(DHT11_TypeDef *DHT11) {
    uint8_t data[5] = {0};
    
    if (DHT11_Start(DHT11) != 0) return 1; // 启动失败
    
    // 读取40位数据（5字节）
    for (int i = 0; i < 5; i++) {
        data[i] = DHT11_ReadByte(DHT11);
    }
    
    // 校验和验证
    if ((data[0] + data[1] + data[2] + data[3]) != data[4]) {
        return 1; // 校验失败
    }
    
    DHT11->Humidity = data[0];
    DHT11->Temperature = data[2];
    return 0;
}

// 启动DHT11（发送开始信号）
static uint8_t DHT11_Start(DHT11_TypeDef *DHT11) {
    Set_Pin_Output(DHT11);
    
    // 主机拉低至少18ms
    HAL_GPIO_WritePin(DHT11->GPIOx, DHT11->GPIO_Pin, GPIO_PIN_RESET);
    delay_us(18000);
    
    // 主机拉高20-40μs
    HAL_GPIO_WritePin(DHT11->GPIOx, DHT11->GPIO_Pin, GPIO_PIN_SET);
    delay_us(30);
    
    // 切换为输入模式，等待DHT11响应
    Set_Pin_Input(DHT11);
    
    // 检测DHT11的低电平响应信号（超时检测）
    uint32_t timeout = 10000; // 10ms超时
    while (HAL_GPIO_ReadPin(DHT11->GPIOx, DHT11->GPIO_Pin) == GPIO_PIN_RESET) {
        if (--timeout == 0) return 1;
        delay_us(1);
    }
    
    // 检测DHT11的高电平信号（超时检测）
    timeout = 10000;
    while (HAL_GPIO_ReadPin(DHT11->GPIOx, DHT11->GPIO_Pin) == GPIO_PIN_SET) {
        if (--timeout == 0) return 1;
        delay_us(1);
    }
    
    return 0; // 启动成功
}

// 读取一个字节的数据
static uint8_t DHT11_ReadByte(DHT11_TypeDef *DHT11) {
    uint8_t byte = 0;
    
    for (int i = 0; i < 8; i++) {
        // 等待低电平结束（数据位开始）
        while (HAL_GPIO_ReadPin(DHT11->GPIOx, DHT11->GPIO_Pin) == GPIO_PIN_RESET);
        
        // 延时40μs判断高低电平（0: 26-28μs, 1: 70μs）
        delay_us(40);
        if (HAL_GPIO_ReadPin(DHT11->GPIOx, DHT11->GPIO_Pin) == GPIO_PIN_SET) {
            byte |= (1 << (7 - i)); // 高位在前
        }
        
        // 等待高电平结束
        while (HAL_GPIO_ReadPin(DHT11->GPIOx, DHT11->GPIO_Pin) == GPIO_PIN_SET);
    }
    
    return byte;
}

// 设置引脚为输出模式
static void Set_Pin_Output(DHT11_TypeDef *DHT11) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DHT11->GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(DHT11->GPIOx, &GPIO_InitStruct);
}

// 设置引脚为输入模式
static void Set_Pin_Input(DHT11_TypeDef *DHT11) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DHT11->GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(DHT11->GPIOx, &GPIO_InitStruct);
}

// 微秒级延时函数（需用户根据实际定时器实现）
__weak void delay_us(uint16_t us) {
    /* 默认实现（需替换为您的实际延时函数） */
    for (uint16_t i = 0; i < us; i++) {
        for (uint16_t j = 0; j < 10; j++) { // 粗略延时（需校准）
            __NOP();
        }
    }
}
