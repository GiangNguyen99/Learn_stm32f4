//TẠO CLOCK LSE 37.768 kHz tại chân PA8
//CAPTURE INPUT SIGNAL TẠI PA0
//GỬI SIGNAL QUA UART2

#include "main.h"

#include <string.h>
#include<stdio.h>
#define TRUE 1;
#define FALSE 0;

void SystemClock_Config(uint8_t clock_freq);
void Error_handler(void);
void TIMER2_Init(void);
void GPIO_Init(void);
void LSE_Configuration(void);
void UART2_Init(void);

TIM_HandleTypeDef htim2;
UART_HandleTypeDef huart2;

uint32_t input_captures[2] = { 0 };
uint8_t count = 1;
volatile uint8_t is_capture_done = FALSE
;

int main(void) {
	uint32_t capture_difference = 0;
	double timer2_cnt_freq = 0;
	double timer2_cnt_res = 0;
	double user_signal_time_period = 0;
	double user_signal_freq = 0;
	char usr_msg[100];
	HAL_Init();
	SystemClock_Config(SYS_CLOCK_FREQ_50_MHZ);
	TIMER2_Init();
	GPIO_Init();
	UART2_Init();

	LSE_Configuration();
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);

	while (1) {
		if (is_capture_done) {
			if (input_captures[1] > input_captures[0])
				capture_difference = input_captures[1] - input_captures[0];
			else
				capture_difference = (0XFFFFFFFF - input_captures[0])
						+ input_captures[1];

			timer2_cnt_freq = (HAL_RCC_GetPCLK1Freq() * 2)
					/ (htim2.Init.Prescaler + 1);
			timer2_cnt_res = 1 / timer2_cnt_freq;
			user_signal_time_period = capture_difference * timer2_cnt_res;
			user_signal_freq = 1 / user_signal_time_period;

			sprintf(usr_msg, "Frequency of the signal applied = %.2f Hz\r\n",
					user_signal_freq); // @suppress("Float formatting support")
			HAL_UART_Transmit(&huart2, (uint8_t*) usr_msg, strlen(usr_msg),
			HAL_MAX_DELAY);

			is_capture_done = FALSE
			;
			HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
		}

	}

	return 0;
}

void SystemClock_Config(uint8_t clock_freq) {
	RCC_OscInitTypeDef osc_init;
	RCC_ClkInitTypeDef clk_init;

	uint32_t FLatency = 0;

//	osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSE
//			| RCC_OSCILLATORTYPE_HSI;
	osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_HSI;
	osc_init.HSEState = RCC_HSE_ON;
//	osc_init.LSEState = RCC_LSE_ON;
	osc_init.HSIState = RCC_HSI_ON;
	osc_init.PLL.PLLState = RCC_PLL_ON;
	osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	switch (clock_freq) {
	case SYS_CLOCK_FREQ_50_MHZ: {
		osc_init.PLL.PLLM = 8;
		osc_init.PLL.PLLN = 100;
		osc_init.PLL.PLLP = 2;

		//not use in this exercise
		osc_init.PLL.PLLQ = 2;
//		osc_init.PLL.PLLR = 2;

		clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
		RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
		clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
		clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
		clk_init.APB2CLKDivider = RCC_HCLK_DIV2;

		FLatency = FLASH_ACR_LATENCY_1WS;
		break;
	}
	case SYS_CLOCK_FREQ_84_MHZ: {
		osc_init.PLL.PLLM = 8;
		osc_init.PLL.PLLN = 168;
		osc_init.PLL.PLLP = 2;
		osc_init.PLL.PLLQ = 2;
//		osc_init.PLL.PLLR = 2;

		clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
		RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
		clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
		clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
		clk_init.APB2CLKDivider = RCC_HCLK_DIV2;

		FLatency = FLASH_ACR_LATENCY_2WS;

		break;
	}
	case SYS_CLOCK_FREQ_100_MHZ: {

		//Enable the clock for the power controller
		__HAL_RCC_PWR_CLK_ENABLE();

		//set regulator voltage scale as 1
		__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

//		//turn on the over drive mode of the voltage regulator
//		__HAL_PWR_OVERDRIVE_ENABLE();

		osc_init.PLL.PLLM = 8;
		osc_init.PLL.PLLN = 200;
		osc_init.PLL.PLLP = 2;
		osc_init.PLL.PLLQ = 2;
//		osc_init.PLL.PLLR = 2;

		clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
		RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
		clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
		clk_init.APB1CLKDivider = RCC_HCLK_DIV4;
		clk_init.APB2CLKDivider = RCC_HCLK_DIV2;

		FLatency = FLASH_ACR_LATENCY_3WS;
		break;

	}

	default:
		return;
	}

	if (HAL_RCC_OscConfig(&osc_init) != HAL_OK) {
		Error_handler();
	}

	if (HAL_RCC_ClockConfig(&clk_init, FLatency) != HAL_OK) {
		Error_handler();
	}

	//Systick configuration
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
}

void LSE_Configuration(void) {

#if 0
	RCC_OscInitTypeDef osc_init;
	osc_init.OscillatorType = RCC_OSCILLATORTYPE_LSE;
	osc_init.LSEState = RCC_LSE_ON;
	if (HAL_RCC_OscConfig(&osc_init) != HAL_OK)
	{
		Error_handler();
	}
#endif
	//     @arg RCC_MCO1: Clock source to output on MCO1 pin(PA8).

	HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSI, RCC_MCODIV_4);
//RCC_MCO1SOURCE_LSE
	//RCC_MCO1SOURCE_HSI
}

void Error_handler(void) {
	while (1)
		;
}

void TIMER2_Init(void) {
	TIM_IC_InitTypeDef tim2_ic;
	htim2.Instance = TIM2;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 0xFFFFFFFF;
	htim2.Init.Prescaler = 1;

	if (HAL_TIM_IC_Init(&htim2) != HAL_OK) {
		Error_handler();
	}

	tim2_ic.ICPolarity = TIM_ICPOLARITY_RISING;
	tim2_ic.ICFilter = 0;
	tim2_ic.ICSelection = TIM_ICSELECTION_DIRECTTI;
	tim2_ic.ICPrescaler = TIM_ICPSC_DIV1;
	if (HAL_TIM_IC_ConfigChannel(&htim2, &tim2_ic, TIM_CHANNEL_1) != HAL_OK) {
		Error_handler();

	}
}
void GPIO_Init(void) {
	__HAL_RCC_GPIOD_CLK_ENABLE();
	GPIO_InitTypeDef led;
	led.Pin = GPIO_PIN_12;
	led.Mode = GPIO_MODE_OUTPUT_PP;
	led.Speed = GPIO_SPEED_FAST;

	HAL_GPIO_Init(GPIOD, &led);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
#if 1
	if (!is_capture_done) {
		if (count == 1) {
			input_captures[0] = __HAL_TIM_GET_COMPARE(htim, TIM_CHANNEL_1);
			count++;
		} else if (count == 2) {
			input_captures[1] = __HAL_TIM_GET_COMPARE(htim, TIM_CHANNEL_1);
			count = 1;
			is_capture_done = TRUE
			;
		}

	}
#endif

}

void UART2_Init(void) {
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	if (HAL_UART_Init(&huart2) != HAL_OK) {
		//There is a problem
		Error_handler();
	}

}
