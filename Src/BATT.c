#include "BATT.h"

uint32_t battDischargeVoltages[6] = {3880,3780,3690,3630,3510,3200};

void BATT_Init(ADC_HandleTypeDef* hadc, BATT_TypeDef* BATTHandle){
	
	BATTHandle->adcHandle = hadc;
	BATTHandle->adcValuesCounter = 0;
	BATTHandle->adcBufferFull = 0;
	BATTHandle->batteryDischargeMap = battDischargeVoltages;
	BATTHandle->lowLevelBattery = 0;
	
	mADC_Config(hadc);	
}

void BATT_StartMeasure(BATT_TypeDef* BATTHandle){
	//uint32_t tempData32;
  HAL_ADC_Start(BATTHandle->adcHandle);
	
	BATTHandle->adcDataReady = 0;
	BATTHandle->preparingNewData = 0;
  HAL_ADC_PollForConversion(BATTHandle->adcHandle,200);
	BATTHandle->adcEnabled = 1;
	//tempData32 = HAL_ADC_GetValue(BATTHandle->adcHandle);	
	
}

void BATT_ReadADCData(BATT_TypeDef* BATTHandle){
	BATTHandle->adcValuesBuffer[BATTHandle->adcValuesCounter++] = 	BATTHandle->currentADCValue;
	if(BATTHandle->adcValuesCounter >=10) {
		BATTHandle->adcBufferFull = 1;
		BATTHandle->adcValuesCounter = 0;
	}
}
uint32_t BATT_CalculateVoltage(BATT_TypeDef* BATTHandle){
	uint32_t tempData32=0;
	uint8_t i;
	for(i=0;i<10;i++){
		tempData32+=BATTHandle->adcValuesBuffer[i];
	}
	tempData32 = tempData32/10;
	
#ifdef RN4871_Nucleo_Test_Board
	#ifdef DEBUG_MODE
		return 3600;
	#endif
#endif
	return (tempData32 * VDD_APPLIED * GAIN)/AD_RANGE_12BITS;
	
}
uint8_t BATT_CalculatePercentage(BATT_TypeDef* BATTHandle){
	uint32_t tempData32=0;
	
	tempData32 = BATT_CalculateVoltage(BATTHandle);

	
	if(tempData32 >= BATTHandle->batteryDischargeMap[0]){		//Voltage value is over maximum expected battery voltage. Battery voltage is 100%
		return 100;
	}
	else if(tempData32 >= BATTHandle->batteryDischargeMap[1]){	//Battery voltage is between 80% and 100%
		return (80+(tempData32 - BATTHandle->batteryDischargeMap[1])*20/(BATTHandle->batteryDischargeMap[0]-BATTHandle->batteryDischargeMap[1]));		
	}
	else if(tempData32 >= BATTHandle->batteryDischargeMap[2]){	//Battery voltage is between 60% and 80%
		return (60+(tempData32 - BATTHandle->batteryDischargeMap[2])*20/(BATTHandle->batteryDischargeMap[1]-BATTHandle->batteryDischargeMap[2]));
	}
	else if(tempData32 >= BATTHandle->batteryDischargeMap[3]){	//Battery voltage is between 40% and 60%
		return (40+(tempData32 - BATTHandle->batteryDischargeMap[3])*20/(BATTHandle->batteryDischargeMap[2]-BATTHandle->batteryDischargeMap[3]));
	}
	else if(tempData32 >= BATTHandle->batteryDischargeMap[4]){	//Battery voltage is between 20% and 40%
		return (20+(tempData32 - BATTHandle->batteryDischargeMap[4])*20/(BATTHandle->batteryDischargeMap[3]-BATTHandle->batteryDischargeMap[4]));
	}
	else if(tempData32 > BATTHandle->batteryDischargeMap[5]){	//Battery voltage is between 0% and 20%
		tempData32 =  ((tempData32 - BATTHandle->batteryDischargeMap[5])*20/(BATTHandle->batteryDischargeMap[4]-BATTHandle->batteryDischargeMap[5]));
		if(tempData32 == 0) return 1;
		return tempData32;
	}
	else {
		return 1;			//Battery voltage is 0%. Returned 1% to be sure that battery level is sent over BLE correctly.
	}
}
