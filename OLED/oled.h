#ifndef __OLED_H
#define __OLED_H 

#include "main.h"
#include "stdlib.h"	

/************************** 类型重定义 **************************/
// 无符号8位整型
#define u8 uint8_t
// 无符号16位整型
#define u16 uint16_t
// 无符号32位整型
#define u32 uint32_t

/************************** 硬件相关宏定义 **************************/
// 声明外部I2C句柄（对应硬件I2C2）
extern I2C_HandleTypeDef hi2c2;

// OLED设备I2C地址（0x78为常用地址，若OLEDADDR脚接VCC则为0x7A）
#define OLED_I2C_ADDR    0x78
// 写命令模式
#define OLED_CMD  0
// 写数据模式
#define OLED_DATA 1
// 列偏移量（适配不同OLED屏的起始列，默认0）
#define OLED_COL_OFFSET 0

/************************** 函数声明 **************************/

/**
 * @brief  OLED屏幕颜色反转控制
 * @param  i: 0-正常显示（黑底白字） 1-颜色反转（白底黑字）
 * @retval 无
 */
void OLED_ColorTurn(u8 i);

/**
 * @brief  OLED屏幕显示方向旋转
 * @param  i: 0-正常显示 1-屏幕旋转180度显示
 * @retval 无
 */
void OLED_DisplayTurn(u8 i);

/**
 * @brief  向OLED写入一个字节（命令/数据）
 * @param  dat: 要写入的字节数据
 * @param  mode: 写入模式（OLED_CMD-命令 OLED_DATA-数据）
 * @retval 无
 */
void OLED_WR_Byte(u8 dat, u8 mode);

/**
 * @brief  开启OLED显示
 * @param  无
 * @retval 无
 */
void OLED_DisPlay_On(void);

/**
 * @brief  关闭OLED显示（进入休眠，低功耗）
 * @param  无
 * @retval 无
 */
void OLED_DisPlay_Off(void);

/**
 * @brief  刷新OLED显示缓冲区到屏幕
 * @param  无
 * @retval 无
 */
void OLED_Refresh(void);

/**
 * @brief  清空OLED显示缓冲区（清屏）
 * @param  无
 * @retval 无
 */
void OLED_Clear(void);

/**
 * @brief  在指定位置画点
 * @param  x: 横坐标（0~127）
 * @param  y: 纵坐标（0~63）
 * @param  t: 点状态（0-熄灭 1-点亮）
 * @retval 无
 */
void OLED_DrawPoint(u8 x,u8 y,u8 t);

/**
 * @brief  在指定位置显示单个字符
 * @param  x: 起始横坐标（0~127）
 * @param  y: 起始纵坐标（0~63）
 * @param  chr: 要显示的字符（ASCII码）
 * @param  size1: 字符大小（16/24等，需匹配字库）
 * @param  mode: 显示模式（0-覆盖显示 1-反色显示）
 * @retval 无
 */
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size1,u8 mode);

/**
 * @brief  在指定位置显示字符串
 * @param  x: 起始横坐标（0~127）
 * @param  y: 起始纵坐标（0~63）
 * @param  chr: 字符串指针（ASCII码）
 * @param  size1: 字符大小（16/24等，需匹配字库）
 * @param  mode: 显示模式（0-覆盖显示 1-反色显示）
 * @retval 无
 */
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 size1,u8 mode);

/**
 * @brief  在指定位置显示数字
 * @param  x: 起始横坐标（0~127）
 * @param  y: 起始纵坐标（0~63）
 * @param  num: 要显示的数字（0~4294967295）
 * @param  len: 显示位数（不足补0，超过则显示完整数字）
 * @param  size1: 数字大小（16/24等，需匹配字库）
 * @param  mode: 显示模式（0-覆盖显示 1-反色显示）
 * @retval 无
 */
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size1,u8 mode);

/**
 * @brief  OLED初始化函数（初始化GPIO、I2C、屏幕寄存器等）
 * @param  无
 * @retval 无
 */
void OLED_Init(void);

#endif