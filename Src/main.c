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
ADC_HandleTypeDef		 BATTAdcHandle;

mTimerHandler_TypeDef  mTimHandle;
BLE_TypeDef	BLEHandle;
ECG_TypeDef ECGHandle;
BATT_TypeDef BATTHandle;

ProgramStageTypeDef programStage;
uint8_t startTIM1;

//--------Variables for storing BLE data in packet-------//
extern uint16_t crcTable[];
extern uint8_t dataPacketsStorage[10][131];

uint8_t dataPacketsRowCounter;
uint16_t packetNumber;
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
//--------Variables for storing BLE data in packet-------//

//--------Variables for storing BLE packets in FLASH-------//
uint8_t dataPacketsFLASHRowCounter;
uint8_t dataPacketsFLASHIndex;
uint32_t flashAddress;
uint32_t flashAddressBuffer[1125];
uint32_t flashAddressIndex = 0;
extern uint8_t flashBusy;
//--------Variables for storing BLE packets in FLASH-------//

//--------Test variables-----//
uint8_t testData[150];
int32_t testCounter=0;
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

  HAL_Init();
	
  SystemClock_Config();	
	
	__HAL_RCC_PWR_CLK_ENABLE();
	 // Check and Clear the Wakeup flag 
  if (__HAL_PWR_GET_FLAG(PWR_FLAG_WUF2) != RESET)
  {
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF2);
  }
	
	programStage = SYSTEM_INIT;
		
	HAL_Delay(100);																				//Wait to power up ADS1294  			
	BATT_Init(&BATTAdcHandle,&BATTHandle);	
		
	ECG_Init(&ECGSpiHandle,&ECGHandle);
	mTimer_Config(&TimHandle, &mTimHandle);
	
#ifdef DEBUG_MODE
	mTimer_LBlinkStatus_Start(&mTimHandle,1000,0);
