/*
 * fsm.h
 *
 *  Created on: Feb 4, 2025
 *      Author: kudaz
 */

#ifndef INC_FSM_H_
#define INC_FSM_H_

#include "stm32f1xx_hal.h"
#include <stdint.h>


typedef enum {
    S_Inicializacion,
	S_TIG,
	S_MMA,
	S_Mode,
	S_Electrode,
	S_HotStart,
	S_ArcForce,
	S_AntiStick,
	S_VRD,
	S_OverHeat,
	S_Error
} State_t;

extern State_t currentState;
extern uint32_t t_fsm;

uint8_t MenuButton(void);
void fsm_Init(void);
void fsm_SetState(State_t newState);
void fsm_Update(void);

#endif /* INC_FSM_H_ */
