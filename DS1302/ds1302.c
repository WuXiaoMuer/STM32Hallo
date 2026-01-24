#include "ds1302.h"
#include "main.h"
#include "gpio.h"
#include "delay.h"

struct TIMEData TimeData;
u8 read_time[7];
 
// CE、SCLK端口初始化（HAL库版本 + PB4/PB5）
void ds1302_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 使能GPIOB时钟
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // 配置CE引脚(PB4) 推挽输出
    GPIO_InitStruct.Pin = CE_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DS1302_PORT, &GPIO_InitStruct);
    CE_L; // 初始拉低

    // 配置SCLK引脚(PB5) 推挽输出
    GPIO_InitStruct.Pin = SCLK_PIN;
    HAL_GPIO_Init(DS1302_PORT, &GPIO_InitStruct);
    SCLK_L; // 初始拉低
}
 
// 配置DATA引脚(PB3)为输出态（HAL库版本）
void ds1302_DATAOUT_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Pin = DATA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DS1302_PORT, &GPIO_InitStruct);
    DATA_L; // 初始拉低
}
 
// 配置DATA引脚(PB3)为输入态（HAL库版本）
void ds1302_DATAINPUT_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStruct.Pin = DATA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL; // 浮空输入
    HAL_GPIO_Init(DS1302_PORT, &GPIO_InitStruct);
}
 
// 向DS1302发送一字节数据
void ds1302_write_onebyte(u8 data)
{
    ds1302_DATAOUT_init();
    u8 count=0;
    SCLK_L;
    for(count=0;count<8;count++)
    {   
        SCLK_L;
        if(data&0x01)
        {
            DATA_H;
        }
        else
        {
            DATA_L;
        }//先准备好数据再发送
        SCLK_H;//拉高时钟线，发送数据
        data>>=1;
    }
}
 
// 向指定寄存器地址发送数据（修正原函数名笔误：wirte_rig -> write_reg）
void ds1302_write_reg(u8 address,u8 data)
{
    u8 temp1=address;
    u8 temp2=data;
    CE_L;SCLK_L;delay_us(1);
    CE_H;delay_us(2);
    ds1302_write_onebyte(temp1);
    ds1302_write_onebyte(temp2);
    CE_L;SCLK_L;delay_us(2);
}
 
// 从指定地址读取一字节数据（修正原函数名笔误：read_rig -> read_reg）
u8 ds1302_read_reg(u8 address)
{
    u8 temp3=address;
    u8 count=0;
    u8 return_data=0x00;
    CE_L;SCLK_L;delay_us(3);
    CE_H;delay_us(3);
    ds1302_write_onebyte(temp3);
    ds1302_DATAINPUT_init();//配置I/O口为输入（修正原函数名笔误）
    delay_us(2);
    for(count=0;count<8;count++)
    {
        delay_us(2);//使电平持续一段时间
        return_data>>=1;
        SCLK_H;delay_us(4);//使高电平持续一段时间
        SCLK_L;delay_us(14);//延时14us后再去读取电压，更加准确
        // HAL库读取PB3引脚电平（替换原标准库函数）
        if(HAL_GPIO_ReadPin(DS1302_PORT, DATA_PIN) == GPIO_PIN_SET)
        {
            return_data=return_data|0x80;
        }
    }
    delay_us(2);
    CE_L;DATA_L;
    return return_data;
}
 
// ds1302初始化函数
void ds1302_init(void)
{
    ds1302_write_reg(0x8e,0x00);//关闭写保护
    ds1302_write_reg(0x80,0x00);//seconds37秒
    ds1302_write_reg(0x82,0x13);//minutes58分
    ds1302_write_reg(0x84,0x12);//hours23时
    ds1302_write_reg(0x86,0x24);//date30日
    ds1302_write_reg(0x88,0x01);//months9月
    ds1302_write_reg(0x8a,0x06);//days星期日
    ds1302_write_reg(0x8c,0x26);//year2020年
    ds1302_write_reg(0x8e,0x80);//开启写保护
}

// 读取实时时间（BCD码）（修正原函数名笔误：ds1032_read_time -> ds1302_read_time）
void ds1302_read_time(void)
{
    read_time[0]=ds1302_read_reg(0x81);//读秒
    read_time[1]=ds1302_read_reg(0x83);//读分
    read_time[2]=ds1302_read_reg(0x85);//读时
    read_time[3]=ds1302_read_reg(0x87);//读日
    read_time[4]=ds1302_read_reg(0x89);//读月
    read_time[5]=ds1302_read_reg(0x8B);//读星期
    read_time[6]=ds1302_read_reg(0x8D);//读年
}
 
// BCD码转十进制（修正原函数名笔误：ds1032_read_realTime -> ds1302_read_realTime）
void ds1302_read_realTime(void)
{
    ds1302_read_time();  //BCD码转换为10进制
    TimeData.second=(read_time[0]>>4)*10+(read_time[0]&0x0f);
    TimeData.minute=((read_time[1]>>4)&(0x07))*10+(read_time[1]&0x0f);
    TimeData.hour=(read_time[2]>>4)*10+(read_time[2]&0x0f);
    TimeData.day=(read_time[3]>>4)*10+(read_time[3]&0x0f);
    TimeData.month=(read_time[4]>>4)*10+(read_time[4]&0x0f);
    TimeData.week=read_time[5];
    TimeData.year=(read_time[6]>>4)*10+(read_time[6]&0x0f)+2000;
}