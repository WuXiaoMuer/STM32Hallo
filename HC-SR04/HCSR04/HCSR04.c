/*
 *	HCSR04.c
 *	HC-SR04���������ģ���.c�ļ�
 *	2026/5/8 v0.1
 *	Ancho.YF
 */
#include "HCSR04.h"
#include "tim.h"

#define HCSR04_TRIG_PIN GPIO_PIN_5
#define HCSR04_TRIG_PORT GPIOA
#define HCSR04_ECHO_PIN GPIO_PIN_6
#define HCSR04_ECHO_PORT GPIOA

void HCSR04_Delay_us(uint32_t us)
{
    uint32_t count = us * 72;
    while(count--);
}

void HCSR04_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    GPIO_InitStruct.Pin = HCSR04_TRIG_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(HCSR04_TRIG_PORT, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = HCSR04_ECHO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(HCSR04_ECHO_PORT, &GPIO_InitStruct);
    
    HAL_TIM_Base_Start(&htim3);
}

float HCSR04_GetDistance(void)
{
    uint32_t time_start = 0;
    uint32_t time_end = 0;
    float distance = 0;
    uint32_t timeout = 0;
    
    HAL_GPIO_WritePin(HCSR04_TRIG_PORT, HCSR04_TRIG_PIN, GPIO_PIN_SET);
    HCSR04_Delay_us(10);
    HAL_GPIO_WritePin(HCSR04_TRIG_PORT, HCSR04_TRIG_PIN, GPIO_PIN_RESET);
    
    timeout = 0;
    while(HAL_GPIO_ReadPin(HCSR04_ECHO_PORT, HCSR04_ECHO_PIN) == GPIO_PIN_RESET)
    {
        timeout++;
        if(timeout > 100000) return -1;
    }
    time_start = __HAL_TIM_GET_COUNTER(&htim3);
    
    timeout = 0;
    while(HAL_GPIO_ReadPin(HCSR04_ECHO_PORT, HCSR04_ECHO_PIN) == GPIO_PIN_SET)
    {
        timeout++;
        if(timeout > 100000) return -1;
    }
    time_end = __HAL_TIM_GET_COUNTER(&htim3);
    
    uint32_t pulse_time = time_end - time_start;
    distance = (pulse_time * 0.034) / 2;
    
    return distance;
}
