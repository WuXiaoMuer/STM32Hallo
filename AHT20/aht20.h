/*
 *	at20.h
 *	at20温湿度传感器的.h文件
 *	2026/5/7 v0.1
 *	Ancho.YF
 */
#ifndef _AHT20_H_
#define _AHT20_H_

#include "i2c.h"

void AHT20_Init();

void AHT20_Read(float *Temperature, float *Humidity);

#endif