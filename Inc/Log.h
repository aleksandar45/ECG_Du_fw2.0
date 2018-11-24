#ifndef __LOG_H
#define __LOG_H

// Includes ------------------------------------------------------------------//
#include "stm32l4xx_hal.h"
#include "mFLASHInt.h"
#include "main.h"

// Defines -------------------------------------------------------------------//  
#define LOG_FLASH_SIZE		2040


// Types ---------------------------------------------------------------------//
typedef struct
{
	uint8_t dataToLog[LOG_FLASH_SIZE];											//data to be logged in flash memory	
	uint8_t loggedData[LOG_FLASH_SIZE];											//logged data previously read from flash
	
	uint16_t dataToLogWriteIndex;							
	uint16_t loggedDataReadIndex;
	
	uint32_t loggedDataAvailable;
	
	uint8_t isError;
	
}Log_TypeDef;

// Functions -----------------------------------------------------------------//
void Log_WriteDataFlash(Log_TypeDef  *LogHandle);
void Log_ReadDataFlash(Log_TypeDef  *LogHandle);
void Log_WriteData(Log_TypeDef* LogHandle, char* string);
uint8_t Log_ReadData(Log_TypeDef* LogHandle, uint8_t* dest);

#endif
