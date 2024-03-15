/*
 * dac.c
 *
 *  Created on: Apr 23, 2023
 *      Author: lcapu
 */

/* includes */
#include "dac.h"

/**
  * @brief  initialize the SPI peripheral to communicate with the DAC
  * 		pin connections: PA5 -> SPI1_CLK, PA7 -> SPI1_MOSI, PB6 -> SPI1_CS
  * @retval int
  */
void DAC_init(void) {
	/* enable clock for SPI1 */
	RCC->APB2ENR 		|=  (RCC_APB2ENR_SPI1EN);

	/* configure GPIOA for PA4 (SPI1 CS), PA5 (SPI1 CLK), PA7 (SPI1 MOSI) */
	RCC->AHB2ENR 		|=  (RCC_AHB2ENR_GPIOAEN);													// enable clock for GPIOA
	GPIOA->AFR[0]   	&= ~(GPIO_AFRL_AFSEL4_Msk | GPIO_AFRL_AFSEL5_Msk | GPIO_AFRL_AFSEL7_Msk);	// clear AFR
	GPIOA->AFR[0]		|=  ( (0x5UL << GPIO_AFRL_AFSEL4_Pos) |										// set PA4, PA5, PA7 to AF5
							  (0x5UL << GPIO_AFRL_AFSEL5_Pos) |
							  (0x5UL << GPIO_AFRL_AFSEL7_Pos) );
	GPIOA->MODER 		&= ~(GPIO_MODER_MODE4 | GPIO_MODER_MODE5 | GPIO_MODER_MODE7); 				// clear mode4, mode5 and mode7
	GPIOA->MODER 		|=  (GPIO_MODER_MODE4_1 | GPIO_MODER_MODE5_1 | GPIO_MODER_MODE7_1);			// set mode5 and mode7 to alternate function mode
	GPIOA->OTYPER 		&= ~(GPIO_OTYPER_OT4 | GPIO_OTYPER_OT5 | GPIO_OTYPER_OT7);					// set OTYPE5 and OTYPE7 to push-pull
	GPIOA->OSPEEDR 		|=  (GPIO_OSPEEDR_OSPEED4 | GPIO_OSPEEDR_OSPEED5 | GPIO_OSPEEDR_OSPEED7);	// set OSPEED5 and OPSEED7 to high speed
	GPIOA->PUPDR 		&= ~(GPIO_PUPDR_PUPD4 | GPIO_PUPDR_PUPD5 | GPIO_PUPDR_PUPD7);				// set PUPD5 and PUPD7 to no pull-up/pull-down

	/* SPI1_CR1
	 * set to master configuration and clk/2 baud rate
	 * */
	SPI1->CR1 			= ( (SPI_CR1_MSTR) );

	/* SPI1_CR2
	 * set data size to 16bit, turn on pulse management and ss output
	 * */
	SPI1->CR2			= ( (DAC_DS << SPI_CR2_DS_Pos) |
							(SPI_CR2_SSOE) |
							(SPI_CR2_NSSP) );

	/* enable peripheral for SPI1 */
	SPI1->CR1			|=  (SPI_CR1_SPE);
}

/**
  * @brief  write a 12-bit value to the DAC
  * @retval int
  */
void DAC_write(uint32_t dig) {
	while(!(SPI1->SR & SPI_SR_TXE));		// wait for empty Tx

	/* write data to DAC(A), set to unbuffered with 1x gain in active mode operation */
	SPI1->DR = ( (dig & TWELVE_BIT_MASK) + 0x3000);
}

/**
  * @brief  convert a voltage value into a 12-bit value to control the DAC
  * 		input given as voltage value * 100. For example 300 = 3.0V
  * @retval int
  */
uint32_t DAC_volt_conv(uint32_t voltage) {
	uint32_t dig = voltage;

	dig *= V_TO_MV_RATIO;			// convert to milliVolts
	//dig /= KEYPAD_RATIO;			// divide by 100 to account for keypad input
	dig /= DAC_RATIO;				// divide by DAC ratio

	if (dig > MAX_VLTGE_VAL) {		// check if greater than maximum digital value
		dig = MAX_VLTGE_VAL;
	}

	return dig;
}
