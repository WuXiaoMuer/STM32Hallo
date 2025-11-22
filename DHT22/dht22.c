#include "dht22.h"
#include <stdint.h>

// 全局变量定义
static GPIO_TypeDef* dht_gpio_port;
static uint16_t dht_gpio_pin;

// 微秒级延时函数(需要根据实际硬件校准)
static void DHT22_DelayUs(uint32_t us)
{
    uint32_t delay = (HAL_RCC_GetHCLKFreq() / 8000000 * us);
    while (delay--) {
        __NOP();
    }
}

// 发送起始信号
static void DHT22_StartSignal(void)
{
    // 配置为输出模式
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = dht_gpio_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(dht_gpio_port, &GPIO_InitStruct);
    
    // 拉低总线至少18ms
    HAL_GPIO_WritePin(dht_gpio_port, dht_gpio_pin, GPIO_PIN_RESET);
    HAL_Delay(20);
    
    // 拉高总线20-40us
    HAL_GPIO_WritePin(dht_gpio_port, dht_gpio_pin, GPIO_PIN_SET);
    DHT22_DelayUs(30);
}

// 检测传感器响应
static uint8_t DHT22_CheckResponse(void)
{
    uint8_t response = 0;
    
    // 配置为输入模式
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = dht_gpio_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(dht_gpio_port, &GPIO_InitStruct);
    
    // 等待总线拉低(最多等待100us)
    DHT22_DelayUs(40);
    if (!HAL_GPIO_ReadPin(dht_gpio_port, dht_gpio_pin)) {
        // 检测到低电平响应
        response = 1;
        
        // 等待低电平结束(最多等待80us)
        while (!HAL_GPIO_ReadPin(dht_gpio_port, dht_gpio_pin));
        
        // 等待高电平结束(最多等待80us)
        while (HAL_GPIO_ReadPin(dht_gpio_port, dht_gpio_pin));
    }
    
    return response;
}

// 读取一个字节数据
static uint8_t DHT22_ReadByte(void)
{
    uint8_t i, data = 0;
    
    for (i = 0; i < 8; i++) {
        // 等待低电平结束
        while (!HAL_GPIO_ReadPin(dht_gpio_port, dht_gpio_pin));
        
        // 延时50us后判断电平状态
        DHT22_DelayUs(50);
        
        // 若仍为高电平则为1，否则为0
        if (HAL_GPIO_ReadPin(dht_gpio_port, dht_gpio_pin)) {
            data |= (1 << (7 - i));
        }
        
        // 等待高电平结束
        while (HAL_GPIO_ReadPin(dht_gpio_port, dht_gpio_pin));
    }
    
    return data;
}

// 初始化DHT22传感器
void DHT22_Init(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    dht_gpio_port = GPIOx;
    dht_gpio_pin = GPIO_Pin;
    
    // 初始化GPIO为输出高电平
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = dht_gpio_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(dht_gpio_port, &GPIO_InitStruct);
    
    HAL_GPIO_WritePin(dht_gpio_port, dht_gpio_pin, GPIO_PIN_SET);
    HAL_Delay(100);  // 等待传感器稳定
}

// 从DHT22读取温湿度数据
void DHT22_ReadData(DHT22_Data_t* data)
{
    uint8_t hum_high, hum_low, temp_high, temp_low, check_sum;
    uint16_t hum_raw, temp_raw;
    
    // 初始化数据为无效
    data->is_valid = 0;
    data->temperature = 0.0f;
    data->humidity = 0.0f;
    
    // 发送起始信号
    DHT22_StartSignal();
    
    // 检测传感器响应
    if (!DHT22_CheckResponse()) {
        return;  // 无响应，数据无效
    }
    
    // 读取40位数据
    hum_high = DHT22_ReadByte();   // 湿度高位
    hum_low = DHT22_ReadByte();    // 湿度低位
    temp_high = DHT22_ReadByte();  // 温度高位
    temp_low = DHT22_ReadByte();   // 温度低位
    check_sum = DHT22_ReadByte();  // 校验和
    
    // 验证校验和
    if (check_sum != (hum_high + hum_low + temp_high + temp_low)) {
        return;  // 校验失败，数据无效
    }
    
    // 计算湿度值(0-100%RH，精度0.1%)
    hum_raw = (hum_high << 8) | hum_low;
    data->humidity = hum_raw / 10.0f;
    
    // 计算温度值(-40~80℃，精度0.1℃)
    temp_raw = (temp_high << 8) | temp_low;
    if (temp_raw & 0x8000) {  // 负温度
        data->temperature = -((temp_raw & 0x7FFF) / 10.0f);
    } else {  // 正温度
        data->temperature = temp_raw / 10.0f;
    }
    
    // 标记数据有效
    data->is_valid = 1;
}
