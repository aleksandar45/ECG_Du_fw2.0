/**
  ******************************************************************************
  * @file    Templates/Src/stm32l4xx_hal_msp.c
  * @author  MCD Application Team
  * @brief   HAL MSP module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "main.h"

/** @addtogroup STM32L4xx_HAL_Driver
  * @{
  */

/** @defgroup HAL_MSP
  * @brief HAL MSP module.
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static DMA_HandleTypeDef hdmaSPI_rx;
static DMA_HandleTypeDef hdmaSPI_tx;
static DMA_HandleTypeDef hdmaUART_tx;
static DMA_HandleTypeDef hdmaUART_rx;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void HAL_MspInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	/*// BOOT0 PIN
	__HAL_RCC_GPIOH_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);	
	HAL_NVIC_SetPriority(EXTI3_IRQn, 0xE, 0);
	HAL_NVIC_EnableIRQ(EXTI3_IRQn);*/
	
	// BT_UART_TX_IND PIN
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);	
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0xE, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	
	
#ifdef RN4871_Nucleo_Test_Board
	__HAL_RCC_GPIOC_CLK_ENABLE();
	// Testbench signal 1
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_RESET);
	
	// Testbench signal 2
	GPIO_InitStruct.Pin = GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET);
	
	//BT_UART_RX_IND
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
#endif
	

	// enable ERROR LED
	LED_ERROR_CLK_ENABLE();
	GPIO_InitStruct.Pin = LED_ERROR_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(LED_ERROR_PORT, &GPIO_InitStruct);

#ifdef RN4871_Nucleo_Test_Board
	HAL_GPIO_WritePin(LED_ERROR_PORT,LED_ERROR_PIN,GPIO_PIN_RESET);
#else
	HAL_GPIO_WritePin(LED_ERROR_PORT,LED_ERROR_PIN,GPIO_PIN_SET);
#endif
		
		
	// enable STATUS1 LED
	LED_STATUS1_CLK_ENABLE();
	GPIO_InitStruct.Pin = LED_STATUS1_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(LED_STATUS1_PORT, &GPIO_InitStruct);
	HAL_GPIO_WritePin(LED_STATUS1_PORT,LED_STATUS1_PIN,GPIO_PIN_RESET);
	
	// enable STATUS2 LED
	LED_STATUS2_CLK_ENABLE();
	GPIO_InitStruct.Pin = LED_STATUS2_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(LED_STATUS2_PORT, &GPIO_InitStruct);

#ifdef RN4871_Nucleo_Test_Board
	HAL_GPIO_WritePin(LED_STATUS2_PORT,LED_STATUS2_PIN,GPIO_PIN_RESET);
#else
	HAL_GPIO_WritePin(LED_STATUS2_PORT,LED_STATUS2_PIN,GPIO_PIN_SET);
#endif
	
	
	// #BT_RESET
	BT_RESET_CLK_ENABLE();
	GPIO_InitStruct.Pin = BT_RESET_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_WritePin(BT_RESET_PORT,BT_RESET_PIN,GPIO_PIN_SET);
  HAL_GPIO_Init(BT_RESET_PORT, &GPIO_InitStruct);
	HAL_GPIO_WritePin(BT_RESET_PORT,BT_RESET_PIN,GPIO_PIN_SET);
	
	// BT_CFG1 (as STATUS1)
	BT_CFG1_CLK_ENABLE();
	GPIO_InitStruct.Pin = BT_CFG1_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(BT_CFG1_PORT, &GPIO_InitStruct);
	
	// BT_CFG2 (as STATUS2)
	BT_CFG2_CLK_ENABLE();
	GPIO_InitStruct.Pin = BT_CFG2_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(BT_CFG2_PORT, &GPIO_InitStruct);
	
	// BT_CFG3 (as UART Mode Switch)
	BT_CFG3_CLK_ENABLE();
	GPIO_InitStruct.Pin = BT_CFG3_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	//GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(BT_CFG3_PORT, &GPIO_InitStruct);
	//HAL_GPIO_WritePin(BT_CFG3_PORT,BT_CFG3_PIN, GPIO_PIN_RESET);
	
	// WKUP pin
	__HAL_RCC_GPIOC_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);		

