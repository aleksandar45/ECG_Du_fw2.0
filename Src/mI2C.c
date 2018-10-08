#include "mI2C.h"


void I2C_Config(I2C_HandleTypeDef *I2CHandle, uint8_t I2Cnum){
	switch (I2Cnum){
		case 1: I2CHandle->Instance = I2C1;
						break;
		case 2: I2CHandle->Instance = I2C2;
						break;
		case 3: I2CHandle->Instance = I2C3;
						break;	
	}
	I2CHandle->Init.AddressingMode 		= I2C_ADDRESSINGMODE_7BIT;
  I2CHandle->Init.Timing						= 0x5075050B;
	I2CHandle->Init.DualAddressMode 	= I2C_DUALADDRESS_DISABLE;
	I2CHandle->Init.GeneralCallMode 	= I2C_GENERALCALL_DISABLE;
	I2CHandle->Init.NoStretchMode 		= I2C_NOSTRETCH_DISABLE;
	I2CHandle->Init.OwnAddress1 			= 0x68;
	I2CHandle->Init.OwnAddress2 			= 0xFF;
	if(HAL_I2C_Init(I2CHandle) != HAL_OK)
  {
//    Error_Handler();
  }
	
}
