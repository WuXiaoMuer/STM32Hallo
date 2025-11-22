#ifndef __DS18B20_H
#define __DS18B20_H

extern uint8_t DS18B20_ROM_Address[8];

void DS18B20_Init(void);
void DS18B20_ConvertT(void);
float DS18B20_ReadT(void);
void DS18B20_ReadROM(void);
uint8_t DS18B20_SearchROM(uint8_t Split_Point);

#endif
