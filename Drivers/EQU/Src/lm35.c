#include "lm35.h"

void LM35_Init(LM35_HandleTypeDef *hlm35, ADC_HandleTypeDef *hadc, uint32_t canal)
{
	hlm35->hadc = hadc;
	hlm35->channel = canal;
}

float LM35_Read_Blocking(LM35_HandleTypeDef *hlm35)
{
	float temperature = -1.0f; // default value = error

	// Configurar el canal específico para esta lectura
	ADC_ChannelConfTypeDef sConfig = {0};
	sConfig.Channel = hlm35->channel;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;

	if (HAL_ADC_ConfigChannel(hlm35->hadc, &sConfig) != HAL_OK) {
		return temperature; // Error de configuración
	}

	// Inicio lectura
	HAL_ADC_Start(hlm35->hadc);

	// Bloqueo hasta obtener la lectura y realizamos la conversion
	if (HAL_ADC_PollForConversion(hlm35->hadc, 100) == HAL_OK) {
		uint32_t adcval = HAL_ADC_GetValue(hlm35->hadc);
		temperature = ((float)adcval) * LM35_ADC_TO_TEMP_CONVERSION_CONST;
	}

	HAL_ADC_Stop(hlm35->hadc);

	return temperature;
}

void LM35_Start_Conversion(LM35_HandleTypeDef *hlm35)
{
	// Configurar el canal específico para esta lectura
	ADC_ChannelConfTypeDef sConfig = {0};
	sConfig.Channel = hlm35->channel;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;

	if (HAL_ADC_ConfigChannel(hlm35->hadc, &sConfig) != HAL_OK) {
		return; // Error de configuración
	}

	// Inicio lectura
	HAL_ADC_Start(hlm35->hadc);
}

bool LM35_Get_Temperature(LM35_HandleTypeDef *hlm35, float *out_temp)
{
	// Si el flag de "Fin de Conversión" (EOC) no está activo, salimos
	// indicando que aún no está listo.
	if (!(__HAL_ADC_GET_FLAG(hlm35->hadc, ADC_FLAG_EOC))){
		return false;
	}

	// Obtenemos el dato crudo y realizamos la conversion
	uint32_t adcval = HAL_ADC_GetValue(hlm35->hadc);
	*out_temp = ((float)adcval) * LM35_ADC_TO_TEMP_CONVERSION_CONST;

	HAL_ADC_Stop(hlm35->hadc);

	return true;
}

