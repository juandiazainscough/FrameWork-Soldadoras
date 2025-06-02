/*
 * flash.h
 *
 *  Created on: Feb 13, 2025
 *      Author: kudaz
 */

#ifndef INC_FLASH_H_
#define INC_FLASH_H_

#include "stm32f1xx_hal.h"
#include <stdint.h>

#define FLASH_PAGE_ADDRESS  0x0800FC00  //ultima pagina de la FLASH

/*
#define A_ADDR  		(FLASH_PAGE_ADDRESS + 0)
#define TIG_ADDR        (FLASH_PAGE_ADDRESS + 4)
#define VRD_ADDR        (FLASH_PAGE_ADDRESS + 8)
#define HOTSTART_ADDR   (FLASH_PAGE_ADDRESS + 12)
#define ANTISTICK_ADDR  (FLASH_PAGE_ADDRESS + 16)
#define ARCFORCE_ADDR   (FLASH_PAGE_ADDRESS + 20)
#define ELECTRODE_ADDR  (FLASH_PAGE_ADDRESS + 24)
// #define X_ADDR   (FLASH_PAGE_ADDRESS + 28)
*/
// Metodo de páginas exclusivas :
#define A_ADDR  		0x0800F800
#define TIG_ADDR        0x0800FC00
#define VRD_ADDR        0x0800F400
#define HOTSTART_ADDR   0x0800F000
#define ANTISTICK_ADDR  0x0800EC00
#define ARCFORCE_ADDR   0x0800E800
#define ELECTRODE_ADDR  0x0800E400

void Flash_Write(uint32_t address, uint32_t data);
uint32_t Flash_Read(uint32_t address);
void Save_Config(uint32_t A, uint32_t TIG, uint32_t VRD, uint32_t HotStart, uint32_t AntiStick, uint32_t ArcForce, uint32_t Electrode);
void Load_Config(uint32_t *A, uint32_t *TIG, uint32_t *VRD, uint32_t *HotStart, uint32_t *AntiStick, uint32_t *ArcForce, uint32_t *Electrode);

#endif /* INC_FLASH_H_ */
