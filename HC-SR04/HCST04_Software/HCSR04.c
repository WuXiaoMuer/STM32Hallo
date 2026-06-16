/*
 *	HCSR04.c
 *	HC-SR04超声波驱动.c文件
 *	2026/5/8 v0.1
 *	Ancho.YF
 */
#include "HCSR04.h"
#include "stm32f1xx_hal.h"

#define HCSR04_TRIG_PIN GPIO_PIN_5
#define HCSR04_TRIG_PORT GPIOB
#define HCSR04_ECHO_PIN GPIO_PIN_8
#define HCSR04_ECHO_PORT GPIOB

// 72MHz主频微秒延时
static void HCSR04_Delay_us(uint32_t us)
{
    uint32_t count = us * 72;
    while(count--);
}

void HCSR04_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    // PB5 TRIG 推挽输出
    GPIO_InitStruct.Pin = HCSR04_TRIG_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(HCSR04_TRIG_PORT, &GPIO_InitStruct);
    HAL_GPIO_WritePin(HCSR04_TRIG_PORT, HCSR04_TRIG_PIN, GPIO_PIN_RESET);
    
    // PB8 ECHO 浮空输入
    GPIO_InitStruct.Pin = HCSR04_ECHO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(HCSR04_ECHO_PORT, &GPIO_InitStruct);
}

float HCSR04_GetDistance(void)
{
    uint32_t pulse_us = 0;
    float distance = 0;
    uint32_t timeout;
    
    // 输出10us触发脉冲
    HAL_GPIO_WritePin(HCSR04_TRIG_PORT, HCSR04_TRIG_PIN, GPIO_PIN_SET);
    HCSR04_Delay_us(10);
    HAL_GPIO_WritePin(HCSR04_TRIG_PORT, HCSR04_TRIG_PIN, GPIO_PIN_RESET);

    
    timeout = 50000; // 50ms超时
    while(HAL_GPIO_ReadPin(HCSR04_ECHO_PORT, HCSR04_ECHO_PIN) == GPIO_PIN_RESET)
    {
        HCSR04_Delay_us(1); // 每次循环停顿，timeout快速递减
        timeout--;
        if(timeout == 0)
        {
            return 999.0f; // 无回波直接返回超大值
        }
    }

    // 等待下降沿，统计高电平时长
    timeout = 50000;
    pulse_us = 0;
    while(HAL_GPIO_ReadPin(HCSR04_ECHO_PORT, HCSR04_ECHO_PIN) == GPIO_PIN_SET)
    {
        HCSR04_Delay_us(1);
        pulse_us++;
        timeout--;
        if(timeout == 0)
        {
            return 999.0f;
        }
    }
    
    // 标准测距公式 cm = us / 58
    distance = (float)pulse_us / 58.0f;

    // 有效量程2~400cm，超出返回999
    if(distance < 2 || distance > 400)
        return 999.0f;

    return distance;
}