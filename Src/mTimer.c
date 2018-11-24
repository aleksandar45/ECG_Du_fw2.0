#include "mTimer.h"

extern mTimerHandler_TypeDef  mTimHandle;
extern BLE_TypeDef	BLEHandle;
extern ECG_TypeDef ECGHandle;
extern BATT_TypeDef BATTHandle;
extern GYACC_TypeDef GYACCHandle;
extern Log_TypeDef  LogHandle;

#ifdef MCU_TEST_DATA
extern int32_t	testCounter;
extern uint8_t testData[150];
extern ProgramStageTypeDef programStage;
#endif

void mTimer_Config(TIM_HandleTypeDef* timHandle, mTimerHandler_TypeDef * mTim){
	TIM_MasterConfigTypeDef sMasterConfig;
	
	mTim->period = 100;
	
	mTim->timer1_counter = 0;
	mTim->timer1_timeout = 0;
	mTim->timer1_enabled = 0;
	mTim->timer1_timeoutFlag = 0;
	
	mTim->lblinkError_counter_state13 = 0;  	
	mTim->lblinkError_state = 0;  						
	mTim->lblinkError_timeout_state13 = 2000;		//2s for state 1 and 3
	mTim->lblinkError_onTimeout = 200;					//200ms time for led on				
	mTim->lblinkError_onFlag = 0;					
	mTim->lblinkError_offTimeout = 500;					//500ms time for led off	
	mTim->lblinkError_counter_state2 = 0;				
	mTim->lblinkError_errNum = 0;							
	mTim->lblinkError_errCounter = 0;					
	mTim->lblinkError_repetitionNum = 0;			
	mTim->lblinkError_repetitionCouter = 0;		
	
	mTim->lblinkStatus_counter = 0;				
	mTim->lblinkStatus_onTimeout = 200;					//200ms for led on		
	mTim->lblinkStatus_onFlag = 0;				
	mTim->lblinkStatus_offTimeout = 0;					//changable time for led off		
	mTim->lblinkStatus_option = 0;				
	mTim->lblinkStatus_enabled = 0;
	
	mTim->lblinkUSBCharge = 0;
	
  //	TIM3 input clock (TIM3CLK)  is set to APB1 clock (PCLK1), since APB1 prescaler is equal to 1.
  //    TIM3CLK = PCLK1
  //    PCLK1 = HCLK
	// 	Initialize TIM3 peripheral as follows:
	//		+ TIMER3 counter clock = 10kHz
	//		+ TIMER3 timeout period = 100ms
	
	timHandle->Instance = TIM3;
  timHandle->Init.Period            = 1000 - 1;
  timHandle->Init.Prescaler         = (uint32_t)(SystemCoreClock / 10000) - 1;;
  timHandle->Init.ClockDivision     = 0;
  timHandle->Init.CounterMode       = TIM_COUNTERMODE_UP;
  timHandle->Init.RepetitionCounter = 0;
  HAL_TIM_Base_Init(timHandle);
	
	HAL_GPIO_WritePin(LED_STATUS1_PORT,LED_STATUS1_PIN,GPIO_PIN_RESET);
	
#ifdef RN4871_Nucleo_Test_Board
	HAL_GPIO_WritePin(LED_STATUS2_PORT,LED_STATUS2_PIN,GPIO_PIN_RESET);
#else
	HAL_GPIO_WritePin(LED_STATUS2_PORT,LED_STATUS2_PIN,GPIO_PIN_SET);
#endif
	
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(timHandle, &sMasterConfig);
  
	//HAL_TIM_Base_Start(timHandle);
  HAL_TIM_Base_Start_IT(timHandle);	
}
void mTimer_Stop(TIM_HandleTypeDef* timHandle, mTimerHandler_TypeDef * mTim){
	HAL_TIM_Base_Start_IT(timHandle);
	HAL_TIM_Base_DeInit(timHandle);
}

