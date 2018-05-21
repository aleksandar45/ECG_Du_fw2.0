#include "mFlashInt.h"

__IO uint8_t flashBusy = 0;

void FLASH_Erase(uint32_t StartAddr, uint32_t EndAddr){
	uint32_t FirstPage = 0, NbOfPages = 0;
	uint32_t PAGEError = 0;
	__IO uint32_t data32 = 0 , MemoryProgramStatus = 0;
	FLASH_EraseInitTypeDef EraseInitStruct;
	
	HAL_NVIC_DisableIRQ(FLASH_IRQn);
	HAL_FLASH_Unlock();	
	//__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);	
	
	FirstPage = GetPage(StartAddr);
	NbOfPages = GetPage(EndAddr) - FirstPage + 1;
	
	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.Banks       = FLASH_BANK_1;
  EraseInitStruct.Page        = FirstPage;
  EraseInitStruct.NbPages     = NbOfPages;

	uint32_t tickstart = HAL_GetTick();
    
  while(__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY)) 
  { 
    if(FLASH_TIMEOUT_VALUE != HAL_MAX_DELAY)
    {
      if((HAL_GetTick() - tickstart) >= FLASH_TIMEOUT_VALUE)
      {
        Error_Handler();
      }
    } 
  }
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
		
  if(HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
  { 
    
    //FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError();
   
    Error_Handler();
  }
	HAL_FLASH_Lock();
	HAL_NVIC_EnableIRQ(FLASH_IRQn);
}
uint32_t GetPage(uint32_t Addr){
  uint32_t page = 0;
  
  if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
  {
    // Bank 1 
    page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
  }
  else
  {
    // Bank 2 
    page = (Addr - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
  }
  
  return page;
}

void  HAL_FLASH_EndOfOperationCallback(uint32_t ReturnValue){
	flashBusy = 0;
}
