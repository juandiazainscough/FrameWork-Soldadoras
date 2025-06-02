/*
 * fonts.h
 *
 *  Created on: Feb 3, 2025
 *      Author: kudaz
 */

#ifndef INC_FONTS_H_
#define INC_FONTS_H_

#include "stm32f1xx_hal.h"
#include <stdint.h>

#define font_1_WIDTH    6
#define font_1_HEIGHT   8

#define font_2_WIDTH    9
#define font_2_HEIGHT   16

#define font_3_WIDTH    20
#define font_3_HEIGHT   24

extern const uint8_t bitmap_MMA [];
extern const uint8_t font_1[552];
extern const uint8_t font_2[1710];
extern const uint8_t font_3[5760];
extern const uint8_t bitmap_logo [];


#endif /* INC_FONTS_H_ */
