/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */
  
  

  
  // 初始化OLED
  OLED_Init();
  // 尝试打开显示
  OLED_DisPlay_On();
  OLED_Clear();
  OLED_Refresh();
  
  // 在OLED上绘制一些测试图形
  // 1. 画横线
  OLED_DrawLine(0, 0, 127, 0, 1);
  OLED_DrawLine(0, 63, 127, 63, 1);
  OLED_Refresh();
  HAL_Delay(100);
  
  // 2. 画竖线
  OLED_DrawLine(0, 0, 0, 63, 1);
  OLED_DrawLine(127, 0, 127, 63, 1);
  OLED_Refresh();
  HAL_Delay(100);
  
  // 3. 画对角线
  OLED_DrawLine(0, 0, 127, 63, 1);
  OLED_DrawLine(0, 63, 127, 0, 1);
  OLED_Refresh();
  HAL_Delay(100);
  
  // 4. 画一个矩形框
  OLED_Clear();
  OLED_DrawLine(10, 10, 117, 10, 1);
  OLED_DrawLine(117, 10, 117, 53, 1);
  OLED_DrawLine(117, 53, 10, 53, 1);
  OLED_DrawLine(10, 53, 10, 10, 1);
  OLED_Refresh();
  HAL_Delay(100);
  
  // 5. 最后清屏
  OLED_Clear();
  OLED_Refresh();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    // 效果1: 跑马灯效果 - 从左到右扫描
    for(int x = 0; x < 128; x++) {
      OLED_Clear();
      OLED_DrawLine(x, 0, x, 63, 1);
      OLED_Refresh();
      HAL_Delay(1);
    }
    HAL_Delay(20);
    
    // 效果2: 反向跑马灯
    for(int x = 127; x >= 0; x--) {
      OLED_Clear();
      OLED_DrawLine(x, 0, x, 63, 1);
      OLED_Refresh();
      HAL_Delay(1);
    }
    HAL_Delay(20);
    
    // 效果3: 波浪动画
    for(int frame = 0; frame < 50; frame++) {
      OLED_Clear();
      for(int x = 0; x < 128; x++) {
        int y = 32 + 15 * ((frame / 5 + x / 3) % 20 - 10);
        if(y >= 0 && y < 64) {
          OLED_DrawPoint(x, y, 1);
        }
      }
      OLED_Refresh();
      HAL_Delay(20);
    }
    
    // 效果4: 同心圆
    for(int r = 1; r < 32; r += 2) {
      OLED_Clear();
      OLED_DrawCircle(64, 32, r);
      OLED_Refresh();
      HAL_Delay(20);
    }
    HAL_Delay(100);
    
    // 效果5: 填充动画
    for(int y = 0; y < 64; y += 2) {
      OLED_DrawLine(0, y, 127, y, 1);
      OLED_Refresh();
      HAL_Delay(20);
    }
    HAL_Delay(100);
    
    // 效果6: 清空动画
    for(int y = 63; y >= 0; y -= 2) {
      OLED_DrawLine(0, y, 127, y, 0);
      OLED_Refresh();
      HAL_Delay(30);
    }
    HAL_Delay(100);
    
    // 效果7: 星爆效果 - 放射线从中心扩散
    for(int len = 1; len < 50; len += 2) {
      OLED_Clear();
      // 8条放射线
      OLED_DrawLine(64, 32, 64, 32 - len, 1);         // 上
      OLED_DrawLine(64, 32, 64, 32 + len, 1);        // 下
      OLED_DrawLine(64, 32, 64 + len, 32, 1);        // 右
      OLED_DrawLine(64, 32, 64 - len, 32, 1);        // 左
      OLED_DrawLine(64, 32, 64 + len * 7/10, 32 - len * 7/10, 1);  // 右上
      OLED_DrawLine(64, 32, 64 + len * 7/10, 32 + len * 7/10, 1); // 右下
      OLED_DrawLine(64, 32, 64 - len * 7/10, 32 - len * 7/10, 1); // 左上
      OLED_DrawLine(64, 32, 64 - len * 7/10, 32 + len * 7/10, 1); // 左下
      OLED_Refresh();
      HAL_Delay(20);
    }
    HAL_Delay(100);
    
    // 效果8: 渐近线条
//    OLED_Clear();
//    for(int i = 0; i < 20; i++) {
//      OLED_DrawLine(0 + i*6, 0, 127 - i*6, 63, 1);
//      OLED_DrawLine(127 - i*6, 0, 0 + i*6, 63, 1);
//    }
//    OLED_Refresh();
//    HAL_Delay(500);
    
    // 效果9: 文字显示 - 数字计数
//    OLED_Clear();
//    for(int i = 0; i <= 99; i++) {
//      OLED_ShowNum(40, 24, i, 2, 16, 1);
//      OLED_Refresh();
//      HAL_Delay(50);
//    }
    
    // 效果10: 文字显示 - 倒计时
//    OLED_Clear();
//    for(int i = 9; i >= 0; i--) {
//      OLED_ShowNum(56, 24, i, 1, 16, 1);
//      OLED_Refresh();
//      HAL_Delay(300);
//    }
    
    // 效果11: 文字显示 - 英文滚动
//    char text[] = "HELLO STM32! ";
//    for(int offset = 0; offset < 150; offset += 4) {
//      OLED_Clear();
//      for(int i = 0; i < 13; i++) {
//        int x = i * 8 - offset;
//        if(x >= -10 && x < 130) {
//          if(x >= 0 && x < 128) {
//            OLED_ShowChar(x, 28, text[i], 16, 1);
//          }
//        }
//      }
//      OLED_Refresh();
//      HAL_Delay(50);
//    }
    
    // 效果12: 文字闪烁效果
//    for(int i = 0; i < 10; i++) {
//      OLED_Clear();
//      OLED_ShowString(20, 24, "STM32", 16, 1);
//      OLED_Refresh();
//      HAL_Delay(200);
//      OLED_Clear();
//      OLED_Refresh();
//      HAL_Delay(200);
//    }
    for(int i = 0; i < 10; i++) {
      OLED_Clear();
      OLED_ShowChinese(0,0,0,16,1);//中
      OLED_ShowChinese(18,0,1,16,1);//景
      OLED_ShowChinese(36,0,2,16,1);//园
      OLED_ShowChinese(54,0,3,16,1);//电
      OLED_ShowChinese(72,0,4,16,1);//子
      OLED_ShowChinese(90,0,5,16,1);//技
      OLED_ShowChinese(108,0,11,16,1);//什
      OLED_Refresh();
      HAL_Delay(200);
      OLED_Clear();
      OLED_Refresh();
      HAL_Delay(200);
    }
    // 效果13: 显示ASCII字符
    OLED_Clear();
    for(char c = 'A'; c <= 'Z'; c++) {
      OLED_Clear();
      OLED_ShowChar(56, 24, c, 24, 1);
      OLED_Refresh();
      HAL_Delay(100);
    }
    HAL_Delay(500);
    
    // LED闪烁
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);  // LED
    HAL_Delay(1000);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET); // LED
    HAL_Delay(100);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
