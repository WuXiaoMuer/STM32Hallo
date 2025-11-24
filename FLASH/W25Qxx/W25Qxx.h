#ifndef __W25Qxx_H
#define __W25Qxx_H
 
/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx.h"
#include "spi.h"


#define W25Q128FV_FLASH_SIZE                  0x1000000 /* 128 MBits => 16MBytes */
#define W25Q128FV_SECTOR_SIZE                 0x10000   /* 256 sectors of 64KBytes */
#define W25Q128FV_SUBSECTOR_SIZE              0x1000    /* 4096 subsectors of 4kBytes */
#define W25Q128FV_PAGE_SIZE                   0x100     /* 65536 pages of 256 bytes */
 
#define W25Q128FV_DUMMY_CYCLES_READ           4
#define W25Q128FV_DUMMY_CYCLES_READ_QUAD      10
 
#define W25Q128FV_BULK_ERASE_MAX_TIME         250000
#define W25Q128FV_SECTOR_ERASE_MAX_TIME       3000
#define W25Q128FV_SUBSECTOR_ERASE_MAX_TIME    800
#define W25Qx_TIMEOUT_VALUE 1000
 
/* Reset Operations */
#define RESET_ENABLE_CMD                     0x66
#define RESET_MEMORY_CMD                     0x99
 
#define ENTER_QPI_MODE_CMD                   0x38
#define EXIT_QPI_MODE_CMD                    0xFF
 
/* Identification Operations */
#define READ_ID_CMD                          0x90
#define DUAL_READ_ID_CMD                     0x92
#define QUAD_READ_ID_CMD                     0x94
#define READ_JEDEC_ID_CMD                    0x9F
 
/* Read Operations */
#define READ_CMD                             0x03
#define FAST_READ_CMD                        0x0B
#define DUAL_OUT_FAST_READ_CMD               0x3B
#define DUAL_INOUT_FAST_READ_CMD             0xBB
#define QUAD_OUT_FAST_READ_CMD               0x6B
#define QUAD_INOUT_FAST_READ_CMD             0xEB
 
/* Write Operations */
#define WRITE_ENABLE_CMD                     0x06
#define WRITE_DISABLE_CMD                    0x04
 
/* Register Operations */
#define READ_STATUS_REG1_CMD                  0x05
#define READ_STATUS_REG2_CMD                  0x35
#define READ_STATUS_REG3_CMD                  0x15
 
#define WRITE_STATUS_REG1_CMD                 0x01
#define WRITE_STATUS_REG2_CMD                 0x31
#define WRITE_STATUS_REG3_CMD                 0x11
 
 
/* Program Operations */
#define PAGE_PROG_CMD                        0x02
#define QUAD_INPUT_PAGE_PROG_CMD             0x32
 
 
/* Erase Operations */
#define SECTOR_ERASE_CMD                     0x20
#define CHIP_ERASE_CMD                       0xC7
 
#define PROG_ERASE_RESUME_CMD                0x7A
#define PROG_ERASE_SUSPEND_CMD               0x75
 
 
/* Flag Status Register */
#define W25Q128FV_FSR_BUSY                    ((uint8_t)0x01)    /*!< busy */
#define W25Q128FV_FSR_WREN                    ((uint8_t)0x02)    /*!< write enable */
#define W25Q128FV_FSR_QE                      ((uint8_t)0x02)    /*!< quad enable */
 
 
#define W25Qx_Enable() 			HAL_GPIO_WritePin(W25_CS_GPIO_Port, W25_CS_Pin, GPIO_PIN_RESET)
#define W25Qx_Disable() 		HAL_GPIO_WritePin(W25_CS_GPIO_Port, W25_CS_Pin, GPIO_PIN_SET)
 
