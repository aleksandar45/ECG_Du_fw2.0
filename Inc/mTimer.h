#ifndef __MTIMER_H
#define __MTIMER_H

// Includes ------------------------------------------------------------------//
#include "stm32l4xx_hal.h"
#include "BLE_RN4871.h"
#include "ECG_ADS1294.h"
#include "main.h"

// Types

typedef struct
{	
  uint16_t period;        //Specifies timer period in ms. This parameter can be any value in range 10ms-1000ms 
	
	uint32_t timer1_counter;			//Specifies timer1 timeout value.
	uint32_t timer1_timeout;			//Internal timer1 counter.
	uint8_t timer1_enabled;				//Timer1 enabled or disabled. This parameter can be 0 or 1.
	uint8_t timer1_timeoutFlag;		//Timer1 status flag. This parameter can be 0 or 1.
	
	//uint8_t lblinkError_enabled;						//Led blink error enabled or disabled. This parameter can be 0 or 1.
	uint16_t lblinkError_counter_state13;  	//Internal led blink error counter for states 1 and 3.
	uint8_t lblinkError_state;  						//Specifies led blink error states. This parameter can be 0,1,2 or 3.
	uint16_t lblinkError_timeout_state13;		//Specifies led blink error timeout for states 1 and 3.
	uint16_t lblinkError_onTimeout;					//Specifies timeout value when led is turned on in state 2.
	uint16_t lblinkError_onFlag;						//Internal flag which specifies whether led is turned on or off.
	uint16_t lblinkError_offTimeout;				//Specifies timeout value when led is turned off in state 2.
	uint16_t lblinkError_counter_state2;			//Internal counter variable which is incremented during led is turned on or off in state 2.
	uint8_t lblinkError_errNum;							//Specifies error number; number of led pules during state 2.
	uint8_t lblinkError_errCounter;					//Internal counter variable which counts up to errNum value.
	uint8_t lblinkError_repetitionNum;			//Specifies number of repetitions
	uint8_t lblinkError_repetitionCouter;		//Internal counter variable which counts up to repetitionNum
	
	uint16_t lblinkStatus_counter;					//Internal counter for on or off state.
	uint16_t lblinkStatus_onTimeout;				//Specifies timeout value which represents time during led diode is on.
	uint16_t lblinkStatus_onFlag;						//Internal flag which specifies whether led is turned on or off.
	uint16_t lblinkStatus_offTimeout;				//Specifies timeout value which represents time during led diode is off.
	uint8_t lblinkStatus_option;						//Specifies which led diode is blinking
	uint8_t lblinkStatus_enabled;						//Specifies whether led blink status timer is enabled.
	
}mTimerHandler_TypeDef;

void mTimer_Config(TIM_HandleTypeDef* timHandle ,mTimerHandler_TypeDef * mTim);

void mTimer_TIM1_Start(mTimerHandler_TypeDef * mTim, uint16_t timeout);

uint8_t mTimer_LBlinkError_Start(mTimerHandler_TypeDef * mTim, uint8_t errorNum, uint8_t repetitionNum);

void mTimer_LBlinkStatus_Start(mTimerHandler_TypeDef * mTim, uint16_t offTimeout, uint8_t transfering);
void mTimer_LBlinkStatus_Stop(mTimerHandler_TypeDef * mTim);

#endif
