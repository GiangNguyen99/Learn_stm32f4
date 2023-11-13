


#include "main.h"

void GPIO_Init(void);
void Error_handler(void);
void TIMER2_Init(void);
void SystemClock_Config_HSE(uint8_t clock_freq);

TIM_HandleTypeDef htimer2;

volatile uint32_t pulse1_value = 25000000; //to produce 500Hz
volatile uint32_t pulse2_value = 12500000; //to produce 1000HZ
volatile uint32_t pulse3_value = 6250000;  //to produce 2000Hz
volatile uint32_t pulse4_value = 3125000;  //to produce 4000Hz

volatile uint32_t ccr_content;

int main(void) {

	HAL_Init();

	SystemClock_Config_HSE(SYS_CLOCK_FREQ_50_MHZ);

	GPIO_Init();

	TIMER2_Init();

	if (HAL_TIM_OC_Start_IT(&htimer2, TIM_CHANNEL_1) != HAL_OK) {
		Error_handler();
	}

	if (HAL_TIM_OC_Start_IT(&htimer2, TIM_CHANNEL_2) != HAL_OK) {
		Error_handler();
	}

	if (HAL_TIM_OC_Start_IT(&htimer2, TIM_CHANNEL_3) != HAL_OK) {
		Error_handler();
	}

	if (HAL_TIM_OC_Start_IT(&htimer2, TIM_CHANNEL_4) != HAL_OK) {
		Error_handler();
	}
	while (1);

	return 0;
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config_HSE(uint8_t clock_freq) {
	RCC_OscInitTypeDef Osc_Init;
	RCC_ClkInitTypeDef Clock_Init;
	uint8_t flash_latency = 0;

	Osc_Init.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSE\
			| RCC_OSCILLATORTYPE_HSI;

	Osc_Init.HSEState = RCC_HSE_ON;
	Osc_Init.LSEState = RCC_LSE_ON;
	Osc_Init.HSIState = RCC_HSI_ON;
	Osc_Init.PLL.PLLState = RCC_PLL_ON;
	Osc_Init.PLL.PLLSource = RCC_PLLSOURCE_HSE;

	switch (clock_freq) {
	case SYS_CLOCK_FREQ_50_MHZ: {
		Osc_Init.PLL.PLLM = 8;
		Osc_Init.PLL.PLLN = 100;
		Osc_Init.PLL.PLLP = 2;

		//not use in this exercise
		Osc_Init.PLL.PLLQ = 2;
		//		Osc_Init.PLL.PLLR = 2;

		Clock_Init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK\
				| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		Clock_Init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
		Clock_Init.AHBCLKDivider = RCC_SYSCLK_DIV1;
		Clock_Init.APB1CLKDivider = RCC_HCLK_DIV2;
		Clock_Init.APB2CLKDivider = RCC_HCLK_DIV2;

		flash_latency = FLASH_ACR_LATENCY_1WS;
		break;
	}
	case SYS_CLOCK_FREQ_84_MHZ: {
		Osc_Init.PLL.PLLM = 8;
		Osc_Init.PLL.PLLN = 168;
		Osc_Init.PLL.PLLP = 2;
		Osc_Init.PLL.PLLQ = 2;
		//		Osc_Init.PLL.PLLR = 2;

		Clock_Init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK\
				| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		Clock_Init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
		Clock_Init.AHBCLKDivider = RCC_SYSCLK_DIV1;
		Clock_Init.APB1CLKDivider = RCC_HCLK_DIV2;
		Clock_Init.APB2CLKDivider = RCC_HCLK_DIV2;

		flash_latency = FLASH_ACR_LATENCY_2WS;

		break;
	}
	case SYS_CLOCK_FREQ_100_MHZ: {

		//Enable the clock for the power controller
		__HAL_RCC_PWR_CLK_ENABLE();

		//set regulator voltage scale as 1
		__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

		//		//turn on the over drive mode of the voltage regulator
		//		__HAL_PWR_OVERDRIVE_ENABLE();

		Osc_Init.PLL.PLLM = 8;
		Osc_Init.PLL.PLLN = 200;
		Osc_Init.PLL.PLLP = 2;
		Osc_Init.PLL.PLLQ = 2;
		//		Osc_Init.PLL.PLLR = 2;

		Clock_Init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK\
				| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		Clock_Init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
		Clock_Init.AHBCLKDivider = RCC_SYSCLK_DIV1;
		Clock_Init.APB1CLKDivider = RCC_HCLK_DIV4;
		Clock_Init.APB2CLKDivider = RCC_HCLK_DIV2;

		flash_latency = FLASH_ACR_LATENCY_3WS;
		break;

	}
	}

	if (HAL_RCC_OscConfig(&Osc_Init) != HAL_OK) {
		Error_handler();
	}

	if (HAL_RCC_ClockConfig(&Clock_Init, flash_latency) != HAL_OK) {
		Error_handler();
	}

	/*Configure the systick timer interrupt frequency (for every 1 ms) */
	uint32_t hclk_freq = HAL_RCC_GetHCLKFreq();
	HAL_SYSTICK_Config(hclk_freq / 1000);

	/**Configure the Systick
	 */
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

}

void GPIO_Init(void) {
	__HAL_RCC_GPIOD_CLK_ENABLE();
	GPIO_InitTypeDef ledgpio;
	ledgpio.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	ledgpio.Mode = GPIO_MODE_OUTPUT_PP;
	ledgpio.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOD, &ledgpio);
}

void TIMER2_Init(void) {

	TIM_OC_InitTypeDef tim2OC_init;

	htimer2.Instance = TIM2;
	htimer2.Init.Period = 0XFFFFFFFF;
	htimer2.Init.Prescaler = 1;
	if (HAL_TIM_OC_Init(&htimer2) != HAL_OK) {
		Error_handler();
	}

	tim2OC_init.OCMode = TIM_OCMODE_TOGGLE;
	tim2OC_init.OCPolarity = TIM_OCPOLARITY_HIGH;
	tim2OC_init.Pulse = pulse1_value;
	if (HAL_TIM_OC_ConfigChannel(&htimer2, &tim2OC_init, TIM_CHANNEL_1)
			!= HAL_OK) {
		Error_handler();
	}

	tim2OC_init.Pulse = pulse2_value;
	if (HAL_TIM_OC_ConfigChannel(&htimer2, &tim2OC_init, TIM_CHANNEL_2)
			!= HAL_OK) {
		Error_handler();
	}

	tim2OC_init.Pulse = pulse3_value;
	if (HAL_TIM_OC_ConfigChannel(&htimer2, &tim2OC_init, TIM_CHANNEL_3)
			!= HAL_OK) {
		Error_handler();
	}

	tim2OC_init.Pulse = pulse4_value;
	if (HAL_TIM_OC_ConfigChannel(&htimer2, &tim2OC_init, TIM_CHANNEL_4)
			!= HAL_OK) {
		Error_handler();
	}

}

void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim) {
	/* TIM2_CH1 toggling with frequency = 500 Hz */
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
		ccr_content = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
		__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, ccr_content + pulse1_value);
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
	}

	/* TIM2_CH2 toggling with frequency = 1000 Hz */
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
		ccr_content = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
		__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_2, ccr_content + pulse2_value);
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);

	}

	/* TIM2_CH3 toggling with frequency = 2000 Hz */
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3) {
		ccr_content = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
		__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_3, ccr_content + pulse3_value);
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);

	}

	/* TIM2_CH4 toggling with frequency = 4000 Hz */
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4) {
		ccr_content = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);
		__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_4, ccr_content + pulse4_value);
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);

	}
}

void Error_handler(void) {
	while (1)
		;
}

