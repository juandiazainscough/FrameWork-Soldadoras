/*
 * fsm.c
 *
 *  Created on: Feb 4, 2025
 *      Author: kudaz
 */

#include "fsm.h"
#include "user.h"
#include "SSD1306.h"
#include "fonts.h"
#include "stm32f1xx_hal.h"
#include "gpio.h"
#include "main.h"
#include "flash.h"
#include <string.h>
#include <stdio.h>



State_t currentState = S_Inicializacion;
uint32_t t_fsm;
uint32_t t_Error;

uint8_t MenuButton(void){
	return button_pressed(TEC1_GPIO_Port, TEC1_Pin);
}
void fsm_Init(void){
	currentState = S_Inicializacion;
	t_fsm = HAL_GetTick();
}
void fsm_SetState(State_t newState){
	currentState = newState;
}
void fsm_Update(void){
	if (OVERHEAT == 1 && currentState != S_OverHeat) {
		  	        currentState = S_OverHeat;
		  	        SSD1306_Fill(0);
		  	        SSD1306_PrintText(0, 0, 2, "OVERHEAT");
		  	        SSD1306_UpdateScreen();
		  	    }
/*	if (!FTest() && currentState != S_Error){
		HAL_Delay(2000);
		if(!FTest()){
		currentState = S_Error;
		}
		else currentState = S_Inicializacion;
	}*/

	switch(currentState){
	  	  case S_Inicializacion:
	  		  SSD1306_DrawBitmap(0, 0, bitmap_logo, 128, 64);
	  		  SSD1306_UpdateScreen();
	  		  if((HAL_GetTick() - t_fsm) >= 2000){
	  			  SSD1306_Fill(0);
	  			  if (TIG == 1){
	  				  currentState = S_TIG;
	  				  SSD1306_PrintText(0, 0, 1, "TIG");
	  			  }
	  			  else{
	  				  currentState = S_MMA;
	  				  SSD1306_PrintText(0, 0, 1, "MMA");
	  			  }
	  			  PrintCurrent(40, 20, 3, 1);
	  			  SSD1306_UpdateScreen();
	  		  }
	  		  break;
	  	  case S_MMA:
	  		  if(MenuButton()){
	  			  currentState = S_Electrode;
	  			  if(Electrode == 1){
	  				  	U_A = A;
	  				  	U_HotStart = HotStart;
	  					U_ArcForce = ArcForce;
	  			  }
	  			  SSD1306_FillRegion(0, 0, 18, 8, 0);	//borra MMA
	  			  PrintCurrent(40, 20, 3, 0);
	  			  SSD1306_UpdateScreen();
	  			  SSD1306_PrintText(0, 0, 2, "Electrode");
	  			  PrintElectrodeValue(Electrode);
	  			  SSD1306_UpdateScreen();
	  			  t_fsm = HAL_GetTick();
	  			  Save_Config(A, TIG, VRD, HotStart, AntiStick, ArcForce, Electrode);
	  		  }

	  		  if(encoder_rotated(&A, 10, 220) && currentState == S_MMA){
		  			 SSD1306_FillRegion(40, 20, 60, 24, 0);
		  			 PrintCurrent(40, 20, 3, 1);
		  			 SSD1306_UpdateScreen();
	  		  }
	  		  break;
	  	  case S_TIG:
	  		  if(MenuButton()){
	  			  currentState = S_Mode;
	  			  SSD1306_FillRegion(0, 0, 18, 8, 0);	//borra TIG
	  			  SSD1306_FillRegion(40, 20, 60, 24, 0); //borra A
	  			  SSD1306_PrintText(0, 0, 1, "LiftTig");
	  			  SSD1306_PrintText(40, 20, 3, " ON");
	  			  SSD1306_UpdateScreen();
	  			  t_fsm = HAL_GetTick();
	  			  Save_Config(A, TIG, VRD, HotStart, AntiStick, ArcForce, Electrode);
	  		  }
	  		  if(encoder_rotated(&A, 10, 220) && currentState == S_TIG){
		  			 SSD1306_FillRegion(40, 20, 60, 24, 0);
		  			 PrintCurrent(40, 20, 3, 1);
		  			 SSD1306_UpdateScreen();
	  		  }
	  		break;
	  	  case S_Electrode:
	  		if(MenuButton()){
	  			if(Electrode == 1){
	  			currentState = S_HotStart;
	  			SSD1306_FillRegion(0, 0, 91, 16, 0);
	  			SSD1306_FillRegion(40, 20, 60, 24, 0);
	  			SSD1306_UpdateScreen();
	  			SSD1306_PrintText(0, 0, 2, "HotStart");
	  			if(HotStart == 100){
	  				sprintf(buff, "+%lu%%", HotStart);
	  			}
	  			if(HotStart < 10){
	  				sprintf(buff, "+  %lu%%", HotStart);
	  			}
	  			if(HotStart >= 10 && HotStart <= 99){
	  				sprintf(buff, "+ %lu%%", HotStart);
	  			}
	  			SSD1306_PrintText(30, 20, 3, buff);
	  			SSD1306_UpdateScreen();
	  			t_fsm = HAL_GetTick();
	  		}
	  			else{
		  		currentState = S_MMA;
		  		SSD1306_FillRegion(0, 0, 91, 16, 0);
		  		SSD1306_FillRegion(40, 20, 60, 24, 0);
		  		SSD1306_UpdateScreen();
		  		SSD1306_PrintText(0, 0, 1, "MMA");
		  		PrintCurrent(40, 20, 3, 1);
		  		SSD1306_UpdateScreen();
	  			}
	  			Save_Config(A, TIG, VRD, HotStart, AntiStick, ArcForce, Electrode);
	  		}
	  		if((HAL_GetTick() - t_fsm) >= 5000){
	  			currentState = S_MMA;
	  			SSD1306_FillRegion(0, 0, 91, 16, 0);
	  			SSD1306_FillRegion(40, 20, 60, 24, 0);
	  			SSD1306_UpdateScreen();
	  			SSD1306_PrintText(0, 0, 1, "MMA");
	  			PrintCurrent(40, 20, 3, 1);
	  			SSD1306_UpdateScreen();
	  			Save_Config(A, TIG, VRD, HotStart, AntiStick, ArcForce, Electrode);
	  		}
	  		if(encoder_rotated(&Electrode, 0, 8) && currentState == S_Electrode){

	  			if(Electrode == 0) Electrode = 7;
	  			if(Electrode == 8) Electrode = 1;
	  			PrintElectrodeValue(Electrode);
	  			t_fsm = HAL_GetTick();
	  		}
	  		break;
	  	  case S_HotStart:
	  		if(MenuButton()){
	  			currentState = S_ArcForce;
	  			SSD1306_Fill(0);
	  			SSD1306_UpdateScreen();
	  			SSD1306_PrintText(0, 0, 2, "ArcForce");
	  			if(ArcForce < 10){
	  				sprintf(buff, "+  %lu%%", ArcForce);
	  			}
	  			if(ArcForce >= 10){
	  				sprintf(buff, "+ %lu%%", ArcForce);
	  			}
	  			SSD1306_PrintText(30, 20, 3, buff);
	  			SSD1306_UpdateScreen();
	  			t_fsm = HAL_GetTick();
	  			Save_Config(A, TIG, VRD, HotStart, AntiStick, ArcForce, Electrode);
	  		}
		  	if((HAL_GetTick() - t_fsm) >= 5000){
		  		currentState = S_MMA;
		  		SSD1306_FillRegion(0, 0, 72, 16, 0);
		  		SSD1306_FillRegion(30, 20, 100, 24, 0);
		  		SSD1306_UpdateScreen();
		  		SSD1306_PrintText(0, 0, 1, "MMA");
		  		PrintCurrent(40, 20, 3, 1);
		  		SSD1306_UpdateScreen();
		  		Save_Config(A, TIG, VRD, HotStart, AntiStick, ArcForce, Electrode);
		  		}
		  	if(encoder_rotated(&HotStart, 0, 100) && currentState == S_HotStart){
		  	    SSD1306_FillRegion(30, 20, 100, 24, 0);

		  	    // 🔹 Formatear correctamente el texto con solo una evaluación
		  	    if (HotStart == 100) {
		  	        sprintf(buff, "+%lu%%", HotStart);
		  	    }
		  	    else if (HotStart < 10) {
		  	        sprintf(buff, "+  %lu%%", HotStart);  // 🔹 Añade un espacio extra para alineación
		  	    }
		  	    else {
		  	        sprintf(buff, "+ %lu%%", HotStart);
		  	    }

		  	    // 🔹 Solo actualiza la pantalla si HotStart cambió
		  	    SSD1306_PrintText(30, 20, 3, buff);
		  	    SSD1306_UpdateScreen();
		  	    t_fsm = HAL_GetTick();
	  		  }
		  	break;
	  	  case S_ArcForce:
	  		if(MenuButton()){
	  			currentState = S_AntiStick;
	  			SSD1306_Fill(0);
	  			SSD1306_PrintText(0, 0, 2, "AntiStick");
	  			if(AntiStick == 1) SSD1306_PrintText(40, 20, 3, " ON");
	  			else SSD1306_PrintText(40, 20, 3, "OFF");
	  			SSD1306_UpdateScreen();
	  			t_fsm = HAL_GetTick();
	  			Save_Config(A, TIG, VRD, HotStart, AntiStick, ArcForce, Electrode);
	  		}
		  	if((HAL_GetTick() - t_fsm) >= 5000){
		  		currentState = S_MMA;
		  		SSD1306_FillRegion(0, 0, 72, 16, 0);
		  		SSD1306_FillRegion(30, 20, 100, 24, 0);
		  		SSD1306_UpdateScreen();
		  		SSD1306_PrintText(0, 0, 1, "MMA");
		  		PrintCurrent(40, 20, 3, 1);
		  		SSD1306_UpdateScreen();
		  		Save_Config(A, TIG, VRD, HotStart, AntiStick, ArcForce, Electrode);
		  		}
		  	if(encoder_rotated(&ArcForce, 0, 50) && currentState == S_ArcForce){
		  	    SSD1306_FillRegion(30, 20, 100, 24, 0);

		  	    // 🔹 Formatear correctamente el texto con solo una evaluación
		  	    if (ArcForce < 10) {
		  	        sprintf(buff, "+  %lu%%", ArcForce);  // 🔹 Añade un espacio extra para alineación
		  	    }
		  	    else {
		  	        sprintf(buff, "+ %lu%%", ArcForce);
		  	    }
		  	    // 🔹 Solo actualiza la pantalla si ArcForce cambió
		  	    SSD1306_PrintText(30, 20, 3, buff);
		  	    SSD1306_UpdateScreen();
		  	    t_fsm = HAL_GetTick();
	  		  }
		  	break;
	  	  case S_AntiStick:
	  		  if(MenuButton()){
	  			 currentState = S_VRD;
	  			 SSD1306_Fill(0);
	  			 SSD1306_PrintText(0, 0, 2, "VRD");
	  			 if(VRD == 1) SSD1306_PrintText(40, 20, 3, " ON");
	  			 else SSD1306_PrintText(40, 20, 3, "OFF");
	  			 SSD1306_UpdateScreen();
	  			 t_fsm = HAL_GetTick();
	  			 Save_Config(A, TIG, VRD, HotStart, AntiStick, ArcForce, Electrode);
	  		  }
			  if((HAL_GetTick() - t_fsm) >= 5000){
				  currentState = S_MMA;
				  SSD1306_Fill(0);
				  SSD1306_UpdateScreen();
				  SSD1306_PrintText(0, 0, 1, "MMA");
				  PrintCurrent(40, 20, 3, 1);
				  SSD1306_UpdateScreen();
				  Save_Config(A, TIG, VRD, HotStart, AntiStick, ArcForce, Electrode);
			  }
			  if(encoder_rotated(&AntiStick, 0, 1) && currentState == S_AntiStick) {

			  	  SSD1306_FillRegion(30, 20, 100, 24, 0);

			  	    // 🔹 Formatear correctamente el texto con solo una evaluación
			  	  if (AntiStick == 1) {
			  		  SSD1306_PrintText(40, 20, 3, " ON");  // 🔹 Añade un espacio extra para alineación
			  	  }
			  	  else {
			  		  SSD1306_PrintText(40, 20, 3, "OFF");
			  	  }
			  	    // 🔹 Solo actualiza la pantalla si AntiStick cambió
			  	  SSD1306_UpdateScreen();
			  	  t_fsm = HAL_GetTick();
			    }
			  break;
	  	  case S_VRD:
	  		  if(MenuButton()){
	  			 currentState = S_Mode;
	  			 SSD1306_Fill(0);
	  			 SSD1306_PrintText(0, 0, 2, "LiftTig");
	  			 SSD1306_PrintText(40, 20, 3, "OFF");
	  			 SSD1306_UpdateScreen();
	  			 Save_Config(A, TIG, VRD, HotStart, AntiStick, ArcForce, Electrode);
	  		  }
			  if((HAL_GetTick() - t_fsm) >= 5000){
				  currentState = S_MMA;
				  SSD1306_Fill(0);
				  SSD1306_UpdateScreen();
				  SSD1306_PrintText(0, 0, 1, "MMA");
				  PrintCurrent(40, 20, 3, 1);
				  SSD1306_UpdateScreen();
				  Save_Config(A, TIG, VRD, HotStart, AntiStick, ArcForce, Electrode);
			  }
			  if(encoder_rotated(&VRD, 0, 1) && currentState == S_VRD) {

			  	  SSD1306_FillRegion(30, 20, 100, 24, 0);

			  	    // 🔹 Formatear correctamente el texto con solo una evaluación
			  	  if (VRD == 1) {
			  		  SSD1306_PrintText(40, 20, 3, " ON");  // 🔹 Añade un espacio extra para alineación
			  	  }
			  	  else {
			  		  SSD1306_PrintText(40, 20, 3, "OFF");
			  	  }
			  	    // 🔹 Solo actualiza la pantalla si VRD cambió
			  	  SSD1306_UpdateScreen();
			  	  t_fsm = HAL_GetTick();
			    }
			  break;
	  	  case S_Mode:
	  		  if(MenuButton() || (HAL_GetTick() - t_fsm) >= 5000 ){
	  			  SSD1306_Fill(0);
	  			  if(TIG == 1){
	  				  currentState = S_TIG;
	  				  SSD1306_PrintText(0, 0, 1, "TIG");
	  			  }
	  			  else{
	  				  currentState = S_MMA;
	  				  SSD1306_PrintText(0, 0, 1, "MMA");
	  			  }
	  			  PrintCurrent(40, 20, 3, 1);
	  			  SSD1306_UpdateScreen();
	  			  t_fsm = HAL_GetTick();
	  			  Save_Config(A, TIG, VRD, HotStart, AntiStick, ArcForce, Electrode);
	  		  }
			  if(encoder_rotated(&TIG, 0, 1) && currentState == S_Mode) {

			  	  SSD1306_FillRegion(30, 20, 100, 24, 0);

			  	    // 🔹 Formatear correctamente el texto con solo una evaluación
			  	  if (TIG == 1) {
			  		  SSD1306_PrintText(40, 20, 3, " ON");  // 🔹 Añade un espacio extra para alineación
			  	  }
			  	  else {
			  		  SSD1306_PrintText(40, 20, 3, "OFF");
			  	  }
			  	    // 🔹 Solo actualiza la pantalla si VRD cambió
			  	  SSD1306_UpdateScreen();
			  	  t_fsm = HAL_GetTick();
			    }
			  break;
	  	  case S_OverHeat:
	  		  if(OVERHEAT == 0){
	  			  FlagOverHeat = 0;
	  			  SSD1306_Fill(0);
	  			  if(TIG == 1){
	  				  currentState = S_TIG;
	  				  SSD1306_PrintText(0, 0, 1, "TIG");
	  			  }
	  			  else{
	  				  currentState = S_MMA;
	  				  SSD1306_PrintText(0, 0, 1, "MMA");
	  			  }
	  			  PrintCurrent(40, 20, 3, 1);
	  			  SSD1306_UpdateScreen();
	  			  t_fsm = HAL_GetTick();
	  		  }
	  		  break;
/*	  	  case S_Error:
	  			if (HAL_GPIO_ReadPin(FTest_GPIO_Port, FTest_Pin) == 0){
	  				SSD1306_Fill(0);
	  				SSD1306_PrintText(0, 0, 1, "Call Service");
	  				SSD1306_UpdateScreen();
	  			}
	  			else currentState = S_Inicializacion;
	  			break;*/




	}//fin switch
}//fin funcion


// Se podrian hacer funciones eraseHotStart() por ejemplo

