/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32f4_discovery_accelerometer.h"
#include "HC_SR04.h"
#include "lm35.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

HCSR04_HandleTypeDef hcsr04_sensor;
LM35_HandleTypeDef lm35_sensor_1;

int16_t accel_data_xyz[3];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_ADC1_Init();
  MX_SPI1_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
	BSP_ACCELERO_Init();

	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);

	hcsr04_sensor.htim = &htim2;
	hcsr04_sensor.IC_Channel = TIM_CHANNEL_1;
	hcsr04_sensor.Trig_Pin = Trigger_Pin;
	hcsr04_sensor.Trig_Port = Trigger_GPIO_Port;

	hcsr04_init(&hcsr04_sensor);
	LM35_Init(&lm35_sensor_1, &hadc1, ADC_CHANNEL_1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	uint32_t accel_last_tick_read = 0;
	const int16_t ACCEL_X_UMBRAL = 100;
	const int16_t ACCEL_Y_UMBRAL = 100;

	uint8_t hcsr04_is_measurement = 0;
	uint32_t hcsr04_last_tick_read = 0;

	uint8_t lm35_1_is_measurement = 0;
	uint32_t lm35_1_last_tick_read = 0;
	float lm35_1_temp = 0.0f;

	while (1) {
		// Sensor ultrasonico HC-SR04
		if (!hcsr04_is_measurement
				&& (HAL_GetTick() - hcsr04_last_tick_read > 100)) {
			hcsr04_is_measurement = 1;

			// hcsr04_start_measurement(&hcsr04_sensor);
		} else if (hcsr04_is_data_ready(&hcsr04_sensor)) {
			hcsr04_is_measurement = 0;
			hcsr04_last_tick_read = HAL_GetTick();

			float distance = hcsr04_get_distance_cm(&hcsr04_sensor);

			HAL_GPIO_WritePin(LD5_GPIO_Port,
					LD3_Pin | LD4_Pin | LD5_Pin | LD6_Pin, GPIO_PIN_RESET);
			if (distance < 20.0) {
				HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_SET);
			} else if (distance < 50.0) {
				HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
			} else {
				HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
			}
		}
		// TIMEOUT DE SEGURIDAD: Si pasan más de 100ms esperando el eco y se colgó, liberamos el flag
		else if (hcsr04_is_measurement
				&& (HAL_GetTick() - hcsr04_last_tick_read > 150)) {
			hcsr04_is_measurement = 0; // Fuerza a reintentar en el siguiente ciclo
			hcsr04_last_tick_read = HAL_GetTick();

			// Limpiar channel del timer
			HAL_TIM_IC_Stop_IT(&htim2, TIM_CHANNEL_1);
		}

		// Sensor de temperatura LM35
		if (!lm35_1_is_measurement
				&& (HAL_GetTick() - lm35_1_last_tick_read > 100)) {
			lm35_1_is_measurement = 1;

			// LM35_Start_Conversion(&lm35_sensor_1);
		} else if (LM35_Get_Temperature(&lm35_sensor_1, &lm35_1_temp)) {
			lm35_1_is_measurement = 0;
			lm35_1_last_tick_read = HAL_GetTick();

			lm35_1_temp += 0.0001f;
		}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

		if (HAL_GetTick() - accel_last_tick_read > 50) {
			BSP_ACCELERO_GetXYZ(accel_data_xyz);

			int16_t x = accel_data_xyz[0];
			int16_t y = accel_data_xyz[1];
			// int16_t z = accel_data[2];

			if (x > ACCEL_X_UMBRAL) {
				HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET);
			} else if (x < -ACCEL_X_UMBRAL) {
				HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
			} else if (y > ACCEL_Y_UMBRAL) {
				HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
			} else if (y < -ACCEL_Y_UMBRAL) {
				HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_RESET);
			} else {
				HAL_GPIO_WritePin(GPIOD, LD3_Pin|LD4_Pin|LD5_Pin|LD6_Pin, GPIO_PIN_RESET);
			}
		}

	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
	// hcsr04_tim_ic_callback(&hcsr04_sensor, htim);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