#ifdef ECG_Du_v2_Board

	// EN_REG pin
	GPIO_InitStruct.Pin = GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15, GPIO_PIN_SET);
	
		//BT_UART_RX_IND
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_RESET);

	
	//#BT_TEST_MODE
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET);
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET);
	
	
	// ECG_CS pin
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);	
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4, GPIO_PIN_SET);
	
	// ECG_DRDY pin
	GPIO_InitStruct.Pin = GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);	
	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 2);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
	
	// ECG_RESET pin
	GPIO_InitStruct.Pin = GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);	
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9, GPIO_PIN_SET);
	
	// ECG_PD pin
	GPIO_InitStruct.Pin = GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);	
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_10, GPIO_PIN_SET);
	
	// ACC_INT pin
	GPIO_InitStruct.Pin = GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);	
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 3, 4);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	
	// M_BAT_EN pin
		__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1, GPIO_PIN_SET);
	
	// CONN_PWR_1 pin
	GPIO_InitStruct.Pin = GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9, GPIO_PIN_SET);
	
	
	
#endif
}
void HAL_MspDeInit(void)
{
	LED_ERROR_CLK_DISABLE();
	LED_STATUS1_CLK_DISABLE();
	LED_STATUS2_CLK_DISABLE();
}
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
  // TIM3 Peripheral clock enable 
   __HAL_RCC_TIM3_CLK_ENABLE();
  
  // Set the TIM3 priority
  HAL_NVIC_SetPriority(TIM3_IRQn, 3, 3);

  // Enable the TIMx global Interrupt 
  HAL_NVIC_EnableIRQ(TIM3_IRQn);
}
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim)
{
  // TIM3 Peripheral clock disable 
   __HAL_RCC_TIM3_CLK_DISABLE();
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{  
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  // Enable GPIO TX/RX clock 
  __HAL_RCC_GPIOA_CLK_ENABLE();

  // Enable USART2 clock 
  __HAL_RCC_USART2_CLK_ENABLE();
	
	// Enable DMA1 clock
	__HAL_RCC_DMA1_CLK_ENABLE();
   
  //BT_UART_TX pin configuration  
  GPIO_InitStruct.Pin       = GPIO_PIN_3;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  //BT_UART_RX pin configuration
  GPIO_InitStruct.Pin       = GPIO_PIN_2;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//BT_UART_RTS pin configuration  
  GPIO_InitStruct.Pin       = GPIO_PIN_0;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


  //BT_UART_CTS pin configuration
  GPIO_InitStruct.Pin       = GPIO_PIN_1;
  GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);

  // Configure the DMA handler for Transmission process 
  hdmaUART_tx.Instance                 = DMA1_Channel7;
  hdmaUART_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  hdmaUART_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdmaUART_tx.Init.MemInc              = DMA_MINC_ENABLE;
  hdmaUART_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdmaUART_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  hdmaUART_tx.Init.Mode                = DMA_NORMAL;
  hdmaUART_tx.Init.Priority            = DMA_PRIORITY_LOW;
  hdmaUART_tx.Init.Request             = DMA_REQUEST_2;

  HAL_DMA_Init(&hdmaUART_tx);

  // Associate the initialized DMA handle to the UART handle 
  __HAL_LINKDMA(huart, hdmatx, hdmaUART_tx);
	
/*	 // Configure the DMA handler for Reception process 
  hdmaUART_rx.Instance                 = DMA1_Channel6;
  hdmaUART_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  hdmaUART_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdmaUART_rx.Init.MemInc              = DMA_MINC_ENABLE;
  hdmaUART_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdmaUART_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  hdmaUART_rx.Init.Mode                = DMA_NORMAL;
  hdmaUART_rx.Init.Priority            = DMA_PRIORITY_HIGH;
  hdmaUART_rx.Init.Request             = DMA_REQUEST_2;

  HAL_DMA_Init(&hdmaUART_rx);

  // Associate the initialized DMA handle to the UART handle 
  __HAL_LINKDMA(huart, hdmarx, hdmaUART_rx);*/
    
  // NVIC configuration for DMA transfer complete interrupt (UART2_TX) 
  HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 0, 2);
  HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);
	
