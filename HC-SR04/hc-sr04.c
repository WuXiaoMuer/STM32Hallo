/*
 * @Author       : yzy
 * @Date         : 2021-05-31 17:03:23
 * @LastEditors  : yzy
 * @LastEditTime : 2021-06-21 22:16:28
 * @Description  : HC-SR04超声波测距模块HAL库驱动源文件
 * @FilePath     : \BSP_HARDWARE\hc-sr04.c
 */
#include "hc-sr04.h"
void __attribute__((optimize("O0"))) DelayUs(uint32_t us)
{
    for (uint32_t i = 0; i < us; i++)
    {
        __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
    }
}
// 定义HC-SR04 Trig引脚（需根据实际硬件连接修改，与CubeMX配置一致）
#define TRIG_GPIO_Port    GPIOB    // Trig引脚端口
#define TRIG_Pin          GPIO_PIN_10// Trig引脚编号

// 初始化HC-SR04信息结构体
Hcsr04InfoTypeDef Hcsr04Info = {
    .edge_state = 0,
    .tim_overflow_counter = 0,
    .prescaler = 0,
    .period = 0,
    .t1 = 0,
    .t2 = 0,
    .high_level_us = 0,
    .distance = 0.0f,
    .instance = NULL,
    .ic_tim_ch = 0,
    .active_channel = HAL_TIM_ACTIVE_CHANNEL_1
};

/**
 * @description: 超声波模块输入捕获定时器通道初始化
 * @param {TIM_HandleTypeDef} *htim: 定时器句柄
 * @param {uint32_t} Channel: 输入捕获通道
 * @return: 无
 */
void Hcsr04Init(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    // 读取定时器配置参数（CubeMX中需配置为1us计数：Prescaler=72-1，Period=65535）
    Hcsr04Info.prescaler = htim->Init.Prescaler;
    Hcsr04Info.period = htim->Init.Period;
    Hcsr04Info.instance = htim->Instance;
    Hcsr04Info.ic_tim_ch = Channel;

    // 配置定时器活跃通道（根据捕获通道匹配）
    switch(Channel)
    {
        case TIM_CHANNEL_1:
            Hcsr04Info.active_channel = HAL_TIM_ACTIVE_CHANNEL_1;
            break;
        case TIM_CHANNEL_2:
            Hcsr04Info.active_channel = HAL_TIM_ACTIVE_CHANNEL_2;
            break;
        case TIM_CHANNEL_3:
            Hcsr04Info.active_channel = HAL_TIM_ACTIVE_CHANNEL_3;
            break;
        case TIM_CHANNEL_4:
            Hcsr04Info.active_channel = HAL_TIM_ACTIVE_CHANNEL_4;
            break;
        default:
            Hcsr04Info.active_channel = HAL_TIM_ACTIVE_CHANNEL_1;
            break;
    }

    // 启动定时器基础计数中断与输入捕获中断
    HAL_TIM_Base_Start_IT(htim);
    HAL_TIM_IC_Start_IT(htim, Channel);
}

/**
 * @description: 发送HC-SR04触发信号
 * @param: 无
 * @return: 无
 */
void Hcsr04Start(void)
{
    HAL_GPIO_WritePin(TRIG_GPIO_Port, TRIG_Pin, GPIO_PIN_SET);
    DelayUs(10);  // 输出10us高电平（满足模块触发要求）
    HAL_GPIO_WritePin(TRIG_GPIO_Port, TRIG_Pin, GPIO_PIN_RESET);
}

/**
 * @description: 定时器计数溢出中断处理函数
 * @param {TIM_HandleTypeDef} *htim: 定时器句柄
 * @return: 无
 */
void Hcsr04TimOverflowIsr(TIM_HandleTypeDef *htim)
{
    // 仅处理关联定时器的溢出中断
    if(htim->Instance == Hcsr04Info.instance)
    {
        Hcsr04Info.tim_overflow_counter++;
    }
}

/**
 * @description: 输入捕获中断处理函数（计算距离：距离 = 高电平时间×声速÷2÷10000（转换为cm））
 * @param {TIM_HandleTypeDef} *htim: 定时器句柄
 * @return: 无
 */
void Hcsr04TimIcIsr(TIM_HandleTypeDef* htim)
{
    // 仅处理关联定时器的指定捕获通道中断
    if((htim->Instance == Hcsr04Info.instance) && (htim->Channel == Hcsr04Info.active_channel))
    {
        if(Hcsr04Info.edge_state == 0)  // 捕获上升沿（Echo引脚开始输出高电平）
        {
            Hcsr04Info.t1 = HAL_TIM_ReadCapturedValue(htim, Hcsr04Info.ic_tim_ch);
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, Hcsr04Info.ic_tim_ch, TIM_INPUTCHANNELPOLARITY_FALLING); // 切换为下降沿捕获
            Hcsr04Info.tim_overflow_counter = 0;  // 溢出计数器清零
            Hcsr04Info.edge_state = 1;            // 标记为等待下降沿
        }
        else if(Hcsr04Info.edge_state == 1)  // 捕获下降沿（Echo引脚结束输出高电平）
        {
            // 读取下降沿时间，叠加溢出次数对应的时间（避免定时器溢出导致计算错误）
            Hcsr04Info.t2 = HAL_TIM_ReadCapturedValue(htim, Hcsr04Info.ic_tim_ch) + Hcsr04Info.tim_overflow_counter * Hcsr04Info.period;
            Hcsr04Info.high_level_us = Hcsr04Info.t2 - Hcsr04Info.t1;  // 计算高电平持续时间（微秒）
            
            // 计算距离（声速按340m/s计算，未启用温度补偿；需补偿时需替换声速公式）
            Hcsr04Info.distance = (Hcsr04Info.high_level_us / 1000000.0f) * 340.0f / 2.0f * 100.0f;
            
            // 恢复为上升沿捕获，准备下一次测量
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, Hcsr04Info.ic_tim_ch, TIM_INPUTCHANNELPOLARITY_RISING);
            Hcsr04Info.edge_state = 0;  // 重置状态
        }
    }
}

/**
 * @description: 读取当前测距结果（限幅处理：模块最大测量距离450cm）
 * @param: 无
 * @return {float}: 测距结果（单位：cm）
 */
float Hcsr04Read(void)
{
    if(Hcsr04Info.distance >= 450.0f)
    {
        Hcsr04Info.distance = 450.0f;
    }
    return Hcsr04Info.distance;
}