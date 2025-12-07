# ST7735 1.8" TFT 驱动（STM32 HAL + 软件 SPI/硬件 SPI + DMA）

为 128×160 像素 ST7735S 彩色 TFT 模块编写的轻量级驱动，已在 **AliExpress/淘宝 1.8" 屏**、**WaveShare 1.8" 屏**、**1.44" 128×128 屏** 实测通过。  
特性：

- 纯 C，无动态内存  
- 同时支持 **软件 SPI（3 线/4 线）** 与 **硬件 SPI + DMA**  
- 自带 3 种字体（7×10、11×18、16×26）  
- 提供画点、线、圆、矩形、三角形、ASCII 字符串、位图（RGB565）  
- 4 个可裁剪方向（0°/90°/180°/270°）  
- 最小帧缓冲仅 1 行，RAM &lt; 300 B  

---

## 1. 硬件连接

| ST7735 引脚 | 功能      | 推荐接法（STM32F103C8 为例） | 其他 MCU 说明           |
| ----------- | --------- | ---------------------------- | ----------------------- |
| VCC         | 3.3 V     | 3V3                          | 5 V 需电平转换          |
| GND         | GND       | GND                          |                         |
| SCL/SCK     | SPI 时钟  | PA5 (SPI1_SCK)               | 可任意 GPIO（软件 SPI） |
| SDA/MOSI    | SPI 数据  | PA7 (SPI1_MOSI)              | 同上                    |
| RES         | 复位      | PB12                         | 任意 GPIO               |
| DC          | 数据/命令 | PB13                         | 任意 GPIO               |
| CS          | 片选      | PB14                         | 任意 GPIO               |
| BL          | 背光      | PB15                         | 任意 GPIO/PWM           |

&gt; 若使用 **硬件 SPI + DMA**，仅需把 SCL/MOSI 映射到对应引脚即可；CS/DC/RES/BL 仍可用任意 GPIO。

---

## 2. 软件环境

- STM32CubeMX 6.x + Cube FW **F1/F4/F7/H7** 任意版本  
- HAL 库（LL 也可，只需重写 3 个底层函数）  
- 编译器：Keil MDK、STM32CubeIDE、IAR、arm-none-gcc 均可  
- 无 OS 要求，已测试裸机及 FreeRTOS

---

## 3. 快速上手（3 分钟跑起来）

### 3.1 CubeMX 关键步骤

1. 启用 **SPI1**  
   - Mode = **Full-Duplex Master**  
   - Hardware NSS = **Disable**（软件片选）  
   - Data Size = **8 bit**  
   - Prescaler = **4**（先保证 18 Mbit/s 以内）  
   - **Enable DMA1 Channel3 (TX)**  

2. 启用 **5 个 GPIO**（输出推挽，无上拉）  
   PB12/ST7735_RES_Pin  
   PB13/ST7735_DC_Pin  
   PB14/ST7735_CS_Pin  
   PB15/ST7735_BL_Pin  

3. 生成代码后，把本驱动所有 `.c/.h` 加入工程，并在 `main.c` 包含：

```c
#include "st7735.h"
#include "testimg.h"   // 128×128 示例图片