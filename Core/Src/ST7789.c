/*
 * ST7789.c
 *
 *  Created on: May 24, 2025
 *      Author: kudaz
 */
#include "ST7789.h"
#include "gpio.h"
#include "fonts.h"
#include <string.h>

//"buffer" 0
#define ST7789_WIDTH  320
#define ST7789_HEIGHT 240
#define BLOCK_PIXELS  4096  // 4096 bytes, 4kB RAM

#define TILE_HEIGHT 15
#define PE_WIDTH 320
#define PE_HEIGHT 148
#define BUFFER_WIDTH  320
#define BUFFER_HEIGHT 15

#define MAX_TEXT_LEN 8 // para texto sin bordes puedo usar 10
#define MAX_CHAR_WIDTH 24
#define MAX_CHAR_HEIGHT 24

static uint16_t buffer[BUFFER_WIDTH * BUFFER_HEIGHT];
static uint16_t dma_tile_row = 0;

extern const uint8_t bitmap_logo2[];

volatile uint8_t dma_busy = 0;

uint8_t dma_block[BLOCK_PIXELS * 2];
uint32_t dma_pixels_sent = 0;
uint16_t dma_fill_color = 0;
uint32_t dma_total_pixels = ST7789_WIDTH * ST7789_HEIGHT;
static uint16_t cursor_x = 0;
static uint16_t cursor_y = 0;

extern SPI_HandleTypeDef hspi1;

typedef enum {
    DMA_MODE_NONE,
    DMA_MODE_FILL,
    DMA_MODE_PELOGO,
	DMA_MODE_TEXT
} DMA_Mode_t;

volatile DMA_Mode_t dma_mode = DMA_MODE_NONE;

