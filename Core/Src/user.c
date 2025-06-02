/*
 * user.c
 *
 *  Created on: Feb 4, 2025
 *      Author: kudaz
 */

#include "user.h"
#include "SSD1306.h"
#include "fsm.h"
#include "tim.h"
#include "adc.h"
#include <stdio.h>

#define DEBOUNCE_TIME 50
#define DEBOUNCE_TIME_ENCODER 200
#define INITIAL_DECREMENT_INTERVAL 400
#define MIN_DECREMENT_INTERVAL 2
#define INITIAL_INCREMENT_INTERVAL 400
#define MIN_INCREMENT_INTERVAL 2

#define MAX_PWM 210

uint32_t Electrode;
uint32_t A;
uint32_t HotStart;
uint32_t ArcForce;
uint32_t AntiStick;
uint32_t VRD;
uint32_t TIG;
int8_t OVERHEAT;
int8_t OV = 0; // contador de termico, para que no actualice una y otra vez
int8_t FlagOverHeat = 0;
uint32_t U_A;
uint32_t U_HotStart;
uint32_t U_ArcForce;

uint32_t t_ov;


uint8_t button_pressed(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
    static uint32_t last_press_time = 0;
    static uint8_t last_state = GPIO_PIN_SET;  // Estado inicial suponiendo pull-up

    uint8_t current_state = HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);  // Leer botón

    if (current_state == GPIO_PIN_RESET && last_state == GPIO_PIN_SET) {  // Detecta flanco descendente
        if ((HAL_GetTick() - last_press_time) >= DEBOUNCE_TIME) {  // Antirrebote
            last_press_time = HAL_GetTick();  // Actualiza el tiempo de la pulsación
            last_state = current_state;
            return 1;  // Retorna 1 SOLO una vez cuando se presiona el botón
        }
    }

    last_state = current_state;
    return 0;  // No se detectó pulsación
}
uint8_t button_pressed_dec(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint32_t *value, uint32_t minValue) {
    static uint32_t last_decrement_time = 0;
    static uint32_t decrement_interval = INITIAL_DECREMENT_INTERVAL;
    static uint32_t last_press_time = 0;
    static uint8_t last_state = GPIO_PIN_SET;

    uint8_t current_state = HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);

    if (current_state == GPIO_PIN_RESET && last_state == GPIO_PIN_SET) {  // 🔹 Flanco descendente
        if ((HAL_GetTick() - last_press_time) >= DEBOUNCE_TIME) {  // 🔹 Antirrebote sin bloqueo
            last_press_time = HAL_GetTick();
            if (*value > minValue) {
                (*value)--;  // 🔹 Decremento único por pulsación rápida
            }
            last_decrement_time = HAL_GetTick();
            return 1;
        }
    }

    // 🔹 Si el botón sigue presionado, disminuye con aceleración progresiva
    if ((current_state == GPIO_PIN_RESET) && (HAL_GetTick() - last_decrement_time >= decrement_interval)) {
        if (*value > minValue) {
            (*value)--;
        }
        last_decrement_time = HAL_GetTick();

        if (decrement_interval > MIN_DECREMENT_INTERVAL) {
            decrement_interval -= 50;
        }
    } else if (current_state == GPIO_PIN_SET) {
        decrement_interval = INITIAL_DECREMENT_INTERVAL;
    }

    last_state = current_state;
    return 0;
}
uint8_t button_pressed_inc(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint32_t *value, uint32_t maxValue) {
    static uint32_t last_increment_time = 0;
    static uint32_t increment_interval = INITIAL_INCREMENT_INTERVAL;
    static uint32_t last_press_time = 0;
    static uint8_t last_state = GPIO_PIN_SET;

    uint8_t current_state = HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);

    if (current_state == GPIO_PIN_RESET && last_state == GPIO_PIN_SET) {  // 🔹 Flanco descendente
        if ((HAL_GetTick() - last_press_time) >= DEBOUNCE_TIME) {  // 🔹 Antirrebote sin bloqueo
            last_press_time = HAL_GetTick();
            if (*value < maxValue) {
                (*value)++;  // 🔹 Incremento único por pulsación rápida
            }
            last_increment_time = HAL_GetTick();
            return 1;
        }
    }

    // 🔹 Si el botón sigue presionado, aumenta con aceleración progresiva
    if ((current_state == GPIO_PIN_RESET) && (HAL_GetTick() - last_increment_time >= increment_interval)) {
        if (*value < maxValue) {
            (*value)++;
        }
        last_increment_time = HAL_GetTick();

        if (increment_interval > MIN_INCREMENT_INTERVAL) {
            increment_interval -= 50;
        }
    } else if (current_state == GPIO_PIN_SET) {
        increment_interval = INITIAL_INCREMENT_INTERVAL;
    }

    last_state = current_state;
    return 0;
}
void PrintElectrodeValue(uint32_t Electrode){
	switch (Electrode){
		case 0:
			Electrode = 7;
			SSD1306_PrintText(40, 20, 3, "ERR");
			SSD1306_UpdateScreen();
			break;
		case 1:
			SSD1306_FillRegion(40, 20, 60, 24, 0);
			SSD1306_PrintText(40, 20, 3, "0.0");
			SSD1306_UpdateScreen();
			A = U_A;
			HotStart = U_HotStart;
			ArcForce = U_ArcForce;
			break;
		case 2:
			SSD1306_FillRegion(40, 20, 60, 24, 0);
			SSD1306_PrintText(40, 20, 3, "1.6");
			SSD1306_UpdateScreen();
			A = 40; // deberia definir E_A como una variable aparte que no cambie la corriente 0.0
			HotStart = 25;   //asignar valores para cada diametro
			ArcForce = 20;
			break;
		case 3:
			SSD1306_FillRegion(40, 20, 60, 24, 0);
			SSD1306_PrintText(40, 20, 3, "2.0");
			SSD1306_UpdateScreen();
			A = 54;
			HotStart = 35;
			ArcForce = 30;
			break;
		case 4:
			SSD1306_FillRegion(40, 20, 60, 24, 0);
			SSD1306_PrintText(40, 20, 3, "2.5");
			SSD1306_UpdateScreen();
			A = 75;
			HotStart = 45;
			ArcForce = 40;
			break;
		case 5:
			SSD1306_FillRegion(40, 20, 60, 24, 0);
			SSD1306_PrintText(40, 20, 3, "3.2");
			SSD1306_UpdateScreen();
			A = 112;
			HotStart = 55;
			ArcForce = 50;
			break;
		case 6:
			SSD1306_FillRegion(40, 20, 60, 24, 0);
			SSD1306_PrintText(40, 20, 3, "4.0");
			SSD1306_UpdateScreen();
			A = 160;
			HotStart = 65;
			ArcForce = 50;
			break;
		case 7:
			SSD1306_FillRegion(40, 20, 60, 24, 0);
			SSD1306_PrintText(40, 20, 3, "5.0");
			SSD1306_UpdateScreen();
			A = 200;
			HotStart = 75;
			ArcForce = 50;
			break;
		case 8:
			Electrode = 1;
			break;
		default:
			Electrode = 1;
			SSD1306_FillRegion(40, 20, 60, 24, 0);
			SSD1306_PrintText(40, 20, 3, "ERR");
			SSD1306_UpdateScreen();
			break;
		}
	}

