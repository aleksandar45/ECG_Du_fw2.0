#include "mRTC.h"

void RTC_Config(RTC_HandleTypeDef *rtcHandle)
{
  /* Configure RTC */
  rtcHandle->Instance = RTC;
  /* Set the RTC time base to 1s */
  /* Configure RTC prescaler and RTC data registers as follow:
    - Hour Format = Format 24
    - Asynch Prediv = Value according to source clock
    - Synch Prediv = Value according to source clock
    - OutPut = Output Disable
    - OutPutPolarity = High Polarity
    - OutPutType = Open Drain */
  rtcHandle->Init.HourFormat = RTC_HOURFORMAT_24;
  rtcHandle->Init.AsynchPrediv = 0x7F;
  rtcHandle->Init.SynchPrediv = 0x00F9;
  rtcHandle->Init.OutPut = RTC_OUTPUT_DISABLE;
  rtcHandle->Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  rtcHandle->Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if(HAL_RTC_Init(rtcHandle) != HAL_OK)
  {
    // Initialization Error 
    //Error_Handler(); 
  }
}
