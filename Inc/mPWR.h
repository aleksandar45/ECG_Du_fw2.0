#ifndef __MPWR_H
#define __MPWR_H

// Includes ------------------------------------------------------------------//
#include "stm32l4xx_hal.h"
#include "mTimer.h"
#include "BLE_RN4871.h"
#include "ECG_ADS1294.h"

void SystemClock_Config(void);

void EnterLowEnergyMODE(void);
void EnterHighEnergyMODE(void);
void EnterStandByMODE(void);

// Private functions
void SystemClockHSI_Config(void);
void SystemClockMSI_Config(void);



#endif

