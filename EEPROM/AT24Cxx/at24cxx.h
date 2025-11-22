#include "main.h"
#include "i2c.h"

extern I2C_HandleTypeDef hi2c1;
#define AT24CXX_HANDLE (&hi2c1)
#define AT24C_DEV_ADDR (0xA0)

#define AT24C02_ADDR_READ 0xA0
#define AT24C02_ADDR_WRITE 0xA0

#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64	  8191
#define AT24C128	16383
#define AT24C256	32767  
//我使用的是AT24C02
#define EE_TYPE AT24C64

#define USE_HARDWARE_IIC 1

void AT24CXX_Init(void);

void AT24CXX_Write(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite);

void AT24CXX_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead);

uint8_t AT24CXX_ReadOneByte(uint16_t ReadAddr);
void AT24CXX_WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite);

uint8_t AT24CXX_Check(void);

uint8_t At24c02_Read_Byte(uint16_t addr, uint8_t* read_buf);
uint8_t At24c02_Write_Byte(uint16_t addr, uint8_t* dat);
