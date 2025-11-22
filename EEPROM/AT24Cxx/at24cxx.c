#include "at24cxx.h"
uint8_t At24c02_Read_Byte(uint16_t addr, uint8_t* read_buf)
{
    return HAL_I2C_Mem_Read(&hi2c1, AT24C02_ADDR_READ, addr, I2C_MEMADD_SIZE_16BIT, read_buf, 1, 1000);
}
uint8_t At24c02_Write_Byte(uint16_t addr, uint8_t* dat)
{
    return HAL_I2C_Mem_Write(&hi2c1, AT24C02_ADDR_WRITE, addr, I2C_MEMADD_SIZE_16BIT, dat, 1, 1000);
}
uint8_t AT24CXX_Check(void)
{
	uint8_t temp;
	uint8_t data = 0XAB;
	AT24CXX_Read(EE_TYPE,&temp,1);//避免每次开机都写AT24CXX			   
	if(temp == 0XAB)
		return 0;		   
	AT24CXX_Write(EE_TYPE,&data,1);
	AT24CXX_Read(EE_TYPE,&temp,1);;	  
	if(temp != 0XAB)
		return 1;
	return 0;											  
}
/**
 * @brief  向AT24C64写入多个字节（处理页写入限制）
 * @param  addr: 起始地址（16位）
 * @param  buf: 要写入的数据缓冲区
 * @param  len: 数据长度
 * @retval 成功写入的字节数
 */
uint16_t AT24C64_WriteBytes(uint16_t addr, uint8_t *buf, uint16_t len)
{
    uint16_t written = 0;
    while (len > 0)
    {
        // 计算当前页剩余可写字节数（AT24C64页大小32字节）
        uint8_t page_remain = 32 - (addr % 32);
        uint8_t write_len = (len < page_remain) ? len : page_remain;
        
        // 写入一页数据
        if (HAL_I2C_Mem_Write(AT24CXX_HANDLE, AT24C_DEV_ADDR, 
                            addr, I2C_MEMADD_SIZE_16BIT, buf + written, write_len, 1000) != HAL_OK)
        {
            break; // 写入失败，退出循环
        }
        
        // 更新计数和地址
        written += write_len;
        addr += write_len;
        len -= write_len;
        
        // 等待当前页写入完成（必须加延时）
        HAL_Delay(5);
    }
    return written;
}
/**
 * @brief  从AT24C64读取多个字节（无长度限制）
 * @param  addr: 起始地址（16位）
 * @param  buf: 接收缓冲区
 * @param  len: 要读取的长度
 * @retval HAL状态码
 */
HAL_StatusTypeDef AT24C64_ReadBytes(uint16_t addr, uint8_t *buf, uint16_t len)
{
    return HAL_I2C_Mem_Read(AT24CXX_HANDLE, AT24C_DEV_ADDR, 
                          addr, I2C_MEMADD_SIZE_16BIT, buf, len, 1000);
}

void AT24CXX_Init(void)
{
	//IIC_Init();//IIC初始化
	AT24CXX_Check();
}