void mTimer_TIM1_Start(mTimerHandler_TypeDef * mTim, uint32_t timeout){
	mTim->timer1_counter = 0;
	mTim->timer1_timeout = timeout;
	mTim->timer1_timeoutFlag = 0;
	mTim->timer1_enabled = 1;
}
uint8_t mTimer_LBlinkError_Start(mTimerHandler_TypeDef * mTim, uint8_t errorNum, uint8_t repetitionNum){
	char message [] = "mTimer/Err_Start_x_x";
	LogHandle.isError = 1;
	
	if(mTim->lblinkUSBCharge) return 0;
	if(mTim->lblinkError_state ==0)		//If led diode is not showing error message start new process.
	{
		
		message[17] = errorNum + 48;
		message[19] = repetitionNum + 48;
		Log_WriteData(&LogHandle, message);
		
		mTim->lblinkError_counter_state13 = 0;  	  										
		mTim->lblinkError_onFlag = 0;					
		mTim->lblinkError_counter_state2 = 0;				
		mTim->lblinkError_errNum = errorNum;							
		mTim->lblinkError_errCounter = 0;					
		mTim->lblinkError_repetitionNum = repetitionNum;			
		mTim->lblinkError_repetitionCouter = 0;	
		
		mTim->lblinkError_state = 1;
		return 1;
	}
	return 0;
}
void mTimer_LBlinkError_Stop(mTimerHandler_TypeDef * mTim){
	mTim->lblinkError_state =0;
	
#ifdef RN4871_Nucleo_Test_Board
	HAL_GPIO_WritePin(LED_ERROR_PORT,LED_ERROR_PIN,GPIO_PIN_RESET);
#else
	HAL_GPIO_WritePin(LED_ERROR_PORT,LED_ERROR_PIN,GPIO_PIN_SET);
#endif

}
void mTimer_LBlinkStatus_Start(mTimerHandler_TypeDef * mTim, uint16_t offTimeout, uint8_t ledDiodeOption){
	
	if(mTim->lblinkUSBCharge) return;
	mTim->lblinkStatus_counter = 0;										
	mTim->lblinkStatus_onFlag = 0;				
	mTim->lblinkStatus_offTimeout = offTimeout;	
	mTim->lblinkStatus_option = ledDiodeOption;		
	
	mTim->lblinkStatus_enabled = 1;
}
void mTimer_LBlinkStatus_Stop(mTimerHandler_TypeDef * mTim){
	mTim->lblinkStatus_enabled = 0;
	HAL_GPIO_WritePin(LED_STATUS1_PORT,LED_STATUS1_PIN,GPIO_PIN_RESET);

#ifdef RN4871_Nucleo_Test_Board
	HAL_GPIO_WritePin(LED_STATUS2_PORT,LED_STATUS2_PIN,GPIO_PIN_RESET);
#else
	HAL_GPIO_WritePin(LED_STATUS2_PORT,LED_STATUS2_PIN,GPIO_PIN_SET);
#endif

}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	
	//HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_6);
#ifdef MCU_TEST_DATA
	uint8_t i;
	if((programStage == BLE_ACQ_TRANSFERING) || (programStage == BLE_ACQ_TRANSFERING_AND_STORING)){
		//HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_SET);
		for(i=0;i<150;i+=3){
			testData[i]=testCounter>>16;
			testData[i+1]=testCounter>>8;
			testData[i+2]=testCounter;
			testCounter--;
		}
		if(testCounter>=45000){
			testCounter = 0;
		}
		if(ECG_ERROR!=ECG_WriteFIFOData(&ECGHandle,testData,1,150)){		//150
			//HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_RESET);
		}
		if(ECG_ERROR!=ECG_WriteFIFOData(&ECGHandle,testData,2,150)){
			//HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_RESET);
		}
		if(ECG_ERROR!=ECG_WriteFIFOData(&ECGHandle,testData,3,150)){
			//HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_RESET);
		}
		
	}
#endif
#ifdef ACC_TEST
	GYACCHandle.pitchAngles[GYACCHandle.angleIndex] = 20;
	GYACCHandle.rollAngles[GYACCHandle.angleIndex++] = 50;
	if(GYACCHandle.angleIndex == 5){
		GYACCHandle.angleIndex = 0;
		GYACCHandle.newDataAvailable = 1;
	}
