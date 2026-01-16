#include "lcd_init.h"

void LCD_GPIO_Init(void)
{
//    // 仅初始化控制引脚，SPI引脚由CubeMX配置初始化
//    GPIO_InitTypeDef GPIO_InitStruct = {0};

//    // 使能GPIOA时钟
//    __HAL_RCC_GPIOA_CLK_ENABLE();

//    // 配置RES/DC/CS/BLK引脚
//    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
//    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//    GPIO_InitStruct.Pull = GPIO_NOPULL;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

//    // 初始化引脚默认电平
//    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5, GPIO_PIN_SET);

    // 原软件SPI引脚初始化代码注释掉
    /*
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能A端口时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;	 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);	  //初始化GPIOA
    GPIO_SetBits(GPIOA,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5);
    */
}

/******************************************************************************
      函数说明：LCD硬件SPI数据写入函数
      入口数据：dat  要写入的串行数据
      返回值：  无
******************************************************************************/
void LCD_Writ_Bus(u8 dat) 
{	
    LCD_CS_Clr();
    // 使用HAL库SPI发送数据（阻塞模式）
    HAL_SPI_Transmit(&hspi1, &dat, 1, 100); // 超时时间100ms
    LCD_CS_Set();
	
    // 原软件SPI代码注释掉
    /*
    u8 i;
    LCD_CS_Clr();
    for(i=0;i<8;i++)
    {			  
        LCD_SCLK_Clr();
        if(dat&0x80)
        {
           LCD_MOSI_Set();
        }
        else
        {
           LCD_MOSI_Clr();
        }
        LCD_SCLK_Set();
        dat<<=1;
    }	
    LCD_CS_Set();	
    */
}

/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
void LCD_WR_DATA8(u8 dat)
{
    LCD_Writ_Bus(dat);
}

/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
void LCD_WR_DATA(u16 dat)
{
    LCD_Writ_Bus(dat>>8);
    LCD_Writ_Bus(dat);
}

/******************************************************************************
      函数说明：LCD写入命令
      入口数据：dat 写入的命令
      返回值：  无
******************************************************************************/
void LCD_WR_REG(u8 dat)
{
    LCD_DC_Clr();//写命令
    LCD_Writ_Bus(dat);
    LCD_DC_Set();//写数据
}

/******************************************************************************
      函数说明：设置起始和结束地址
      入口数据：x1,x2 设置列的起始和结束地址
                y1,y2 设置行的起始和结束地址
      返回值：  无
******************************************************************************/
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2)
{
    if(USE_HORIZONTAL==0)
    {
        LCD_WR_REG(0x2a);//列地址设置
        LCD_WR_DATA(x1+2);
        LCD_WR_DATA(x2+2);
        LCD_WR_REG(0x2b);//行地址设置
        LCD_WR_DATA(y1+1);
        LCD_WR_DATA(y2+1);
        LCD_WR_REG(0x2c);//储存器写
    }
    else if(USE_HORIZONTAL==1)
    {
        LCD_WR_REG(0x2a);//列地址设置
        LCD_WR_DATA(x1+2);
        LCD_WR_DATA(x2+2);
        LCD_WR_REG(0x2b);//行地址设置
        LCD_WR_DATA(y1+1);
        LCD_WR_DATA(y2+1);
        LCD_WR_REG(0x2c);//储存器写
    }
    else if(USE_HORIZONTAL==2)
    {
        LCD_WR_REG(0x2a);//列地址设置
        LCD_WR_DATA(x1+1);
        LCD_WR_DATA(x2+1);
        LCD_WR_REG(0x2b);//行地址设置
        LCD_WR_DATA(y1+2);
        LCD_WR_DATA(y2+2);
        LCD_WR_REG(0x2c);//储存器写
    }
    else
    {
        LCD_WR_REG(0x2a);//列地址设置
        LCD_WR_DATA(x1+1);
        LCD_WR_DATA(x2+1);
        LCD_WR_REG(0x2b);//行地址设置
        LCD_WR_DATA(y1+2);
        LCD_WR_DATA(y2+2);
        LCD_WR_REG(0x2c);//储存器写
    }
}