void ST7789_Reset(void) {
    HAL_GPIO_WritePin(SPI_RES_GPIO_Port, SPI_RES_Pin,  GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(SPI_RES_GPIO_Port, SPI_RES_Pin, GPIO_PIN_SET);
    HAL_Delay(10);
}
void ST7789_WriteCommand(uint8_t cmd) {
    HAL_GPIO_WritePin(SPI_DC_GPIO_Port, SPI_DC_Pin, GPIO_PIN_RESET); // Command
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
}
void ST7789_WriteData(uint8_t *data, uint16_t size) {
    HAL_GPIO_WritePin(SPI_DC_GPIO_Port, SPI_DC_Pin, GPIO_PIN_SET); // Data
    HAL_SPI_Transmit(&hspi1, data, size, HAL_MAX_DELAY);
}
void ST7789_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    uint8_t data[4];

    ST7789_WriteCommand(0x2A); // Column address set
    data[0] = x0 >> 8; data[1] = x0 & 0xFF;
    data[2] = x1 >> 8; data[3] = x1 & 0xFF;
    ST7789_WriteData(data, 4);

    ST7789_WriteCommand(0x2B); // Row address set
    data[0] = y0 >> 8; data[1] = y0 & 0xFF;
    data[2] = y1 >> 8; data[3] = y1 & 0xFF;
    ST7789_WriteData(data, 4);

    ST7789_WriteCommand(0x2C); // Memory write
}
void ST7789_DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
    uint8_t data[2] = { color >> 8, color & 0xFF };
    ST7789_SetAddressWindow(x, y, x, y);
    ST7789_WriteData(data, 2);
}
void ST7789_Init(void) {
    ST7789_Reset();

    ST7789_WriteCommand(0x36); // MADCTL
    uint8_t madctl = 0xA0;
    ST7789_WriteData(&madctl, 1);

    ST7789_WriteCommand(0x3A); // COLMOD
    uint8_t colmod = 0x05; // 16-bit color
    ST7789_WriteData(&colmod, 1);

    ST7789_WriteCommand(0xB2); // Porch control
    uint8_t porch[] = {0x0C, 0x0C, 0x00, 0x33, 0x33};
    ST7789_WriteData(porch, sizeof(porch));

    ST7789_WriteCommand(0xB7); // Gate control
    uint8_t gate = 0x35;
    ST7789_WriteData(&gate, 1);

    ST7789_WriteCommand(0xBB); // VCOM
    uint8_t vcom = 0x2B;
    ST7789_WriteData(&vcom, 1);

    ST7789_WriteCommand(0xC0); // LCMCTRL
    uint8_t lcm = 0x2C;
    ST7789_WriteData(&lcm, 1);

    ST7789_WriteCommand(0xC2); // VDV and VRH
    uint8_t vdv = 0x01;
    ST7789_WriteData(&vdv, 1);

    ST7789_WriteCommand(0xC3); // VRHS
    uint8_t vrhs = 0x0B;
    ST7789_WriteData(&vrhs, 1);

    ST7789_WriteCommand(0xC4); // VDV SET
    uint8_t vdv2 = 0x20;
    ST7789_WriteData(&vdv2, 1);

    ST7789_WriteCommand(0xC6); // FRCTRL2
    uint8_t fr = 0x0F;
    ST7789_WriteData(&fr, 1);

    ST7789_WriteCommand(0xD0); // PWCTRL1
    uint8_t pwctrl[] = {0xA4, 0xA1};
    ST7789_WriteData(pwctrl, 2);

    ST7789_WriteCommand(0xE0); // Positive gamma correction
    uint8_t gamma_pos[] = {0xD0,0x00,0x05,0x0E,0x15,0x0D,0x37,0x43,0x47,0x09,0x15,0x12,0x16,0x19};
    ST7789_WriteData(gamma_pos, sizeof(gamma_pos));

    ST7789_WriteCommand(0xE1); // Negative gamma correction
    uint8_t gamma_neg[] = {0xD0,0x00,0x05,0x0D,0x0C,0x06,0x2D,0x44,0x40,0x0E,0x1C,0x18,0x16,0x19};
    ST7789_WriteData(gamma_neg, sizeof(gamma_neg));

    ST7789_WriteCommand(0x21); // Inversion on

    ST7789_WriteCommand(0x11); // Sleep out
    HAL_Delay(120);
    ST7789_WriteCommand(0x29); // Display on
}
void ST7789_Fill_DMA(uint16_t color) {
    ST7789_SetAddressWindow(0, 0, ST7789_WIDTH - 1, ST7789_HEIGHT - 1);
    dma_busy = 1;
    dma_mode = DMA_MODE_FILL;

    dma_fill_color = color;
    dma_pixels_sent = 0;

    // Llenar el bloque con el color
    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;
    for (int i = 0; i < BLOCK_PIXELS * 2; i += 2) {
        dma_block[i] = hi;
        dma_block[i+1] = lo;
    }

    // Mandar la primera porción
    HAL_GPIO_WritePin(SPI_DC_GPIO_Port, SPI_DC_Pin, GPIO_PIN_SET);
    HAL_SPI_Transmit_DMA(&hspi1, dma_block, BLOCK_PIXELS * 2);
}
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
    if (hspi != &hspi1) return;

    switch (dma_mode) {
        case DMA_MODE_FILL:
            dma_pixels_sent += BLOCK_PIXELS;
            if (dma_pixels_sent < dma_total_pixels) {
                uint32_t remaining = dma_total_pixels - dma_pixels_sent;
                uint32_t next_block = (remaining > BLOCK_PIXELS) ? BLOCK_PIXELS : remaining;
                HAL_SPI_Transmit_DMA(&hspi1, dma_block, next_block * 2);
            } else {
                dma_busy = 0;
                dma_mode = DMA_MODE_NONE;
            }
            break;

        case DMA_MODE_PELOGO:
            if (dma_tile_row < PE_HEIGHT) {
                ST7789_DrawPELogo_NextBlock();  // función específica de PELogo
            } else {
                dma_busy = 0;
                dma_mode = DMA_MODE_NONE;
            }
            break;
        case DMA_MODE_TEXT:
        		dma_busy = 0;
        		dma_mode = DMA_MODE_NONE;
        	break;
        case DMA_MODE_NONE:
        	if(dma_busy) dma_busy = 0;
        default:
            dma_busy = 0;
            break;
    }
}
void ST7789_SetCursor(uint16_t x, uint16_t y) {
    cursor_x = x;
    cursor_y = y;
}
void ST7789_PrintChar(uint16_t x, uint16_t y, uint8_t font_size, char ch, uint16_t color) {
    if (ch < 32) return;  // Caracteres válidos desde el 32 en ASCII
    ch -= 32;

    uint8_t i, j;
    uint16_t pos;

    switch(font_size) {
        case 1: {
            pos = ch * font_1_WIDTH;
            for (i = 0; i < font_1_WIDTH; i++) {
                uint8_t colData = font_1[pos + i];
                for (j = 0; j < 8; j++) {
                    if (colData & (1 << j)) {
                        ST7789_DrawPixel(x + i, y + j, color);
                    }
                }
            }
            break;
        }

        case 2: {
            pos = ch * (font_2_WIDTH * 2);
            for (i = 0; i < font_2_WIDTH; i++) {
                uint8_t colData = font_2[pos + i];
                for (j = 0; j < 8; j++) {
                    if (colData & (1 << j)) {
                        ST7789_DrawPixel(x + i, y + j, color);
                    }
                }
            }
            for (i = 0; i < font_2_WIDTH; i++) {
                uint8_t colData = font_2[pos + font_2_WIDTH + i];
                for (j = 0; j < 8; j++) {
                    if (colData & (1 << j)) {
                        ST7789_DrawPixel(x + i, y + 8 + j, color);
                    }
                }
            }
            break;
        }

        case 3: {
            pos = ch * (font_3_WIDTH * 3);
            uint16_t pos2 = pos + 1;
            uint16_t pos3 = pos + 2;

            for (i = 0; i < font_3_WIDTH; i++) {
                uint8_t colData = font_3[pos];
                for (j = 0; j < 8; j++) {
                    if (colData & (1 << j)) {
                        ST7789_DrawPixel(x + i, y + j, color);
                    }
                }
                pos += 3;
            }

            for (i = 0; i < font_3_WIDTH; i++) {
                uint8_t colData = font_3[pos2];
                for (j = 0; j < 8; j++) {
                    if (colData & (1 << j)) {
                        ST7789_DrawPixel(x + i, y + 8 + j, color);
                    }
                }
                pos2 += 3;
            }

            for (i = 0; i < font_3_WIDTH; i++) {
                uint8_t colData = font_3[pos3];
                for (j = 0; j < 8; j++) {
                    if (colData & (1 << j)) {
                        ST7789_DrawPixel(x + i, y + 16 + j, color);
                    }
                }
                pos3 += 3;
            }
            break;
        }

        default:
            break;
    }
}
void ST7789_PrintChar_DMA(uint16_t x, uint16_t y, uint8_t font_size, char ch, uint16_t color, uint16_t bg) {
    if (dma_busy || ch < 32) return;
    ch -= 32;

    uint8_t i, j;
    uint16_t pos;
    uint8_t width = 0, height = 0;
    uint16_t buf_index = 0;

    switch (font_size) {
        case 1:
            pos = ch * font_1_WIDTH;
            width = font_1_WIDTH;
            height = 8;
            for (j = 0; j < 8; j++) {
                for (i = 0; i < width; i++) {
                    uint8_t col = font_1[pos + i];
                    uint16_t pixel = (col & (1 << j)) ? color : bg;
                    buffer[buf_index++] = pixel;
                }
            }
            break;

        case 2:
            pos = ch * (font_2_WIDTH * 2);
            width = font_2_WIDTH;
            height = 16;
            for (j = 0; j < 8; j++) {
                for (i = 0; i < width; i++) {
                    uint8_t col = font_2[pos + i];
                    uint16_t pixel = (col & (1 << j)) ? color : bg;
                    buffer[buf_index++] = pixel;
                }
            }
            for (j = 0; j < 8; j++) {
                for (i = 0; i < width; i++) {
                    uint8_t col = font_2[pos + width + i];
                    uint16_t pixel = (col & (1 << j)) ? color : bg;
                    buffer[buf_index++] = pixel;
                }
            }
            break;

        case 3:
            pos = ch * (font_3_WIDTH * 3);
            width = font_3_WIDTH;
            height = 24;
            for (j = 0; j < 8; j++) {
                for (i = 0; i < width; i++) {
                    uint8_t col = font_3[pos + (i * 3)];
                    uint16_t pixel = (col & (1 << j)) ? color : bg;
                    buffer[buf_index++] = pixel;
                }
            }
            for (j = 0; j < 8; j++) {
                for (i = 0; i < width; i++) {
                    uint8_t col = font_3[pos + (i * 3) + 1];
                    uint16_t pixel = (col & (1 << j)) ? color : bg;
                    buffer[buf_index++] = pixel;
                }
            }
            for (j = 0; j < 8; j++) {
                for (i = 0; i < width; i++) {
                    uint8_t col = font_3[pos + (i * 3) + 2];
                    uint16_t pixel = (col & (1 << j)) ? color : bg;
                    buffer[buf_index++] = pixel;
                }
            }
            break;

        default:
            return;
    }

    // Configurar ventana
    ST7789_SetAddressWindow(x, y, x + width - 1, y + height - 1);
    HAL_GPIO_WritePin(SPI_DC_GPIO_Port, SPI_DC_Pin, GPIO_PIN_SET);
    dma_busy = 1;
    HAL_SPI_Transmit_DMA(&hspi1, (uint8_t*)buffer, width * height * 2);
}
void ST7789_DrawBitmap(uint16_t x, uint16_t y, const uint8_t *bitmap, uint16_t w, uint16_t h, uint16_t color, uint16_t bg) {
    uint8_t bytesPerRow = (w + 7) / 8;

    for (uint16_t j = 0; j < h; j++) {
        for (uint16_t i = 0; i < w; i++) {
            uint8_t byte = bitmap[j * bytesPerRow + (i / 8)];
            uint8_t bit = (byte >> (7 - (i % 8))) & 0x01;

            ST7789_DrawPixel(x + i, y + j, bit ? color : bg);
        }
    }
}
void ST7789_DrawPELogo_DMA(void) {
    if (dma_busy) return;
    dma_busy = 1;
    dma_mode = DMA_MODE_PELOGO;
    dma_tile_row = 0;
    ST7789_DrawPELogo_NextBlock();
}

