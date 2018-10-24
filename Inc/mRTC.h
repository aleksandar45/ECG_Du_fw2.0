#ifndef __MRTC_H
#define __MRTC_H

// Includes ------------------------------------------------------------------//
#include "stm32l4xx_hal.h"

// Functions ------------------------------------------------------------------//
void RTC_Config(RTC_HandleTypeDef *rtcHandle);
void RTC_Disable(RTC_HandleTypeDef *rtcHandle);

#endif
