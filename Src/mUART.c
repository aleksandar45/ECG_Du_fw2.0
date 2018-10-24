#include "mUART.h"

extern BLE_TypeDef	BLEHandle;
extern ProgramStageTypeDef programStage;

void mUART_Config(UART_HandleTypeDef *huart, uint32_t baudRate,uint32_t hwControl){
	
	huart->Instance        = USART2;

  huart->Init.BaudRate     = baudRate;
  huart->Init.WordLength   = UART_WORDLENGTH_8B;
  huart->Init.StopBits     = UART_STOPBITS_1;
  huart->Init.Parity       = UART_PARITY_NONE;
  huart->Init.HwFlowCtl    = hwControl;//UART_HWCONTROL_NONE;		//UART_HWCONTROL_RTS_CTS
  huart->Init.Mode         = UART_MODE_TX_RX;
  huart->Init.OverSampling = UART_OVERSAMPLING_16;
	
  if(HAL_UART_DeInit(huart) != HAL_OK)
  {
    //Error_Handler();
  }  		
  if(HAL_UART_Init(huart) != HAL_OK)
  {
    //Error_Handler();
  }
	
}


HAL_StatusTypeDef mUART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
	uint16_t i;
  /* Check that a Rx process is not already ongoing */
  if(huart->RxState == HAL_UART_STATE_READY)
  {
    if((pData == NULL ) || (Size == 0U))
    {
      return HAL_ERROR;
    }
		for(i=0;i<Size;i++){
			pData[i] = 0;
		}
    
    /* Process Locked */
    __HAL_LOCK(huart);
    
    huart->pRxBuffPtr  = pData;
    huart->RxXferSize  = Size;
    huart->RxXferCount = Size;
    huart->RxISR       = NULL;
    
    /* Computation of UART mask to apply to RDR register */
    UART_MASK_COMPUTATION(huart);
    
    huart->ErrorCode = HAL_UART_ERROR_NONE;
    huart->RxState = HAL_UART_STATE_BUSY_RX;
    
    /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
    SET_BIT(huart->Instance->CR3, USART_CR3_EIE);
    
#if defined(USART_CR1_FIFOEN)
    /* Configure Rx interrupt processing*/
    if ((huart->FifoMode == UART_FIFOMODE_ENABLE) && (Size >= huart->NbRxDataToProcess))
    {
      /* Set the Rx ISR function pointer according to the data word length */
      if ((huart->Init.WordLength == UART_WORDLENGTH_9B) && (huart->Init.Parity == UART_PARITY_NONE))
      {
        huart->RxISR = UART_RxISR_16BIT_FIFOEN;
      }
      else
      {
        huart->RxISR = UART_RxISR_8BIT_FIFOEN;
      }
      
      /* Process Unlocked */
      __HAL_UNLOCK(huart);
      
      /* Enable the UART Parity Error interrupt and RX FIFO Threshold interrupt */
      SET_BIT(huart->Instance->CR1, USART_CR1_PEIE);
      SET_BIT(huart->Instance->CR3, USART_CR3_RXFTIE);
    }
    else
#endif
    {
      /* Set the Rx ISR function pointer according to the data word length */
      if ((huart->Init.WordLength == UART_WORDLENGTH_9B) && (huart->Init.Parity == UART_PARITY_NONE))
      {
        //huart->RxISR = UART_RxISR_16BIT;
      }
      else
      {
        huart->RxISR = mUART_RxISR_8BIT;
      }
      
       /* Process Unlocked */
      __HAL_UNLOCK(huart);
      
     /* Enable the UART Parity Error interrupt and Data Register Not Empty interrupt */
#if defined(USART_CR1_FIFOEN)
      SET_BIT(huart->Instance->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE_RXFNEIE);
#else
      SET_BIT(huart->Instance->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE);
#endif
    }
    
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

static void mUART_RxISR_8BIT(UART_HandleTypeDef *huart)
{
  uint16_t uhMask = huart->Mask;
  uint16_t  uhdata;
	uint8_t receivedData;
  
  /* Check that a Rx process is ongoing */
  if(huart->RxState == HAL_UART_STATE_BUSY_RX)
  {
    uhdata = (uint16_t) READ_REG(huart->Instance->RDR);
		receivedData = (uint8_t)(uhdata & (uint8_t)uhMask);
    *huart->pRxBuffPtr++ = receivedData;
    BLEHandle.uartDataAvailable++;
		
		if(--huart->RxXferCount == 0){
			huart->pRxBuffPtr  = BLEHandle.uartReceiveBuffer;
			huart->RxXferSize  = 200;
			huart->RxXferCount = 200;
			BLEHandle.uartBufferForward ^=0x01;
			
		/*if((receivedData == '#') || (receivedData == 0x0A) || (--huart->RxXferCount == 0))
    {
			if(receivedData == '#')
				BLEHandle.statusMessageReceived = 1;
			else if(receivedData == 0x0A){
				BLEHandle.ackOrAppMessageReceived = 1;
			} 		
			*huart->pRxBuffPtr++ = 0;*/
			
 /*     // Disable the UART Parity Error Interrupt and RXNE interrupt//
#if defined(USART_CR1_FIFOEN)
      CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE_RXFNEIE | USART_CR1_PEIE));
#else
      CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
#endif
      
      // Disable the UART Error Interrupt: (Frame error, noise error, overrun error) //
      CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);
     
      // Rx process is completed, restore huart->RxState to Ready //
      huart->RxState = HAL_UART_STATE_READY;
      
      // Clear RxISR function pointer //
      huart->RxISR = NULL;
      
      HAL_UART_RxCpltCallback(huart);*/
    }
  }
  else
  {
    /* Clear RXNE interrupt flag */
    __HAL_UART_SEND_REQ(huart, UART_RXDATA_FLUSH_REQUEST);
  }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  // Set transmission flag: trasfer complete
	BLEHandle.transmitCMDCompleted = 1;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  // Set transmission flag: trasfer complete
	/*if(BLEHandle.ackOrAppMessageReceived){
		BLE_ParseAckOrAppMessage(&BLEHandle);
		BLEHandle.ackOrAppMessageReceived = 0;
	}
	else if(BLEHandle.statusMessageReceived){
		BLE_ParseStatusMessage(&BLEHandle);
		BLEHandle.statusMessageReceived = 0;
	}
	else {
		BLEHandle.bleStatus = BLE_ERROR;
	}*/
	UartHandle->RxState =HAL_UART_STATE_READY;
	/*if(programStage == BLE_WAIT_MLDP_AND_CONN_PARAMS){
		HAL_UART_Receive_IT(UartHandle,BLEHandle.uartReceiveBuffer,200);
	}
	else{*/
		mUART_Receive_IT(UartHandle,BLEHandle.uartReceiveBuffer,200);
	//}
	
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle)
{
	if(!BLEHandle.uartErrorExpected){
    BLEHandle.bleStatus = BLE_ERROR;
		BLEHandle.ErrorNumber = BLE_UART_ERROR;			
	}
}