void LCD_Init(void)
{
    LCD_GPIO_Init();//初始化GPIO
	
    LCD_RES_Clr();//复位
    HAL_Delay(100);
    LCD_RES_Set();
    HAL_Delay(100);
	
    LCD_BLK_Set();//打开背光
    HAL_Delay(100);
	
    //************* Start Initial Sequence **********//
    LCD_WR_REG(0x11); //Sleep out 
    HAL_Delay(120);              //Delay 120ms 
    //------------------------------------ST7735S Frame Rate-----------------------------------------// 
    LCD_WR_REG(0xB1); 
    LCD_WR_DATA8(0x05); 
    LCD_WR_DATA8(0x3C); 
    LCD_WR_DATA8(0x3C); 
    LCD_WR_REG(0xB2); 
    LCD_WR_DATA8(0x05);
    LCD_WR_DATA8(0x3C); 
    LCD_WR_DATA8(0x3C); 
    LCD_WR_REG(0xB3); 
    LCD_WR_DATA8(0x05); 
    LCD_WR_DATA8(0x3C); 
    LCD_WR_DATA8(0x3C); 
    LCD_WR_DATA8(0x05); 
    LCD_WR_DATA8(0x3C); 
    LCD_WR_DATA8(0x3C); 
    //------------------------------------End ST7735S Frame Rate---------------------------------// 
    LCD_WR_REG(0xB4); //Dot inversion 
    LCD_WR_DATA8(0x03); 
    //------------------------------------ST7735S Power Sequence---------------------------------// 
    LCD_WR_REG(0xC0); 
    LCD_WR_DATA8(0x28); 
    LCD_WR_DATA8(0x08); 
    LCD_WR_DATA8(0x04); 
    LCD_WR_REG(0xC1); 
    LCD_WR_DATA8(0XC0); 
    LCD_WR_REG(0xC2); 
    LCD_WR_DATA8(0x0D); 
    LCD_WR_DATA8(0x00); 
    LCD_WR_REG(0xC3); 
    LCD_WR_DATA8(0x8D); 
    LCD_WR_DATA8(0x2A); 
    LCD_WR_REG(0xC4); 
    LCD_WR_DATA8(0x8D); 
    LCD_WR_DATA8(0xEE); 
    //---------------------------------End ST7735S Power Sequence-------------------------------------// 
    LCD_WR_REG(0xC5); //VCOM 
    LCD_WR_DATA8(0x1A); 
    LCD_WR_REG(0x36); //MX, MY, RGB mode 
    if(USE_HORIZONTAL==0)LCD_WR_DATA8(0x00);
    else if(USE_HORIZONTAL==1)LCD_WR_DATA8(0xC0);
    else if(USE_HORIZONTAL==2)LCD_WR_DATA8(0x70);
    else LCD_WR_DATA8(0xA0); 
    //------------------------------------ST7735S Gamma Sequence---------------------------------// 
    LCD_WR_REG(0xE0); 
    LCD_WR_DATA8(0x04); 
    LCD_WR_DATA8(0x22); 
    LCD_WR_DATA8(0x07); 
    LCD_WR_DATA8(0x0A); 
    LCD_WR_DATA8(0x2E); 
    LCD_WR_DATA8(0x30); 
    LCD_WR_DATA8(0x25); 
    LCD_WR_DATA8(0x2A); 
    LCD_WR_DATA8(0x28); 
    LCD_WR_DATA8(0x26); 
    LCD_WR_DATA8(0x2E); 
    LCD_WR_DATA8(0x3A); 
    LCD_WR_DATA8(0x00); 
    LCD_WR_DATA8(0x01); 
    LCD_WR_DATA8(0x03); 
    LCD_WR_DATA8(0x13); 
    LCD_WR_REG(0xE1); 
    LCD_WR_DATA8(0x04); 
    LCD_WR_DATA8(0x16); 
    LCD_WR_DATA8(0x06); 
    LCD_WR_DATA8(0x0D); 
    LCD_WR_DATA8(0x2D); 
    LCD_WR_DATA8(0x26); 
    LCD_WR_DATA8(0x23); 
    LCD_WR_DATA8(0x27); 
    LCD_WR_DATA8(0x27); 
    LCD_WR_DATA8(0x25); 
    LCD_WR_DATA8(0x2D); 
    LCD_WR_DATA8(0x3B); 
    LCD_WR_DATA8(0x00); 
    LCD_WR_DATA8(0x01); 
    LCD_WR_DATA8(0x04); 
    LCD_WR_DATA8(0x13); 
    //------------------------------------End ST7735S Gamma Sequence-----------------------------// 
    LCD_WR_REG(0x3A); //65k mode 
    LCD_WR_DATA8(0x05); 
    LCD_WR_REG(0x29); //Display on 
}