void PrintCurrent (uint8_t x, uint8_t y, uint8_t font_size, uint8_t color){
		uint8_t w = 1;
		uint8_t h = 1;
	if(font_size == 1){
		w = 6 * 3;		//definir segun
		h = 8;
	}
	else if(font_size == 2){
		w = 9 * 3;		//definir segun
		h = 12;
	}
	else if(font_size == 3){
		w = 20 * 3;		//definir segun
		h = 24;
	}
	if(color == 1){
		if(A>=10 && A<100) sprintf(buff, " %lu", A);
		else sprintf(buff, "%lu", A);
		SSD1306_PrintText(x, y, font_size, buff);
	}
	else{
		SSD1306_FillRegion(x, y, w, h, 0);
	}
//	SSD1306_UpdateScreen();
}

void CheckOverheat(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
    static uint32_t last_check_time = 0;
    static uint8_t last_state = 2;  // 🚀 Usamos un valor inválido para detectar el primer arranque

    uint8_t current_state = HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);  // Leer el estado actual

    // 🚀 Si es la primera ejecución, forzar la lectura inicial sin debounce
    if (last_state == 2) {
        last_state = current_state;
        OVERHEAT = (current_state == GPIO_PIN_RESET) ? 1 : 0;  // Si el térmico ya está en 0V, activar OVERHEAT
        if (OVERHEAT == 1) FlagOverHeat = 1;
        return;  // Salimos en la primera ejecución para evitar continuar con debounce
    }

    // 🚀 Si hubo un cambio de estado, iniciamos el debounce
    if (current_state != last_state) {
        last_check_time = HAL_GetTick();
    }

    // 🚀 Debounce: Solo actualizar si el estado se mantiene estable por 50ms
    if ((HAL_GetTick() - last_check_time) >= 50) {
        if (current_state == GPIO_PIN_RESET) {  // 🔥 Si el térmico activó (pin en 0V)
            OVERHEAT = 1;
            FlagOverHeat = 1;
        }
        else if (OVERHEAT == 1) {  // ❄ Si estaba en sobrecalentamiento y el pin vuelve a 3.3V o 5V
            OVERHEAT = 0;
            t_ov = HAL_GetTick();
        }
    }

    last_state = current_state;  // Guardar el estado actual para la próxima iteración
}
void SetVoltagePWM(float current_A) {
    static uint8_t pwm_running = 1;
    static uint8_t hotstart_active = 1;
    static uint8_t arcforce_active = 0;
    static uint32_t t_hotstart = 0;
    static uint8_t last_welding_state = 0;

    uint8_t welding_state = DetectWelding(&hadc1);

    // 🚀 Si estamos en TIG, desactivar HotStart y ArcForce
    if (TIG == 1) {
        hotstart_active = 0;
        arcforce_active = 0;
    }

    float target_A = current_A;

    // 🚀 HotStart: Aumenta corriente si está activo
    if (hotstart_active) {
        target_A += (target_A * HotStart / 100.0);
    }

    // 🚀 ArcForce: Se mantiene activo mientras welding_state == 2
    if (welding_state == 2) {
        arcforce_active = 1;
    } else {
        arcforce_active = 0; // Se desactiva si la soldadura se estabiliza o se corta
    }

    // 🚀 Si ArcForce está activo, aumentar corriente
    if (arcforce_active) {
        target_A += (target_A * ArcForce / 100.0);
    }

    // 🚀 Si se detecta la soldadura por primera vez después de haber dejado de soldar, iniciar HotStart
    if (hotstart_active && last_welding_state == 0 && (welding_state == 1 || welding_state == 2)) {
        t_hotstart = HAL_GetTick();
    }

    // 🚀 Desactivar HotStart después de 1 segundo
    if (hotstart_active && t_hotstart != 0 && (HAL_GetTick() - t_hotstart >= 1000)) {
        hotstart_active = 0;
    }

    // 🚀 Si el usuario deja de soldar completamente, preparar HotStart de nuevo
    if (welding_state == 0 && TIG == 0) {
        hotstart_active = 1;
    }

    last_welding_state = welding_state;

    // 🚀 Manejo normal del PWM y protección contra sobrecalentamiento
    if (FlagOverHeat == 1) {
        if (pwm_running) {
            HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
            pwm_running = 0;
        }
        return;
    }

    if (!pwm_running) {
        HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
        pwm_running = 1;
    }

    if (target_A < 10) target_A = 10;
    if (target_A > 220) target_A = 220;

    uint32_t duty_cycle = (target_A - 10) * MAX_PWM / (220 - 10);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, duty_cycle);
}

