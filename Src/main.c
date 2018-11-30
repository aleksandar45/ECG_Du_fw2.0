/**
  ******************************************************************************
  * @file    Templates/Src/main.c 
  * @author  MCD Application Team
  * @brief   Main program body
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
#include "main.h"

/** @addtogroup STM32L4xx_HAL_Examples
  * @{
  */

/** @addtogroup Templates
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef    TimHandle;
UART_HandleTypeDef	 BLEUartHandle;
SPI_HandleTypeDef		 ECGSpiHandle;
I2C_HandleTypeDef		 ACCI2cHandle;
ADC_HandleTypeDef		 BATTAdcHandle;
RTC_HandleTypeDef 	 RTCHandle;
Log_TypeDef  				 LogHandle;

mTimerHandler_TypeDef  mTimHandle;
BLE_TypeDef	BLEHandle;
BLE_DFU_Typedef BLE_DFUHandle;
ECG_TypeDef ECGHandle;
BATT_TypeDef BATTHandle;
GYACC_TypeDef GYACCHandle;

ProgramStageTypeDef programStage;
uint8_t startTIM1;

//--------Variables for storing BLE data in packet-------//
extern uint16_t crcTable[];
extern uint8_t dataPacketsStorage[DATA_BUFFER_SIZE][131];
extern uint8_t finalDataPaketStorage[129];

uint32_t dataPacketsRowCounter;
uint32_t dataMemoryIndex;
uint32_t missRange1, missRange2;
uint16_t packetNumber;
uint16_t finalPacketNumber;
uint16_t packetIndex;
uint32_t previousData1,previousData2,previousData3;
uint32_t currentData1,currentData2,currentData3;
uint32_t difference;
uint16_t i,rem,rem_tmp;
uint8_t ch1MSB,ch1CSB,ch1LSB;
uint8_t ch2MSB,ch2CSB,ch2LSB;
uint8_t ch3MSB,ch3CSB,ch3LSB;
uint8_t dataPacket[130] = "ANssD11D12D13S0S1S2S0S1S2S0S1S2S0S1S2S0S1S2S0S1S2S0S1S2S0S1S2S0S1S2S0S1S2S0S1S2S0S1S2S0S1S2S0S1S2S0S1S2S0S1S2S0S1S2S0S1S2S0S1S2CC";
uint8_t batteryPacket[10] = "BAT:000\r\n";
uint8_t accPacket[19] = "ACC:+000+111+222\r\n";
//uint8_t dfuErrPacket[11] = "DFU:ErrX\r\n";
uint8_t errPacket[9] = "ERR:01\r\n";
uint8_t dbgPacket[26] = "DBG:01234567890123456789\r\n";
uint8_t batteryPercentage;
//--------Variables for storing BLE data in packet-------//

//--------Test variables-----//
uint8_t testData[150];
int32_t testCounter=0;
uint8_t startCounter = 0;
uint8_t tempData8;
uint16_t tempData16;
uint32_t tempData32;
//--------Test variables-----//


// Private function prototypes -----------------------------------------------*/
// Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{	
	
	//=====Configuration steps to build program on different memory location====//  
	//	Options for target - Target - IROM1 = 0x08001000
	//	system_stm32l4xx.c
	//		SCB->VTOR = ((uint32_t)0x08001000U);  //Vector Table Relocation in Internal FLASH 
	//=====Configuration steps to build program on different memory location====//  

  HAL_Init();	
	
  SystemClock_Config();	

	//BLE_Init(&BLEUartHandle,&BLEHandle,&BLE_DFUHandle,0);
	//while(HAL_GPIO_ReadPin(BT_CFG1_PORT,BT_CFG1_PIN) == GPIO_PIN_SET){}	
		
	programStage = SYSTEM_INIT;
	//programStage = SYSTEM_INIT_WITHOUT_BLE;
	LogHandle.isError = 0;
	BLEHandle.shorterWakeupTimeout = 0;
	RTC_Config(&RTCHandle);
	
	__HAL_RCC_PWR_CLK_ENABLE();
	
	 // Check wakeup type
  if (__HAL_PWR_GET_FLAG(PWR_FLAG_WUF2) != RESET)							//BLE connection wakup	
  {
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF2);
		programStage = SYSTEM_INIT_WITHOUT_BLE;
  }
	if(__HAL_PWR_GET_FLAG(PWR_FLAG_WUFI) != RESET)							//RTC timer wakeup
  {
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUFI); 									  
		programStage = SYSTEM_INIT;
  }
	HAL_RTCEx_DeactivateWakeUpTimer(&RTCHandle);

		
	HAL_Delay(200);																				//Wait to power up ADS1294  			
	BATT_Init(&BATTAdcHandle,&BATTHandle);	
		
	ECG_Init(&ECGSpiHandle,&ECGHandle);
  GYACC_Init(&ACCI2cHandle,&GYACCHandle);
	mTimer_Config(&TimHandle, &mTimHandle);
	
	
	if(HAL_GPIO_ReadPin(GPIOH,GPIO_PIN_3) == GPIO_PIN_SET) {
		mTimer_LBlinkStatus_Stop(&mTimHandle);
		mTimer_LBlinkError_Stop(&mTimHandle);
		mTimHandle.lblinkUSBCharge = 1;
	} 
	
	
