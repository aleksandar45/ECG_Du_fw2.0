#include "Log.h"

#ifdef DEBUG_MODE
void Log_WriteDataFlash(Log_TypeDef  *LogHandle){
	uint32_t i;
	uint32_t Address = 0;
	uint64_t data64;
	
	FLASH_Erase(FLASH_DBG_START_ADR, FLASH_DBG_END_ADR);
	HAL_FLASH_Unlock();
	
	Address = FLASH_DBG_START_ADR;
	
	data64 = LogHandle->dataToLogWriteIndex;
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Address, data64);
	Address = Address + 8;

	data64 = 0;
	for(i=0;i<LOG_FLASH_SIZE;i++){
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
void Log_ReadDataFlash(Log_TypeDef  *LogHandle){
	uint32_t Address = 0;
	Address = FLASH_DBG_START_ADR;
	__IO uint32_t data32, temp32 = 0;
	uint32_t i=0;
	
	data32 = *(__IO uint32_t *) Address;
	Address = Address + 4;
	temp32 = *(__IO uint32_t *) Address;
	Address = Address + 4;	
	if(data32 > LOG_FLASH_SIZE) {
		LogHandle->loggedDataAvailable = 0;
		return;
	}
	else{
		LogHandle->loggedDataAvailable = data32;
	}
	
	while (Address < FLASH_DBG_END_ADR)
  {
		data32 = *(__IO uint32_t *)Address;
		Address = Address + 4;
		temp32 = *(__IO uint32_t *)Address;
		Address = Address + 4;
		LogHandle->loggedData[i] = temp32>>24;
		LogHandle->loggedData[i+1] = temp32>>16;
		LogHandle->loggedData[i+2] = temp32>>8;
		LogHandle->loggedData[i+3] = temp32 & 0xFF;
    LogHandle->loggedData[i+4] = data32>>24;
		LogHandle->loggedData[i+5] = data32>>16;
		LogHandle->loggedData[i+6] = data32>>8;
		LogHandle->loggedData[i+7] = data32 & 0xFF;
		i=i+8;
	}
}

void Log_WriteData(Log_TypeDef* LogHandle, char* string){
	uint8_t size = 0;
	while(1){
		if(string[size]!=0){
			if(LogHandle->dataToLogWriteIndex<LOG_FLASH_SIZE){
				LogHandle->dataToLog[LogHandle->dataToLogWriteIndex++] = string[size++];
			}			
		}
		else{
			break;
		}
		if(size>=20) break;
	}
}
uint8_t Log_ReadData(Log_TypeDef* LogHandle, uint8_t* dest){
	uint8_t i;
	int16_t size = LogHandle->loggedDataAvailable;
	if(size >0){
		for(i=0;i<20;i++){
			if(size>0)
				dest[i] = LogHandle->loggedData[LogHandle->loggedDataReadIndex++];
			else
				dest[i] = 'x';
			size--;
		}
		if(size<=0){
			LogHandle->loggedDataAvailable = 0;
		}
		else{
			LogHandle->loggedDataAvailable-=20;
		}
		return 1;
	}	
	return 0;
}
#else
__weak void Log_WriteDataFlash(Log_TypeDef  *LogHandle){
  UNUSED(LogHandle);
}
__weak void Log_ReadDataFlash(Log_TypeDef  *LogHandle){
	UNUSED(LogHandle);
}
__weak void Log_WriteData(Log_TypeDef* LogHandle, char* string){
	UNUSED(LogHandle);
	UNUSED(string);	
}
__weak uint8_t Log_ReadData(Log_TypeDef* LogHandle, uint8_t* dest){
	UNUSED(LogHandle);
	UNUSED(dest);	
	return 0;
}
#endif