#define W25Qx_OK            ((uint8_t)0x00)
#define W25Qx_ERROR         ((uint8_t)0x01)
#define W25Qx_BUSY          ((uint8_t)0x02)
#define W25Qx_TIMEOUT		((uint8_t)0x03)
 
 /**
  * @brief  初始化 W25Qx 系列 SPI Flash 芯片。
  * @param  None
  * @retval uint8_t: 初始化状态。
  *         - 0: 成功
  *         - 非0: 失败
  * @note   此函数通常会调用 SPI 底层初始化，并通过读取芯片 ID 来验证通信是否正常。
  */
uint8_t BSP_W25Qx_Init(void);

/**
  * @brief  向 W25Qx 发送复位命令。
  * @param  None
  * @retval None
  * @note   这是一个静态辅助函数，仅在本文件内可见。
  */
static void BSP_W25Qx_Reset(void);

/**
  * @brief  读取 W25Qx 的状态寄存器。
  * @param  None
  * @retval uint8_t: 状态寄存器的值。
  * @note   这是一个静态辅助函数，通常用于检查写操作是否完成 (WIP bit)。
  */
static uint8_t BSP_W25Qx_GetStatus(void);

/**
  * @brief  发送写使能命令 (Write Enable)。
  * @param  None
  * @retval uint8_t: 操作状态。
  *         - 0: 成功
  *         - 非0: 失败
  * @note   在执行任何写或擦除操作之前，必须先调用此函数。
  */
uint8_t BSP_W25Qx_WriteEnable(void);

/**
  * @brief  读取 W25Qx 的制造商 ID 和设备 ID。
  * @param  ID: 指向一个至少 2 字节数组的指针，用于存储读取到的 ID。
  *         - ID[0] 将包含制造商 ID (如 0xEF 代表 Winbond)
  *         - ID[1] 将包含设备 ID (如 0x15 代表 W25Q128)
  * @retval None
  */
void BSP_W25Qx_Read_ID(uint8_t *ID);

/**
  * @brief  从指定地址读取数据。
  * @param  pData: 指向用于存储读取数据的缓冲区的指针。
  * @param  ReadAddr: 要读取的起始地址 (24位地址)。
  * @param  Size: 要读取的数据字节数。
  * @retval uint8_t: 操作状态。
  *         - 0: 成功
  *         - 非0: 失败
  */
uint8_t BSP_W25Qx_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size);

/**
  * @brief  向指定地址写入数据。
  * @param  pData: 指向要写入的数据缓冲区的指针。
  * @param  WriteAddr: 要写入的起始地址 (24位地址)。
  * @param  Size: 要写入的数据字节数。
  * @retval uint8_t: 操作状态。
  *         - 0: 成功
  *         - 非0: 失败
  * @note   此函数内部应处理页边界问题。如果写入的数据跨页，
  *         函数需要进行拆分，确保每一页的写入都在页内完成。
  *         写入前会自动调用写使能。
  */
uint8_t BSP_W25Qx_Write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size);

/**
  * @brief  擦除一个指定的块 (Block)。
  * @param  Address: 块内的任意一个地址。函数会自动定位到该地址所在的块。
  * @retval uint8_t: 操作状态。
  *         - 0: 成功
  *         - 非0: 失败
  * @note   W25Qx 的擦除操作只能将位从 1 置为 0。擦除单位有块 (Block, 通常为 64KB)、
  *         扇区 (Sector, 通常为 4KB) 等。此函数执行块擦除。
  *         擦除前会自动调用写使能，并且需要等待操作完成。
  */
uint8_t BSP_W25Qx_Erase_Block(uint32_t Address);

/**
  * @brief  擦除整个 Flash 芯片。
  * @param  None
  * @retval uint8_t: 操作状态。
  *         - 0: 成功
  *         - 非0: 失败
  * @note   此操作会删除芯片上的所有数据，耗时较长 (可能需要几十秒)。
  *         擦除前会自动调用写使能，并且需要等待操作完成。
  */
uint8_t BSP_W25Qx_Erase_Chip(void);


#endif 
 
 