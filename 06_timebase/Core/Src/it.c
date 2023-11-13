/*
 * it.c
 *
 *  Created on: Aug 5, 2023
 *      Author: hoang
 */
#include "stm32f4xx_hal.h"

void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}