#endif	

	//=============ADC battery measure=================//
	if(BATTHandle.adcEnabled){
		if(BATTHandle.preparingNewData){
			BATTHandle.preparingNewData = 0;
		}
		else{
			if(BATTHandle.adcDataReady == 0){
				BATTHandle.currentADCValue = HAL_ADC_GetValue(BATTHandle.adcHandle);
				BATTHandle.adcDataReady = 1;
				BATTHandle.preparingNewData = 1;			
			}
		}
	}
	//=============ADC battery measure=================//
	
	//==============BLE module error occured==========//
	BLEHandle.CMDTimeoutCounter+=mTimHandle.period;
	if(BLEHandle.bleStatus == BLE_ERROR){		
		mTimer_LBlinkStatus_Stop(&mTimHandle);
		mTimer_LBlinkError_Start(&mTimHandle,BLEHandle.ErrorNumber,2);
	}
	/*if((BLEHandle.statusMessageReceived==1)&&(BLEHandle.checkStatusBusy==0)){
		BLE_ParseStatusMessage(&BLEHandle);
		BLEHandle.uartHandle->RxState =HAL_UART_STATE_READY;
		mUART_Receive_IT(BLEHandle.uartHandle,BLEHandle.uartReceiveBuffer,200);
		BLEHandle.statusMessageReceived = 0;
	}*/
	//==============BLE module error occured==========//
	
	//==============ECG module error occured==========//
	if(ECGHandle.ecgStatus == ECG_ERROR){	
		mTimer_LBlinkStatus_Stop(&mTimHandle);
		mTimer_LBlinkError_Start(&mTimHandle,ECGHandle.ErrorNumber,3);
	}
	//==============ECG module error occured==========//
	
	if(mTimHandle.timer1_enabled){
		mTimHandle.timer1_counter += mTimHandle.period;
		if(mTimHandle.timer1_counter >= mTimHandle.timer1_timeout){
			mTimHandle.timer1_timeoutFlag = 1;	//set timeout flag
			mTimHandle.timer1_enabled = 0;			//disable timer1
		}
	}
	if(mTimHandle.lblinkError_state != 0){
		if(mTimHandle.lblinkError_state == 2){
			mTimHandle.lblinkError_counter_state2 += mTimHandle.period;
			if(mTimHandle.lblinkError_onFlag){
				if(mTimHandle.lblinkError_counter_state2 >= mTimHandle.lblinkError_onTimeout){
					mTimHandle.lblinkError_onFlag = 0;
#ifdef RN4871_Nucleo_Test_Board
					HAL_GPIO_WritePin(LED_ERROR_PORT,LED_ERROR_PIN,GPIO_PIN_RESET);
#else
					HAL_GPIO_WritePin(LED_ERROR_PORT,LED_ERROR_PIN,GPIO_PIN_SET);
#endif
					
					mTimHandle.lblinkError_counter_state2 = 0;
				}
				
			}
			else {
				if(mTimHandle.lblinkError_counter_state2 >= mTimHandle.lblinkError_offTimeout){
					mTimHandle.lblinkError_onFlag = 1;
#ifdef RN4871_Nucleo_Test_Board
					HAL_GPIO_WritePin(LED_ERROR_PORT,LED_ERROR_PIN,GPIO_PIN_SET);
#else
					HAL_GPIO_WritePin(LED_ERROR_PORT,LED_ERROR_PIN,GPIO_PIN_RESET);
#endif
					
					mTimHandle.lblinkError_errCounter++;
					if(mTimHandle.lblinkError_errCounter>= (mTimHandle.lblinkError_errNum + 1)){
						mTimHandle.lblinkError_state = 3;
						
						mTimHandle.lblinkError_onFlag = 0;
						mTimHandle.lblinkError_errCounter = 0;
					}
					mTimHandle.lblinkError_counter_state2 = 0;
				}
			}
		}
		if((mTimHandle.lblinkError_state == 1) || (mTimHandle.lblinkError_state == 3)){
#ifdef RN4871_Nucleo_Test_Board
			HAL_GPIO_WritePin(LED_ERROR_PORT,LED_ERROR_PIN,GPIO_PIN_SET);
#else
			HAL_GPIO_WritePin(LED_ERROR_PORT,LED_ERROR_PIN,GPIO_PIN_RESET);
#endif
			mTimHandle.lblinkError_counter_state13 += mTimHandle.period;
			
			if(mTimHandle.lblinkError_counter_state13 >= mTimHandle.lblinkError_timeout_state13){
				if(mTimHandle.lblinkError_state == 1){
					mTimHandle.lblinkError_state = 2;			
#ifdef RN4871_Nucleo_Test_Board
					HAL_GPIO_WritePin(LED_ERROR_PORT,LED_ERROR_PIN,GPIO_PIN_RESET);
#else
					HAL_GPIO_WritePin(LED_ERROR_PORT,LED_ERROR_PIN,GPIO_PIN_SET);
#endif										
					
					mTimHandle.lblinkError_counter_state13 = 0;
				}
				if(mTimHandle.lblinkError_state == 3){
					mTimHandle.lblinkError_repetitionCouter ++;
					if(mTimHandle.lblinkError_repetitionCouter >= mTimHandle.lblinkError_repetitionNum){
						mTimHandle.lblinkError_state = 0;
#ifdef RN4871_Nucleo_Test_Board
						HAL_GPIO_WritePin(LED_ERROR_PORT,LED_ERROR_PIN,GPIO_PIN_RESET);
#else
						HAL_GPIO_WritePin(LED_ERROR_PORT,LED_ERROR_PIN,GPIO_PIN_SET);
#endif
						Error_Handler();
					}
					else {
						mTimHandle.lblinkError_state = 1;
					}
					
					mTimHandle.lblinkError_counter_state13 = 0;
				}
			}			
		}
	}
	
	if(mTimHandle.lblinkStatus_enabled){
		mTimHandle.lblinkStatus_counter += mTimHandle.period;
		if(mTimHandle.lblinkStatus_onFlag){
			if(mTimHandle.lblinkStatus_counter >= mTimHandle.lblinkStatus_onTimeout){
				switch (mTimHandle.lblinkStatus_option){
					case 0:
						HAL_GPIO_WritePin(LED_STATUS1_PORT,LED_STATUS1_PIN,GPIO_PIN_RESET);
						break;
					case 1:
						HAL_GPIO_WritePin(LED_STATUS1_PORT,LED_STATUS2_PIN,GPIO_PIN_RESET);
						break;
					case 2:
						HAL_GPIO_WritePin(LED_STATUS1_PORT,LED_STATUS1_PIN,GPIO_PIN_RESET);
						HAL_GPIO_WritePin(LED_STATUS1_PORT,LED_STATUS2_PIN,GPIO_PIN_RESET);
						break;
					default:
						HAL_GPIO_WritePin(LED_STATUS1_PORT,LED_STATUS1_PIN,GPIO_PIN_RESET);
						
				}								
				mTimHandle.lblinkStatus_counter = 0;
				mTimHandle.lblinkStatus_onFlag = 0;
			}			
		}
		else{
			if(mTimHandle.lblinkStatus_counter >= mTimHandle.lblinkStatus_offTimeout){
				switch (mTimHandle.lblinkStatus_option){
					case 0:
						HAL_GPIO_WritePin(LED_STATUS1_PORT,LED_STATUS1_PIN,GPIO_PIN_SET);
						break;
					case 1:
						HAL_GPIO_WritePin(LED_STATUS1_PORT,LED_STATUS2_PIN,GPIO_PIN_SET);
						break;
					case 2:
						HAL_GPIO_WritePin(LED_STATUS1_PORT,LED_STATUS1_PIN,GPIO_PIN_SET);
						HAL_GPIO_WritePin(LED_STATUS1_PORT,LED_STATUS2_PIN,GPIO_PIN_SET);
						break;
					default:
						HAL_GPIO_WritePin(LED_STATUS1_PORT,LED_STATUS1_PIN,GPIO_PIN_SET);
						
				}		
				mTimHandle.lblinkStatus_counter = 0;
				mTimHandle.lblinkStatus_onFlag = 1;
			}
		}
	}

}
