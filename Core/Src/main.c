/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Function generator with keypad input.
  ******************************************************************************
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* includes */
#include "main.h"
#include "keypad.h"
#include "dac.h"
#include "tim2.h"
#include "lut.h"

/* private function prototypes */
void SystemClock_Config(void);

/* global variables */
uint32_t *waveform_data;			// set to a LUT for a given waveform
uint32_t waveform_index;			// current index in LUT
uint32_t waveform_freq;				// current frequency setting (1-5 for 100-500Hz)
uint32_t waveform_duty;				// current duty cycle (only applies to square wave)
int32_t square_wave_flag;			// flag for square wave mode

/*
* @brief  Initialization and main loop to run project indefinitely.
* @retval int
*/
int main(void) {
  /* reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* configure the system clock */
  SystemClock_Config();

  /* initialize state, waveform data, index, frequency, and duty cycle for 100Hz 50% duty square wave */
  waveform_data = SQUARE_LUT_50;	// 50% square waveform LUT
  square_wave_flag = 1;				// set square wave flag high
  waveform_index = 0;				// initial index of LUT is 0
  waveform_freq = 1;				// 100Hz
  waveform_duty = 5;				// 50%

  /* configure keypad, timer, and dac */
  config_keypad();
  TIM2_config();
  DAC_init();

  /* initialize local variables */
  int32_t key_pressed = -1;

  /* infinite loop */
  while (1) {
	  /* wait for keypad input */
	  while(key_pressed == -1) {
		  key_pressed = determine_keypress();
	  }

	  /* execute changes based off key pressed */
	  change_waveform(key_pressed);

	  /* wait for keypad to be released (with debounce) */
	  while(1) {
		  key_pressed = determine_keypress();		// check keypress value
		  if(key_pressed == -1) {					// check if key is let go
			  for(int i = 0; i < 50000; i++);		// delay for debounce
			  key_pressed = determine_keypress();	// check keypress value again
			  if(key_pressed == -1) {				// check if key is still let go
				  break;
			  }
		  }
	  }
  }

}

/*
* @brief  Changes waveform settings based off given key press.
* @retval void
*/
void change_waveform(int32_t key_pressed) {
  switch (key_pressed) {
	  case 0:	// set duty cycle to 50%
		  if (square_wave_flag) {
			  waveform_duty = 5;
			  _change_waveform_duty(waveform_duty);
		  }
		  break;
	  case 1:	// set frequency to 100Hz
		  waveform_index = 0;
		  waveform_freq = 1;
		  break;
	  case 2:	// set frequency to 200Hz
		  waveform_index = 0;
		  waveform_freq = 2;
		  break;
	  case 3:	// set frequency to 300Hz
		  waveform_index = 0;
		  waveform_freq = 3;
		  break;
	  case 4:	// set frequency to 400Hz
		  waveform_index = 0;
		  waveform_freq = 4;
		  break;
	  case 5:	// set frequency to 500Hz
		  waveform_index = 0;
		  waveform_freq = 5;
		  break;
	  case 6:	// change to sine wave
		  waveform_index = 0;
		  waveform_data = SIN_LUT;
		  square_wave_flag = 0;
		  break;
	  case 7:	// change to triangle wave
		  waveform_index = 0;
		  waveform_data = TRIANGLE_LUT;
		  square_wave_flag = 0;
		  break;
	  case 8:	// change to ramp wave
		  waveform_index = 0;
		  waveform_data = RAMP_LUT;
		  square_wave_flag = 0;
		  break;
	  case 9:	// change to square wave
		  if (!square_wave_flag) {
			  waveform_index = 0;
			  waveform_data = SQUARE_LUT_50;
			  square_wave_flag = 1;
		  }
		  break;
	  case 10:	// * input, decrease duty cycle by 10%
		  if (square_wave_flag) {
			  waveform_duty -= 1;
			  if (waveform_duty < 1) {	// duty cycle cannot be less than 10%
				  waveform_duty = 1;
			  }
			  _change_waveform_duty(waveform_duty);
		  }
		  break;
	  case 12:	// # input, increase duty cycle by 10%
		  if (square_wave_flag) {
			  waveform_duty += 1;
			  if (waveform_duty > 9) {	// duty cycle cannot be greater than 90%
				  waveform_duty = 9;
			  }
			  _change_waveform_duty(waveform_duty);
		  }
		  break;
	  default:	// catch-all, return to initial waveform
		  waveform_data = SQUARE_LUT_50;
		  waveform_index = 0;
		  waveform_freq = 1;	// 100Hz
		  waveform_duty = 5;	// 50%
		  break;
  }
}

/**
  * @brief Changes waveform LUT for square wave based on given duty cycle
  * @retval None
  */
void _change_waveform_duty(uint32_t duty_cycle) {
	switch(duty_cycle) {
		case (1):
			waveform_data = SQUARE_LUT_10;
			break;
		case (2):
			waveform_data = SQUARE_LUT_20;
			break;
		case (3):
			waveform_data = SQUARE_LUT_30;
			break;
		case (4):
			waveform_data = SQUARE_LUT_40;
			break;
		case (5):
			waveform_data = SQUARE_LUT_50;
			break;
		case (6):
			waveform_data = SQUARE_LUT_60;
			break;
		case (7):
			waveform_data = SQUARE_LUT_70;
			break;
		case (8):
			waveform_data = SQUARE_LUT_80;
			break;
		case (9):
			waveform_data = SQUARE_LUT_90;
			break;
		default:
			waveform_data = SQUARE_LUT_50;
	}

	waveform_index = 0;		// reset index
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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  //RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;	// 4 MHz
  //RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_9;	// 24 MHz
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_10;	// 32 MHz using
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
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
