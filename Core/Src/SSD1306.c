/*
 * SSD1306.c
 *
 *  Created on: Feb 2, 2025
 *      Author: kudaz
 */

#include "SSD1306.h"
#include "stm32f1xx_hal.h"
#include <string.h>
#include "fonts.h"

extern I2C_HandleTypeDef hi2c1;
uint8_t SSD1306_Buffer[SSD1306_BUFFER_SIZE];

static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;
char buff[16];

// FONTS
/*
const uint8_t mi_bitmap[] = {
    // Array generado con tu herramienta preferida, por ejemplo:
    0xFF, 0x81, 0xBD, 0xA5, 0xA5, 0xBD, 0x81, 0xFF,
    // ... y así sucesivamente, dependiendo del tamaño (w x h)
}; */

const uint8_t bitmap_MMA2 [] = {		// 110 x 53
	0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xfc, 0xfe, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xfc, 0xfe, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xfc, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xfc, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xff, 0xff, 0xfc, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x7f, 0xff, 0xfc, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f,
	0xff, 0xfc, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xfc,
	0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xfc, 0xf8, 0x00,
	0x3e, 0x00, 0x1f, 0x07, 0xc0, 0x03, 0xe0, 0x07, 0x80, 0x1f, 0xff, 0xfc, 0xf0, 0x00, 0x3f, 0x00,
	0x3f, 0x0f, 0xc0, 0x07, 0xe0, 0x07, 0xc0, 0x1f, 0xff, 0xfc, 0xf0, 0x00, 0x3f, 0x00, 0x3f, 0x0f,
	0xe0, 0x07, 0xe0, 0x07, 0xc0, 0x1f, 0xff, 0xfc, 0xf0, 0x00, 0x3f, 0x00, 0x3f, 0x0f, 0xe0, 0x0f,
	0xe0, 0x0f, 0xc0, 0x0f, 0xff, 0xfc, 0xf0, 0x00, 0x3f, 0x80, 0x7f, 0x0f, 0xe0, 0x0f, 0xe0, 0x0f,
	0xc0, 0x0f, 0xff, 0xfc, 0xe0, 0x00, 0x3f, 0x80, 0x7f, 0x0f, 0xf0, 0x0f, 0xe0, 0x0f, 0xe0, 0x07,
	0xff, 0xfc, 0xe0, 0x00, 0x3f, 0x80, 0x7f, 0x0f, 0xf0, 0x0f, 0xe0, 0x0f, 0xe0, 0x07, 0xff, 0xfc,
	0xe0, 0x00, 0x3f, 0x80, 0xff, 0x0f, 0xf0, 0x1f, 0xe0, 0x1f, 0xe0, 0x03, 0xff, 0xfc, 0xe0, 0x00,
	0x3f, 0xc0, 0xff, 0x0f, 0xf8, 0x1f, 0xe0, 0x1f, 0xe0, 0x03, 0xff, 0xfc, 0xe0, 0x00, 0x3f, 0xc0,
	0xff, 0x0f, 0xf8, 0x1f, 0xe0, 0x1f, 0xf0, 0x01, 0xff, 0xfc, 0xe0, 0x00, 0x3f, 0xc1, 0xff, 0x0f,
	0xf8, 0x3f, 0xe0, 0x1f, 0xf0, 0x01, 0xff, 0xfc, 0xe0, 0x00, 0x3f, 0xe1, 0xff, 0x0f, 0xf8, 0x3f,
	0xe0, 0x1f, 0xf0, 0x00, 0xff, 0xfc, 0xc0, 0x00, 0x3f, 0xe1, 0xff, 0x0f, 0xfc, 0x3f, 0xe0, 0x3f,
	0xf0, 0x00, 0xff, 0xfc, 0xc0, 0x00, 0x3f, 0xe1, 0xff, 0x0f, 0xfc, 0x7f, 0xe0, 0x3f, 0xf8, 0x00,
	0xff, 0xfc, 0xc0, 0x00, 0x3f, 0xf3, 0xff, 0x0f, 0xfc, 0x7f, 0xe0, 0x3e, 0xf8, 0x00, 0x7f, 0xfc,
	0xc0, 0x00, 0x3f, 0xf3, 0xdf, 0x0f, 0xfe, 0x7b, 0xe0, 0x3e, 0xf8, 0x00, 0x7f, 0xfc, 0xc0, 0x00,
	0x3e, 0xf3, 0xdf, 0x0f, 0xfe, 0xfb, 0xe0, 0x7c, 0xf8, 0x00, 0x3f, 0xfc, 0xc0, 0x00, 0x3e, 0xf7,
	0xdf, 0x0f, 0xde, 0xf3, 0xe0, 0x7c, 0xf8, 0x00, 0x3f, 0xfc, 0xe0, 0x00, 0x3e, 0xff, 0x9f, 0x0f,
	0xdf, 0xf3, 0xe0, 0x7c, 0x7c, 0x00, 0x1f, 0xfc, 0xe0, 0x00, 0x3e, 0x7f, 0x9f, 0x0f, 0xdf, 0xf3,
	0xe0, 0x7c, 0x7c, 0x00, 0x1f, 0xfc, 0xe0, 0x00, 0x3e, 0x7f, 0x9f, 0x0f, 0xcf, 0xf3, 0xe0, 0xfc,
	0x7c, 0x00, 0x0f, 0xfc, 0xe0, 0x00, 0x3e, 0x7f, 0x9f, 0x0f, 0xcf, 0xe3, 0xe0, 0xf8, 0x7c, 0x00,
	0x0f, 0xfc, 0xe0, 0x00, 0x3e, 0x3f, 0x1f, 0x0f, 0xcf, 0xe3, 0xe0, 0xf8, 0x7e, 0x00, 0x0f, 0xfc,
	0xe0, 0x00, 0x3e, 0x3f, 0x1f, 0x0f, 0xc7, 0xe3, 0xe0, 0xf8, 0x7e, 0x00, 0x07, 0xfc, 0xe0, 0x00,
	0x3e, 0x3f, 0x1f, 0x0f, 0xc7, 0xc3, 0xe1, 0xff, 0xfe, 0x00, 0x07, 0xfc, 0xe0, 0x00, 0x3e, 0x3e,
	0x1f, 0x0f, 0xc7, 0xc3, 0xe1, 0xff, 0xfe, 0x00, 0x03, 0xfc, 0xf0, 0x00, 0x3e, 0x1e, 0x1f, 0x0f,
	0xc3, 0xc3, 0xe1, 0xff, 0xff, 0x00, 0x03, 0xfc, 0xf0, 0x00, 0x3e, 0x1e, 0x1f, 0x0f, 0xc3, 0x83,
	0xe1, 0xff, 0xff, 0x00, 0x01, 0xfc, 0xf0, 0x00, 0x3e, 0x00, 0x1f, 0x0f, 0xc0, 0x03, 0xe3, 0xff,
	0xff, 0x00, 0x01, 0xfc, 0xf0, 0x00, 0x3e, 0x00, 0x1f, 0x0f, 0xc0, 0x03, 0xe3, 0xf0, 0x1f, 0x00,
	0x00, 0xfc, 0xf0, 0x00, 0x3e, 0x00, 0x1f, 0x0f, 0xc0, 0x03, 0xe3, 0xf0, 0x1f, 0x80, 0x00, 0xfc,
	0xf8, 0x00, 0x3e, 0x00, 0x1f, 0x0f, 0xc0, 0x03, 0xe3, 0xe0, 0x1f, 0x80, 0x00, 0x7c, 0xf8, 0x00,
	0x3e, 0x00, 0x1f, 0x0f, 0xc0, 0x03, 0xe3, 0xe0, 0x1f, 0x80, 0x00, 0x7c, 0xf8, 0x00, 0x3e, 0x00,
	0x1f, 0x0f, 0xc0, 0x03, 0xe7, 0xe0, 0x1f, 0x80, 0x00, 0x7c, 0xf8, 0x00, 0x3c, 0x00, 0x0f, 0x07,
	0x80, 0x03, 0xc3, 0xc0, 0x0f, 0x80, 0x00, 0x3c, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x1c, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x1c, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c,
	0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0xfe, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0xfe, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};




