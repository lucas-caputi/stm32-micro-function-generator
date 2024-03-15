/*
 * tim2.h
 *
 *  Created on: Apr 12, 2023
 *      Author: lcapu
 */

#ifndef INC_TIM2_H_
#define INC_TIM2_H_

/* includes */
#include "stm32l4xx_hal.h"

/* defines */
#define TIM2_CCR1_CYCLES 200

/* function prototypes */
void TIM2_config(void);
void TIM2_IRQHandler(void);

#endif /* INC_TIM2_H_ */
