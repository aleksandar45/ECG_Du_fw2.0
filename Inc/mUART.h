#ifndef __MUART_H
#define __MUART_H

// Includes ------------------------------------------------------------------//
#include "stm32l4xx_hal.h"
#include "BLE_RN4871.h"

void mUART_Config(UART_HandleTypeDef *huart, uint32_t baudRate, uint32_t hwControl);
HAL_StatusTypeDef mUART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
static void mUART_RxISR_8BIT(UART_HandleTypeDef *huart);
	
#endif

