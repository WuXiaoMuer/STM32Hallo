/*
 *	HCSR04.h
 *	HC-SR04���������ģ���.h�ļ�
 *	2026/5/8 v0.1
 *	Ancho.YF
 */
#ifndef _HCSR04_H_
#define _HCSR04_H_

#include "main.h"
#include "stm32f1xx_hal.h"

void HCSR04_Init(void);

float HCSR04_GetDistance(void);

#endif