// Funciones

void SSD1306_WriteCommand(uint8_t command) {
    uint8_t data[2];
    data[0] = 0x00;      // Control byte para comando
    data[1] = command;   // Comando a enviar

    if(HAL_I2C_Master_Transmit(&hi2c1, SSD1306_I2C_ADDRESS, data, 2, HAL_MAX_DELAY) != HAL_OK) {
        // Aquí podrías manejar el error (por ejemplo, parpadeando un LED o quedándote en un while)
    }
}

void SSD1306_Init(void) {
    HAL_Delay(100);  // Espera a que el display se estabilice

    SSD1306_WriteCommand(0xAE); // Display OFF

    SSD1306_WriteCommand(0xD5); // Set display clock divide ratio/oscillator frequency
    SSD1306_WriteCommand(0x80); // Valor recomendado

    SSD1306_WriteCommand(0xA8); // Set multiplex ratio
    SSD1306_WriteCommand(0x3F); // Para 64 píxeles de alto (0x3F = 63)

    SSD1306_WriteCommand(0xD3); // Set display offset
    SSD1306_WriteCommand(0x00); // Sin offset

    SSD1306_WriteCommand(0x40); // Set start line (línea de inicio en 0)

    SSD1306_WriteCommand(0x8D); // Charge Pump Setting
    SSD1306_WriteCommand(0x14); // Habilita el charge pump

    SSD1306_WriteCommand(0x20); // Memory addressing mode
    SSD1306_WriteCommand(0x00); // Horizontal addressing mode

    SSD1306_WriteCommand(0xA1); // Set segment remap (columna 0 a la derecha)
    SSD1306_WriteCommand(0xC8); // Set COM output scan direction (de abajo hacia arriba)

    SSD1306_WriteCommand(0xDA); // Set COM pins hardware configuration
    SSD1306_WriteCommand(0x12);

    SSD1306_WriteCommand(0x81); // Set contrast control
    SSD1306_WriteCommand(0xCF);

    SSD1306_WriteCommand(0xD9); // Set pre-charge period
    SSD1306_WriteCommand(0xF1);

    SSD1306_WriteCommand(0xDB); // Set VCOMH deselect level
    SSD1306_WriteCommand(0x40);

    SSD1306_WriteCommand(0xA4); // Resume to RAM content display (no forzado)
    SSD1306_WriteCommand(0xA6); // Set Normal Display (no invertido)

    SSD1306_WriteCommand(0xAF); // Display ON
}

