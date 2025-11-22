#include "stm32f10x.h"                  // Device header
#include "OneWire.h"
#include "Delay.h"

/*引脚配置*/
#define DS18B20_DQ        GPIO_Pin_0    //PB0

#define DS18B20_SEARCH_ROM               0xF0
#define DS18B20_READ_ROM                 0x33
#define DS18B20_MATCH_ROM                0x55
#define DS18B20_SKIP_ROM                 0xCC
#define DS18B20_ALARM_SEARCH             0xEC
#define DS18B20_CONVERT_T                0x44
#define DS18B20_WRITE_SCRATCHPAD         0x4E
#define DS18B20_READ_SCRATCHPAD          0xBE
#define DS18B20_COPY_SCRATCHPAD          0x48
#define DS18B20_RECALL_E2                0xB8
#define DS18B20_READ_POWER_SUPPLY        0xB4

uint8_t DS18B20_ROM_Address[8];    //存储搜索到的ROM码
uint8_t DS18B20_ROM_HasCfmd[8];    //存储已确认的位，0未确认，1已确认

/**
  * @brief  DS18B20初始化函数
  * @param  无
  * @retval 无
  */
void DS18B20_Init(void)
{
	OneWire_Init();
	OneWire_Start();
	OneWire_SendByte(DS18B20_SKIP_ROM);     //跳过ROM指令
	OneWire_SendByte(DS18B20_CONVERT_T);    //开始温度转换指令
	Delay_ms(500);                          //等待温度转换完成
}

/**
  * @brief  DS18B20温度转换函数
  * @param  无
  * @retval 无
  */
void DS18B20_ConvertT(void)
{
	uint8_t i;
	
	OneWire_Start();
//	OneWire_SendByte(DS18B20_SKIP_ROM);     //跳过ROM指令
	OneWire_SendByte(DS18B20_MATCH_ROM);    //匹配ROM指令
	for(i = 0; i < 8; i++)                  //发送64位ROM码
	{
		OneWire_SendByte(DS18B20_ROM_Address[i]);
	}
	OneWire_SendByte(DS18B20_CONVERT_T);    //开始温度转换指令
}

/**
  * @brief  DS18B20温度读取函数
  * @param  无
  * @retval 返回测得的温度，精度：0.0625℃
  */
float DS18B20_ReadT(void)
{
	uint8_t TLSB, TMSB, i;
	int16_t Temp;
	float T;
	
	OneWire_Start();
//	OneWire_SendByte(DS18B20_SKIP_ROM);           //跳过ROM指令
	OneWire_SendByte(DS18B20_MATCH_ROM);          //匹配ROM指令
	for(i = 0; i < 8; i++)                        //发送64位ROM码
	{
		OneWire_SendByte(DS18B20_ROM_Address[i]);
	}
	OneWire_SendByte(DS18B20_READ_SCRATCHPAD);    //读暂存器指令
	TLSB = OneWire_ReceiveByte();
	TMSB = OneWire_ReceiveByte();
	Temp = (TMSB << 8) | TLSB;
	T = Temp / 16.0;
	return T;
}

/**
  * @brief  DS18B20读取ROM码函数
  * @param  无
  * @retval 无
  */
void DS18B20_ReadROM(void)
{
	uint8_t i;
	
	OneWire_Start();
	OneWire_SendByte(DS18B20_READ_ROM);    //读ROM指令        
	for(i = 0; i < 8; i++)
	{
		DS18B20_ROM_Address[i] = OneWire_ReceiveByte();
	}
}

/**
  * @brief  DS18B20搜索ROM码函数，执行一次获取一个DS18B20的ROM码，通过DS18B20_ROM_Address读取，
            调用格式如下：
            uint8_t Sp = 0;
            do
            {
                Sp = DS18B20_SearchROM(Sp);
            }
            while(Sp);
  * @param  Split_Point 上次最后一个未确认的分叉点
  * @retval 返回本次最后一个未确认的分叉点
  */
uint8_t DS18B20_SearchROM(uint8_t Split_Point)
{
	uint8_t Len = 64, Pos = 0, Pb, Cb, Temp;
	
	if(Split_Point == 0)  {Split_Point = 64;}    //分叉点的初始值应该用64
	OneWire_Start();
	OneWire_SendByte(DS18B20_SEARCH_ROM);    //搜索ROM指令

	//依次遍历8个字节的每1位
	while(Len--)
	{
		Pb = OneWire_ReceiveBit();    //读取当前位Bit值
		Cb = OneWire_ReceiveBit();    //读取当前位Bit值的补码
		if(Pb && Cb)    //两位都是1，表示没有设备
		{
			return 0;
		}
		else if(Pb)     //Pb = 1，Cb = 0，说明当前位为1
		{
			DS18B20_ROM_Address[Pos / 8] |= 0x01 << (Pos % 8);     //Address记录该位为1
			OneWire_SendBit(1);
			DS18B20_ROM_HasCfmd[Pos / 8] |= 0x01 << (Pos % 8);     //HasCfmd记录该位为1，表示已确认
		}
		else if(Cb)     //Pb = 0，Cb = 1，说明当前位为0
		{
			DS18B20_ROM_Address[Pos / 8] &= ~(0x01 << (Pos % 8));    //Address记录该位为0
			OneWire_SendBit(0);
			DS18B20_ROM_HasCfmd[Pos / 8] |= 0x01 << (Pos % 8);       //HasCfmd记录该位为1，表示已确认
		}
		else            //两位都是0，出现分叉点
		{
			if(Split_Point == 64 || Pos > Split_Point)    //比上次记录的点更深，出现了新的分叉点
			{
				DS18B20_ROM_Address[Pos / 8] &= ~(0x01 << (Pos % 8));    //本次走0，Address记录该位为0
				OneWire_SendBit(0);
				DS18B20_ROM_HasCfmd[Pos / 8] &= ~(0x01 << (Pos % 8));    //HasCfmd记录该位为0，表示未确认
				Split_Point = Pos;                                       //记录新的分叉点位置 
			}
			else if(Pos == Split_Point)                   //到达了上次记录的分叉点位置
			{
				DS18B20_ROM_Address[Pos / 8] |= 0x01 << (Pos % 8);    //本次走1，Address记录该位为1
				OneWire_SendBit(1);
				DS18B20_ROM_HasCfmd[Pos / 8] |= 0x01 << (Pos % 8);    //HasCfmd记录该位为1，表示已确认
			}
			else                                          //到达了上次记录的分叉点位置之前
			{
				Temp = DS18B20_ROM_Address[Pos / 8] & (0x01 << (Pos % 8));    //继续使用上次记录的值
				OneWire_SendBit(Temp);
			}
		}
		Pos++;
	}
	if(Split_Point == 64)  {Split_Point = 0;}    //分叉点初值未改变，直接置0
	
	//定位本次最后一个分叉点
	while(Split_Point > 0)
	{
		Temp = DS18B20_ROM_HasCfmd[Split_Point / 8] & (0x01 << (Split_Point % 8));
		if(!Temp)  {break;}
		Split_Point--;
	}
	return Split_Point;
}
