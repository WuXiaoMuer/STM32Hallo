
#include "SL_SC7A20H_driver.h"

#define SL_SC7A20H_FIFO_CTRL_REG   (unsigned char)0X2E
#define SL_SC7A20H_FIFO_SRC_REG    (unsigned char)0X2F
#define SL_SC7A20H_SPI_OUT_X_L     (unsigned char)0X27
#define SL_SC7A20H_IIC_OUT_X_L     (unsigned char)0XA8

static unsigned char  SL_SPI_IIC_INTERFACE         = 0;

#define SL_SC7A20H_INIT_REG1_NUM 8
static unsigned char SL_SC7A20H_INIT_REG1[SL_SC7A20H_INIT_REG1_NUM*3]=
{
		0x24,0x80,0x00,
    0x2E,0x00,0x00,
    0x1f,0x00,0x00,
    0x20,0x47,0x00,
    0x21,0x70,0x00,   
    0x23,0x98,0x00,
    0x24,0x40,0x00,
    0x2E,0x4F,0x00,
};

signed char SL_SC7A20H_Driver_Init(unsigned char Sl_spi_iic_init,unsigned char Sl_pull_up_mode)
{
	unsigned char i=0;
	unsigned char id1=0;	
	unsigned char id2=0;	

	if(Sl_spi_iic_init==0)
		SL_SPI_IIC_INTERFACE  = 0;//spi
	else
		SL_SPI_IIC_INTERFACE  = 1;//iic

	SL_SC7A20H_I2c_Spi_Read(SL_SPI_IIC_INTERFACE, 0x0f, 1, &id1);
	SL_SC7A20H_I2c_Spi_Read(SL_SPI_IIC_INTERFACE, 0x70, 1, &id2);
	
	if ((id1 != 0x11) || (id2 != 0x28))
 	  return 0;
		
	SL_SC7A20H_I2c_Spi_Write(SL_SPI_IIC_INTERFACE, 0x57, Sl_pull_up_mode);		
    
  for(i=0;i<SL_SC7A20H_INIT_REG1_NUM;i++)
  {
    SL_SC7A20H_I2c_Spi_Write(SL_SPI_IIC_INTERFACE, SL_SC7A20H_INIT_REG1[3*i], SL_SC7A20H_INIT_REG1[3*i+1]);
  }  

  for(i=0;i<SL_SC7A20H_INIT_REG1_NUM;i++)
  {
    SL_SC7A20H_I2c_Spi_Read(SL_SPI_IIC_INTERFACE, SL_SC7A20H_INIT_REG1[3*i], 1, &SL_SC7A20H_INIT_REG1[3*i+2]);
  }

  for(i=2;i<SL_SC7A20H_INIT_REG1_NUM;i++)
  {
    if(SL_SC7A20H_INIT_REG1[3*i+1]!=SL_SC7A20H_INIT_REG1[3*i+2]) break;
  }  

  if(i != SL_SC7A20H_INIT_REG1_NUM)
  {
		if(SL_SPI_IIC_INTERFACE==0)
			return -1;//reg write and read error by SPI
		else
			return -2;//reg write and read error by IIC
  }

  return id2;
}

unsigned char SL_SC7A20H_Read_FIFO_Buf(signed short *x_buf,signed short *y_buf,signed short *z_buf)
{
	unsigned char  i=0;
	unsigned char  sc7a20_data[7];
	unsigned char  SL_FIFO_ACCEL_NUM;

  SL_SC7A20H_I2c_Spi_Read(SL_SPI_IIC_INTERFACE, SL_SC7A20H_FIFO_SRC_REG, 1, &SL_FIFO_ACCEL_NUM); 
    
	if (SL_FIFO_ACCEL_NUM & 0x40)  
		SL_FIFO_ACCEL_NUM = 32;
	else               
		SL_FIFO_ACCEL_NUM = SL_FIFO_ACCEL_NUM&0x1f;   

	for(i=0;i<SL_FIFO_ACCEL_NUM;i++)
	{
		if(SL_SPI_IIC_INTERFACE==0)
			SL_SC7A20H_I2c_Spi_Read(SL_SPI_IIC_INTERFACE, SL_SC7A20H_SPI_OUT_X_L, 7, &sc7a20_data[0]);
		else
			SL_SC7A20H_I2c_Spi_Read(SL_SPI_IIC_INTERFACE, SL_SC7A20H_IIC_OUT_X_L, 6, &sc7a20_data[1]);
			
		x_buf[i] =(signed short int)(((unsigned char)sc7a20_data[2] * 256 ) + (unsigned char)sc7a20_data[1]);
		y_buf[i] =(signed short int)(((unsigned char)sc7a20_data[4] * 256 ) + (unsigned char)sc7a20_data[3]);
		z_buf[i] =(signed short int)(((unsigned char)sc7a20_data[6] * 256 ) + (unsigned char)sc7a20_data[5]);
	}	

	SL_SC7A20H_I2c_Spi_Write(SL_SPI_IIC_INTERFACE, SL_SC7A20H_FIFO_CTRL_REG, 0X00);		
  SL_SC7A20H_I2c_Spi_Write(SL_SPI_IIC_INTERFACE, SL_SC7A20H_FIFO_CTRL_REG, 0X4F);

  return SL_FIFO_ACCEL_NUM;
}


