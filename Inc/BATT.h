#ifndef __BATT_H
#define __BATT_H

// Includes ------------------------------------------------------------------//
#include "stm32l4xx_hal.h"
#include "mADC.h"
#include "main.h"

// Defines -------------------------------------------------------------------//
#define VDD_APPLIED                      			((uint32_t)2700)    // Value of analog voltage supply Vdda (unit: mV).
#define AD_RANGE_12BITS                   		((uint32_t)4095)    // Max value with a full range of 12 bits. 
#define	LOW_BATTERY_LEVEL_LOW_VOLTAGE					((uint32_t)3000)		// Low level of battery voltage in mV; lower value. Low and high values are introduced to make hysteresis.
#define LOW_BATTERY_LEVEL_HIGH_VOLTAGE				((uint32_t)3100)		// Low level of battery voltage in mV; higher value. Low and high values are introduced to make hysteresis.

#ifdef RN4871_Nucleo_Test_Board
	#define GAIN																								1			// Gain level of voltage divisor (1/VSCALE)
#else 
	#define GAIN																								2			// Gain level of voltage divisor (1/VSCALE)
#endif

// Types ---------------------------------------------------------------------//

typedef struct
{
			ADC_HandleTypeDef* adcHandle;					//ADC handle
	
			uint32_t adcValuesBuffer[10];					//Buffer for adc values storage
			uint8_t adcValuesCounter;							//Index of adcValuesBuffer
			uint32_t currentADCValue;							//Currently read adc value
	
			uint32_t currentBatteryVoltage;				//Current battery voltage value in mV		
			uint32_t *batteryDischargeMap;				//Aproximation of battery voltage vs percentage buffer. Expected 6 voltages; so each value represents 20% of discharge. Voltage values are in mV
			uint8_t	 lowLevelBattery;							//Flag that is enabled when low battery level is detected
	
			uint8_t  adcBufferFull;								//Flag that specifies if adcValuesBuffer is filled completely with adc values
			uint8_t  adcEnabled;									//Flag that specifies whether adc conversion is enabled or disabled	
__IO 	uint8_t  adcDataReady;								//Flag that specifies whether new adc data is read and stored in currentADCValue variable
__IO  uint8_t  preparingNewData;						//Flag that specifies if new data is being measured
	
}BATT_TypeDef;

void BATT_Init(ADC_HandleTypeDef* adcHandle, BATT_TypeDef* BATTHandle);
void BATT_StartMeasure(BATT_TypeDef* BATTHandle);
void BATT_ReadADCData(BATT_TypeDef* BATTHandle);
uint32_t BATT_CalculateVoltage(BATT_TypeDef* BATTHandle);
uint8_t BATT_CalculatePercentage(BATT_TypeDef* BATTHandle);
#endif

