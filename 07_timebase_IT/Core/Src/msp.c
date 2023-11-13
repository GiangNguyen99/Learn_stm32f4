/*
 * msp.c
 *
 *  Created on: Aug 5, 2023
 *      Author: hoang
 */

#include "stm32f4xx_hal.h"

void HAL_MspInit(void) {
	//Here will do low level processor specific inits.
	//1. Set up the priority grouping of the arm cortex mx processor
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

	//2. Enable the required system exceptions of the arm cortex mx processor
	SCB->SHCSR |= 0x7 << 16; //usage fault, memory fault and bus fault system exceptions

	//3. configure the priority for the system exceptions
	HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
	HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
	HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
}
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htimer) {

	//1. enable the clock for the TIM6 peripheral
	__HAL_RCC_TIM2_CLK_ENABLE();

	//2. Enable the IRQ of TIM2
	HAL_NVIC_EnableIRQ(TIM2_IRQn);

	//3. setup the priority for TIM2_IRQn
	HAL_NVIC_SetPriority(TIM2_IRQn, 10, 0);
}
