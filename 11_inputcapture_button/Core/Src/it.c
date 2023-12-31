/*
 * it.c
 *
 *  Created on: Aug 5, 2023
 *      Author: hoang
 */
#include "it.h"

extern TIM_HandleTypeDef htim2;

void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

void TIM2_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim2);
}
