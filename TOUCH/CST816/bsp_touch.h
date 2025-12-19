#ifndef __BSP_TOUCH_H__
#define __BSP_TOUCH_H__

#include "type.h"
#include "config.h"
#include "gvar.h"

//#define CST816_ADDRESS 0x2a
#define CST816_ADDRESS (0x15U << 1)

#define GestureID                     0x01 //触摸屏中与手势识别
#define FingerNum                     0x02 //记录触摸手指数量
#define XposH                     0x03 //X 坐标高位相关的寄存器地址
#define XposL                     0x04 //X 坐标低位部分的寄存器地址
#define YposH                     0x05 //Y 坐标高位相关的寄存器地址
#define YposL                     0x06 //Y 坐标低位部分的寄存器地址
#define ChipID                     0xA7 //访问触摸屏芯片的唯一标识符寄存器的地址
#define VersionID                 0xA9 //触摸屏芯片版本相关的寄存器地址
#define SleepMode                0xE5 //控制触摸屏进入或退出睡眠模式的寄存器地址
#define MotionMask                     0xEC //对某些运动相关操作的屏蔽或启用设置
#define IrqPluseWidth                             0xED //中断低脉冲宽度相关的寄存器地址
#define NorScanPer                     0xEE //触摸屏正常扫描周期相关的寄存器地址
#define MotionSlAngle                             0xEF //涉及到运动滑动角度相关
#define LpAutoWakeTime                             0xF4 //长按自动唤醒时间
#define LpScanTH                 0xF5 //触摸屏的长按扫描阈值
#define LpScanWin             0xF6 //触摸屏的长按扫描窗口相关
#define LpScanFreq             0xF7 //触摸屏的长按扫描频率
#define LpScanIdac             0xF8 //长按扫描电流
#define AutoSleepTime     0xF9 //触摸屏自动进入睡眠模式的时间相关
#define IrqCtl                     0xFA //触摸屏的中断控制相关
#define AutoReset             0xFB //触摸屏自动复位相关
#define LongPressTime     0xFC //触摸屏的长按时间
#define IOCtl                     0xFD //触摸屏的输入输出控制
#define DisAutoSleep         0xFE //禁止触摸屏自动进入睡眠模式

extern void bsp_touch_init(void);//触摸屏初始化
extern void bsp_touch_getXY(Point *point);//获取触摸坐标

#endif
