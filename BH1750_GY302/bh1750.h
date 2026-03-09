#ifndef __BH1750_H__
#define __BH1750_H__

#include "main.h"

//ADDR GND 0x23 = 0x46
//ADDR VCC 0x5C = 0x47
//ADDR SDA 0x39 = 0x72
#define BH1750_ADDR 0x46

void BH1750_Init(void);
float BH1750_ReadLux(void);

#endif