if(programStage == SYSTEM_INIT_WITHOUT_BLE){
	
	BLE_Init(&BLEUartHandle,&BLEHandle,&BLE_DFUHandle,1);
	
	mTimer_LBlinkStatus_Start(&mTimHandle,1000,0);
	
	if(HAL_GPIO_ReadPin(BT_CFG1_PORT,BT_CFG1_PIN) == GPIO_PIN_RESET){	
		BLEHandle.connectionStatus = CONNECTED;
		programStage = BLE_WAIT_MLDP_AND_CONN_PARAMS;
		startTIM1 = 1;
		BLEHandle.connParamsUpdateCounter = 0;
	}
	else{
		programStage = BLE_WAIT_CONN;
		startTIM1 = 1;
	}
}
else if(programStage == SYSTEM_INIT){	
	mTimer_LBlinkStatus_Start(&mTimHandle,200,0);
	BLE_Init(&BLEUartHandle,&BLEHandle,&BLE_DFUHandle,0);

	programStage = BLE_BATT_MEAS;
	//programStage = BLE_WAIT_CONN;
	startTIM1 = 1;
}

	BATT_StartMeasure(&BATTHandle);	


	Log_ReadDataFlash(&LogHandle);

	
	while(1){
		/*if((BLEHandle.uartBufferForward == 0)&&(BLEHandle.uartHandle->RxXferCount<50)){
				BLEHandle.uartHandle->RxXferSize  = 200;
			}	
		*/
		if(HAL_GPIO_ReadPin(GPIOH,GPIO_PIN_3) == GPIO_PIN_SET) {
			mTimer_LBlinkStatus_Stop(&mTimHandle);
			mTimer_LBlinkError_Stop(&mTimHandle);
			mTimHandle.lblinkUSBCharge = 1;
		} 
		//===========Start timeout timer for each program stage========//
		if((programStage == BLE_BATT_MEAS) && (startTIM1)){
			mTimer_TIM1_Start(&mTimHandle,3000);
			//mTimer_TIM1_Start(&mTimHandle,30000);
			startTIM1 = 0;
		}
		if((programStage == BLE_WAIT_CONN) && (startTIM1)){			
			mTimer_TIM1_Start(&mTimHandle,35000);
			if(!BATTHandle.lowLevelBattery){
				mTimer_LBlinkStatus_Start(&mTimHandle,1000,0);
			}
			startTIM1 = 0;
		}
		if((programStage == BLE_DFU_OTA) && (startTIM1)){			
			mTimer_TIM1_Start(&mTimHandle,180000);
			if(!BATTHandle.lowLevelBattery){
				mTimer_LBlinkStatus_Start(&mTimHandle,100,0);
			}
			startTIM1 = 0;
		}		
		if((programStage == BLE_WAIT_MLDP_AND_CONN_PARAMS) && (startTIM1)){
			mTimer_TIM1_Start(&mTimHandle,12000);
			startTIM1 = 0;
		}
		if((programStage == BLE_WAIT_BATTERY_INF) && (startTIM1)){
			mTimer_TIM1_Start(&mTimHandle,1000);
			startTIM1 = 0;
		}
		if((programStage == BLE_WAIT_START_ACQ) && (startTIM1)){	
#ifdef DEBUG_MODE	
			mTimer_TIM1_Start(&mTimHandle,30000);
#else
			mTimer_TIM1_Start(&mTimHandle,10000);
#endif			
			
			if(!BATTHandle.lowLevelBattery){			
				mTimer_LBlinkStatus_Start(&mTimHandle,500,0);
			}
			startTIM1 = 0;
		}
		if(((programStage == BLE_ACQ_TRANSFERING)|| (programStage==BLE_ACQ_TRANSFERING_AND_STORING)) && (startTIM1)){
			mTimer_TIM1_Start(&mTimHandle,50000);
			if(!BATTHandle.lowLevelBattery){
				mTimer_LBlinkStatus_Start(&mTimHandle,300,0);
			}
			startTIM1 = 0;
		}
		if((programStage == BLE_MEMORY_TRANSFERING) && (startTIM1)){
			mTimer_TIM1_Start(&mTimHandle,50000);
			if(!BATTHandle.lowLevelBattery){
				mTimer_LBlinkStatus_Start(&mTimHandle,300,0);
			}
			startTIM1 = 0;
		}
		//===========Start timeout timer for each program stage========//				
		
		//====================Time-out occured=========================//
		if(mTimHandle.timer1_timeoutFlag){
			if(programStage==BLE_WAIT_BATTERY_INF){
				
				/*tempData8 = BATT_CalculatePercentage(&BATTHandle);	
				Log_ReadData(&LogHandle);
				if((LogHandle.loggedData[0] > 0) && (LogHandle.loggedData[0] <= 100)){
					if(((LogHandle.loggedData[0] + 5)> tempData8 )&&(tempData8 > (LogHandle.loggedData[0])))
						batteryPercentage = LogHandle.loggedData[0];
					else 
						batteryPercentage = tempData8;
				}
				else{
					batteryPercentage = tempData8;
				}
				LogHandle.dataToLog[0] = batteryPercentage;*/
				if(BATTHandle.adcBufferFull){
					batteryPercentage = BATT_CalculatePercentage(&BATTHandle);	
					BLEHandle.batteryAdvPercentage = batteryPercentage;					
					value2DecimalString((char*)batteryPacket + 4, batteryPercentage);				
					BLE_SendData(&BLEHandle,batteryPacket,9);	
					programStage = BLE_WAIT_START_ACQ;
					startTIM1 = 1;
				}				
			}
			else {		
				
				if(BATTHandle.adcBufferFull){
					batteryPercentage = BATT_CalculatePercentage(&BATTHandle);		
					BLEHandle.batteryAdvPercentage = batteryPercentage;			
				}
								
				BLE_EnterCMDMode(&BLEHandle, WAIT_CMD_RESP,ERROR_IGNORE);		
				HAL_Delay(200);
				BLE_EnterLPMode(&BLEHandle);				
				EnterStandByMODE();
				BLE_ExitLPMode(&BLEHandle);
				HAL_NVIC_SystemReset();
			}
		}
		//====================Time-out occured=========================//
		
		//=======Read battery value and check battery voltage==========//
		if(BATTHandle.adcBufferFull){
			BATTHandle.currentBatteryVoltage = BATT_CalculateVoltage(&BATTHandle);

			if(BATTHandle.currentBatteryVoltage <= LOW_BATTERY_LEVEL_LOW_VOLTAGE){
				if(!BATTHandle.lowLevelBattery){
					mTimer_LBlinkStatus_Stop(&mTimHandle);
					mTimer_LBlinkStatus_Start(&mTimHandle,50,1);
					BATTHandle.lowLevelBattery = 1;
				}
			}
			if(BATTHandle.currentBatteryVoltage>=LOW_BATTERY_LEVEL_HIGH_VOLTAGE){				
				if(BATTHandle.lowLevelBattery){
					BATTHandle.lowLevelBattery = 0;
					mTimer_LBlinkStatus_Stop(&mTimHandle);			//put old state of blink status
				}
			}

		}			
		if(BATTHandle.adcDataReady){
			BATT_ReadADCData(&BATTHandle);
			BATTHandle.adcDataReady = 0;
		}
		//=======Read battery value and check battery voltage==========//
		
		//================Read accelerometer values====================//
		if(GYACCHandle.dataReadyTask){
			GYACC_ReadDataFromSensor(&GYACCHandle);
			GYACCHandle.dataReadyTask = 0;
		}
		//================Read accelerometer values====================//
		
		//==========Check BLE module status using STATUS pins========//
		if(HAL_GPIO_ReadPin(BT_CFG1_PORT,BT_CFG1_PIN) == GPIO_PIN_RESET){			
			if(HAL_GPIO_ReadPin(BT_CFG2_PORT,BT_CFG2_PIN) == GPIO_PIN_SET){				//Data sesion opened (MLDP mode)
				BLEHandle.cmdMode = 0;
			}	
		}
		else if((BLEHandle.connectionStatus==CONNECTED) && (HAL_GPIO_ReadPin(BT_CFG2_PORT,BT_CFG2_PIN) == GPIO_PIN_RESET)){
			if((programStage == BLE_ACQ_TRANSFERING)||(programStage==BLE_ACQ_TRANSFERING_AND_STORING)){
#ifdef ECG_Du_v2_Board
					ECG_Stop_Acquisition(&ECGHandle);
					GYACC_Stop_Acquisition(&GYACCHandle);
#endif
					EnterLowEnergyMODE();					
				}				
				BLEHandle.connectionStatus = DISCONNECTED_ADVERTISING;	
				programStage = BLE_WAIT_CONN;
				startTIM1 = 1;				
				BLE_EnterCMDMode(&BLEHandle, WAIT_CMD_RESP, ERROR_IGNORE);		
				HAL_Delay(20);
				BLEHandle.cmdMode = 1;
		}
		//==========Check BLE module status using STATUS pins========//
				
		checkUartMessage(&BLEHandle,NO_SEEK_TO_END);
		//==========Check whether status message is received========//
		if(BLEHandle.statusMessage.messageUpdated){
			if(BLEHandle.statusMessage.message == CONNECT){
				BLEHandle.connectionStatus = CONNECTED;
				programStage = BLE_WAIT_MLDP_AND_CONN_PARAMS;
				startTIM1 = 1;
				BLEHandle.connParamsUpdateCounter = 0;
			}
			//----Expected connection parameters------//
			// (1st step) CONN_PARAM,0006,0000,07D0
			// (2nd step) CONN_PARAM,00027,0000,07D0
			// (3rd step) CONN_PARAM,000C,0000,0200 - after notification enable
			else if(BLEHandle.statusMessage.message == CONN_PARAM){
				Log_WriteData(&LogHandle, "main/CONN_PARAM_xxxx");
				if(programStage == BLE_WAIT_MLDP_AND_CONN_PARAMS){
					if(BLEHandle.connParamsUpdateCounter == 2){						
						BLEHandle.connParamsUpdateCounter = 2;
					}
					BLEHandle.connParamsUpdateCounter++;
					if((BLEHandle.statusMessage.param1<=12)&&(BLEHandle.statusMessage.param3>=400) && (!BLEHandle.cmdMode)) {
					//if(!BLEHandle.cmdMode) {
						//programStage = ECG_INIT;
						mTimer_TIM1_Start(&mTimHandle,1000);																		
						programStage = BLE_WAIT_BATTERY_INF;
						//programStage = BLE_WAIT_START_ACQ;
						startTIM1 = 1;					
					}
				}
			}
			else if(BLEHandle.statusMessage.message == STREAM_OPEN){
				if((HAL_GPIO_ReadPin(BT_CFG1_PORT,BT_CFG1_PIN) == GPIO_PIN_RESET) && (HAL_GPIO_ReadPin(BT_CFG2_PORT,BT_CFG2_PIN) == GPIO_PIN_SET)){
					BLEHandle.cmdMode = 0;						
				}								
			}
			else if(BLEHandle.statusMessage.message == DISCONNECT){
				if(BLEHandle.connectionStatus == CONNECTED){
					if((programStage == BLE_ACQ_TRANSFERING)||(programStage==BLE_ACQ_TRANSFERING_AND_STORING)){
#ifdef ECG_Du_v2_Board
						ECG_Stop_Acquisition(&ECGHandle);
						GYACC_Stop_Acquisition(&GYACCHandle);
#endif
						EnterLowEnergyMODE();
					}				
					BLEHandle.connectionStatus = DISCONNECTED_ADVERTISING;	
					programStage = BLE_WAIT_CONN;
					startTIM1 = 1;
					
					BLE_EnterCMDMode(&BLEHandle, WAIT_CMD_RESP, ERROR_IGNORE);		
					HAL_Delay(20);
					BLEHandle.cmdMode = 1;
				}
				
			}
			BLEHandle.statusMessage.messageUpdated = 0;
		}
		//==========Check whether status message is received========//		
		
		if(BLEHandle.ackOrAppMessage.messageUpdated){
			if(BLEHandle.ackOrAppMessage.message==APP_DFU_PACKET){
				if(BLE_DFU_Process(&BLEHandle)==BLE_OK){
					if(BLEHandle.dfuHandle->dfuCompleted){
						errPacket[4] = '0';													//ERROR 00 means everything is OK
						errPacket[5] = '0';
						BLE_SendData(&BLEHandle,errPacket,8);						
						HAL_NVIC_SystemReset();	
					}					
				}
				else{
					errPacket[4] = '9';													//ERROR 9x refers to DFU errors						
					errPacket[5] = BLEHandle.dfuHandle->dfuError + 0x30;
					BLE_SendData(&BLEHandle,errPacket,8);
					EnterLowEnergyMODE();
					
					programStage = BLE_WAIT_CONN;
					startTIM1 = 1;
				}
				BLEHandle.ackOrAppMessage.messageUpdated = 0;
			}
			else if(BLEHandle.ackOrAppMessage.message == APP_DFU_OTA){
				RTC_Disable(&RTCHandle);
				BLE_DFU_Init(&BLEHandle);
				
				programStage = BLE_DFU_OTA;
				startTIM1 = 1;
				BLEHandle.ackOrAppMessage.messageUpdated = 0;
			}
		}
		
		if((BLEHandle.connectionStatus==CONNECTED) && (!BLEHandle.cmdMode)){				
			//--------Check if App command message is received---------//
			if(BLEHandle.ackOrAppMessage.messageUpdated){
				if(BLEHandle.ackOrAppMessage.message==APP_DOK){
					if((programStage == BLE_ACQ_TRANSFERING)||(programStage==BLE_ACQ_TRANSFERING_AND_STORING)){
						if(BLEHandle.dataOKRespEnabled){												//not used
							if(BLEHandle.ackOrAppMessage.param1 == packetNumber){						
								BLEHandle.dataOKWaiting=0;
								BLEHandle.repeatDataPacketCounter = 0;
								packetNumber++;
							}
							else{
								BLEHandle.repeatDataPacket = 1;
							}
						}
					}
				}
				else if(BLEHandle.ackOrAppMessage.message==APP_DNOK){
					if((programStage == BLE_ACQ_TRANSFERING)||(programStage==BLE_ACQ_TRANSFERING_AND_STORING)){
						BLEHandle.repeatDataPacket = 1;
						BLEHandle.repeatDataPacketNumber = BLEHandle.ackOrAppMessage.param1;
					}
				}
				else if(BLEHandle.ackOrAppMessage.message==APP_STOP_ACQ){
					if((programStage == BLE_ACQ_TRANSFERING)||(programStage==BLE_ACQ_TRANSFERING_AND_STORING)||(programStage==BLE_MEMORY_TRANSFERING)){
						programStage = BLE_WAIT_START_ACQ;
						startTIM1 = 1;	
						
						ECG_Stop_Acquisition(&ECGHandle);
						GYACC_Stop_Acquisition(&GYACCHandle);
						
						EnterLowEnergyMODE();
					}
				}
				else if(BLEHandle.ackOrAppMessage.message==APP_START_ACQ){
					if(programStage == BLE_WAIT_START_ACQ){

						EnterHighEnergyMODE();
												
#ifdef RN4871_Nucleo_Test_Board					
						startCounter++;
						/*if((startCounter%4)==0){
							HAL_Delay(400);
							programStage = BLE_MEMORY_TRANSFERING;		
							dataMemoryIndex = 0;
							missRange1 = 0;
							missRange2 = 750;
							dataMemoryIndex = missRange1;							
						}
						else{*/
							HAL_Delay(400);
							programStage = BLE_ACQ_TRANSFERING;
							dataPacketsRowCounter = 0;							
						//}
#else																			
						programStage = BLE_ACQ_TRANSFERING;
#endif
						
					  startTIM1 = 1;
						
						packetNumber = 0;
						finalPacketNumber = 0;
						BLEHandle.dataOKWaiting = 0;
						BLEHandle.repeatDataPacket = 0;
						BLEHandle.repeatDataPacketCounter = 0;						
						testCounter = 0;

						ECG_Start_Acquisition(&ECGHandle);			
						GYACC_Start_Acquisition(&GYACCHandle);
					}
					
				}				
				else if(BLEHandle.ackOrAppMessage.message==APP_MISS_RANGE){
					
					programStage = BLE_MEMORY_TRANSFERING;			
					missRange1 = BLEHandle.ackOrAppMessage.param1;
					missRange2 = BLEHandle.ackOrAppMessage.param2;
					dataMemoryIndex = 0;
					
					ECG_Stop_Acquisition(&ECGHandle);
					GYACC_Stop_Acquisition(&GYACCHandle);
					startTIM1 = 1;
					
				}
				else if(BLEHandle.ackOrAppMessage.message == APP_END_BLOCK){
					if(programStage == BLE_ACQ_TRANSFERING){
						programStage = BLE_ACQ_TRANSFERING_AND_STORING;
						
						startTIM1 = 1;
					}
					finalPacketNumber = BLEHandle.ackOrAppMessage.param1;
				}
				else if(BLEHandle.ackOrAppMessage.message==APP_OFF){
					if(BATTHandle.adcBufferFull){
						batteryPercentage = BATT_CalculatePercentage(&BATTHandle);		
						BLEHandle.batteryAdvPercentage = batteryPercentage;
					}			
					
					HAL_Delay(200);
					BLE_EnterCMDMode(&BLEHandle, WAIT_CMD_RESP,ERROR_IGNORE);
					HAL_Delay(20);
					BLE_EnterLPMode(&BLEHandle);
					EnterStandByMODE();
					BLE_ExitLPMode(&BLEHandle);
					HAL_NVIC_SystemReset();
				}
				else if(BLEHandle.ackOrAppMessage.message == APP_STAY){
					startTIM1 = 1;
				}
				else if(BLEHandle.ackOrAppMessage.message == APP_DFU){
					if(HAL_GPIO_ReadPin(GPIOH,GPIO_PIN_3) == GPIO_PIN_SET){
						HAL_NVIC_SystemReset();
					}
				}
				BLEHandle.ackOrAppMessage.messageUpdated = 0;
			}
			//--------Check if App command message is received---------//
			
			//---------------Send missing packets over BLE---------------//
			if(programStage == BLE_MEMORY_TRANSFERING){				
				if(dataMemoryIndex < dataPacketsRowCounter){
					
					tempData32 = dataPacketsStorage[dataMemoryIndex][4];
					tempData32<<=8;
					tempData32 |= dataPacketsStorage[dataMemoryIndex][5];
					
					if((tempData32 >= missRange1) && (tempData32<= missRange2)){
						for(i=0;i<129;i++){
							dataPacket[i] = dataPacketsStorage[dataMemoryIndex][i+2];						
						}
						if(BLE_ERROR == BLE_SendData(&BLEHandle,dataPacket,129)){
								programStage = BLE_WAIT_START_ACQ;
								startTIM1 = 1;	
						}
						HAL_Delay(35);
					}
					
					if(tempData32 > missRange2){
						BLE_SendData(&BLEHandle,finalDataPaketStorage,129);
						programStage = BLE_WAIT_START_ACQ;
						startTIM1 = 1;	
					}
					
					//if(tempData32 != dataMemoryIndex){
					//	tempData32 = 0;						
					//}
					//if((dataMemoryIndex >950)||(dataMemoryIndex > missRange2)) {
					//	programStage = BLE_WAIT_START_ACQ;
					//}
					
					dataMemoryIndex++;							
				}				
			}
			//---------------Send missing packets over BLE---------------//
			
			//-------------------Send debug data------------------------//
#ifdef DEBUG_MODE			
			if((programStage == BLE_WAIT_START_ACQ)||(programStage == BLE_ACQ_TRANSFERING)){
				if(Log_ReadData(&LogHandle,dbgPacket+4)!=0){
					if(BLE_ERROR == BLE_SendData(&BLEHandle,dbgPacket,26)){
						programStage = BLE_WAIT_CONN;
						startTIM1 = 1;						
					}
				}				
				HAL_Delay(5);
			}
#endif
			//-------------------Send debug data------------------------//
			
			//--------Prepare BLE transfering packet---------//
			if((programStage == BLE_ACQ_TRANSFERING)||(programStage==BLE_ACQ_TRANSFERING_AND_STORING)){					

				if(!BLEHandle.dataOKWaiting){
					if(ECGHandle.dataFIFOAvailable > 60){
						
						//--------------Prepare BLE packet for transfer============//
						dataPacket[2] = packetNumber >>8;																		//MSB first
						dataPacket[3] = packetNumber;													

						dataPacket[6]=ECG_ReadFIFODataCH1(&ECGHandle);											//First sample MSB CH1
						dataPacket[9]=ECG_ReadFIFODataCH2(&ECGHandle); 											//First sample MSB CH2
						dataPacket[12]=ECG_ReadFIFODataCH3(&ECGHandle);											//First sample MSB CH3
						
						dataPacket[5]=ECG_ReadFIFODataCH1(&ECGHandle);											//First sample CSB CH1
						dataPacket[8]=ECG_ReadFIFODataCH2(&ECGHandle); 											//First sample CSB CH2
						dataPacket[11]=ECG_ReadFIFODataCH3(&ECGHandle);											//First sample CSB CH3
						
						dataPacket[4]=ECG_ReadFIFODataCH1(&ECGHandle);											//First sample LSB CH1
						dataPacket[7]=ECG_ReadFIFODataCH2(&ECGHandle); 											//First sample LSB CH2
						dataPacket[10]=ECG_ReadFIFODataCH3(&ECGHandle);											//First sample LSB CH3
						
						previousData1 = __threeByteToInt(previousData1,dataPacket[6],dataPacket[5],dataPacket[4]);
						previousData2 = __threeByteToInt(previousData2,dataPacket[9],dataPacket[8],dataPacket[7]);
						previousData3 = __threeByteToInt(previousData3,dataPacket[12],dataPacket[11],dataPacket[10]);

						
						for(packetIndex=13;packetIndex<127;packetIndex=packetIndex+6){
							ch1MSB = ECG_ReadFIFODataCH1(&ECGHandle);
							ch2MSB = ECG_ReadFIFODataCH2(&ECGHandle);
							ch3MSB = ECG_ReadFIFODataCH3(&ECGHandle);
							
							ch1CSB = ECG_ReadFIFODataCH1(&ECGHandle);
							ch2CSB = ECG_ReadFIFODataCH2(&ECGHandle);
							ch3CSB = ECG_ReadFIFODataCH3(&ECGHandle);
							
							ch1LSB = ECG_ReadFIFODataCH1(&ECGHandle);
							ch2LSB = ECG_ReadFIFODataCH2(&ECGHandle);
							ch3LSB = ECG_ReadFIFODataCH3(&ECGHandle);
							
							currentData1 = __threeByteToInt(currentData1,ch1MSB,ch1CSB,ch1LSB);
							currentData2 = __threeByteToInt(currentData2,ch2MSB,ch2CSB,ch2LSB);
							currentData3 = __threeByteToInt(currentData3,ch3MSB,ch3CSB,ch3LSB);

							difference=currentData1-previousData1;
							dataPacket[packetIndex]=difference; 																						//LSB fist
							dataPacket[packetIndex+1]=difference>>8;
							previousData1 = currentData1;
							
							difference=currentData2-previousData2;
							dataPacket[packetIndex+2]=difference; 																					//LSB fist
							dataPacket[packetIndex+3]=difference>>8;
							previousData2 = currentData2;
							
							difference=currentData3-previousData3;
							dataPacket[packetIndex+4]=difference; 																					//LSB fist
							dataPacket[packetIndex+5]=difference>>8;
							previousData3 = currentData3;
											
						}
						//--------------Prepare BLE packet for transfer============//
						
						//----------CRC calculation-----------------//
						rem = 0x0000;
						for (i = 0; i<127; i++) {																													
									rem_tmp = dataPacket[i];
									rem = (rem >> 8) ^ crcTable[(rem ^ rem_tmp) & 0xff];
						}
						dataPacket[127]=rem>>8; 																													//CRC MSB first
						dataPacket[128]=rem;
						/*if((packetNumber==210)|| (packetNumber==510) ){
							dataPacket[127]=0;
							dataPacket[128]=0;
						}*/
						//----------CRC calculation-----------------//
						
						//----------Send Data packet---------------//
						//if(((packetNumber >110) && (packetNumber < 130)) || ((packetNumber >810) && (packetNumber < 830))){}
						//else{
							
							if(BLE_ERROR == BLE_SendData(&BLEHandle,dataPacket,129)){
								programStage = BLE_WAIT_CONN;
								startTIM1 = 1;						
#ifdef ECG_Du_v2_Board
								ECG_Stop_Acquisition(&ECGHandle);
								GYACC_Stop_Acquisition(&GYACCHandle);
#endif						
								EnterLowEnergyMODE();
							}
						//}
						
						dataPacket[127]=rem>>8; 	
						dataPacket[128]=rem;
						//----------Send Data packet---------------//
						
						//----------Store Data packet in internal memory-------//
						if(programStage==BLE_ACQ_TRANSFERING_AND_STORING){						
							dataPacketsStorage[dataPacketsRowCounter][0]=packetNumber>>8;
							dataPacketsStorage[dataPacketsRowCounter][1]=packetNumber;
							for(i=2;i<131;i++){
								dataPacketsStorage[dataPacketsRowCounter][i] = dataPacket[i-2];
							}
							dataPacketsRowCounter++;
							if(dataPacketsRowCounter >=DATA_BUFFER_SIZE){
								programStage = BLE_ACQ_TRANSFERING;								
							}							
						}
						if((finalPacketNumber>0) && (packetNumber == finalPacketNumber)){
							for(i=0;i<129;i++){
								finalDataPaketStorage[i] = dataPacket[i];
							}
						}
						//----------Store Data packet in internal memory-------//
						
						
						if(BLEHandle.dataOKRespEnabled){								//not used				
							BLEHandle.dataOKWaiting=1;										
						}
						else {
							//-------If DNOK is received, repeat requested packet-------//
							if(BLEHandle.repeatDataPacket){
								for(i=0;i<20;i++){
									tempData16 = dataPacketsStorage[dataPacketsRowCounter - 1 - i][0];
									tempData16<<=8;
									tempData16 |= (dataPacketsStorage[dataPacketsRowCounter -1 - i][1] & 0x00FF);
									if(tempData16 ==  BLEHandle.repeatDataPacketNumber){
										if(BLE_ERROR == BLE_SendData(&BLEHandle,&dataPacketsStorage[i][2],129)){
											programStage = BLE_WAIT_CONN;
											startTIM1 = 1;						
#ifdef ECG_Du_v2_Board
											ECG_Stop_Acquisition(&ECGHandle);
											GYACC_Stop_Acquisition(&GYACCHandle);
#endif						
											EnterLowEnergyMODE();
										}										
										HAL_Delay(2);
										break;
									}
								}
								BLEHandle.repeatDataPacket = 0;
								BLEHandle.repeatDataPacketCounter++;
								if(BLEHandle.repeatDataPacketCounter >=7){
									programStage = BLE_WAIT_START_ACQ;
									BLEHandle.bleStatus = BLE_ERROR;
									BLEHandle.ErrorNumber = BLE_DNOK_OVF_ERROR;
									startTIM1 = 1;
									EnterLowEnergyMODE();
								}									
							}
							//-------If DNOK is received, repeat requested packet-------//
							
							packetNumber++;								
						}
					}
					if(ECGHandle.dataFIFOAvailable> (ECG_FIFO_SIZE*4/5)){
						programStage = BLE_WAIT_START_ACQ;
						startTIM1 = 1;						
#ifdef ECG_Du_v2_Board
						ECG_Stop_Acquisition(&ECGHandle);
						GYACC_Stop_Acquisition(&GYACCHandle);
#endif						
						EnterLowEnergyMODE();						
					}
				}
				else{																						//not used
					if(BLEHandle.repeatDataPacket){
						BLE_SendData(&BLEHandle,dataPacket,129);							
						BLEHandle.repeatDataPacketCounter++;
						if(BLEHandle.repeatDataPacketCounter >=7){
							programStage = BLE_WAIT_START_ACQ;
							BLEHandle.bleStatus = BLE_ERROR;
							BLEHandle.ErrorNumber = BLE_DNOK_OVF_ERROR;
							startTIM1 = 1;
							EnterLowEnergyMODE();
							//BLE_SendData(&BLEHandle,errorPacket,129);  //transmit error message
						}
					}
				}
				if(GYACCHandle.newDataAvailable){					
					GYACCHandle.newDataAvailable = 0;	
					GYACC_CalculateAngles(&GYACCHandle,accPacket);
					if(BLE_ERROR == BLE_SendData(&BLEHandle,accPacket,18)){
						programStage = BLE_WAIT_CONN;
						startTIM1 = 1;						
#ifdef ECG_Du_v2_Board
						ECG_Stop_Acquisition(&ECGHandle);
						GYACC_Stop_Acquisition(&GYACCHandle);
#endif						
						EnterLowEnergyMODE();
					}		
					
				}
			}
			//--------Prepare BLE transfering packet---------//					
			
		}
		
		
	}

}


void Error_Handler(void)
{	
	if((BLEHandle.ErrorNumber == BLE_UART_ERROR)|| (BLEHandle.ErrorNumber ==BLE_ENTER_CMD_MODE_ERROR)){
		
	}
	else if(ECGHandle.ErrorNumber == ECG_FIFO_ERROR){
	}
	else{
		/*BLE_EnterCMDMode(&BLEHandle, WAIT_CMD_RESP,ERROR_IGNORE);				
		BLE_EnterLPMode(&BLEHandle);	
		EnterStandByMODE();*/
	}	
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char *file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
