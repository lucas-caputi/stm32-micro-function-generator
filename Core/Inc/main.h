/*
 * main.h
 *
 *  Created on: May 3, 2023
 *      Author: lcapu
 */

/* define to prevent recursive inclusion */
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* includes */
#include "stm32l4xx_hal.h"

/* function prototypes */
void change_waveform(int32_t key_pressed);
void _change_waveform_duty(uint32_t duty_cycle);

/* exported functions prototypes */
void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
