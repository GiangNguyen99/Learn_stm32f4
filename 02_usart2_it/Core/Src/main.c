#include "stm32f4xx_hal.h"
#include <string.h>

#define TRUE 1
#define FALSE 0

void SystemClockConfig(void);
void UART2_Init(void);
void Error_handler(void);
uint8_t convert_Uppercase(uint8_t data);

UART_HandleTypeDef huart2;
char *pData = "Nguyen Xuan Hoang Giang - 02_uart2_it\n\r";
uint8_t rc_data;
uint32_t count = 0;
uint8_t data_buffer[100];
uint8_t reception_complete = FALSE;

int main(void) {

	HAL_Init();
	SystemClockConfig();
	UART2_Init();
	uint16_t len_of_data = strlen(pData);
	HAL_UART_Transmit(&huart2, (uint8_t*) pData, len_of_data, HAL_MAX_DELAY);

	while(reception_complete!=TRUE)
	HAL_UART_Receive_IT(&huart2, &rc_data, 1);

	return 0;
}

void SystemClockConfig(void) {

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

void Error_handler(void) {
	while (1)
		;
}

uint8_t convert_Uppercase(uint8_t data) {
	if (data >= 'a' && data <= 'z')
		data -= 32;
	return data;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if(rc_data== '\r')
	{
		reception_complete = TRUE;
		data_buffer[count++] = '\r';
		HAL_UART_Transmit(&huart2, data_buffer, count, HAL_MAX_DELAY);

	}
	else
	{
		data_buffer[count++] = rc_data;
	}
}

