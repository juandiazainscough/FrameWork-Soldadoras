/*
 * notas.h
 *
 *  Created on: Feb 4, 2025
 *      Author: kudaz
 */

#ifndef INC_NOTAS_H_
#define INC_NOTAS_H_

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


	  TICK

	   if((HAL_GetTick() - t_inicio) >= 1000){
		  HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
		  t_inicio = HAL_GetTick();
	  }
	   if(button_pressed(TEC1_GPIO_Port, TEC1_Pin)){
		  HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
	  }

	  main
	   MX_GPIO_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  SSD1306_Init();
  fsm_Init();

  SSD1306_Fill(0);
  SSD1306_UpdateScreen();
  uint32_t t_HeartBeat;
  t_HeartBeat = HAL_GetTick();
  while (1)
  {
 	  fsm_Update();
	  if((HAL_GetTick() - t_HeartBeat) >= 500){
		  HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
		  t_HeartBeat = HAL_GetTick();
	  	  }
	  SetVoltagePWM(A);
	  CheckOverheat(TERMIC_GPIO_Port, TERMIC_Pin);

  }

}
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "tim.h"
#include "gpio.h"
#include "user.h"
#include "SSD1306.h"
#include "fsm.h"
#include "flash.h"

  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  MX_ADC1_Init();
  HAL_ADC_Start(&hadc1);
  SSD1306_Init();
  fsm_Init();

  SSD1306_Fill(0);
  SSD1306_UpdateScreen();

  Load_Config(&A, &TIG, &VRD, &HotStart, &AntiStick, &ArcForce, &Electrode);

  uint32_t t_HeartBeat = HAL_GetTick();

  while (1)
  {
    /* USER CODE END WHILE
	  uint8_t weldStatus = DetectWelding(&hadc1);
 	  fsm_Update();
	  if((HAL_GetTick() - t_HeartBeat) >= 500){
		  HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
		  t_HeartBeat = HAL_GetTick();
	  	  }
	  SetVoltagePWM(A);
	  CheckOverheat(TERMIC_GPIO_Port, TERMIC_Pin);
	  DetectGenerating(DG_GPIO_Port, DG_Pin, VRD, weldStatus);



  }

}
PROXIMOS PASOS:

1-ENCODER 2-PRESETS 3-LETRA 4-ADC 5-PCB 6-TEMPERATURA ...
 */

#endif /* INC_NOTAS_H_ */
