/*
 * tim2.c
 *
 *  Created on: Apr 16, 2023
 *      Author: lcapu
 */

/* includes */
#include "tim2.h"
#include "dac.h"
#include "definitions.h"

/* declare global vars */
extern uint32_t *waveform_data;				// set to one of four LUTs
extern uint32_t waveform_index;			    // current index in LUT
extern uint32_t waveform_freq;				// current frequency setting (1-5 for 100-500Hz)
extern uint32_t waveform_duty;				// current duty cycle (only applies to square wave)

/**
  * @brief configures TIM2
  * @retval None
  */
void TIM2_config(void) {
	RCC->APB1ENR1 	|= (RCC_APB1ENR1_TIM2EN);	// enable clock for TIM2
	TIM2->DIER 		|= (TIM_DIER_UIE);			// enable update interrupt
	TIM2->DIER		|= (TIM_DIER_CC1IE);		// enable capture compare 1 interrupt
	TIM2->SR 		&= ~(TIM_SR_UIF);			// clear update interrupt flag
	TIM2->SR 		&= ~(TIM_SR_CC1IF);			// clear capture compare 1 interrupt flag
	TIM2->ARR		 = 0xFFFFFFFF;				// 4 MHz clock, counting to max number
	TIM2->CCR1  	 = TIM2_CCR1_CYCLES - 1;	// set capture compare 1
	TIM2->CR1 		|= (TIM_CR1_CEN);			// enable TIM2

	// enable interrupts in NVIC
	NVIC->ISER[0] = (1 << (TIM2_IRQn & 0x1F));

	// enable interrupts globally
	__enable_irq();
}

/**
  * @brief Interrupt service routine for TIM2
  * @retval None
  */
void TIM2_IRQHandler(void) {
	if (TIM2->SR & TIM_SR_CC1IF) {					// if capture compare 1 interrupt flag is set
		TIM2->CCR1 += TIM2_CCR1_CYCLES;				// increment CCR1
		TIM2->SR &= ~(TIM_SR_CC1IF);				// clear flag
		DAC_write(waveform_data[waveform_index]);	// write to DAC
		waveform_index += waveform_freq;			// increment waveform index
		if (waveform_index == LUT_SIZE) {			// check for waveform index overflow
			waveform_index = 0;
		}
	}
}