void ST7789_DrawPELogo_NextBlock(void) {
    uint8_t bytesPerRow = (PE_WIDTH + 7) / 8;
    uint16_t rows = (PE_HEIGHT - dma_tile_row > BUFFER_HEIGHT) ? BUFFER_HEIGHT : (PE_HEIGHT - dma_tile_row);
    uint32_t index = 0;

    for (uint16_t j = 0; j < rows; j++) {
        for (uint16_t i = 0; i < PE_WIDTH; i++) {
            uint8_t byte = bitmap_logo2[(dma_tile_row + j) * bytesPerRow + (i / 8)];
            uint8_t bit = (byte >> (7 - (i % 8))) & 0x01;
            buffer[index++] = bit ? 0x0000 : 0x001F;
        }
    }

    ST7789_SetAddressWindow(0, dma_tile_row + 46, PE_WIDTH - 1, dma_tile_row + 46 + rows - 1);
    HAL_GPIO_WritePin(SPI_DC_GPIO_Port, SPI_DC_Pin, GPIO_PIN_SET);
    HAL_SPI_Transmit_DMA(&hspi1, (uint8_t*)buffer, PE_WIDTH * rows * 2);
    dma_tile_row += rows;
}

void ST7789_PrintTextInBox_DMA(uint16_t x, uint16_t y, uint8_t font_size, const char *text,
                               uint8_t fila_sup, uint8_t fila_inf,
                               uint16_t fg_color, uint16_t bg_color) {
    if (dma_busy) return;
    dma_busy = 1;
    dma_mode = DMA_MODE_TEXT;

    uint8_t char_width, char_height;
    switch(font_size) {
        case 1: char_width = font_1_WIDTH; char_height = 8; break;
        case 2: char_width = font_2_WIDTH; char_height = 16; break;
        case 3: char_width = font_3_WIDTH; char_height = 24; break;
        default: dma_busy = 0; return;
    }

    size_t len = strlen(text);
    if (len > MAX_TEXT_LEN) len = MAX_TEXT_LEN;

    uint16_t total_width = len * char_width + 2;
    uint16_t total_height = char_height + fila_sup + fila_inf;

    for (uint32_t i = 0; i < total_width * total_height; i++)
        buffer[i] = bg_color;

    for (size_t c = 0; c < len; c++) {
        char ch = text[c];
        if (ch < 32) continue;
        ch -= 32;
        uint32_t base_x = 1 + c * char_width;

        switch(font_size) {
            case 1: {
                uint16_t pos = ch * font_1_WIDTH;
                for (uint8_t i = 0; i < font_1_WIDTH; i++) {
                    uint8_t col = font_1[pos + i];
                    for (uint8_t j = 0; j < 8; j++) {
                        if (col & (1 << j))
                            buffer[(fila_sup + j) * total_width + base_x + i] = fg_color;
                    }
                }
                break;
            }
            case 2: {
                uint16_t pos = ch * (font_2_WIDTH * 2);
                for (uint8_t i = 0; i < font_2_WIDTH; i++) {
                    uint8_t col1 = font_2[pos + i];
                    uint8_t col2 = font_2[pos + font_2_WIDTH + i];
                    for (uint8_t j = 0; j < 8; j++) {
                        if (col1 & (1 << j))
                            buffer[(fila_sup + j) * total_width + base_x + i] = fg_color;
                        if (col2 & (1 << j))
                            buffer[(fila_sup + 8 + j) * total_width + base_x + i] = fg_color;
                    }
                }
                break;
            }
            case 3: {
                uint16_t pos = ch * (font_3_WIDTH * 3);
                for (uint8_t i = 0; i < font_3_WIDTH; i++) {
                    uint8_t col1 = font_3[pos + i * 3 + 0];
                    uint8_t col2 = font_3[pos + i * 3 + 1];
                    uint8_t col3 = font_3[pos + i * 3 + 2];
                    for (uint8_t j = 0; j < 8; j++) {
                        if (col1 & (1 << j))
                            buffer[(fila_sup + j) * total_width + base_x + i] = fg_color;
                        if (col2 & (1 << j))
                            buffer[(fila_sup + 8 + j) * total_width + base_x + i] = fg_color;
                        if (col3 & (1 << j))
                            buffer[(fila_sup + 16 + j) * total_width + base_x + i] = fg_color;
                    }
                }
                break;
            }
        }
    }

    ST7789_SetAddressWindow(x, y, x + total_width - 1, y + total_height - 1);
    HAL_GPIO_WritePin(SPI_DC_GPIO_Port, SPI_DC_Pin, GPIO_PIN_SET);
    HAL_SPI_Transmit_DMA(&hspi1, (uint8_t*)buffer, total_width * total_height * 2);
}
