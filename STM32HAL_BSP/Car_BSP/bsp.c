#include "bsp.h"

int16_t Servo1 = 0;
int16_t Servo2 = 0;

void motor(int16_t Lspeed, int16_t Rspeed){
    //最低速度8500 最大速度19999
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
    TIM4->CCR3 = Rspeed;
    if(Rspeed < 0){//后退
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
    }
    else{//前进
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
    }
    TIM4->CCR4 = Lspeed;
    if(Lspeed < 0){//后退
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
    }
    else{//前进
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
    }
}
void motor_stop(void){
    HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_3);
    HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_4);
    TIM4->CCR4 = 0;
    TIM4->CCR3 = 0;
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
}


void Servo1_Angle(int16_t angle){
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
  if(angle > 180) angle = 180;
  if(angle < 0) angle = 0;
  TIM4->CCR3 = 500 + (angle/180.0*2000);
  Servo1 = angle;
}
void Servo2_Angle(int16_t angle){
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
  if(angle > 180) angle = 180;
  if(angle < 0) angle = 0;
  TIM4->CCR4 = 500 + (angle/180.0*2000);
  Servo2 = angle;
}


