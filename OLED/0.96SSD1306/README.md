# STM32 OLED 驱动库

> 📦 轻量级、模块化、易于集成的 STM32 OLED 显示驱动库，支持 128x64 分辨率的 SSD1306/SSD1315 等常见 OLED 屏幕。 

## ✨ 特性

- 支持 **128×64 像素** 单色 OLED 显示屏（如 SSD1306、SSD1315）
- 提供 **I²C** 和 **SPI** 两种通信接口（可根据硬件配置选择）
- 内置常用图形绘制函数：
  - 点、线、矩形、圆形
  - 字符串显示（支持 6×8、8×16 字体）
  - 自定义位图显示
- 模块化设计，便于移植到不同 STM32 型号
- 低资源占用，适用于资源受限的嵌入式系统
- 代码清晰，注释完整，适合学习与二次开发

## 📁 文件结构说明

├── OLED.h          // 主头文件，包含函数声明与配置宏

├── OLED.c          // 主驱动实现，包含初始化、清屏、刷新、绘图等核心函数

├── OLED_Data.h     // 字符与图片数据头文件（字体、图标等）

└── OLED_Data.c     // 字符与图片数据定义（如 ASCII 字模、自定义图标）

## ⚙️ 使用说明

### 1. 硬件连接

根据所选通信方式连接 OLED：

#### I²C 模式（推荐）

|      |                   |
| ---- | ----------------- |
| VCC  | 3.3V              |
| GND  | GND               |
| SCL  | I²C1_SCL (如 PB6) |
| SDA  | I²C1_SDA (如 PB7) |

> 注意：I²C 地址默认为 `0x78`（写）/ `0x79`（读），部分模块为 `0x7A`，可在 `OLED.h` 中修改。 

#### SPI 模式（四线）

|      |               |
| ---- | ------------- |
| VCC  | 3.3V          |
| GND  | GND           |
| SCL  | SPI_SCK       |
| SDA  | SPI_MOSI      |
| RES  | GPIO          |
| DC   | GPIO          |
| CS   | GPIO / SPI CS |

### 2. 软件配置

- 在 OLED.h中选择通信方式：

  \#define OLED_I2C    // 启用 I²C 模式

  // #define OLED_SPI // 启用 SPI 模式（取消注释并注释 I2C）

- 实现底层硬件接口（如 `I2C_WriteByte()` 或 `SPI_WriteByte()`）

- 在 main.c中初始化：

  OLED_Init();

  OLED_Clear();

  OLED_ShowString(0, 0, "Hello OLED!", 16);

  OLED_Refresh(); // 若为自动刷新可省略

### 3. 常用 API

|                                          |                              |
| ---------------------------------------- | ---------------------------- |
| `OLED_Init()`                            | 初始化 OLED                  |
| `OLED_Clear()`                           | 清屏                         |
| `OLED_Refresh()`                         | 刷新显示缓冲区到屏幕         |
| `OLED_ShowChar(x, y, ch, size)`          | 显示单个字符                 |
| `OLED_ShowString(x, y, str, size)`       | 显示字符串（size: 12 或 16） |
| `OLED_DrawPoint(x, y)`                   | 画点                         |
| `OLED_DrawLine(x1, y1, x2, y2)`          | 画线                         |
| `OLED_DrawBMP(x, y, width, height, bmp)` | 显示位图                     |

## 📦 依赖

- STM32 HAL 库 或 标准外设库（根据实现而定）
- I²C 或 SPI 外设已正确初始化
- `OLED_Data.c` 中包含 ASCII 6×8 和 8×16 字模