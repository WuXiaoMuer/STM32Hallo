#ifndef __OneWire_H
#define __OneWire_H

void OneWire_Init(void);
uint8_t OneWire_Start(void);
void OneWire_SendBit(uint8_t Bit);
uint8_t OneWire_ReceiveBit(void);
void OneWire_SendByte(uint8_t Byte);
uint8_t OneWire_ReceiveByte(void);

#endif