/*****************************************
函数名：void AT24CXX_WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite)
参数：WriteAddr :要写入数据的地址  DataToWrite：要写入的数据
功能描述：从指定地址开始写入1个字节数据
返回值：无
*****************************************/
void AT24CXX_WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite)
{
#if USE_HARDWARE_IIC == 1
	uint8_t buf[2] = {WriteAddr>>8,WriteAddr%256};
	if(EE_TYPE>AT24C16)
	{
		if(HAL_I2C_Master_Transmit(AT24CXX_HANDLE,AT24C_DEV_ADDR,buf,2,0xff) != HAL_OK)
		{
		
		}
	}else{
		if(HAL_I2C_Master_Transmit(AT24CXX_HANDLE,AT24C_DEV_ADDR,buf+1,1,0xff) != HAL_OK)
		{
		
		}	
	}
	if(HAL_I2C_Master_Transmit(AT24CXX_HANDLE,AT24C_DEV_ADDR,&DataToWrite,1,0xff) != HAL_OK)
	{
	
	}
  HAL_Delay(5);
#else
  /* 根据不同的24CXX型号，发送高位地址
     * 1，24C16以上的型号，分2个字节发送地址
     * 2，24C16及以下的型号，发送1个低字节地址 + 占用器件地址的bit1~bit3位（用于表示高位地址，最多11位地址）
     *    对于24C01/02，其器件地址格式（8bit）为: 1 0 1 0 A2  A1 A0 R/W
     *    对于24C04，   其器件地址格式（8bit）为: 1 0 1 0 A2  A1 a8 R/W
     *    对于24C08，   其器件地址格式（8bit）为: 1 0 1 0 A2  a9 a8 R/W
     *    对于24C16，   其器件地址格式（8bit）为: 1 0 1 0 a10 a9 a8 R/W
     *    R/W      : 读/写控制位，0：写；1：读
     *    A0/A1/A2 : 对应器件的1/2/3引脚（只有24C01/02/04/08有这些脚）
     *    a8/a9/a10: 对应存储整列的高位地址，11bit地址最多可以表示2048个位置，可以寻址24C16及以内的型号
     */
    iic_start();                                    /* 产生IIC起始信号 */
    if (EE_TYPE > AT24C16)                          /* 24C16以上的型号，分2个字节发送地址 */
    {
        iic_send_byte(AT24C_DEV_ADDR);                        /* 发送写命令 */
        iic_wait_ack();                             /* 每发送完一个字节都要等待ACK */
        iic_send_byte(WriteAddr >> 8);                   /* 发送高字节地址 */
    }
    else                                            /* 24C16及以下的型号，发送1个低字节地址 + 占用器件地址的bit1~bit3位（用于表示高位地址，最多11位地址） */
    {
        iic_send_byte(AT24C_DEV_ADDR + ((WriteAddr >> 8) << 1));   /* 发送0xA0+高位a8/a9/a10地址，写命令 */
    }
    iic_wait_ack();                                 /* 每发送完一个字节都要等待ACK */
    iic_send_byte(WriteAddr % 256);                      /* 发送低位地址 */
    iic_wait_ack();
    
    iic_send_byte(WriteAddr);                            /* IIC发送一个字节 */
    iic_wait_ack();
    iic_stop();                                     /* 产生IIC停止信号 */
    HAL_Delay(10);                                   /* EEPROM的写入比较慢，必须等到10ms后再写下一个字节 */
#endif
}
/*****************************************
函数名：uint8_t AT24CXX_ReadOneByte(uint16_t ReadAddr)
参数： ReadAddr：要读取数据的地址 pBuffer：回填数据首地址
功能描述：从指定地址开始读取1个字节数据
返回值：返回读取到的数据
*****************************************/
uint8_t AT24CXX_ReadOneByte(uint16_t ReadAddr)
{
#if USE_HARDWARE_IIC == 1
    uint8_t buf[2] = {ReadAddr>>8, ReadAddr%256}, DataToRead = 0;
    HAL_StatusTypeDef status;

    // 1. 先发送要读取的地址（与写入时的地址处理逻辑完全一致！）
    if(EE_TYPE > AT24C16)
    {
        status = HAL_I2C_Master_Transmit(AT24CXX_HANDLE, AT24C_DEV_ADDR, buf, 2, 1000);
    }
    else
    {
        status = HAL_I2C_Master_Transmit(AT24CXX_HANDLE, AT24C_DEV_ADDR, buf+1, 1, 1000);
    }
    if(status != HAL_OK)
    {
        
        return 0xFF;  // 地址发送失败，返回FF
    }

    // 2. 读取数据
    status = HAL_I2C_Master_Receive(AT24CXX_HANDLE, AT24C_DEV_ADDR, &DataToRead, 1, 1000);
    if(status != HAL_OK)
    {
        
        return 0xFF;  // 数据读取失败，返回FF
    }

    return DataToRead;  // 读取成功，返回正确数据
#else
    uint8_t data;
    
    /* 根据不同的24CXX型号，发送高位地址
     * 1，24C16以上的型号，分2个字节发送地址
     * 2，24C16及以下的型号，发送1个低字节地址 + 占用器件地址的bit1~bit3位（用于表示高位地址，最多11位地址）
     *    对于24C01/02，其器件地址格式（8bit）为: 1 0 1 0 A2  A1 A0 R/W
     *    对于24C04，   其器件地址格式（8bit）为: 1 0 1 0 A2  A1 a8 R/W
     *    对于24C08，   其器件地址格式（8bit）为: 1 0 1 0 A2  a9 a8 R/W
     *    对于24C16，   其器件地址格式（8bit）为: 1 0 1 0 a10 a9 a8 R/W
     *    R/W      : 读/写控制位，0：写；1：读
     *    A0/A1/A2 : 对应器件的1/2/3引脚（只有24C01/02/04/08有这些脚）
     *    a8/a9/a10: 对应存储整列的高位地址，11bit地址最多可以表示2048个位置，可以寻址24C16及以内的型号
     */
    iic_start();                                    /* 产生IIC起始信号 */
    if (EE_TYPE > AT24C16)                          /* 24C16以上的型号，分2个字节发送地址 */
    {
        iic_send_byte(AT24C_DEV_ADDR);                        /* 发送写命令 */
        iic_wait_ack();                             /* 每发送完一个字节都要等待ACK */
        iic_send_byte(ReadAddr >> 8);                   /* 发送高字节地址 */
    }
    else                                            /* 24C16及以下的型号，发送1个低字节地址 + 占用器件地址的bit1~bit3位（用于表示高位地址，最多11位地址） */
    {
        iic_send_byte(AT24C_DEV_ADDR + ((ReadAddr >> 8) << 1));   /* 发送0xA0+高位a8/a9/a10地址，写命令 */
    }
    iic_wait_ack();                                 /* 每发送完一个字节都要等待ACK */
    iic_send_byte(ReadAddr % 256);                      /* 发送低位地址 */
    iic_wait_ack();
    
    iic_start();                                    /* 产生IIC起始信号 */
    iic_send_byte(AT24C_DEV_ADDR+1);                            /* 发送读命令 */
    iic_wait_ack();
    data = iic_read_byte(0);                        /* IIC读取一个字节 */
    iic_stop();                                     /* 产生IIC停止信号 */
    
    return data;
#endif
} 

/*****************************************
函数名：void AT24CXX_Write(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite)
参数：WriteAddr :要写入数据的地址  pBuffer：要写入的数据的首地址 NumToWrite：要写入数据的长度
功能描述：从指定地址开始写入多个字节数据
返回值：无
*****************************************/
void AT24CXX_Write(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite)
{
	for(int i = 0; i < NumToWrite; i++)
	{
		AT24CXX_WriteOneByte(WriteAddr,pBuffer[i]);
	}
}
/*****************************************
函数名：AT24CXX_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead)
参数： ReadAddr：要读取数据的地址 pBuffer：回填数据首地址 NumToRead:数据长度
功能描述：从指定地址开始读取多个字节数据
返回值：无
*****************************************/
void AT24CXX_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead)
{
	for(int i = 0; i < NumToRead; i++)
	{
		pBuffer[i] = AT24CXX_ReadOneByte(ReadAddr);
	}
} 