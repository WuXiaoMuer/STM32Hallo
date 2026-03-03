#ifndef __BSP_H__
#define __BSP_H__

#include "app.h"

extern int16_t Servo1;
extern int16_t Servo2;
// 电机控制函数
void motor(int16_t Lspeed, int16_t Rspeed);
void motor_stop(void);
void Servo1_Angle(int16_t angle);
void Servo2_Angle(int16_t angle);
#endif