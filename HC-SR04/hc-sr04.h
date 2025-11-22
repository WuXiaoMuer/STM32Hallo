/*
 * @Author       : yzy
 * @Date         : 2021-05-31 17:03:27
 * @LastEditors  : yzy
 * @LastEditTime : 2021-06-21 22:16:28
 * @Description  : HC-SR04超声波测距模块HAL库驱动头文件
 * @FilePath     : \BSP_HARDWARE\hc-sr04.h
 */
#ifndef HCSR04_H_
#define HCSR04_H_

#include "main.h"


//温度补偿

// HC-SR04模块信息结构体，存储定时器参数、捕获时间及测距结果
typedef struct
{
    uint8_t  edge_state;          // 边沿捕获状态：0-等待上升沿，1-等待下降沿
    uint16_t tim_overflow_counter;// 定时器溢出计数器
    uint32_t prescaler;           // 定时器预分频系数
    uint32_t period;              // 定时器自动重装载值
    uint32_t t1;                  // 上升沿捕获时间
    uint32_t t2;                  // 下降沿捕获时间
    uint32_t high_level_us;       // Echo引脚高电平持续时间（微秒）
    float    distance;            // 测距结果（厘米）
    TIM_TypeDef* instance;        // 关联的定时器实例（如TIM2）
    uint32_t ic_tim_ch;           // 输入捕获通道（如TIM_CHANNEL_4）
    HAL_TIM_ActiveChannel active_channel;// 定时器活跃通道
}Hcsr04InfoTypeDef;

extern Hcsr04InfoTypeDef Hcsr04Info;  // 外部声明，供其他文件调用

/**
 * @description: 超声波模块输入捕获定时器通道初始化
 * @param {TIM_HandleTypeDef} *htim: 定时器句柄（需提前在CubeMX中配置）
 * @param {uint32_t} Channel: 输入捕获通道（如TIM_CHANNEL_4）
 * @return: 无
 */
void Hcsr04Init(TIM_HandleTypeDef *htim, uint32_t Channel);

/**
 * @description: 发送HC-SR04触发信号（Trig引脚输出≥10us高电平）
 * @param: 无
 * @return: 无
 */
void Hcsr04Start(void);

/**
 * @description: 定时器计数溢出中断处理函数（需在main.c中重定义中断回调函数调用）
 * @param {TIM_HandleTypeDef} *htim: 触发溢出中断的定时器句柄
 * @return: 无
 */
void Hcsr04TimOverflowIsr(TIM_HandleTypeDef *htim);

/**
 * @description: 输入捕获中断处理函数（计算高电平时间与距离，需在main.c中重定义中断回调函数调用）
 * @param {TIM_HandleTypeDef} *htim: 触发捕获中断的定时器句柄
 * @return: 无
 */
void Hcsr04TimIcIsr(TIM_HandleTypeDef* htim);

/**
 * @description: 读取当前测距结果（限幅：超过450cm时返回450cm）
 * @param: 无
 * @return {float}: 测距结果（单位：cm，精度±0.1cm）
 */
float Hcsr04Read(void);

#endif /* HCSR04_H_ */