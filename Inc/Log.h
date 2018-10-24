#ifndef __LOG_H
#define __LOG_H

// Includes ------------------------------------------------------------------//
#include "stm32l4xx_hal.h"
#include "mFLASHInt.h"

// Defines -------------------------------------------------------------------//  


// Types ---------------------------------------------------------------------//
typedef struct
{
	uint8_t dataToLog[2048];											//data to be logged in flash memory
	uint8_t loggedData[2048];											//logged data previously read from flash
	
}Log_TypeDef;

// Functions -----------------------------------------------------------------//
void Log_WriteData(Log_TypeDef  *LogHandle);
void Log_ReadData(Log_TypeDef  *LogHandle);

#endif
