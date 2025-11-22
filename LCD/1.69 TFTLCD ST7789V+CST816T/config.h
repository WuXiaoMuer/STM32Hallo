/**
  ******************************************************************************
  * @file    config.h
  * @brief   系统配置参数头文件
  *          该文件包含了所有硬件模块的使能开关、引脚定义、尺寸参数和颜色定义等。
  *
  * @author  [WXM]
  * @version [V1.0.0]
  * @date    [2025.11.21]
  *
  * @copyright Copyright (c) [2025] [WXM]. 保留所有权利。
  ******************************************************************************
  */

#ifndef __CONFIG_H
#define __CONFIG_H

#include "stm32h7xx_hal.h"

// -----------------------------------------------------------------------------
// 模块使能配置 (Module Enable Configuration)
// -----------------------------------------------------------------------------

#define LCD_ENABLE       1       // 1: 启用LCD模块, 0: 禁用
#define MOTOR_ENABLE     1       // 1: 启用电机模块, 0: 禁用
#define ENCODER_ENABLE   1       // 1: 启用编码器模块, 0: 禁用
#define TOUCH_ENABLE     1       // 1: 启用触摸模块, 0: 禁用
#define MPU6050_ENABLE   1       // 1: 启用MPU6050模块, 0: 禁用

// -----------------------------------------------------------------------------
// LCD 配置 (LCD Configuration)
// -----------------------------------------------------------------------------

#define LCD_WIDTH        240     // LCD屏幕宽度（像素）
#define LCD_HEIGHT       320     // LCD屏幕高度（像素）

// LCD接口配置
extern SPI_HandleTypeDef hspi2;
#define LCD_PORT         hspi2   // LCD所使用的SPI端口

// LCD控制引脚电平定义
#define LCD_BL_ON        GPIO_PIN_SET    // 背光开启电平
#define LCD_BL_OFF       GPIO_PIN_RESET  // 背光关闭电平
#define LCD_CS_ON        GPIO_PIN_RESET  // CS片选有效电平
#define LCD_CS_OFF       GPIO_PIN_SET    // CS片选无效电平
#define LCD_RST_ON       GPIO_PIN_RESET  // RST复位有效电平
#define LCD_RST_OFF      GPIO_PIN_SET    // RST复位无效电平
#define LCD_DC_DATA      GPIO_PIN_SET    // DC引脚：高电平表示数据
#define LCD_DC_COMM      GPIO_PIN_RESET  // DC引脚：低电平表示命令

#define LCD_TIMEOUT      500             // LCD操作超时时间（毫秒）

// LCD显示缓冲区
extern uint16_t lcd_buf[LCD_WIDTH * LCD_HEIGHT]; // 声明LCD帧缓冲区
#define LCD_BUF_ADD_BAS  ((uint32_t)lcd_buf)     // LCD帧缓冲区基地址
#define LCD_REFRESH_CYCLE 250                  // LCD屏幕刷新周期（毫秒）

// -----------------------------------------------------------------------------
// 颜色定义 (Color Definitions)
// -----------------------------------------------------------------------------
// 颜色格式为RGB565
#define WHITE           0xFFFF      // 白色
#define BLACK           0x0000      // 黑色
#define RED             0xF800      // 红色
#define GREEN           0x07E0      // 绿色
#define BLUE            0x001F      // 蓝色
#define YELLOW          0xFFE0      // 黄色
#define CYAN            0x07FF      // 青色
#define MAGENTA         0xF81F      // 品红色
#define ORANGE          0xFC00      // 橙色
#define PURPLE          0x780F      // 紫色
#define BROWN           0x9C00      // 棕色
#define LIGHT_GRAY      0xD69A      // 浅灰色
#define DARK_GRAY       0x7BEF      // 深灰色
#define LIGHT_BLUE      0x841F      // 浅蓝色
#define LIGHT_GREEN     0x07FF      // 浅绿色
#define LIGHT_RED       0xF81F      // 浅红色
#define LIGHT_YELLOW    0xFFE0      // 浅黄色
#define LIGHT_CYAN      0x07FF      // 浅青色
#define LIGHT_MAGENTA   0xF81F      // 浅品红色

#define BACKGROUND      WHITE       // 系统默认背景色

extern uint16_t colors[];

// -----------------------------------------------------------------------------
// I2C 设备配置 (I2C Device Configuration)
// -----------------------------------------------------------------------------

// 触摸屏I2C配置
extern I2C_HandleTypeDef hi2c2;
#define TOUCH_PORT      hi2c2       // 触摸屏所使用的I2C端口

// MPU6050 I2C配置
extern I2C_HandleTypeDef hi2c3;
#define MPU6050_PORT    hi2c3       // MPU6050所使用的I2C端口

#endif