uint16_t Read_ADC_Value(ADC_HandleTypeDef* hadc) {
    HAL_ADC_Start(hadc);
    HAL_ADC_PollForConversion(hadc, HAL_MAX_DELAY);
    uint16_t adc_value = HAL_ADC_GetValue(hadc);
    HAL_ADC_Stop(hadc);
    return adc_value;
}

float ADC_to_Voltage(uint16_t adc_value) {
    return (adc_value * 3.3) / 4095.0;
}

uint8_t DetectWelding(ADC_HandleTypeDef* hadc) {
    uint16_t adc_value = Read_ADC_Value(hadc);
    float voltage = ADC_to_Voltage(adc_value);

    if(TIG == 1){
    	return 0;
    }
    if (voltage < 1.0) {
        return 2;  // 🚀 Electrodo a punto de pegarse (activar arcForce) REVISAR VALORES
    }
    else if (voltage < 1.9) {
        return 1;  // 🚀 Se está soldando
    }
    else {
        return 0;  // ❄ No se está soldando
    }
}
//uint32_t adc_values[4];  // 0: Arco, 1: HotStart, 2: ArcForce, 3: AntiStick
/*uint8_t DetectWelding(ADC_HandleTypeDef* hadc) {
    updateADC_Readings();

    if (TIG == 1) return 0;  // En TIG no se aplica lógica de Electrodo

    uint32_t tensionArco = filterADC(adc_values[0]);
    uint32_t presetArcForce  = filterADC_Channel(1, adc_values[1]);
    uint32_t presetHotStart  = filterADC_Channel(2, adc_values[2]);
//    uint32_t presetAntiStick = filterADC_Channel(3, adc_values[3]);

    if (tensionArco < presetArcForce) return 2;   // Arco débil → aplicar ArcForce
    else if (tensionArco < presetHotStart) return 1; // Arco inicial → aplicar HotStart
    else return 0;  // Arco normal o no hay soldadura

    // 👇 Podés agregar esto en el futuro si querés una respuesta especial para AntiStick
    // if (tensionArco < presetAntiStick) return 3;
}*/
/*uint8_t DetectWelding(ADC_HandleTypeDef* hadc) {
    updateADC_Readings();

    if (TIG == 1) return 0; // En TIG no se aplica lógica de Electrodo

    float arcforce_V = ADC_to_Voltage(filterADC_Channel(1, adc_values[1]));
    float hotstart_V = ADC_to_Voltage(filterADC_Channel(2, adc_values[2]));

    if (arcforce_V < 1.0) return 2;      // El preset cayó debajo del umbral
    else if (hotstart_V < 2.0) return 1; // Igual lógica para HotStart
    else return 0;
}*/
void DetectGenerating(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint8_t VRD, uint8_t welding) {
    static uint32_t t_lastWeld = 0;
    if(TIG == 1){
    	HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET); // En TIG la tensión de arco debe ser 25V SIEMPRE
    	return;											  // Para eso C8 = 0V
    }
    if (VRD == 0) {
        HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET); // Con VRD desactivado, la tensión de arco en vacío debe ser 60V
        return;
    }
    if (VRD == 1){
    	HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET); // con VRD activado, la tensión de arco en vacío debe ser 25V
    													  // pero cuando detectamos soldadura debe volver a la normaliidad (60V)
    }

    if (welding != 0) { // si detectamos que comienza a soldar, generamos arco = C8 en 5V
        HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET);
        t_lastWeld = HAL_GetTick(); // tiempo de la última soldadura
    }
    else if ((HAL_GetTick() - t_lastWeld) > 3000) {
        // 🚀 Si pasaron más de 3 segundos sin soldar, bajamos la PATA 8 a 0V
        HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);
    }
}

