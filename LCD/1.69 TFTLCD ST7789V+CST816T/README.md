# STM32 TFT-LCD 与 电阻触摸屏 BSP 驱动库

> 🖥️ 一套完整的板级支持包（BSP），用于驱动常见 2.4"~3.8" 彩色 TFT-LCD 屏幕（如 ILI9341）并支持 XPT2046 电阻式触摸，专为 STM32 系列微控制器设计。 

## ✨ 核心特性

- ✅ 支持主流 **TFT-LCD 驱动芯片**（如 ILI9341、ST7789、HX8347、RM68042 等）
- ✅ 支持 **240×320** 或 **320×480** 分辨率（可配置）
- ✅ 集成 **XPT2046 / ADS7843 电阻式触摸屏控制器** 驱动
- ✅ 提供丰富图形 API：
  - 像素点、直线、矩形、圆、填充区域
  - 字符串显示（支持 12×12、16×16、24×24 等 ASCII 与汉字点阵）
  - BMP/JPEG 图片显示（如 `LCD_ShowPicture`）
  - 背景/前景色设置、清屏、区域刷新
- ✅ 支持 **硬件 SPI + FSMC（或 GPIO 比特 bang）** 多种通信方式
- ✅ 模块化设计：`bsp_lcd` 负责显示，`bsp_touch` 负责触摸，解耦清晰
- ✅ 触摸校准功能（通常在 `main.c` 初始化流程中完成）

## 📁 文件结构说明

├── main.c          // 主程序入口，演示 LCD 初始化、触摸校准与图形测试

├── bsp_lcd.h       // LCD 驱动头文件：寄存器定义、颜色宏、函数声明

├── bsp_lcd.c       // LCD 驱动实现：初始化、底层写命令/数据、图形绘制

├── bsp_touch.h     // 触摸屏头文件：触摸结构体、校准参数、函数声明

└── bsp_touch.c     // 触摸驱动实现：SPI 读取 ADC、坐标转换、校准算法

## ⚙️ 使用说明

### 1. 硬件连接（典型配置）

#### LCD 接口（以 SPI + GPIO 控制为例）：

|       |                 |
| ----- | --------------- |
| SCK   | SPI1_SCK (PA5)  |
| MOSI  | SPI1_MOSI (PA7) |
| CS    | GPIO (PA4)      |
| DC/RS | GPIO (PA6)      |
| RST   | GPIO (PA8)      |
| BL    | GPIO / PWM 背光 |

> 注：部分高性能屏使用 **FSMC 并行接口**（16 位数据线 + 控制线），请根据硬件调整 `bsp_lcd.c`。 

#### 触摸屏接口（XPT2046）：

|        |                  |
| ------ | ---------------- |
| T_CLK  | SPI2_SCK (PB13)  |
| T_MOSI | SPI2_MOSI (PB15) |
| T_MISO | SPI2_MISO (PB14) |
| T_CS   | GPIO (PB12)      |
| IRQ    | GPIO (可选)      |

> ⚠️ 注意：**LCD 与 Touch 通常使用不同的 SPI 外设**（如 SPI1 + SPI2）以避免冲突。 

### 2. 软件配置

- 在 bsp_lcd.h中选择 LCD 驱动型号：

  \#define USE_ILI9341

  // #define USE_ST7789

  // #define USE_RM68042

- 在 bsp_touch.h中确认触摸芯片：

  

  \#define TOUCH_XPT2046

- 实现底层 SPI 读写函数（通常基于 HAL 库）：

  - `LCD_SPI_WriteData()`
  - `TOUCH_SPI_ReadWriteByte()`

- 在 main.c 中初始化：

  LCD_Init();        // 初始化 LCD

  LCD_Clear(WHITE);  // 清屏为白色

  Touch_Init();      // 初始化触摸

  Touch_Calibrate(); // 首次运行建议校准

### 3. 常用 API 示例

#### LCD 图形操作

LCD_DrawPoint(100, 100, RED);

LCD_DrawLine(0, 0, 240, 320, BLUE);

LCD_ShowString(10, 10, "Hello TFT!", RED, BLACK, 16);

LCD_ShowChinese(50, 50, "你好", GREEN, 24);

LCD_FillArea(0, 0, 100, 100, YELLOW);

#### 触摸操作

TP_INFO tp;

if (Touch_Scan(&tp) == TOUCH_PRESSED) {

​    printf("Touch at (%d, %d)\n", tp.x, tp.y);

​    LCD_DrawCircle(tp.x, tp.y, 10, RED);

}

## 🔧 触摸校准说明

- 首次使用需运行 `Touch_Calibrate()`，屏幕会显示 **4 个十字标定点**
- 用户依次点击标定点，系统自动计算校准参数（`cal_x`, `cal_y` 等）
- 校准参数可保存至 Flash 或 EEPROM 以避免重复校准

## 📦 依赖

- STM32 HAL 库（推荐）或标准外设库
- 已正确初始化 SPI（用于 LCD 和 Touch）
- 若使用 FSMC，需配置 FSMC 外设及时序

## 📌 注意事项

- **SPI 时钟频率**：LCD 可达 20~50 MHz，Touch 建议 ≤ 2 MHz（XPT2046 限制）
- **引脚复用**：确保 SPI、GPIO 功能正确映射
- **电源稳定性**：TFT 背光电流较大，建议独立供电
- **抗干扰**：触摸屏走线远离高频信号，必要时增加滤波电容

## 📄 许可证

GPL3.0

## 🙌 示例效果

`main.c` 通常包含一个演示界面：

- 显示彩色色块
- 绘制几何图形
- 实时响应触摸位置（画点或画线）
- 显示中英文混合文本

> 💡 **提示**：此 BSP 已广泛用于 STM32F103、STM32F407、STM32H743 等开发板（如探索者、战舰、阿波罗）。 