void SSD1306_UpdateScreen(void) {
    // Se asume que el display está en modo horizontal addressing.
    for(uint8_t page = 0; page < (SSD1306_HEIGHT / 8); page++) {
        // Selecciona la página
        SSD1306_WriteCommand(0xB0 + page);
        // Establece la dirección de columna en 0 (bajo y alto)
        SSD1306_WriteCommand(0x00);
        SSD1306_WriteCommand(0x10);

        // Prepara un bloque de datos para la página:
        // - Primer byte: 0x40 (control de datos)
        // - Luego, 128 bytes de datos de la página
        uint8_t data[1 + SSD1306_WIDTH];
        data[0] = 0x40;  // Control byte indicando que los siguientes son datos

        // Copia los datos del buffer correspondientes a la página actual
        memcpy(&data[1], &SSD1306_Buffer[SSD1306_WIDTH * page], SSD1306_WIDTH);

        // Envía los datos por I2C
        if(HAL_I2C_Master_Transmit(&hi2c1, SSD1306_I2C_ADDRESS, data, sizeof(data), HAL_MAX_DELAY) != HAL_OK) {
            // Manejo de error
        }
    }
}

void SSD1306_Fill(uint8_t color) {
    // Si color es 1, llenamos con 0xFF (todos los píxeles encendidos),
    // si es 0, llenamos con 0x00 (todos apagados)
    memset(SSD1306_Buffer, (color ? 0xFF : 0x00), SSD1306_BUFFER_SIZE);
}

