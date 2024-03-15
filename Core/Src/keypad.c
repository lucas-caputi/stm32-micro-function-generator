/**
  ******************************************************************************
  * @file           : keypad.c
  * @brief          : library for keypad
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  *	keypad Ports:
  * 	column 0 -> PC4
  *		column 1 -> PC5
  *		column 2 -> PC6
  *		row 0 -> PC7
  *		row 1 -> PC8
  *		row 2 -> PC9
  *		row 3 -> PC10
  *
  *	each keypad input will be represented in binary on LED output except:
  *		* -> STAR
  *		# -> HASHTAG
  */

/* includes */
#include "keypad.h"

/* defines */
#define STAR 10
#define HASHTAG 12
#define COL_0_PIN 4
#define COL_1_PIN 5
#define COL_2_PIN 6
#define COL_0_ODR GPIO_ODR_OD4
#define COL_1_ODR GPIO_ODR_OD5
#define COL_2_ODR GPIO_ODR_OD6
#define COL_MASK (COL_0_ODR | COL_1_ODR | COL_2_ODR)
#define ROW_0_PIN 7
#define ROW_1_PIN 8
#define ROW_2_PIN 9
#define ROW_3_PIN 10
#define ROW_0_IDR GPIO_IDR_ID7
#define ROW_1_IDR GPIO_IDR_ID8
#define ROW_2_IDR GPIO_IDR_ID9
#define ROW_3_IDR GPIO_IDR_ID10
#define ROW_MASK (ROW_0_IDR | ROW_1_IDR | ROW_2_IDR | ROW_3_IDR)


/**
  * @brief Keypad Configuration
  * @retval None
  */
void config_keypad()
{
	/* enable clock for GPIOC */
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

	/* configure columns (PC4 through PC6)
	 * all pins output mode, no PUPD, low speed, push-pull otype, set ODR
	 */
	GPIOC->MODER &= ~(		GPIO_MODER_MODE4_Msk |
							GPIO_MODER_MODE5_Msk |
							GPIO_MODER_MODE6_Msk
							);
	GPIOC->MODER |= (		GPIO_MODER_MODE4_0 |
							GPIO_MODER_MODE5_0 |
							GPIO_MODER_MODE6_0
							);
	GPIOC->PUPDR &= ~(		GPIO_PUPDR_PUPD4_Msk |
							GPIO_PUPDR_PUPD5_Msk |
							GPIO_PUPDR_PUPD6_Msk
							);
	GPIOC->OSPEEDR &= ~(	GPIO_OSPEEDR_OSPEED4_Msk |
							GPIO_OSPEEDR_OSPEED5_Msk |
							GPIO_OSPEEDR_OSPEED6_Msk
							);
	GPIOC->OTYPER &= ~(		GPIO_OTYPER_OT4 |
							GPIO_OTYPER_OT5 |
							GPIO_OTYPER_OT6
							);
	GPIOC->ODR |= (			GPIO_ODR_OD4 |
							GPIO_ODR_OD5 |
							GPIO_ODR_OD6
							);

	/* configure rows (PC7 - PC10)
	 * all pins input mode, pull down
	 */
	GPIOC->MODER &= ~(		GPIO_MODER_MODE7_Msk |
							GPIO_MODER_MODE8_Msk |
							GPIO_MODER_MODE9_Msk |
							GPIO_MODER_MODE10_Msk
							);
	GPIOC->PUPDR &= ~(		GPIO_PUPDR_PUPD7_Msk |
							GPIO_PUPDR_PUPD8_Msk |
							GPIO_PUPDR_PUPD9_Msk |
							GPIO_PUPDR_PUPD10_Msk
							);
	GPIOC->PUPDR |= (		GPIO_PUPDR_PUPD7_1 |
							GPIO_PUPDR_PUPD8_1 |
							GPIO_PUPDR_PUPD9_1 |
							GPIO_PUPDR_PUPD10_1
							);
}

/**
  * @brief Determines if a key has been pressed on keypad.
  * @retval 32-bit integer value of key pressed. -1 on no button press.
  * 		Exception is "*" which returns 14 and "#" which returns 15.
  */
int32_t determine_keypress()
{
	int32_t col;
	int32_t row;
	int32_t keypress;
	int32_t row_input = (GPIOC->IDR & ROW_MASK);

	if(row_input == 0)									// if no rows are high
		return -1;										// no key press has occurred, return -1

	for(col = COL_0_PIN; col <= COL_2_PIN; col++)		// cycle through columns 0-2
	{
		GPIOC->ODR &= ~(COL_MASK);						// turn off all columns
		GPIOC->ODR |= (1 << col);						// turn on one column
		row_input = (GPIOC->IDR & ROW_MASK);			// read rows
		if (row_input != 0)								// if row is still high
		{
			col = col - COL_0_PIN + 1;					// get column that is pressed (1-3)
			row = row_input >> (ROW_0_PIN + 1);			// determine row that is pressed (0-3)
			if (row == 4)								// check if in 3rd row
				row = 3;
			keypress = 3 * row + col;					// determine value of key pressed (1 - 12)
			if (keypress == 11)							// check if 0 was pressed
				keypress = 0;
			GPIOC->ODR |= COL_MASK;						// turn on all columns
			return keypress;							// return key press
		}
	}

	GPIOC->ODR |= COL_MASK;								// turn on all columns
	return -1;											// glitch occurred, return -1
}
