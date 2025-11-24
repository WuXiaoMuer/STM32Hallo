- # W25Qxx SPI Flash 驱动库

  ## 概述

  本库提供了针对 W25Qxx 系列 SPI Flash 芯片的驱动实现，基于 STM32 HAL 库开发，支持芯片初始化、数据读写、扇区擦除和整片擦除等基本操作。适用于需要使用 SPI Flash 进行数据存储的嵌入式项目。

  ## 支持的芯片型号

  主要针对 W25Q128FV 进行开发，兼容 W25Q 系列其他型号（需根据实际型号修改宏定义）：

  - 支持 128Mbit (16MB) 存储容量
  - 支持 64KB 扇区擦除、4KB 子扇区擦除和整片擦除
  - 支持 256 字节页编程

  ## 接口说明

  ### 初始化函数

  ```c
  uint8_t BSP_W25Qx_Init(void);
  ```

  - 功能：初始化 W25Qx 芯片
  - 返回值：初始化状态（W25Qx_OK 表示成功）

  ### 数据读写函数

  ```c
  // 读取数据
  uint8_t BSP_W25Qx_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size);
  
  // 写入数据
  uint8_t BSP_W25Qx_Write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size);
  ```

  - 参数：
    - pData：数据缓冲区指针
    - ReadAddr/WriteAddr：操作起始地址（24 位）
    - Size：数据大小（字节）
  - 返回值：操作状态（W25Qx_OK 表示成功）

  ### 擦除函数

  ```c
  // 扇区擦除（64KB）
  uint8_t BSP_W25Qx_Erase_Block(uint32_t Address);
  
  // 整片擦除
  uint8_t BSP_W25Qx_Erase_Chip(void);
  ```

  - 参数：
    - Address：扇区擦除时指定扇区内任意地址
  - 返回值：操作状态（W25Qx_OK 表示成功）

  ### 其他函数

  ```c
  // 读取芯片ID
  void BSP_W25Qx_Read_ID(uint8_t *ID);
  
  // 写使能
  uint8_t BSP_W25Qx_WriteEnable(void);
  ```

  ## 使用方法

  1. 确保 SPI 外设已正确初始化（参考 stm32f1xx_hal_spi.h）
  2. 配置 CS 引脚（通过 W25Qx_Enable ()/W25Qx_Disable () 宏定义）
  3. 调用 BSP_W25Qx_Init () 初始化芯片
  4. 进行读写操作前需确保对应区域已擦除
  5. 写入操作会自动处理跨页问题

  ## 注意事项

  1. 写入操作前必须进行擦除（擦除后所有位为 1，写入只能将 1 改为 0）
  2. 擦除和写入操作需要一定时间，函数内部已包含等待机制
  3. 整片擦除耗时较长（最长约 250 秒），使用时需注意
  4. 操作超时时间可通过 W25Qx_TIMEOUT_VALUE 宏定义修改
  5. 不同型号芯片的扇区大小和擦除时间可能不同，需根据实际型号调整宏定义

  ## 宏定义说明

  重要宏定义位于 W25Qxx.h 中：

  - 存储容量和扇区大小定义
  - 命令字定义（读、写、擦除等操作命令）
  - 状态寄存器标志位
  - 超时时间设置
  - CS 引脚控制宏

  ## 错误码说明

  - W25Qx_OK：操作成功
  - W25Qx_ERROR：操作错误
  - W25Qx_BUSY：设备忙
  - W25Qx_TIMEOUT：操作超时