void SSD1306_DrawPixel(uint8_t x, uint8_t y, uint8_t color) {
    if(x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) {
        return;
    }

    // Cada byte en el buffer representa 8 filas (una página)
    uint16_t byteIndex = x + (y / 8) * SSD1306_WIDTH;
    uint8_t bitMask = 1 << (y % 8);

    if(color)
        SSD1306_Buffer[byteIndex] |= bitMask;
    else
        SSD1306_Buffer[byteIndex] &= ~bitMask;
}

void SSD1306_SetCursor(uint8_t x, uint8_t y) {
    cursor_x = x;
    cursor_y = y;
}

void SSD1306_DrawBitmap(uint8_t x, uint8_t y, const uint8_t *bitmap, uint8_t w, uint8_t h) {
    // Calcula cuántos bytes ocupa cada fila del bitmap.
    // Se asume 1 bit por píxel; se empaquetan 8 píxeles por byte.
    uint8_t bytesPerRow = (w + 7) / 8;

    for(uint8_t j = 0; j < h; j++) {
        for(uint8_t i = 0; i < w; i++) {
            // Obtén el byte correspondiente de la fila j
            uint8_t byte = bitmap[j * bytesPerRow + (i / 8)];
            // Extrae el bit correspondiente (suponiendo que el bit 7 es el primer píxel)
            uint8_t bit = (byte >> (7 - (i % 8))) & 0x01;
            SSD1306_DrawPixel(x + i, y + j, bit);
        }
    }
}

void SSD1306_DrawBitmapAtCursor(const uint8_t *bitmap, uint8_t w, uint8_t h) {
    SSD1306_DrawBitmap(cursor_x, cursor_y, bitmap, w, h);
}

