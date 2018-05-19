#include "mSPI.h"

void mSPI_Config(SPI_HandleTypeDef *SpiHandle){
	
	SpiHandle->Instance               = SPI1;
	SpiHandle->Init.CLKPhase          = SPI_PHASE_2EDGE;
	SpiHandle->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
 
  SpiHandle->Init.Direction         = SPI_DIRECTION_2LINES;
  SpiHandle->Init.CLKPolarity       = SPI_POLARITY_LOW;
  SpiHandle->Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
  SpiHandle->Init.CRCPolynomial     = 7;
  SpiHandle->Init.DataSize          = SPI_DATASIZE_8BIT;
  SpiHandle->Init.FirstBit          = SPI_FIRSTBIT_MSB;
  SpiHandle->Init.NSS               = SPI_NSS_SOFT;
	SpiHandle->Init.NSSPMode					= SPI_NSS_PULSE_DISABLE;
  SpiHandle->Init.TIMode            = SPI_TIMODE_DISABLE;
  SpiHandle->Init.Mode 						 = SPI_MODE_MASTER;
  if(HAL_SPI_Init(SpiHandle) != HAL_OK)
  {
    //Error_Handler();
  }
	
}