#define CERO_CERO  0b00
#define CERO_UNO   0b01
#define UNO_UNO    0b11
#define UNO_CERO   0b10

uint8_t encoder_rotated(uint32_t *value, uint32_t minValue, uint32_t maxValue) {
    static uint8_t last_state = 0;
    static uint32_t last_tick = 0;
    static uint8_t consecutive_steps = 1;

    uint8_t A = HAL_GPIO_ReadPin(ENCA_GPIO_Port, ENCA_Pin);
    uint8_t B = HAL_GPIO_ReadPin(ENCB_GPIO_Port, ENCB_Pin);
    uint8_t state = (A << 1) | B;
    uint32_t now = HAL_GetTick();

    if (state == 0b00 && last_state != 0b00 && (now - last_tick) >= DEBOUNCE_TIME_ENCODER) {
        last_tick = now;

        // Detectar dirección según estado anterior
        if (last_state == 0b01) {  // Horario
            *value += consecutive_steps;
        } else if (last_state == 0b10) {  // Antihorario
            if (*value > minValue + consecutive_steps - 1) {
                *value -= consecutive_steps;
            } else {
                *value = minValue;
            }
        }

        // Limitar el valor máximo
        if (*value > maxValue) *value = maxValue;

        // Aumentar aceleración
        if (consecutive_steps < 10) consecutive_steps++;

        last_state = state;
        return 1;
    }

    // Si pasó mucho tiempo sin detección, reiniciamos la aceleración
    if ((now - last_tick) >= 400) {
        consecutive_steps = 1;
    }

    last_state = state;
    return 0;
}
uint32_t adc_values[4];  // 0: Arco, 1: HotStart, 2: ArcForce, 3: AntiStick

