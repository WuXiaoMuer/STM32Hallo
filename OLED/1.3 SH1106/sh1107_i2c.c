#include "sh1107_i2c.h"


void sh1107_i2c_initialization(void)             //I2C初始化
{
  I2C_InitTypeDef  I2C_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  // Enable I2C and GPIO clocks
  RCC_APB2PeriphClockCmd(SH1107_GPIO_RCC, ENABLE);

  RCC_APB1PeriphClockCmd(SH1107_I2C_RCC, ENABLE);
 
  
  // configure SH1107 I2C
  GPIO_InitStructure.GPIO_Pin = SH1107_SCL | SH1107_SDA;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_Init(SH1107_GPIO, &GPIO_InitStructure);
	
	
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x55;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 200000;
	
	I2C_Init(SH1107_I2C, &I2C_InitStructure);
	
	I2C_Cmd(SH1107_I2C, ENABLE);
}

void sh1107_i2c_start_write(void)           //产生I2C开始信号并写入器件地址和写命令
{
	I2C_AcknowledgeConfig(SH1107_I2C, ENABLE);
	I2C_GenerateSTART(SH1107_I2C, ENABLE);
	while(!I2C_CheckEvent(SH1107_I2C, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(SH1107_I2C, sh1107_address, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(SH1107_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
}

void sh1107_i2c_stop(void)                  //产生I2C结束信号
{
	I2C_GenerateSTOP(SH1107_I2C, ENABLE);
}

void sh1107_i2c_write_byte(u8 data)         //I2C写入一字节数据
{
	I2C_SendData(SH1107_I2C, data);
	while(!I2C_CheckEvent(SH1107_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
}



void sh1107_write_one_byte_command(u8 command)
{
	sh1107_i2c_start_write();
	sh1107_i2c_write_byte(0x80);          //控制命令，下面要写入一字节操作
	sh1107_i2c_write_byte(command);
	sh1107_i2c_stop();
}

void sh1107_write_two_bytes_command(u8 command1, u8 command2)
{
	sh1107_i2c_start_write();
	sh1107_i2c_write_byte(0x00);          //控制命令，下面要写入多字节操作
	sh1107_i2c_write_byte(command1);
	sh1107_i2c_write_byte(command2);
	sh1107_i2c_stop();
}

//写入图像函数
//输入参数依次为，起始列，起始页，图像所占列数，图像所占页数和图像数据的指针地址
void sh1107_write_display_data(u8 column, u8 page, u8 columns, u8 pages, char *data)
{
	sh1107_write_one_byte_command(column & 0x0f);          //设置列地址低四位
	sh1107_write_one_byte_command((column>>4) | 0x10);     //设置列地址高三位
	while(pages)
	{
		char i = columns;
		sh1107_write_one_byte_command(page | 0xb0);          //设置页地址
		sh1107_write_one_byte_command(0xe0);                 //开始写入数据
		sh1107_i2c_start_write();
		sh1107_i2c_write_byte(0x40);                         //控制命令，下面要写入多字节数据
		while (i)
		{
			sh1107_i2c_write_byte(*data);
			i--;
			data++;
		}
		sh1107_i2c_stop();
		sh1107_write_one_byte_command(0xee);                 //停止写入数据
		pages--;
		page++;
	}
}
