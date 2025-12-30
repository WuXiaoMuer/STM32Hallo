#include "as5600.h"




unsigned char write_reg(unsigned char reg, unsigned char value)
{
	return HAL_I2C_Mem_Write(&hi2c1, Slave_Addr, reg, I2C_MEMADD_SIZE_8BIT, &value, 1, 50);
}

unsigned char write_regs(unsigned char reg, unsigned char *value, unsigned char len)
{
	return HAL_I2C_Mem_Write(&hi2c1, Slave_Addr, reg, I2C_MEMADD_SIZE_8BIT, value, len, 50);
}

unsigned char read_reg(unsigned char reg, unsigned char* buf, unsigned short len)
{
	return HAL_I2C_Mem_Read(&hi2c1, Slave_Addr, reg, I2C_MEMADD_SIZE_8BIT, buf, len, 50);
}



float i2c_AS5600_get_angle(void)
{   float angle_d;
	int16_t in_angle;
    uint8_t temp[2]={0,0};
		read_reg( Angle_Hight_Register_Addr, temp, 2);
    //printf("%d  %d\n",temp[0],temp[1]);
    in_angle = ((int16_t)temp[0] <<8) | (temp[1]);
    angle_d = (float)in_angle * cpr / 4096;
		//printf("angle=%.2f\n",angle_d);

		return angle_d;
	//angle_d为弧度制，范围在0-6.28	
}

