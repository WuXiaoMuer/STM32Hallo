#include "stm32f10x.h"

//MCU片上外设定义
#define SH1107_GPIO GPIOB
#define SH1107_I2C I2C1
#define SH1107_GPIO_RCC RCC_APB2Periph_GPIOB
#define SH1107_I2C_RCC RCC_APB1Periph_I2C1
#define SH1107_SCL GPIO_Pin_6
#define SH1107_SDA GPIO_Pin_7

//器件地址
#define sh1107_address 0x78

void sh1107_i2c_initialization(void);
void sh1107_i2c_start_write(void);
void sh1107_i2c_stop(void);
void sh1107_i2c_write_byte(u8 data);
void sh1107_write_one_byte_command(u8 command);
void sh1107_write_two_bytes_command(u8 command1, u8 command2);
void sh1107_write_display_data(u8 column, u8 page, u8 width, u8 height, char *data);
