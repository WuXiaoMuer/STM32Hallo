#include "stm32f10x.h"                  // Device header
#include "Delay.h"

/*引脚配置*/
#define OneWire_DQ        GPIO_Pin_0    //PB0

/**
  * @brief  OneWire初始化
  * @param  无
  * @retval 无
  */
void OneWire_Init(void)
{
    //配置RCC
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    //配置GPIO
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;    //开漏输入模式
    GPIO_InitStructure.GPIO_Pin = OneWire_DQ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**
  * @brief  OneWire开始函数，主机总线拉低至少480us，然后释放总线，等待15 ~ 60us后，存在的从机会拉低
            60 ~ 240us以响应主机，之后从机释放总线
  * @param  无
  * @retval 返回接收的应答位，0表示应答，1表示非应答
  */
uint8_t OneWire_Start(void)
{
    uint8_t AckBit;

    GPIO_SetBits(GPIOB, OneWire_DQ);
    GPIO_ResetBits(GPIOB, OneWire_DQ);
    Delay_us(500);
    GPIO_SetBits(GPIOB, OneWire_DQ);
    Delay_us(70);
    AckBit = GPIO_ReadInputDataBit(GPIOB, OneWire_DQ);
    Delay_us(500);
    return AckBit;
}

/**
  * @brief  OneWire发送一位函数，主机将总线拉低60 ~ 120us，然后释放总线，表示发送0，主机将总线拉低
            1 ~ 15us，然后释放总线表示发送1，从机将在总线拉低30us后读取电平，整个时间片应大于60us
  * @param  Bit 要发送的一位
  * @retval 无
  */
void OneWire_SendBit(uint8_t Bit)
{
    GPIO_ResetBits(GPIOB, OneWire_DQ);
    Delay_us(5);
    GPIO_WriteBit(GPIOB, OneWire_DQ, (BitAction)Bit);
    Delay_us(55);
    GPIO_SetBits(GPIOB, OneWire_DQ);
}

/**
  * @brief  OneWire接收一位函数，主机将总线拉低1 ~ 15us，然后释放总线，并在拉低后15us内读取总线电平，
            读取低电平则为接收0，读取高电平则为接收1，整个时间片应大于60us
  * @param  无
  * @retval 返回接收的一位
  */
uint8_t OneWire_ReceiveBit(void)
{
    uint8_t Bit;

    GPIO_ResetBits(GPIOB, OneWire_DQ);
    Delay_us(5);
    GPIO_SetBits(GPIOB, OneWire_DQ);
    Delay_us(5);
    Bit = GPIO_ReadInputDataBit(GPIOB, OneWire_DQ);
    Delay_us(50);
    return Bit;
}

/**
  * @brief  OneWire发送一个字节函数
  * @param  Byte 要发送的字节
  * @retval 无
  */
void OneWire_SendByte(uint8_t Byte)
{
    uint8_t i;

    for(i = 0; i < 8; i++)
    {
    	OneWire_SendBit(Byte & (0x01 << i));
    }
}

/**
  * @brief  OneWire接收一个字节函数
  * @param  无
  * @retval 返回接收的字节
  */
uint8_t OneWire_ReceiveByte(void)
{
    uint8_t i, Byte = 0x00;
    
    for(i = 0; i < 8; i++)
    {
    	if(OneWire_ReceiveBit())  {Byte |= (0x01 << i);}
    }
    return Byte;
}
