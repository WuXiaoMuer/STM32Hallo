#ifndef __BSP_LCD_H__
#define __BSP_LCD_H__

#include "main.h"
#include "type.h"
#include "config.h"



// 函数功能：液晶屏初始化
// 输入参数：无
// 输出参数：无
// 返回值：无
extern void lcd_init(void);
// 函数功能：开启液晶屏背光
// 输入参数：无
// 输出参数：无
// 返回值：无
extern void bsp_lcd_bl_on(void);
// 函数功能：关闭液晶屏背光
// 输入参数：无
// 输出参数：无
// 返回值：无
extern void bsp_lcd_bl_off(void);
// 函数功能：清除液晶屏显示
// 输入参数：无
// 输出参数：无
// 返回值：无
extern void lcd_clear(void);
// 函数功能：液晶屏刷新
// 输入参数：无
// 输出参数：无
// 返回值：无
extern void bsp_lcd_refresh(void);
// 函数功能：绘制水平线 用于绘制从点point1到点point2的水平线
// 输入参数：point1 水平线起点坐标
//         point2 水平线终点坐标
//         color 水平线颜色
// 输出参数：无
// 返回值：无
extern void lcd_line(Point* point1, Point* point2, rgb565 color);
// 函数功能：绘制圆 用于绘制圆心为point,半径为radius的圆
// 输入参数：point 圆的圆心坐标
//         radius 圆的半径
//         color 圆的颜色
// 输出参数：无
// 返回值：无
extern void lcd_circle(Point* point, uint16_t radius, rgb565 color);
// 函数功能：绘制字符 用于绘制字符c,字符位置为point,字符颜色为color
// 输入参数：c 要绘制的字符
//         point 字符的位置坐标
//         font_size 字符的字体大小
//         color 字符的颜色
// 输出参数：无
// 返回值：无
extern void lcd_ascii(Point* point,char* c, uint16_t font_size, rgb565 color);

extern void lcd_uint(Point* point,uint32_t uint, uint16_t font_size, rgb565 color);

extern void lcd_chinese(Point* point,char* c, uint16_t font_size, rgb565 color);

extern void lcd_hex(Point* point,uint16_t uint, uint16_t font_size, rgb565 color);

// 函数功能：写入显存点 用于将颜色color写入显存点(point->x,point->y)
// 输入参数：point 显存点坐标
//         color 颜色
// 输出参数：无
// 返回值：无
extern void lcd_buf_writePoint(Point* point, rgb565 color);

void lcd_buf_writePoint(Point* point, rgb565 color);


#endif