#endif
	
	BLE_Init(&BLEUartHandle,&BLEHandle);	
	
	BATT_StartMeasure(&BATTHandle);	
	
	programStage = BLE_WAIT_CONN;
	startTIM1 = 1;
	
	while(1){
		//===========Start timeout timer for each program stage========//
		if((programStage == BLE_WAIT_CONN) && (startTIM1)){			
			mTimer_TIM1_Start(&mTimHandle,35000);
#ifdef DEBUG_MODE
			if(!BATTHandle.lowLevelBattery){
				mTimer_LBlinkStatus_Start(&mTimHandle,1000,0);
			}
#endif
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
			mTimer_TIM1_Start(&mTimHandle,10000);
#ifdef DEBUG_MODE		
			if(!BATTHandle.lowLevelBattery){			
				mTimer_LBlinkStatus_Start(&mTimHandle,500,0);
			}
#endif
			startTIM1 = 0;
		}
		if(((programStage == BLE_ACQ_TRANSFERING)|| (programStage==BLE_ACQ_TRANSFERING_AND_STORING)) && (startTIM1)){
			mTimer_TIM1_Start(&mTimHandle,50000);
#ifdef DEBUG_MODE
			if(!BATTHandle.lowLevelBattery){
				mTimer_LBlinkStatus_Start(&mTimHandle,300,0);
			}
#endif
			startTIM1 = 0;
		}
		//===========Start timeout timer for each program stage========//
		
		//====================Timerout occured=========================//
		if(mTimHandle.timer1_timeoutFlag){
			if(programStage==BLE_WAIT_BATTERY_INF){
					tempData8 = BATT_CalculatePercentage(&BATTHandle);
					value2DecimalString((char*)batteryPacket + 4, tempData8);
					BLE_SendData(&BLEHandle,batteryPacket,9);	
					programStage = BLE_WAIT_START_ACQ;
					startTIM1 = 1;
				}
			else {
				if(!BLEHandle.cmdMode){
					BLE_EnterCMDMode(&BLEHandle, WAIT_CMD_RESP);
				}
				BLE_EnterLPMode(&BLEHandle);				
				EnterStandByMODE();
				BLE_ExitLPMode(&BLEHandle);
				HAL_NVIC_SystemReset();
			}
		}
		//====================Timerout occured=========================//
		
		//=======Read battery value and check battery voltage==========//
		if(BATTHandle.adcBufferFull){
			BATTHandle.currentBatteryVoltage = BATT_CalculateVoltage(&BATTHandle);
//#ifdef ECG_Du_v1_Board
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
//#endif
		}			
		if(BATTHandle.adcDataReady){
			BATT_ReadADCData(&BATTHandle);
			BATTHandle.adcDataReady = 0;
		}
		//=======Read battery value and check battery voltage==========//
		
		//==========Check BLE module status using STATUS pins========//
		if(HAL_GPIO_ReadPin(BT_CFG1_PORT,BT_CFG1_PIN) == GPIO_PIN_RESET){
			if(HAL_GPIO_ReadPin(BT_CFG2_PORT,BT_CFG2_PIN) == GPIO_PIN_SET){				//Data sesion opened (MLDP mode)
				BLEHandle.cmdMode = 0;
			}	
		}
		else if((BLEHandle.connectionStatus==CONNECTED) && (HAL_GPIO_ReadPin(BT_CFG2_PORT,BT_CFG2_PIN) == GPIO_PIN_RESET)){
			if((programStage == BLE_ACQ_TRANSFERING)||(programStage==BLE_ACQ_TRANSFERING_AND_STORING)){
#ifdef ECG_Du_v1_Board
					ECG_Stop_Acquisition(&ECGHandle);
#endif
					EnterLowEnergyMODE();					
				}				
				BLEHandle.connectionStatus = DISCONNECTED_ADVERTISING;	
				programStage = BLE_WAIT_CONN;
				startTIM1 = 1;
				if(!BLEHandle.cmdMode){
					BLE_EnterCMDMode(&BLEHandle, WAIT_CMD_RESP);
				}
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
				if(programStage == BLE_WAIT_MLDP_AND_CONN_PARAMS){
					if(BLEHandle.connParamsUpdateCounter == 2){						
						BLEHandle.connParamsUpdateCounter = 2;
					}
					BLEHandle.connParamsUpdateCounter++;
					//if((BLEHandle.statusMessage.param1<=12)&&(BLEHandle.statusMessage.param3>=400) && (!BLEHandle.cmdMode) && (BLEHandle.connParamsUpdateCounter >=3)) {
					if((BLEHandle.statusMessage.param1==12)&&(BLEHandle.statusMessage.param3==512) && (!BLEHandle.cmdMode)) {
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
#ifdef ECG_Du_v1_Board
						ECG_Stop_Acquisition(&ECGHandle);
#endif
						EnterLowEnergyMODE();
					}				
					BLEHandle.connectionStatus = DISCONNECTED_ADVERTISING;	
					programStage = BLE_WAIT_CONN;
					startTIM1 = 1;
					if(!BLEHandle.cmdMode){
						BLE_EnterCMDMode(&BLEHandle, WAIT_CMD_RESP);
					}
					BLEHandle.cmdMode = 1;
				}
				
			}
			BLEHandle.statusMessage.messageUpdated = 0;
		}
		//==========Check whether status message is received========//		
		
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
					if((programStage == BLE_ACQ_TRANSFERING)||(programStage==BLE_ACQ_TRANSFERING_AND_STORING)){
						programStage = BLE_WAIT_START_ACQ;
						startTIM1 = 1;
						
#ifdef ECG_Du_v1_Board
						ECG_Stop_Acquisition(&ECGHandle);
#endif
						
						EnterLowEnergyMODE();
					}
				}
				else if(BLEHandle.ackOrAppMessage.message==APP_START_ACQ){
					if(programStage == BLE_WAIT_START_ACQ){
						programStage = BLE_ACQ_TRANSFERING;
					  startTIM1 = 1;
						EnterHighEnergyMODE();
						
						packetNumber = 0;
						BLEHandle.dataOKWaiting = 0;
						BLEHandle.repeatDataPacket = 0;
						BLEHandle.repeatDataPacketCounter = 0;
						dataPacketsRowCounter = 0;
						testCounter = 0;
						
#ifdef FLASH_DATA_STORAGE			
						dataPacketsFLASHRowCounter = dataPacketsRowCounter;
						dataPacketsFLASHIndex = 0;
						flashAddress = FLASH_DATA_STORAGE_START_ADDR;
						flashAddressIndex = 0;
						programStage = BLE_ACQ_TRANSFERING_AND_STORING;
						FLASH_Erase(FLASH_DATA_STORAGE_START_ADDR, FLASH_DATA_STORAGE_END_ADDR);						
#endif
						
#ifdef ECG_Du_v1_Board
						ECG_Start_Acquisition(&ECGHandle);
#endif							
					}
					
				}
				else if(BLEHandle.ackOrAppMessage.message==APP_OFF){
					BLE_EnterCMDMode(&BLEHandle, WAIT_CMD_RESP);
					BLE_EnterLPMode(&BLEHandle);
					EnterStandByMODE();
					BLE_ExitLPMode(&BLEHandle);
					HAL_NVIC_SystemReset();
				}
				else if(BLEHandle.ackOrAppMessage.message == APP_STAY){
					startTIM1 = 1;
				}
				
				BLEHandle.ackOrAppMessage.messageUpdated = 0;
			}
			//--------Check if App command message is received---------//
				
			//-------------Store packets in FLASH memory---------------//
			if(programStage==BLE_ACQ_TRANSFERING_AND_STORING){
				if((dataPacketsFLASHRowCounter!=dataPacketsRowCounter) && (!flashBusy)){
					if (HAL_FLASH_Program_IT(FLASH_TYPEPROGRAM_DOUBLEWORD, flashAddress, dataPacketsStorage[dataPacketsFLASHRowCounter][dataPacketsFLASHIndex+4]) == HAL_OK)
					{
						//Data packet [2:129] is stored
						flashAddress = flashAddress + 8;
						if(flashAddress >= FLASH_DATA_STORAGE_END_ADDR){		//this is irregular state
							programStage = BLE_ACQ_TRANSFERING;
						}
						flashBusy = 1;
						dataPacketsFLASHIndex++;
						if(dataPacketsFLASHIndex>=127){
							dataPacketsFLASHIndex = 0;
							dataPacketsFLASHRowCounter++;
							if(dataPacketsFLASHRowCounter >=10){
								dataPacketsFLASHRowCounter = 0;
							}
						}
					}
					
				}
			}
			//-------------Store packets in FLASH memory---------------//
			
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
						if(BLE_ERROR == BLE_SendData(&BLEHandle,dataPacket,129)){
							programStage = BLE_WAIT_CONN;
							startTIM1 = 1;						
#ifdef ECG_Du_v1_Board
							ECG_Stop_Acquisition(&ECGHandle);
#endif						
							EnterLowEnergyMODE();
						}
						
						dataPacket[127]=rem>>8; 	
						dataPacket[128]=rem;
						//----------Send Data packet---------------//
						
						//----------Store Data packet in internal memory-------//
						dataPacketsStorage[dataPacketsRowCounter][0]=packetNumber>>8;
						dataPacketsStorage[dataPacketsRowCounter][1]=packetNumber;
						for(i=2;i<131;i++){
							dataPacketsStorage[dataPacketsRowCounter][i] = dataPacket[i-2];
						}
						dataPacketsRowCounter++;
						if(dataPacketsRowCounter >=10){
							dataPacketsRowCounter = 0;
						}
						//----------Store Data packet in internal memory-------//
						
						
						if(BLEHandle.dataOKRespEnabled){								//not used				
							BLEHandle.dataOKWaiting=1;										
						}
						else {
							//-------If DNOK is received, repeat requested packet-------//
							if(BLEHandle.repeatDataPacket){
								for(i=0;i<10;i++){
									tempData16 = dataPacketsStorage[i][0];
									tempData16<<=8;
									tempData16 |= (dataPacketsStorage[i][1] & 0x00FF);
									if(tempData16 ==  BLEHandle.repeatDataPacketNumber){
										if(BLE_ERROR == BLE_SendData(&BLEHandle,&dataPacketsStorage[i][2],129)){
											programStage = BLE_WAIT_CONN;
											startTIM1 = 1;						
#ifdef ECG_Du_v1_Board
											ECG_Stop_Acquisition(&ECGHandle);
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
						ECGHandle.ecgStatus = ECG_ERROR;
						ECGHandle.ErrorNumber = ECG_FIFO_ERROR;
						startTIM1 = 1;
						EnterLowEnergyMODE();
						//BLE_SendData(&BLEHandle,errorPacket,129);  //transmit error message
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
		EnterStandByMODE();
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
