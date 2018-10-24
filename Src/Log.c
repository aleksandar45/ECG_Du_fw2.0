#include "Log.h"

void Log_WriteData(Log_TypeDef  *LogHandle){
	uint32_t i;
	uint32_t Address = 0;
	uint64_t data64;
	
	FLASH_Erase(FLASH_DBG_START_ADR, FLASH_DBG_END_ADR);
	HAL_FLASH_Unlock();
	
	Address = FLASH_DBG_START_ADR;
	data64 = 0;
	for(i=0;i<2048;i++){
		data64 <<=8;
		data64 |= LogHandle->dataToLog[i];
		
		if((i%8)==7){
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Address, data64);
			Address = Address + 8;
			data64 = 0;
		}
	}	
	HAL_FLASH_Lock();
}
void Log_ReadData(Log_TypeDef  *LogHandle){
	uint32_t Address = 0;
	Address = FLASH_DBG_START_ADR;
	__IO uint32_t data32 = 0;
	uint32_t i=0;
	
	while (Address < FLASH_DBG_END_ADR)
  {
		data32 = *(__IO uint32_t *)Address;
		LogHandle->loggedData[i] = data32>>24;
		LogHandle->loggedData[i+1] = data32>>16;
		LogHandle->loggedData[i+2] = data32>>8;
		LogHandle->loggedData[i+3] = data32 & 0xFF;
    Address = Address + 4;
		i=i+3;
	}
	
}