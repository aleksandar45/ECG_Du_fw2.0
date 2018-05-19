#include "mFlashInt.h"

uint32_t StartAddress;
uint32_t EndAddress;

void FLASH_Erase(uint32_t StartAddr, uint32_t EndAddr){
	uint32_t FirstPage = 0, NbOfPages = 0;
	uint32_t PAGEError = 0;
	__IO uint32_t data32 = 0 , MemoryProgramStatus = 0;
	FLASH_EraseInitTypeDef EraseInitStruct;
	
	HAL_FLASH_Unlock();
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);	
	
	FirstPage = GetPage(StartAddress);
	NbOfPages = GetPage(EndAddress) - FirstPage + 1;
	
	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.Banks       = FLASH_BANK_1;
  EraseInitStruct.Page        = FirstPage;
  EraseInitStruct.NbPages     = NbOfPages;
  if(HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
  { 
    /*
      FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError();
    */
    //Error_Handler();
  }
	HAL_FLASH_Lock();
}

void FLASH_Program32(uint32_t offsetAddress,uint64_t data){
	
	uint32_t Address = StartAddress + offsetAddress;

	HAL_FLASH_Unlock();
  if (Address < EndAddress)
  {
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Address, data) == HAL_OK)
    {
      Address = Address + 8;
    }
    else
    { 
      /* Error occurred while writing data in Flash memory. 
         User can add here some code to deal with this error */
      /*
        FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError();
      */
      Error_Handler();
    }
  }
	
	HAL_FLASH_Lock();
}
uint32_t FLASH_Read32(uint32_t offsetAddress){
	__IO uint32_t data32;
	data32 = *(__IO uint32_t*)(StartAddress + offsetAddress);
	return data32;
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