/*void updateADC_Readings(void) {
    HAL_ADC_Start(&hadc1);
    for (int i = 0; i < 4; i++) {
        HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
        adc_values[i] = HAL_ADC_GetValue(&hadc1);
    }
    HAL_ADC_Stop(&hadc1);
}*/
/*void DetectWelding(void) {
    updateADC_Readings();

    uint32_t tensionArco = adc_values[0];
    uint32_t presetArcForce = adc_values[1];
    uint32_t presetHotStart = adc_values[2];
    uint32_t presetAntiStick = adc_values[3];

    if (tensionArco < presetArcForce) activarArcForce();
    if (tensionArco < presetHotStart) activarHotStart();
    if (tensionArco < presetAntiStick) activarAntiStick();
}
*/
/*uint32_t filterADC(uint32_t new_sample) {
    static uint32_t stage1 = 0, stage2 = 0;
    static uint8_t initialized = 0;
    const uint8_t alpha1 = 8;
    const uint8_t alpha2 = 6;

    if (!initialized) {
        stage1 = new_sample;
        stage2 = new_sample;
        initialized = 1;
    }

    stage1 += (new_sample - stage1) / alpha1;
    stage2 += (stage1 - stage2) / alpha2;
    return stage2;
}*/

// 🎯 Filtro simple por canal para los presets (HotStart, ArcForce, AntiStick)
/*uint32_t filterADC_Channel(uint8_t ch, uint32_t sample) {
    static uint32_t filtered[4] = {0};  // suponiendo máximo 4 canales
    static uint8_t initialized[4] = {0};
    const uint8_t alpha = 6;

    if (!initialized[ch]) {
        filtered[ch] = sample;
        initialized[ch] = 1;
    }

    filtered[ch] += (sample - filtered[ch]) / alpha;
    return filtered[ch];
}
*/
uint8_t FTest(void) {
    return HAL_GPIO_ReadPin(FTest_GPIO_Port, FTest_Pin) == GPIO_PIN_SET;
}
