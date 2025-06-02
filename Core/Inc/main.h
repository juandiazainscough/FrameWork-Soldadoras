/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* TEST SSD1306

      SSD1306_FillRegion(0, 0, 126, 1, 1);
	  SSD1306_PrintCharInBox(0, 1, 1, 'A');
	  SSD1306_PrintCharInBox(6, 1, 1, 'B');
	  SSD1306_PrintCharInBox(12, 1, 1, 'C');
	  SSD1306_PrintCharInBox(18, 1, 1, 'D');
	  SSD1306_PrintCharInBox(24, 1, 1, 'E');
	  SSD1306_PrintCharInBox(30, 1, 1, 'F');
	  SSD1306_PrintCharInBox(36, 1, 1, 'G');
	  SSD1306_PrintCharInBox(42, 1, 1, 'H');
	  SSD1306_PrintCharInBox(48, 1, 1, 'I');
	  SSD1306_PrintCharInBox(54, 1, 1, 'J');
	  SSD1306_PrintCharInBox(60, 1, 1, 'K');
	  SSD1306_PrintCharInBox(66, 1, 1, 'L');
	  SSD1306_PrintCharInBox(72, 1, 1, 'M');
	  SSD1306_PrintCharInBox(78, 1, 1, 'N');
	  SSD1306_PrintCharInBox(84, 1, 1, 'O');
	  SSD1306_PrintCharInBox(90, 1, 1, 'P');
	  SSD1306_PrintCharInBox(96, 1, 1, 'Q');
	  SSD1306_PrintCharInBox(102, 1, 1, 'R');
	  SSD1306_PrintCharInBox(108, 1, 1, 'S');
	  SSD1306_PrintCharInBox(114, 1, 1, 'T');
	  SSD1306_PrintCharInBox(120, 1, 1, 'V');
	  SSD1306_FillRegion(126, 0, 1, 9, 1);
	  SSD1306_PrintCharInBox(0, 9, 1, 'W');
	  SSD1306_PrintCharInBox(6, 9, 1, 'X');
	  SSD1306_PrintCharInBox(12, 9, 1, 'Y');
	  SSD1306_PrintCharInBox(18, 9, 1, 'Z');

	  SSD1306_PrintCharInBox(0, 22, 2, 'A');
	  SSD1306_PrintCharInBox(9, 22, 2, 'B');
	  SSD1306_PrintCharInBox(18, 22, 2, 'C');
	  SSD1306_PrintCharInBox(27, 22, 2, 'D');
	  SSD1306_PrintCharInBox(36, 22, 2, 'E');
	  SSD1306_PrintCharInBox(45, 22, 2, 'F');
	  SSD1306_PrintCharInBox(54, 22, 2, 'G');
	  SSD1306_PrintCharInBox(63, 22, 2, 'H');
	  SSD1306_PrintCharInBox(72, 22, 2, 'I');
	  SSD1306_PrintCharInBox(81, 22, 2, 'J');
	  SSD1306_PrintCharInBox(90, 22, 2, 'K');

	  SSD1306_PrintCharInBox(0, 40, 3, 'Z');
	  SSD1306_PrintCharInBox(20, 40, 3, 'A');
	  SSD1306_PrintCharInBox(40, 40, 3, 'B');
	  SSD1306_PrintCharInBox(60, 40, 3, 'C');
	  SSD1306_PrintCharInBox(80, 40, 3, 'D');
	  SSD1306_PrintCharInBox(100, 40, 3, 'E');
	  SSD1306_PrintCharInBox(120, 40, 3, 'F');

	  SSD1306_UpdateScreen();

	  HAL_Delay(3000);
	  SSD1306_FillRegion(0, 0, 127, 9, 0);
	  SSD1306_UpdateScreen();

	  HAL_Delay(3000);
	  SSD1306_Fill(0);
	  SSD1306_UpdateScreen();

	  SSD1306_PrintText(0, 0, 1, "MMA");
	  SSD1306_PrintText(20, 20, 2, "VRD");
	  SSD1306_PrintText(40, 40, 3, "10");
	  SSD1306_UpdateScreen();

	  HAL_Delay(3000);
	  SSD1306_Fill(0);
	  SSD1306_UpdateScreen();

	  SSD1306_PrintTextInBox(0, 1, 1, "MMA");
	  SSD1306_PrintTextInBox(20, 20, 2, "VRD");
	  SSD1306_PrintTextInBox(40, 40, 3, "180");
	  SSD1306_UpdateScreen();

	  while(1){
	  }

 Menu IRON 250 Pro:
 	  SSD1306_PrintTextInBox(0, 1, 1, "MMA", 1, 1);	// x, y, font, text, u,r

	  SSD1306_PrintTextInBox(0, 20, 1, " HOT ", 1, 1);
	  SSD1306_PrintTextInBox(0, 28, 1, "START", 1, 1);

	  SSD1306_PrintTextInBox(0, 38, 1, " ARC ", 1, 1);
	  SSD1306_PrintTextInBox(0, 46 , 1, "FORCE", 1, 1);

	  SSD1306_PrintTextInBox(0, 56, 1, "ANTISTICK", 1, 1);

	  SSD1306_PrintText(75, 0, 1, "VRD");

	  SSD1306_PrintText(60, 20, 3, "180");

	  SSD1306_UpdateScreen();


	  LOGO Prestaciones:

	  SSD1306_DrawBitmap(0, 0, bitmap_logo, 128, 64);
	  SSD1306_UpdateScreen();
	  HAL_Delay(2000);
	  SSD1306_Fill(0);
	  SSD1306_UpdateScreen();
 */
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define TEC1_Pin GPIO_PIN_13
#define TEC1_GPIO_Port GPIOC
#define EIZQ_Pin GPIO_PIN_14
#define EIZQ_GPIO_Port GPIOC
#define EDER_Pin GPIO_PIN_15
#define EDER_GPIO_Port GPIOC
#define ENCB_Pin GPIO_PIN_0
#define ENCB_GPIO_Port GPIOB
#define LED1_Pin GPIO_PIN_10
#define LED1_GPIO_Port GPIOB
#define TERMIC_Pin GPIO_PIN_11
#define TERMIC_GPIO_Port GPIOB
#define DG_Pin GPIO_PIN_10
#define DG_GPIO_Port GPIOA
#define ENCA_Pin GPIO_PIN_11
#define ENCA_GPIO_Port GPIOA
#define FTest_Pin GPIO_PIN_12
#define FTest_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
