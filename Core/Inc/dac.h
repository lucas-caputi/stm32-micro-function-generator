/*
 * dac.h
 *
 *  Created on: Apr 23, 2023
 *      Author: lcapu
 */

#ifndef INC_DAC_H_
#define INC_DAC_H_

/* includes */
#include "stm32l4xx_hal.h"
#include <stdint.h>

/* defines */
#define DAC_DS 15 // 16
#define DAC_WC_SEL (0x1UL << 15)
#define DAC_WC_IB (0x1UL << 14)
#define DAC_WC_OG (0x1UL << 13)
#define DAC_WC_OS (0x1UL << 12)
#define V_TO_MV_RATIO 1000;
#define DAC_RATIO 0.8056 	// 3300 / 2^12
#define KEYPAD_RATIO 100
#define MAX_VLTGE_VAL 4095
#define TWELVE_BIT_MASK 0xFFF

/* function declarations */
void DAC_init(void);
void DAC_write(uint32_t dig);
uint32_t DAC_volt_conv(uint32_t voltage);

#endif /* INC_DAC_H_ */
