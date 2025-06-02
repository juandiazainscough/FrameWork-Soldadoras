/*
 * ST7789.h
 *
 *  Created on: May 24, 2025
 *      Author: kudaz
 */

#ifndef INC_ST7789_H_
#define INC_ST7789_H_

#include "stm32f1xx_hal.h"
#include <stdint.h>

extern volatile uint8_t dma_busy;

void ST7789_Reset(void);
void ST7789_WriteCommand(uint8_t cmd);
void ST7789_WriteData(uint8_t *data, uint16_t size);
void ST7789_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void ST7789_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void ST7789_Init(void);
void ST7789_Fill(uint16_t color);
void ST7789_Fill_DMA(uint16_t color);
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi);
void ST7789_SetCursor(uint16_t x, uint16_t y);
void ST7789_PrintChar(uint16_t x, uint16_t y, uint8_t font_size, char ch, uint16_t color);
void ST7789_PrintChar_DMA(uint16_t x, uint16_t y, uint8_t font_size, char ch, uint16_t color, uint16_t bg);
void ST7789_DrawBitmap(uint16_t x, uint16_t y, const uint8_t *bitmap, uint16_t w, uint16_t h, uint16_t color, uint16_t bg);
void ST7789_DrawBitmap_DMA(uint16_t x, uint16_t y, const uint8_t *bitmap, uint16_t w, uint16_t h, uint16_t color, uint16_t bg);
//void ST7789_DrawPELogo(void);
void ST7789_DrawPELogo_DMA(void);
void ST7789_DrawPELogo_NextBlock(void);
void ST7789_FillRegion(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void ST7789_PrintTextInBox(uint16_t x, uint16_t y, uint8_t font_size, const char *text, uint8_t fila_sup, uint8_t fila_inf,  uint16_t fg_color, uint16_t bg_color);
void ST7789_PrintTextInBox_DMA(uint16_t x, uint16_t y, uint8_t font_size, const char *text,
                               uint8_t fila_sup, uint8_t fila_inf,
                               uint16_t fg_color, uint16_t bg_color);
#endif /* INC_ST7789_H_ */
