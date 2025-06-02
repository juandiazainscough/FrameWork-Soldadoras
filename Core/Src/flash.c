/*
 * flash.c
 *
 *  Created on: Feb 13, 2025
 *      Author: kudaz
 */
#include "flash.h"
#include <stdio.h>
#include "gpio.h"

void Flash_Write(uint32_t address, uint32_t data) {
    HAL_FLASH_Unlock();  // 🔓 Desbloquear memoria Flash
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, data);  // ✍ Escribir dato
    HAL_FLASH_Lock();  // 🔒 Bloquear la Flash
}
uint32_t Flash_Read(uint32_t address) {
    return *(volatile uint32_t*)address;  // 📖 Leer directamente desde la Flash
}
void Save_Config(uint32_t A, uint32_t TIG, uint32_t VRD, uint32_t HotStart, uint32_t AntiStick, uint32_t ArcForce, uint32_t Electrode) {
    uint32_t current_A = Flash_Read(A_ADDR);
    uint32_t current_TIG = Flash_Read(TIG_ADDR);
    uint32_t current_VRD = Flash_Read(VRD_ADDR);
    uint32_t current_HotStart = Flash_Read(HOTSTART_ADDR);
    uint32_t current_AntiStick = Flash_Read(ANTISTICK_ADDR);
    uint32_t current_ArcForce = Flash_Read(ARCFORCE_ADDR);
    uint32_t current_Electrode = Flash_Read(ELECTRODE_ADDR);

    HAL_FLASH_Unlock();  // 🔓 Desbloqueamos la Flash UNA SOLA VEZ

    FLASH_EraseInitTypeDef eraseInitStruct;
    uint32_t pageError = 0;
    eraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    eraseInitStruct.NbPages = 1;

    // 📌 Solo borramos y escribimos en la página de la variable que cambió
    if (current_A != A) {
        eraseInitStruct.PageAddress = A_ADDR;
        HAL_FLASHEx_Erase(&eraseInitStruct, &pageError);
        Flash_Write(A_ADDR, A);
    }
    if (current_TIG != TIG) {
        eraseInitStruct.PageAddress = TIG_ADDR;
        HAL_FLASHEx_Erase(&eraseInitStruct, &pageError);
        Flash_Write(TIG_ADDR, TIG);
    }
    if (current_VRD != VRD) {
        eraseInitStruct.PageAddress = VRD_ADDR;
        HAL_FLASHEx_Erase(&eraseInitStruct, &pageError);
        Flash_Write(VRD_ADDR, VRD);
    }
    if (current_HotStart != HotStart) {
        eraseInitStruct.PageAddress = HOTSTART_ADDR;
        HAL_FLASHEx_Erase(&eraseInitStruct, &pageError);
        Flash_Write(HOTSTART_ADDR, HotStart);
    }
    if (current_AntiStick != AntiStick) {
        eraseInitStruct.PageAddress = ANTISTICK_ADDR;
        HAL_FLASHEx_Erase(&eraseInitStruct, &pageError);
        Flash_Write(ANTISTICK_ADDR, AntiStick);
    }
    if (current_ArcForce != ArcForce) {
        eraseInitStruct.PageAddress = ARCFORCE_ADDR;
        HAL_FLASHEx_Erase(&eraseInitStruct, &pageError);
        Flash_Write(ARCFORCE_ADDR, ArcForce);
    }
    if (current_Electrode != Electrode) {
        eraseInitStruct.PageAddress = ELECTRODE_ADDR;
        HAL_FLASHEx_Erase(&eraseInitStruct, &pageError);
        Flash_Write(ELECTRODE_ADDR, Electrode);
    }

    HAL_FLASH_Lock();  // 🔒 Bloqueamos la Flash SOLO AL FINAL
}
void Load_Config(uint32_t *A, uint32_t *TIG, uint32_t *VRD, uint32_t *HotStart, uint32_t *AntiStick, uint32_t *ArcForce, uint32_t *Electrode) {
    *A = Flash_Read(A_ADDR);
    *TIG = Flash_Read(TIG_ADDR);
    *VRD = Flash_Read(VRD_ADDR);
    *HotStart = Flash_Read(HOTSTART_ADDR);
    *AntiStick = Flash_Read(ANTISTICK_ADDR);
    *ArcForce = Flash_Read(ARCFORCE_ADDR);
    *Electrode = Flash_Read(ELECTRODE_ADDR);

    // 📌 Si la Flash está vacía, asignamos valores por defecto
    if (*A == 0xFFFFFFFF) *A = 10;
    if (*TIG == 0xFFFFFFFF) *TIG = 0;
    if (*VRD == 0xFFFFFFFF) *VRD = 0;
    if (*HotStart == 0xFFFFFFFF) *HotStart = 0;
    if (*AntiStick == 0xFFFFFFFF) *AntiStick = 0;
    if (*ArcForce == 0xFFFFFFFF) *ArcForce = 0;
    if (*Electrode == 0xFFFFFFFF) *Electrode = 1;
}
