1. **OLED 显示屏相关操作**
   - 初始化：`bsp_oled_init`函数通过向 OLED 写入一系列初始化命令完成 OLED 的初始化。
   - 数据写入：`OLED_Writ_Bus`用于向 OLED 写入一个字节数据，`OLED_WR_REG`用于向 OLED 写入命令。
   - 显示控制：`bsp_oled_show`函数通过 SPI_DMA 方式将缓冲区数据发送到 OLED 进行显示；`bsp_oled_clear`函数用于清空 OLED 显示缓冲区。
   - 图形绘制：包括`bsp_oled_point`（画点）、`bsp_oled_line`（画线）、`bsp_oled_circle`（画圆）等函数，可在 OLED 上绘制各种图形。
   - 字符显示：`bsp_oled_ascii`用于显示 ASCII 字符，`bsp_oled_hz`用于显示汉字，通过查找字符库并在对应位置画点来实现。
   - 滚动控制：`ssd1331_hw_scroll_start`函数用于启动 OLED 硬件滚动，支持水平、垂直、对角线等多种滚动模式，并可设置滚动参数；`ssd1331_hw_scroll_stop`函数用于停止滚动。