/*
 * SSD1306.h
 *
 *  Created on: Feb 2, 2025
 *      Author: kudaz
 */

#ifndef INC_SSD1306_H_
#define INC_SSD1306_H_

#include "stm32f1xx_hal.h"
#include <stdint.h>
#include "OLED.h"

#define SSD1306_WIDTH       128
#define SSD1306_HEIGHT      64
#define SSD1306_BUFFER_SIZE (SSD1306_WIDTH * SSD1306_HEIGHT / 8)

//#define SSD1306_I2C_ADDRESS (0x3C << 1)
extern char buff[16];
extern uint8_t SSD1306_Buffer[SSD1306_BUFFER_SIZE];

//extern const uint8_t bitmap_MMA [];

void SSD1306_Init(void);
void SSD1306_UpdateScreen(void);
void SSD1306_WriteCommand(uint8_t command);
void SSD1306_Fill(uint8_t color);
void SSD1306_DrawPixel(uint8_t x, uint8_t y, uint8_t color);
void SSD1306_SetCursor(uint8_t x, uint8_t y);
void SSD1306_DrawBitmap(uint8_t x, uint8_t y, const uint8_t *bitmap, uint8_t w, uint8_t h);
void SSD1306_DrawBitmapAtCursor(const uint8_t *bitmap, uint8_t w, uint8_t h);
void SSD1306_PrintChar(uint8_t x, uint8_t y, uint8_t font_size, char ch);
void SSD1306_FillRegion(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color);
void SSD1306_PrintCharInBox(uint8_t x, uint8_t y, uint8_t font_size, char ch, uint8_t fila_sup, uint8_t col_derecha);
void SSD1306_PrintText(uint8_t x, uint8_t y, uint8_t font_size, const char *text);
void SSD1306_PrintTextInBox(uint8_t x, uint8_t y, uint8_t font_size, const char *text, uint8_t fila_sup, uint8_t col_derecha);
#endif /* INC_SSD1306_H_ */