/*	// NVIC configuration for DMA transfer complete interrupt (UART2_RX) 
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);*/
      
  // NVIC for USART, to catch the RX complete 
  HAL_NVIC_SetPriority(USART2_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(USART2_IRQn);
}

void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
  //Reset peripherals 
  __HAL_RCC_USART2_FORCE_RESET();
  __HAL_RCC_USART2_RELEASE_RESET();
	
/*
  // Disable peripherals and GPIO Clocks 
  // Configure BT_UART_TX as alternate function
  HAL_GPIO_DeInit(GPIOA, GPIO_PIN_3);
  // Configure BT_UART_RX as alternate function 
  HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2);
	// Configure BT_UART_RTS as alternate function
  HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0);
  // Configure BT_UART_CTS as alternate function 
  HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1);
	*/
	
	// De-Initialize the DMA associated to transmission process 
 /* HAL_DMA_DeInit(&hdmaUART_tx);*/
	
/*	// De-Initialize the DMA associated to reception process 
  HAL_DMA_DeInit(&hdmaUART_rx);*/
}

void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
  GPIO_InitTypeDef          GPIO_InitStruct;

  //Enable BATT_MEAS pin clock
  __HAL_RCC_GPIOB_CLK_ENABLE();
  
  // Enable ADC clock 
  __HAL_RCC_ADC_CLK_ENABLE();
  

  //BATT_MEAS pin configuration
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  

  // NVIC configuration for ADC interrupt  
  HAL_NVIC_SetPriority(ADC1_IRQn, 5, 5);
  HAL_NVIC_EnableIRQ(ADC1_IRQn);
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc)
{
  //Reset peripherals
  __HAL_RCC_ADC_FORCE_RESET();
  __HAL_RCC_ADC_RELEASE_RESET();

  // Disable peripherals and GPIO Clocks 
  // De-initialize the ADC Channel GPIO pin //
  HAL_GPIO_DeInit(GPIOB, GPIO_PIN_0);

  // Disable the NVIC configuration for ADC interrupt
  HAL_NVIC_DisableIRQ(ADC1_IRQn);
}

