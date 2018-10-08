/**
  ******************************************************************************
  * @file    Templates/Inc/main.h 
  * @author  MCD Application Team
  * @brief   Header for main.c module
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
  
// Define to prevent recursive inclusion -------------------------------------//
#ifndef __MAIN_H
#define __MAIN_H

// Includes ------------------------------------------------------------------//
#include "stm32l4xx_hal.h"
#include "mTimer.h"
#include "mPWR.h"
#include "mUART.h"
#include "mADC.h"
#include "mFLASHInt.h"
#include "BLE_RN4871.h"
#include "ECG_ADS1294.h"
#include "BATT.h"

// Defines ------------------------------------------------------------------//
#define DEBUG_MODE
//#define RN4871_Nucleo_Test_Board

#ifdef RN4871_Nucleo_Test_Board
	#define DATA_BUFFER_SIZE			980
	
	#define MCU_TEST_DATA
#else
	#define ECG_Du_v1_Board
	#define DATA_BUFFER_SIZE			980
	
	//#define MCU_TEST_DATA
	//#define ECG_TEST_DATA
#endif

#ifdef RN4871_Nucleo_Test_Board
	#define LED_ERROR_PIN                      GPIO_PIN_5
	#define LED_ERROR_PORT                     GPIOA
	#define LED_ERROR_CLK_ENABLE()             __HAL_RCC_GPIOA_CLK_ENABLE()
	#define LED_ERROR_CLK_DISABLE()            __HAL_RCC_GPIOA_CLK_DISABLE()

	#define LED_STATUS1_PIN                    GPIO_PIN_5
	#define LED_STATUS1_PORT                   GPIOA
	#define LED_STATUS1_CLK_ENABLE()           __HAL_RCC_GPIOA_CLK_ENABLE()
	#define LED_STATUS1_CLK_DISABLE()          __HAL_RCC_GPIOA_CLK_DISABLE()

	#define LED_STATUS2_PIN                    GPIO_PIN_5
	#define LED_STATUS2_PORT                   GPIOA
	#define LED_STATUS2_CLK_ENABLE()           __HAL_RCC_GPIOA_CLK_ENABLE()
	#define LED_STATUS2_CLK_DISABLE()          __HAL_RCC_GPIOA_CLK_DISABLE()
	
	#define BT_RESET_PIN											 GPIO_PIN_12
	#define BT_RESET_PORT                      GPIOA
	#define BT_RESET_CLK_ENABLE()           	 __HAL_RCC_GPIOA_CLK_ENABLE()
	#define BT_RESET_CLK_DISABLE()          	 __HAL_RCC_GPIOA_CLK_DISABLE()
	
	#define BT_CFG1_PIN												 GPIO_PIN_13
	#define BT_CFG1_PORT                     	 GPIOC
	#define BT_CFG1_CLK_ENABLE()           		 __HAL_RCC_GPIOC_CLK_ENABLE()
	#define BT_CFG1_CLK_DISABLE()          		 __HAL_RCC_GPIOC_CLK_DISABLE()
	
	#define BT_CFG2_PIN												 GPIO_PIN_14
	#define BT_CFG2_PORT                     	 GPIOC
	#define BT_CFG2_CLK_ENABLE()           		 __HAL_RCC_GPIOC_CLK_ENABLE()
	#define BT_CFG2_CLK_DISABLE()          		 __HAL_RCC_GPIOC_CLK_DISABLE()
	
	#define BT_CFG3_PIN												 GPIO_PIN_15
	#define BT_CFG3_PORT                     	 GPIOC
	#define BT_CFG3_CLK_ENABLE()           		 __HAL_RCC_GPIOC_CLK_ENABLE()
	#define BT_CFG3_CLK_DISABLE()          		 __HAL_RCC_GPIOC_CLK_DISABLE()
	
#endif
#ifdef ECG_Du_v1_Board
	#define LED_ERROR_PIN                      GPIO_PIN_7
	#define LED_ERROR_PORT                     GPIOB
	#define LED_ERROR_CLK_ENABLE()             __HAL_RCC_GPIOB_CLK_ENABLE()
	#define LED_ERROR_CLK_DISABLE()            __HAL_RCC_GPIOB_CLK_DISABLE()

	#define LED_STATUS1_PIN                    GPIO_PIN_8
	#define LED_STATUS1_PORT                   GPIOB
	#define LED_STATUS1_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()
	#define LED_STATUS1_CLK_DISABLE()          __HAL_RCC_GPIOB_CLK_DISABLE()

	#define LED_STATUS2_PIN                    GPIO_PIN_7
	#define LED_STATUS2_PORT                   GPIOB
	#define LED_STATUS2_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()
	#define LED_STATUS2_CLK_DISABLE()          __HAL_RCC_GPIOB_CLK_DISABLE()
	
	#define BT_RESET_PIN											 GPIO_PIN_6
	#define BT_RESET_PORT                     GPIOB
	#define BT_RESET_CLK_ENABLE()           	 __HAL_RCC_GPIOB_CLK_ENABLE()
	#define BT_RESET_CLK_DISABLE()          	 __HAL_RCC_GPIOB_CLK_DISABLE()
	
	#define BT_CFG1_PIN												 GPIO_PIN_14
	#define BT_CFG1_PORT                     	 GPIOB
	#define BT_CFG1_CLK_ENABLE()           		 __HAL_RCC_GPIOB_CLK_ENABLE()
	#define BT_CFG1_CLK_DISABLE()          		 __HAL_RCC_GPIOB_CLK_DISABLE()
	
	#define BT_CFG2_PIN												 GPIO_PIN_2
	#define BT_CFG2_PORT                     	 GPIOB
	#define BT_CFG2_CLK_ENABLE()           		 __HAL_RCC_GPIOB_CLK_ENABLE()
	#define BT_CFG2_CLK_DISABLE()          		 __HAL_RCC_GPIOB_CLK_DISABLE()
	
	#define BT_CFG3_PIN												 GPIO_PIN_14
	#define BT_CFG3_PORT                     	 GPIOC
	#define BT_CFG3_CLK_ENABLE()           		 __HAL_RCC_GPIOC_CLK_ENABLE()
	#define BT_CFG3_CLK_DISABLE()          		 __HAL_RCC_GPIOC_CLK_DISABLE()
#endif



// Exported types ------------------------------------------------------------//
typedef enum {
	SYSTEM_INIT = 0,							//BLE module Initialization 
	BLE_WAIT_CONN = 1,				//Wait for BLE connection
	BLE_WAIT_MLDP_AND_CONN_PARAMS = 3,   //Wait for BLE connection parameters update and notification enable on UART Transparent characteristic
	ECG_INIT = 4,							//Initialize ECG module
	ECG_WAIT_LEAD_ON = 5,			//Wait leads on status to start acquisition
	BLE_WAIT_BATTERY_INF = 6,	//Wait some delay to send Battery information
	BLE_WAIT_START_ACQ = 7,		//Wait start acquisition CMD
	BLE_ACQ_TRANSFERING = 8,	//Transfering ECG data over BLE
	BLE_ACQ_TRANSFERING_AND_STORING = 9,	//Transfering ECG data over BLE and storing packets in FLASH memory
	BLE_MEMORY_TRANSFERING = 10,	//Transfering memorized data over BLE (missing packets)
}ProgramStageTypeDef;

// Exported constants --------------------------------------------------------//
// Exported macro ------------------------------------------------------------//
#define __threeByteToInt(OUT,B1,B2,B3)  (OUT = B1, OUT<<=8, OUT+= B2, OUT<<=8, OUT+= B3, ((OUT & 0x00800000)==0x00800000 ? OUT|=0xFF000000 : OUT ))

/* Exported functions ------------------------------------------------------- */
void Error_Handler(void);

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