void SSD1306_PrintChar(uint8_t x, uint8_t y, uint8_t font_size, char ch) {
    // Ajustamos el carácter para que el primer carácter del array corresponda a ASCII 32
    if(ch < 32)
        return;
    ch = ch - 32;

    uint8_t i, j;
    uint16_t pos;

    switch(font_size) {
        case 1: {
            // Fuente de tamaño 1: ancho = font_1_WIDTH, altura = 8
            pos = ch * font_1_WIDTH;  // Posición en el array font_1
            for(i = 0; i < font_1_WIDTH; i++) {
                uint8_t colData = font_1[pos + i];
                for(j = 0; j < 8; j++) {
                    // Si el bit j de colData está en 1, dibuja el píxel
                    if(colData & (1 << j))
                        SSD1306_DrawPixel(x + i, y + j, 1);
                    // Opcional: si deseas dibujar también el fondo en 0, podrías llamar a SSD1306_DrawPixel(x+i, y+j, 0) en el else.
                }
            }
            break;
        }
        case 2: {
            // Fuente de tamaño 2: ancho = font_2_WIDTH, altura = 16 (dos páginas de 8 píxeles)
            pos = ch * (font_2_WIDTH * 2);  // Posición en font_2
            // Primera página (parte superior de 8 píxeles)
            for(i = 0; i < font_2_WIDTH; i++) {
                uint8_t colData = font_2[pos + i];
                for(j = 0; j < 8; j++) {
                    if(colData & (1 << j))
                        SSD1306_DrawPixel(x + i, y + j, 1);
                }
            }
            // Segunda página (parte inferior de 8 píxeles)
            for(i = 0; i < font_2_WIDTH; i++) {
                uint8_t colData = font_2[pos + font_2_WIDTH + i];
                for(j = 0; j < 8; j++) {
                    if(colData & (1 << j))
                        SSD1306_DrawPixel(x + i, y + 8 + j, 1);
                }
            }
            break;
        }
        case 3: {
            // Fuente de tamaño 3: ancho = font_3_WIDTH, altura = 24 (tres páginas de 8 píxeles)
            pos = ch * (font_3_WIDTH * 3);  // Posición base en font_3
            uint16_t pos2 = pos + 1;  // Para la segunda "página"
            uint16_t pos3 = pos + 2;  // Para la tercera "página"
            // Primera página (8 píxeles superiores)
            for(i = 0; i < font_3_WIDTH; i++) {
                uint8_t colData = font_3[pos];
                for(j = 0; j < 8; j++) {
                    if(colData & (1 << j))
                        SSD1306_DrawPixel(x + i, y + j, 1);
                }
                pos += 3;  // Avanza a la siguiente columna (los datos están intercalados)
            }
            // Segunda página (píxeles medios)
            for(i = 0; i < font_3_WIDTH; i++) {
                uint8_t colData = font_3[pos2];
                for(j = 0; j < 8; j++) {
                    if(colData & (1 << j))
                        SSD1306_DrawPixel(x + i, y + 8 + j, 1);
                }
                pos2 += 3;
            }
            // Tercera página (píxeles inferiores)
            for(i = 0; i < font_3_WIDTH; i++) {
                uint8_t colData = font_3[pos3];
                for(j = 0; j < 8; j++) {
                    if(colData & (1 << j))
                        SSD1306_DrawPixel(x + i, y + 16 + j, 1);
                }
                pos3 += 3;
            }
            break;
        }
        default:
            // Si el tamaño de fuente no es reconocido, no se imprime nada.
            break;
    }
}
void SSD1306_FillRegion(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color) {
    for(uint8_t i = x; i < x + w; i++) {
        for(uint8_t j = y; j < y + h; j++) {
            SSD1306_DrawPixel(i, j, color);
        }
    }
}
void SSD1306_PrintCharInBox(uint8_t x, uint8_t y, uint8_t font_size, char ch, uint8_t fila_sup, uint8_t col_derecha) {
    uint8_t char_width, char_height;
    uint16_t pos;
    uint8_t i, j;

    // Determinamos el área a rellenar según el tamaño de fuente
    switch(font_size) {
        case 1:
            char_width = font_1_WIDTH;
            char_height = font_1_HEIGHT;  // 8 píxeles
            break;
        case 2:
            char_width = font_2_WIDTH;
            char_height = font_2_HEIGHT;  // 16 píxeles
            break;
        case 3:
            char_width = font_3_WIDTH;
            char_height = font_3_HEIGHT;  // 24 píxeles
            break;
        default:
            return; // Tamaño de fuente no soportado
    }

    // Dibuja un recuadro relleno en blanco (color 1)
    SSD1306_FillRegion(x, y, char_width, char_height, 1);

    // Ajusta el carácter para que el primer elemento del array corresponda al ASCII 32
    if(ch < 32) return;
    ch = ch - 32;

    // Según el tamaño de fuente, dibujamos el carácter en negro (color 0)
    switch(font_size) {
        case 1: {
            // Fuente de tamaño 1: cada carácter ocupa font_1_WIDTH bytes y 8 filas.
            pos = ch * font_1_WIDTH;
            for(i = 0; i < font_1_WIDTH; i++) {
                uint8_t colData = font_1[pos + i];
                for(j = 0; j < 8; j++) {
                    // Si el bit está encendido en la fuente, dibujamos el píxel en negro (0)
                    if(colData & (1 << j))
                        SSD1306_DrawPixel(x + i, y + j, 0);
                    else
                        SSD1306_DrawPixel(x + i, y + j, 1); // Deja el fondo blanco
                }
            }
            if(fila_sup==1){
            	SSD1306_FillRegion(x, y-1, char_width, 1, 1);
            }
            if(col_derecha==1){
            	SSD1306_FillRegion(x+char_width, y-1, 1, char_height+1, 1);
            }
            break;
        }
        case 2: {
            // Fuente de tamaño 2: 16 píxeles de alto (dos páginas de 8 píxeles).
            pos = ch * (font_2_WIDTH * 2);
            // Primera página (parte superior)
            for(i = 0; i < font_2_WIDTH; i++) {
                uint8_t colData = font_2[pos + i];
                for(j = 0; j < 8; j++) {
                    if(colData & (1 << j))
                        SSD1306_DrawPixel(x + i, y + j, 0);
                    else
                        SSD1306_DrawPixel(x + i, y + j, 1);
                }
            }
            // Segunda página (parte inferior)
            for(i = 0; i < font_2_WIDTH; i++) {
                uint8_t colData = font_2[pos + font_2_WIDTH + i];
                for(j = 0; j < 8; j++) {
                    if(colData & (1 << j))
                        SSD1306_DrawPixel(x + i, y + 8 + j, 0);
                    else
                        SSD1306_DrawPixel(x + i, y + 8 + j, 1);
                }
            }
            if(fila_sup==1){
                SSD1306_FillRegion(x, y-1, char_width, 1, 1);
                        }
            if(col_derecha==1){
                SSD1306_FillRegion(x+char_width, y-1, 1, char_height+1, 1);
                        }
            break;
        }
        case 3: {
            // Fuente de tamaño 3: 24 píxeles de alto (tres páginas de 8 píxeles).
            pos = ch * (font_3_WIDTH * 3);
            uint16_t pos2 = pos + 1;
            uint16_t pos3 = pos + 2;
            // Primera página (superior, 8 píxeles)
            for(i = 0; i < font_3_WIDTH; i++) {
                uint8_t colData = font_3[pos];
                for(j = 0; j < 8; j++) {
                    if(colData & (1 << j))
                        SSD1306_DrawPixel(x + i, y + j, 0);
                    else
                        SSD1306_DrawPixel(x + i, y + j, 1);
                }
                pos += 3;
            }
            // Segunda página (media)
            for(i = 0; i < font_3_WIDTH; i++) {
                uint8_t colData = font_3[pos2];
                for(j = 0; j < 8; j++) {
                    if(colData & (1 << j))
                        SSD1306_DrawPixel(x + i, y + 8 + j, 0);
                    else
                        SSD1306_DrawPixel(x + i, y + 8 + j, 1);
                }
                pos2 += 3;
            }
            // Tercera página (inferior)
            for(i = 0; i < font_3_WIDTH; i++) {
                uint8_t colData = font_3[pos3];
                for(j = 0; j < 8; j++) {
                    if(colData & (1 << j))
                        SSD1306_DrawPixel(x + i, y + 16 + j, 0);
                    else
                        SSD1306_DrawPixel(x + i, y + 16 + j, 1);
                }
                pos3 += 3;
            }
            if(fila_sup==1){
                SSD1306_FillRegion(x, y-1, char_width, 1, 1);
                        }
            if(col_derecha==1){
                SSD1306_FillRegion(x+char_width, y-1, 1, char_height+1, 1);
                        }
            break;
        }
        default:
            break;
    }
}
void SSD1306_PrintText(uint8_t x, uint8_t y, uint8_t font_size, const char *text) {
    while (*text) {
        SSD1306_PrintChar(x, y, font_size, *text);
        // Avanzamos el cursor horizontal según el tamaño de fuente
        switch (font_size) {
            case 1:
                x += font_1_WIDTH;
                break;
            case 2:
                x += font_2_WIDTH;
                break;
            case 3:
                x += font_3_WIDTH;
            default:
                // Si el tamaño no es válido, no se avanza
                break;
        }
        text++;
    }
}
void SSD1306_PrintTextInBox(uint8_t x, uint8_t y, uint8_t font_size, const char *text, uint8_t fila_sup, uint8_t col_derecha) {
    while (*text) {
        SSD1306_PrintCharInBox(x, y, font_size, *text, fila_sup, col_derecha);
        // Avanzamos el cursor horizontal según el tamaño de fuente
        switch (font_size) {
            case 1:
                x += font_1_WIDTH;
                break;
            case 2:
                x += font_2_WIDTH;
                break;
            case 3:
                x += font_3_WIDTH;
                break;
         /*   case 4:
                x += font_4_WIDTH;
                break;*/
            default:
                break;
        }
        text++;
    }
}
