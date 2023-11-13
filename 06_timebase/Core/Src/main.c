#include "main.h"
#include <string.h>
void SystemClockConfig(void);
void Error_handler(void);
void TIMER2_Init(void);
void GPIO_Init(void);


TIM_HandleTypeDef htim2;
void delay(void);

void delay(void)
{
	for (int i = 0; i < 50000; ++i) {

	}
}

int main(void) {

	HAL_Init();
	SystemClockConfig();
	TIMER2_Init();
	GPIO_Init();


	HAL_TIM_Base_Start(&htim2);

	while (1)
	{

		while( (TIM2->SR & TIM_SR_UIF) )
		{

			HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
			TIM2->SR = 0;

		}

	}
	return 0;
}

void SystemClockConfig(void) {

}

void Error_handler(void) {
	while (1)
		;
}

void TIMER2_Init(void)
{
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 24;
	htim2.Init.Period = 64000-1;
//	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if(HAL_TIM_Base_Init(&htim2) != HAL_OK)
	{
		Error_handler();
	}
}
void GPIO_Init(void)
{
	__HAL_RCC_GPIOD_CLK_ENABLE();
	GPIO_InitTypeDef led;
	led.Pin = GPIO_PIN_12;
	led.Mode = GPIO_MODE_OUTPUT_PP;
	led.Speed = GPIO_SPEED_FAST;

	HAL_GPIO_Init(GPIOD, &led);
}


