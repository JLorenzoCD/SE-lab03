#ifndef LM35_F4_H
#define LM35_F4_H

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

#ifndef LM35_ADC_TO_TEMP_CONVERSION_CONST
/*
 * brief Constante utilizada para tranformar el valor del ADC
 * a una temperatura en grados Celsius. Dicho valor se obtiene
 * de realizar la siguiente cuenta:
 * 		- VREF_MV: voltaje de referencia en mV (p.e. 3.3V es 3300.0f)
 * 		- MAX_VALUE_ADC: maximo valor del ADC (p.e. si el ADC es de 12 bits entonces 4095.0f)
 *
 * 	  LM35_ADC_TO_TEMP_CONVERSION_CONST = (VREF_MV / MAX_VALUE_ADC) / 10.0f ~ 0.080586081f
 * */
#define LM35_ADC_TO_TEMP_CONVERSION_CONST 0.080586081f
#endif

/**
 * @brief Estructura de la instancia del sensor LM35.
 */
typedef struct {
	// Public
	ADC_HandleTypeDef   *hadc;        // Puntero al ADC a utilizar (ej: &hadc1)
    uint32_t            channel; 	  // Canal del ADC utilizado (ej: TIM_CHANNEL_1)
} LM35_HandleTypeDef;


/**
 * @brief Inicializa la estructura del sensor.
 *
 * @param hlm35 Puntero a la estructura del sensor.
 * @param hadc Puntero al handle del ADC (configurado en CubeMX).
 * @param canal Canal utilizado para la lectura del ADC (ej: ADC_CHANNEL_1).
 */
void LM35_Init(LM35_HandleTypeDef *hlm35, ADC_HandleTypeDef *hadc, uint32_t canal);

/**
 * @brief Inicia una lectura, bloquea el micro hasta obtenerla,
 * realiza la conversion del dato obtenido por el ADC a una
 * temperatura en °C y la retorna.
 *
 * @param hlm35 Puntero a la estructura del sensor.
 * @return La última medicion de la temperatura medida
 * en °C. -1.0f si hay error.
 */
float LM35_Read_Blocking(LM35_HandleTypeDef *hlm35);

/**
 * @brief Inicia una lectura no bloqueante.
 *
 * @param hlm35 Puntero a la estructura del sensor.
 */
void LM35_Start_Conversion(LM35_HandleTypeDef *hlm35);

/**
 * @brief Inicia una lectura no bloqueante.
 *
 * @param hlm35 Puntero a la estructura del sensor.
 * @param out_temp Puntero donde almacenar la temperatura.
 * @return false si no termino la lectura, true si termino la
 * lectura y guardo la temperatura en °C en out_temp.
 */
bool LM35_Get_Temperature(LM35_HandleTypeDef *hlm35, float *out_temp);

#endif // LM35_F4_H
