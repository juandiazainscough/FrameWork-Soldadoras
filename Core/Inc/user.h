/*
 * user.h
 *
 *  Created on: Feb 4, 2025
 *      Author: kudaz
 */

#ifndef INC_USER_H_
#define INC_USER_H_

#include "stm32f1xx_hal.h"
#include <stdint.h>

extern uint32_t A;
extern uint32_t Electrode;
extern uint32_t HotStart;
extern uint32_t ArcForce;
extern uint32_t AntiStick;
extern uint32_t VRD;
extern uint32_t TIG;
extern int8_t OVERHEAT;
extern int8_t OV;
extern uint32_t t_ov;
extern int8_t FlagOverHeat;


//extern uint32_t E_HotStart; // Electrode HotStart
extern uint32_t U_HotStart; // User HotStart (Electrode = 0.0)
//extern uint32_t E_ArcForce;
extern uint32_t U_ArcForce;
//extern uint32_t E_A;
extern uint32_t U_A;

uint8_t button_pressed(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
uint8_t button_pressed_dec(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint32_t *value, uint32_t minValue);
uint8_t button_pressed_inc(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint32_t *value, uint32_t maxValue);
void PrintElectrodeValue(uint32_t Electrode);
void PrintCurrent (uint8_t x, uint8_t y, uint8_t font_size, uint8_t color);
void SetVoltagePWM(float current_A);
void CheckOverheat(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
uint16_t Read_ADC_Value(ADC_HandleTypeDef* hadc);
float ADC_to_Voltage(uint16_t adc_value);
uint8_t DetectWelding(ADC_HandleTypeDef* hadc);
void DetectGenerating(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint8_t VRD, uint8_t welding);
uint8_t encoder_rotated(uint32_t *value, uint32_t minValue, uint32_t maxValue);
void updateADC_Readings(void);
uint32_t filterADC(uint32_t new_sample);
uint32_t filterADC_Channel(uint8_t ch, uint32_t sample);
uint8_t FTest();
#endif /* INC_USER_H_ */