void HAL_SPI_MspInit(SPI_HandleTypeDef *spi){
	GPIO_InitTypeDef  GPIO_InitStruct;


	//SPI1 clock enable
	__HAL_RCC_SPI1_CLK_ENABLE();
	
	//SPI1 GPIO port clock enable	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	
	// Enable DMA1 clock
	__HAL_RCC_DMA1_CLK_ENABLE();
	
	//ECG_SCK GPIO pin configuration  
	GPIO_InitStruct.Pin       = GPIO_PIN_5;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	 
	//ECG_MISO GPIO pin configuration  
	GPIO_InitStruct.Pin 			= GPIO_PIN_6;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	
	// ECG_MOSI GPIO pin configuration  
	GPIO_InitStruct.Pin = GPIO_PIN_7;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 	
	
	
	// Configure the DMA handler for Reception process
	hdmaSPI_rx.Instance                 = DMA1_Channel2;
	hdmaSPI_rx.Init.Request             = DMA_REQUEST_1;
	hdmaSPI_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
	hdmaSPI_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
	hdmaSPI_rx.Init.MemInc              = DMA_MINC_ENABLE;
	hdmaSPI_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	hdmaSPI_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
	hdmaSPI_rx.Init.Mode                = DMA_NORMAL;
	hdmaSPI_rx.Init.Priority            = DMA_PRIORITY_HIGH;

	HAL_DMA_Init(&hdmaSPI_rx);
	
	// Associate the initialized DMA handle to the SPI handle 
	__HAL_LINKDMA(spi, hdmarx, hdmaSPI_rx);
	
		// Configure the DMA handler for Transmission process
	hdmaSPI_tx.Instance                 = DMA1_Channel3;
	hdmaSPI_tx.Init.Request             = DMA_REQUEST_1;
	hdmaSPI_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
	hdmaSPI_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
	hdmaSPI_tx.Init.MemInc              = DMA_MINC_ENABLE;
	hdmaSPI_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	hdmaSPI_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
	hdmaSPI_tx.Init.Mode                = DMA_NORMAL;
	hdmaSPI_tx.Init.Priority            = DMA_PRIORITY_LOW;

	HAL_DMA_Init(&hdmaSPI_tx);
	
	// Associate the initialized DMA handle to the SPI handle 
	__HAL_LINKDMA(spi, hdmatx, hdmaSPI_tx);
     
  // NVIC configuration for DMA transfer complete interrupt (SPI1_RX) 
	HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 1, 1);
	HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);
	
	// NVIC configuration for DMA transfer complete interrupt (SPI1_TX) 
	HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 1, 2);
	HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);
		
}
void HAL_SPI_MspDeInit(SPI_HandleTypeDef *spi){
	
	//	Reset peripherals
	__HAL_RCC_SPI1_FORCE_RESET();
	__HAL_RCC_SPI1_RELEASE_RESET();

	// Disable peripherals and GPIO Clocks
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5);		//Configure SPI SCK as alternate function
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_6);		//Configure SPI MISO as alternate function
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_7);		//Configure SPI MOSI as alternate function
	
	// De-Initialize the DMA associated to reception process 
	//HAL_DMA_DeInit(&hdmaSPI_rx);
	
	// De-Initialize the DMA associated to transmision process 
	//HAL_DMA_DeInit(&hdmaSPI_tx);
	
}
void HAL_I2C_MspInit(I2C_HandleTypeDef *i2c){
	GPIO_InitTypeDef  GPIO_InitStruct;
  RCC_PeriphCLKInitTypeDef  RCC_PeriphCLKInitStruct;
  
	if(i2c->Instance == I2C2){
		// Configure the I2C clock source. The clock is derived from the SYSCLK //
		RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2C2;
		RCC_PeriphCLKInitStruct.I2c2ClockSelection = RCC_I2C2CLKSOURCE_SYSCLK;
		HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct);

		//I2C2 clock enable
		__HAL_RCC_I2C2_CLK_ENABLE();
		
		//I2C2 GPIO port clock enable	
		__HAL_RCC_GPIOB_CLK_ENABLE();
		
		//ACC_SCL GPIO pin configuration  
		GPIO_InitStruct.Pin       = GPIO_PIN_10;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
		//GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull      = GPIO_PULLUP;
		GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		//HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,GPIO_PIN_RESET);
		 
		//ACC_SCL GPIO pin configuration  
		GPIO_InitStruct.Pin 			= GPIO_PIN_11;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
		//GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
		GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		//HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11,GPIO_PIN_RESET);
	}
}
void HAL_I2C_MspDeInit(I2C_HandleTypeDef *i2c){
	if(i2c->Instance == I2C2){
		//	Reset peripherals
		__HAL_RCC_I2C2_FORCE_RESET();
		__HAL_RCC_I2C2_RELEASE_RESET();
		
		// Disable peripherals and GPIO Clocks
		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10);		//Configure ACC_SCL as alternate function
		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_11);		//Configure ACC_SDA as alternate function		
	}
}
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

  // Configure the RTC clock source 
  RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    //while(1);
  }

  // Select LSI as RTC clock source 
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  { 
    //while(1);
  }

  // Enable the RTC peripheral Clock ####################################*/
  __HAL_RCC_RTC_ENABLE();
  
  // Configure the NVIC for RTC Alarm 
  HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 0x5, 0);
  HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
}

/**
  * @brief RTC MSP De-Initialization 
  *        This function freeze the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  * @param hrtc: RTC handle pointer
  * @retval None
  */
void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc)
{
  /*##-1- Reset peripherals ##################################################*/
  __HAL_RCC_RTC_DISABLE();
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
