/*
 * keypad.h
 *
 *  Created on: Apr 10, 2023
 *      Author: lcapu
 */

#ifndef SRC_KEYPAD_H_
#define SRC_KEYPAD_H_

/* includes */
#include "stm32l4xx_hal.h"
#include <stdint.h>

/* function prototypes */
void config_keypad();
int32_t determine_keypress();

#endif /* SRC_KEYPAD_H_ */
