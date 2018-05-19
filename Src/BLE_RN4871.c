#include "BLE_RN4871.h"
char advertisingNameCMD[30] = "IA,09,524E2D";

void BLE_Init(UART_HandleTypeDef* uartHandle, BLE_TypeDef* BLEHandle){
	uint16_t i;
	
	BLEHandle->uartHandle = uartHandle;
	BLEHandle->connectionStatus = DISCONNECTED_ADVERTISING;
	BLEHandle->bleStatus = BLE_OK;
	BLEHandle->uartErrorExpected = 1;
	BLEHandle->uartBufferForward = 1;
	BLEHandle->uartDataAvailable = 0;
	BLEHandle->uartErrorRecover = 0;
	BLEHandle->uartBufferReadPointer = BLEHandle->uartReceiveBuffer;
#ifdef BLE_DATA_DOK_RESPONSE
	BLEHandle->dataOKRespEnabled = 1;
#else
	BLEHandle->dataOKRespEnabled = 0;
#endif
	
	
	//=================Check BLE UART status============//
	uartHandle->RxState =HAL_UART_STATE_READY;
	mUART_Config(uartHandle,460800,UART_HWCONTROL_CTS);//UART_HWCONTROL_CTS
	mUART_Receive_IT(uartHandle,BLEHandle->uartReceiveBuffer,200);	
	//HAL_UART_Receive_DMA(uartHandle, BLEHandle->uartReceiveBuffer,200);

	HAL_GPIO_WritePin(BT_RESET_PORT,BT_RESET_PIN,GPIO_PIN_RESET);				//Hardware Reset					
	HAL_Delay(1);
	HAL_GPIO_WritePin(BT_RESET_PORT,BT_RESET_PIN,GPIO_PIN_SET);
	HAL_Delay(100);
	
	if(compareUartMessage(BLEHandle,"%REBOOT",7,SEEK_TO_END)==0){
	//if(mByteCmp(BLEHandle->uartReceiveBuffer,"%REBOOT",7)==0){					//check BLE UART with BaudRate = 115200 and hardware control off
		BLEHandle->uartBaudRate = 460800;
		BLEHandle->uartHWControl = UART_HWCONTROL_CTS;
		
	}
	else{						
			BLEHandle->uartBufferForward = 1;
			BLEHandle->uartDataAvailable = 0;
			BLEHandle->uartBufferReadPointer = BLEHandle->uartReceiveBuffer;
			uartHandle->RxState =HAL_UART_STATE_READY;
			mUART_Config(uartHandle,115200,UART_HWCONTROL_NONE);
			mUART_Receive_IT(uartHandle,BLEHandle->uartReceiveBuffer,200);
			
			HAL_GPIO_WritePin(BT_RESET_PORT,BT_RESET_PIN,GPIO_PIN_RESET);				//Hardware Reset					
			HAL_Delay(1);
			HAL_GPIO_WritePin(BT_RESET_PORT,BT_RESET_PIN,GPIO_PIN_SET);
			HAL_Delay(500);
			
			if(compareUartMessage(BLEHandle,"%REBOOT",7,SEEK_TO_END)==0){				//check BLE UART with BaudRate = 460800 and hardware control	off
				BLEHandle->uartBaudRate = 115200;
				BLEHandle->uartHWControl = UART_HWCONTROL_NONE;
			}
			else{
				BLEHandle->bleStatus = BLE_ERROR;
				BLEHandle->ErrorNumber = BLE_REBOOT_ERROR;
				return;			
				
			}
	}
	BLEHandle->uartErrorExpected = 0;																					//When checking BLE UART with wrong speed BLE_ERROR is always generated. 
	//=================Check BLE UART status============//
	
	//=================Initialize BLE module with necessary parameters============//	
	BLE_EnterCMDMode(BLEHandle, NO_WAIT_CMD_RESP);	
	HAL_Delay(20);
	if(compareUartMessage(BLEHandle,"CMD",3,SEEK_TO_END)!=0){
		uartHandle->RxState =HAL_UART_STATE_READY;
		mUART_Config(uartHandle,BLEHandle->uartBaudRate,UART_HWCONTROL_NONE);
		mUART_Receive_IT(uartHandle,BLEHandle->uartReceiveBuffer,200);			
	}

#ifdef UART_BAUD_RATE_460800
	BLE_SendCMD(BLEHandle,"K,1",NO_WAIT_CMD_RESP);					//Klll connection if exists
	HAL_Delay(60);
	BLE_SendCMD(BLEHandle,"V",NO_WAIT_CMD_RESP);						//Klll connection if exists
	HAL_Delay(60);
	BLE_SendCMD(BLEHandle,"NB,09,524E42",NO_WAIT_CMD_RESP);		//Set non-connectable beacon advertisement payload (Complete local name = "RNB")
	HAL_Delay(60);
	BLE_SendCMD(BLEHandle,"NA,Z",NO_WAIT_CMD_RESP);						//Clear advertisement payload
	HAL_Delay(60);
	BLE_SendCMD(BLEHandle,"SC,0",NO_WAIT_CMD_RESP);						//Enable connectable advertisement
	HAL_Delay(60);
	BLE_SendCMD(BLEHandle,"S%,%,#",NO_WAIT_CMD_RESP);					//Set pre and post delimiter
	HAL_Delay(60);
	BLE_SendCMD(BLEHandle,"SB,01",NO_WAIT_CMD_RESP);					//Set baud rate to 460800
	HAL_Delay(60);
	BLE_SendCMD(BLEHandle,"SR,C000",NO_WAIT_CMD_RESP);				//Set UartFlowControl (mask 0x8000), Set No Prompt (0x4000)
	HAL_Delay(60);
	
	compareUartMessage(BLEHandle,"CMD",3,SEEK_TO_END);				//dummy clear
	
	BLE_SendCMD(BLEHandle,"D",NO_WAIT_CMD_RESP);							//Take BLE module information	
	HAL_Delay(150);
	
	if(compareUartMessage(BLEHandle,"BTA=",4,NO_SEEK_TO_END)==0){
		for(i=0;i<12;i++){
			BLEHandle->macAddressString[i] = *BLEHandle->uartBufferReadPointer;
			BLEHandle->uartBufferReadPointer++;
			if(BLEHandle->uartBufferReadPointer>= (BLEHandle->uartReceiveBuffer + 200)){
				BLEHandle->uartBufferReadPointer = BLEHandle->uartReceiveBuffer;
			}
		}
	}
	else{
		BLEHandle->macAddressString[0]=0;
		strcpy(BLEHandle->macAddressString,"D0D0D0D0D0D0");
	}
	compareUartMessage(BLEHandle,"Name",4,SEEK_TO_END);				//dummy clear
	
	BLEHandle->uartBaudRate = 460800;
	BLEHandle->uartHWControl = UART_HWCONTROL_CTS;
#else
	BLE_SendCMD(BLEHandle,"K,1",NO_WAIT_CMD_RESP);						//Klll connection if exists
	HAL_Delay(60);
	BLE_SendCMD(BLEHandle,"NB,09,524E42",NO_WAIT_CMD_RESP);		//Set non-connectable beacon advertisement payload (Complete local name = "RNB")
	HAL_Delay(60);
	BLE_SendCMD(BLEHandle,"NA,Z",NO_WAIT_CMD_RESP);						//Clear advertisement payload
	HAL_Delay(60);
	BLE_SendCMD(BLEHandle,"SC,0",NO_WAIT_CMD_RESP);						//Enable connectable advertisement
	HAL_Delay(60);
	BLE_SendCMD(BLEHandle,"S%,%,#",NO_WAIT_CMD_RESP);					//Set pre and post delimiter
	HAL_Delay(60);
	BLE_SendCMD(BLEHandle,"SB,03",NO_WAIT_CMD_RESP);					//Set baud rate to 115200
	HAL_Delay(60);
	BLE_SendCMD(BLEHandle,"SR,4000",NO_WAIT_CMD_RESP);				//Reset UartFlowControl (mask 0x8000), Set No Prompt (0x4000)
	HAL_Delay(60);

	compareUartMessage(BLEHandle,"CMD",3,SEEK_TO_END);				//dummy clear
		
	BLE_SendCMD(BLEHandle,"D",NO_WAIT_CMD_RESP);							//Take BLE module information
	HAL_Delay(150);
	
	if(compareUartMessage(BLEHandle,"BTA=",4,NO_SEEK_TO_END)==0){
		for(i=0;i<12;i++){
			BLEHandle->macAddressString[i] = *BLEHandle->uartBufferReadPointer;
			BLEHandle->uartBufferReadPointer++;
			if(BLEHandle->uartBufferReadPointer>= (BLEHandle->uartReceiveBuffer + 200)){
				BLEHandle->uartBufferReadPointer = BLEHandle->uartReceiveBuffer;
			}
		}
	}
	else{
		BLEHandle->macAddressString[0]=0;
		strcpy(BLEHandle->macAddressString,"D0D0D0D0D0D0");
	}
	compareUartMessage(BLEHandle,"Name",4,SEEK_TO_END);				//dummy clear
	
	BLEHandle->uartBaudRate = 115200;
	BLEHandle->uartHWControl = UART_HWCONTROL_NONE;
#endif
	
	HAL_GPIO_WritePin(BT_RESET_PORT,BT_RESET_PIN,GPIO_PIN_RESET);				//Hardware Reset					
	HAL_Delay(1);
	HAL_GPIO_WritePin(BT_RESET_PORT,BT_RESET_PIN,GPIO_PIN_SET);
	HAL_Delay(100);
	
	if(compareUartMessage(BLEHandle,"%REBOOT#",8,SEEK_TO_END)!=0){			
		BLEHandle->bleStatus = BLE_ERROR;
		BLEHandle->ErrorNumber = BLE_REBOOT_ERROR;
		return;
	}
	
	BLEHandle->statusMessage.messageUpdated =0;
	//=================Initialize BLE module with necessary parameters============//
	
	
	//=================Initialize BLE module============//
	BLE_EnterCMDMode(BLEHandle, WAIT_CMD_RESP);
	
	//--------------SR cmd - Set supported feature------------//
		//0x8000 Enable Flow Control
		//0x4000 No Prompt
		//0x2000 Fast Mode
		//0x1000 No Beacon Scan
		//0x0800 No Connect Scan
		//0x0400 No Duplicate Scan Result Filter
		//0x0200 Passive Scan
		//0x0100 UART Transparent without ACK
		//0x0080 Reboot after Disconnection
		//0x0040 Running Script after Power On
		//0x0020 Support RN4020MLDP streaming service
		//0x0008 Command Mode Guard
		//-------------------------------------------------------//
	if(BLEHandle->uartHWControl== UART_HWCONTROL_CTS){			//HW control		
		BLE_SendCMD(BLEHandle,"SR,C000",WAIT_CMD_RESP);				
	}
	else{																												//No HW control
		BLE_SendCMD(BLEHandle,"SR,4000",WAIT_CMD_RESP);
		
	}
	
	//---------------SS cmd - Set supported feature-----------//
	//0x80 Device Information
	//0x40 UART Transparent
	//0x20 Beacon
	//0x10 Reserved
	//--------------------------------------------------------//
	BLE_SendCMD(BLEHandle,"SS,E0",WAIT_CMD_RESP);
	BLE_SendCMD(BLEHandle,"SO,1",WAIT_CMD_RESP);
	//BLE_SendCMD(BLEHandle,"SGA,0",WAIT_CMD_RESP);						//Set advertisement power to highest
	BLE_SendCMD(BLEHandle,"SW,0A,07",WAIT_CMD_RESP);				//Route Status 1 to CFG1 pin (P1_2)
	BLE_SendCMD(BLEHandle,"SW,0B,08",WAIT_CMD_RESP);				//Route Status 2 to CFG2 pin (P1_3)
	
	//comment two lines when using RN4871 with wrong firmware
	BLE_SendCMD(BLEHandle,"SW,0C,04",WAIT_CMD_RESP);				//Route UART_RX_INDICATION to CFG1 pin (P1_2)
	BLE_SendCMD(BLEHandle,"SW,0D,00",WAIT_CMD_RESP);				//Route Status 2 to CFG2 pin (P1_3)	
	
	BLE_SendCMD(BLEHandle,"ST,0006,000C,0000,0200",WAIT_CMD_RESP);	//Set preferable connection parameters
	BLE_SendCMD(BLEHandle,"S-,RN",WAIT_CMD_RESP);						//Set serialized device name (RN_xxxx, where xxxx is last 2 bytes of MAC address)
	BLE_SendCMD(BLEHandle,"SDF,1.2",WAIT_CMD_RESP);					//Set device firmware revision
	BLE_SendCMD(BLEHandle,"SDH,v1.0",WAIT_CMD_RESP);				//Set device hardware revision
	BLE_SendCMD(BLEHandle,"SDM,RN4871",WAIT_CMD_RESP);			//Set device model name (BLE module name)
	BLE_SendCMD(BLEHandle,"SDN,Heart Beam",WAIT_CMD_RESP);	//Set device manufacturer name
	BLE_SendCMD(BLEHandle,"SDR,N/A",WAIT_CMD_RESP);					//Set device software revision name
	BLE_SendCMD(BLEHandle,"SDS,1000-06",WAIT_CMD_RESP);			//Set device serial number name
	
	HAL_GPIO_WritePin(BT_RESET_PORT,BT_RESET_PIN,GPIO_PIN_RESET);				//Hardware Reset					
	HAL_Delay(1);
	HAL_GPIO_WritePin(BT_RESET_PORT,BT_RESET_PIN,GPIO_PIN_SET);
	HAL_Delay(100);

	if(compareUartMessage(BLEHandle,"%REBOOT#",8,SEEK_TO_END)!=0){				
		BLEHandle->bleStatus = BLE_ERROR;
		BLEHandle->ErrorNumber = BLE_REBOOT_ERROR;
		return;
	}	
	
	BLEHandle->statusMessage.messageUpdated =0;
	//=================Initialize BLE module============//

	
	BLE_EnterCMDMode(BLEHandle, WAIT_CMD_RESP);
	BLE_SendCMD(BLEHandle,"IA,Z",WAIT_CMD_RESP);			
	BLE_SendCMD(BLEHandle,"A",WAIT_CMD_RESP);
	//BLE_SendCMD(BLEHandle,"JC",WAIT_CMD_RESP);
	//BLE_SendCMD(BLEHandle,"&,DF1234567890",WAIT_CMD_RESP);
	//BLE_SendCMD(BLEHandle,"&C",WAIT_CMD_RESP);
	BLE_SendCMD(BLEHandle,"IA,01,06",WAIT_CMD_RESP);
	string2hexString(advertisingNameCMD + 12,BLEHandle->macAddressString + 8);	
	BLE_SendCMD(BLEHandle,advertisingNameCMD,WAIT_CMD_RESP);	
	
	
}

RN4871_UARTStatusTypeDef BLE_EnterCMDMode(BLE_TypeDef* BLEHandle, BLE_CMDWaitRespTypeDef waitCMDResponse){
	uint8_t message[5] = {'$','$','$'};
	
	
	HAL_Delay(3);
	BLEHandle->transmitCMDCompleted = 0;
	BLEHandle->ackOrAppMessage.messageUpdated = 0;
	HAL_UART_Transmit_DMA(BLEHandle->uartHandle,message,3);
	HAL_Delay(3);
	
	
	BLEHandle->CMDTimeoutCounter = 0;	
	while(BLEHandle->transmitCMDCompleted ==0){
		if(BLEHandle->CMDTimeoutCounter >= 200){								//transmit timeout occured
			//BLEHandle->bleStatus = BLE_ERROR;
			return BLE_ERROR;
		}
	}
	if(waitCMDResponse == WAIT_CMD_RESP){
		BLEHandle->CMDTimeoutCounter = 0;
		while(BLEHandle->ackOrAppMessage.messageUpdated == 0){
			checkUartMessage(BLEHandle,NO_SEEK_TO_END);
			if(BLEHandle->CMDTimeoutCounter >= 300){			//receive timeout occured
				BLEHandle->bleStatus = BLE_ERROR;
				BLEHandle->ErrorNumber =  BLE_ENTER_CMD_MODE_ERROR;
				return BLE_ERROR;
			}
		}
		if(BLEHandle->ackOrAppMessage.message != CMD){
			BLEHandle->bleStatus = BLE_ERROR;
			BLEHandle->ErrorNumber =  BLE_ENTER_CMD_MODE_ERROR;
			return BLE_ERROR;
		}
		BLEHandle->ackOrAppMessage.messageUpdated = 0;
		
		BLEHandle->cmdMode = 1;
	}	
	return BLE_OK;
}
RN4871_UARTStatusTypeDef BLE_EnterLPMode(BLE_TypeDef* BLEHandle){
	if(HAL_GPIO_ReadPin(BT_CFG1_PORT,BT_CFG1_PIN) == GPIO_PIN_RESET){
		BLE_SendCMD(BLEHandle,"K,1",WAIT_CMD_RESP);													//Kill active BLE connection	
		if(BLE_ERROR != BLE_CheckStatusMessage(BLEHandle,5000)){
			if(BLEHandle->statusMessage.messageUpdated){
				if(BLEHandle->statusMessage.message == DISCONNECT){
					
				}
				BLEHandle->statusMessage.messageUpdated = 0;
			}
		}
		else{			
			BLE_SendCMD(BLEHandle,"IA,Z",NO_WAIT_CMD_RESP);
			HAL_Delay(60);		
			//BLE_SendCMD(BLEHandle,"&,DF1234567890",NO_WAIT_CMD_RESP);					//MAC
			//HAL_Delay(60);
			BLE_SendCMD(BLEHandle,"A,3E8",NO_WAIT_CMD_RESP);													//Start LP advertising after 30s timeout
			HAL_Delay(60);
			//BLE_SendCMD(BLEHandle,"O,0",WAIT_CMD_RESP);
			HAL_Delay(60);
			compareUartMessage(BLEHandle,"AOK",3,SEEK_TO_END);				//dummy clear
			
			return BLE_OK;
		}
	}
	
	BLE_SendCMD(BLEHandle,"IA,Z",WAIT_CMD_RESP);
	//BLE_SendCMD(BLEHandle,"&,DF1234567890",WAIT_CMD_RESP);								//MAC
	BLE_SendCMD(BLEHandle,"A,3E8",WAIT_CMD_RESP);															//Start LP advertising after 30s timeout
			
	//BLE_SendCMD(BLEHandle,"O,0",WAIT_CMD_RESP);
	if(BLEHandle->bleStatus == BLE_ERROR){
		BLE_Init(BLEHandle->uartHandle,BLEHandle);
		BLE_SendCMD(BLEHandle,"IA,Z",WAIT_CMD_RESP);
		//BLE_SendCMD(BLEHandle,"&,DF1234567890",WAIT_CMD_RESP);								//MAC
		BLE_SendCMD(BLEHandle,"A,3E8",WAIT_CMD_RESP);													//Start LP advertising after 30s timeout		
	}
	BLEHandle->connectionStatus = DISCONNECTED_LPADVERTISING;
	
	return BLE_OK;
}
RN4871_UARTStatusTypeDef BLE_ExitLPMode(BLE_TypeDef* BLEHandle){
	BLEHandle->bleStatus = BLE_OK;

	
	BLE_SendCMD(BLEHandle,"K,1",WAIT_CMD_RESP);												//Kill active BLE connection	
	if(BLEHandle->bleStatus == BLE_ERROR){
		BLEHandle->bleStatus = BLE_OK;																	//If peer device is not connected Err message will be received	
	}
	BLE_CheckStatusMessage(BLEHandle,2000);														//Wait for BLE module to disconnect
	if(BLEHandle->statusMessage.message == DISCONNECT){
		BLEHandle->connectionStatus = DISCONNECTED_ADVERTISING;
		BLE_SendCMD(BLEHandle,"Y",WAIT_CMD_RESP);												//Disable advertising	
	}		
	else{
		BLEHandle->bleStatus = BLE_ERROR;
		return BLE_OK;
	}	

	return BLE_OK;
}

RN4871_UARTStatusTypeDef BLE_SendCMD(BLE_TypeDef* BLEHandle, char* cmdString, BLE_CMDWaitRespTypeDef waitCMDResponse){
	char cr[1] = {0x0D};
	uint8_t i;
	
	for(i=0; i<200;i++){
		if(cmdString[i]==0) break;
		BLEHandle->uartTransmitBuffer[i] = cmdString[i];
	}
	BLEHandle->uartTransmitBuffer[i++]=cr[0];

/*	if(waitCMDResponse == WAIT_CMD_RESP){
		BLEHandle->responseDelimiter = 0x0A;
		BLEHandle->receiveCMDCompleted = 0;
		BLEHandle->uartHandle->RxState =HAL_UART_STATE_READY;
		mUART_Receive_IT(BLEHandle->uartHandle,BLEHandle->uartReceiveBuffer,200);
	}*/
	
	BLEHandle->transmitCMDCompleted = 0;
	BLEHandle->ackOrAppMessage.messageUpdated = 0;
	HAL_UART_Transmit_DMA(BLEHandle->uartHandle,BLEHandle->uartTransmitBuffer,i);
	
	BLEHandle->CMDTimeoutCounter = 0;
	while(BLEHandle->transmitCMDCompleted ==0){
		if(BLEHandle->CMDTimeoutCounter >= 200){								//transmit timeout occured
			//BLEHandle->bleStatus = BLE_ERROR;
			
			return BLE_ERROR;
		}
	}
	
	if(waitCMDResponse == WAIT_CMD_RESP){
		BLEHandle->CMDTimeoutCounter = 0;
		//while(BLEHandle->receiveCMDCompleted == 0){
		while(BLEHandle->ackOrAppMessage.messageUpdated == 0){
			checkUartMessage(BLEHandle,NO_SEEK_TO_END);
			if(BLEHandle->CMDTimeoutCounter >= 500){			//receive timeout occured	
				BLEHandle->bleStatus = BLE_ERROR;
				BLEHandle->ErrorNumber = BLE_SEND_CMD_ERROR;
				return BLE_ERROR;
			}
		}
		
		if(BLEHandle->ackOrAppMessage.message != AOK){
			BLEHandle->bleStatus = BLE_ERROR;
			BLEHandle->ErrorNumber = BLE_SEND_CMD_ERROR;
			return BLE_ERROR;
		}
		BLEHandle->ackOrAppMessage.messageUpdated = 0;
	}	
		
	return BLE_OK;
}
RN4871_UARTStatusTypeDef BLE_SendData(BLE_TypeDef* BLEHandle, uint8_t* dataBuffer, uint16_t size){
	uint8_t i;
	if(size>500) return BLE_ERROR;
	for(i=0; i<size;i++){
		BLEHandle->uartTransmitBuffer[i] = dataBuffer[i];
	}

	BLEHandle->transmitCMDCompleted =0;
	BLEHandle->CMDTimeoutCounter = 0;
	HAL_UART_Transmit_DMA(BLEHandle->uartHandle,BLEHandle->uartTransmitBuffer,size);
	
	while(BLEHandle->transmitCMDCompleted ==0){
		if(BLEHandle->CMDTimeoutCounter >= 300){
			
			BLEHandle->connectionStatus = DISCONNECTED_ADVERTISING;
			BLEHandle->bleStatus = BLE_OK;
			BLEHandle->uartErrorExpected = 1;
			BLEHandle->uartBufferForward = 1;
			BLEHandle->uartDataAvailable = 0;
			BLEHandle->uartErrorRecover = 0;
			BLEHandle->uartBufferReadPointer = BLEHandle->uartReceiveBuffer;
		
#ifdef BLE_DATA_DOK_RESPONSE
			BLEHandle->dataOKRespEnabled = 1;
#else
			BLEHandle->dataOKRespEnabled = 0;
#endif
			BLEHandle->uartHandle->RxState =HAL_UART_STATE_READY;
			mUART_Config(BLEHandle->uartHandle,BLEHandle->uartBaudRate,BLEHandle->uartHWControl);
			mUART_Receive_IT(BLEHandle->uartHandle,BLEHandle->uartReceiveBuffer,200);

			HAL_GPIO_WritePin(BT_RESET_PORT,BT_RESET_PIN,GPIO_PIN_RESET);				//Hardware Reset					
			HAL_Delay(1);
			HAL_GPIO_WritePin(BT_RESET_PORT,BT_RESET_PIN,GPIO_PIN_SET);
			HAL_Delay(100);
			
			if(compareUartMessage(BLEHandle,"%REBOOT",7,SEEK_TO_END)==0){
				BLEHandle->uartBaudRate = 460800;
				BLEHandle->uartHWControl = UART_HWCONTROL_CTS;
				
			}
			
			BLEHandle->statusMessage.messageUpdated =0;

			BLE_EnterCMDMode(BLEHandle, WAIT_CMD_RESP);
			BLE_SendCMD(BLEHandle,"IA,Z",WAIT_CMD_RESP);			
			BLE_SendCMD(BLEHandle,"A",WAIT_CMD_RESP);			
			BLE_SendCMD(BLEHandle,"IA,01,06",WAIT_CMD_RESP);
			string2hexString(advertisingNameCMD + 12,BLEHandle->macAddressString + 8);	
			BLE_SendCMD(BLEHandle,advertisingNameCMD,WAIT_CMD_RESP);
			
			return BLE_ERROR;
		}			
	}
	BLEHandle->transmitCMDCompleted =0;
		
	return BLE_OK;
}

RN4871_UARTStatusTypeDef BLE_CheckStatusMessage(BLE_TypeDef* BLEHandle, uint32_t timeout){
	
	BLEHandle->CMDTimeoutCounter = 0;
	//if(BLEHandle->statusMessage.messageUpdated) return BLE_OK;	
	while(!BLEHandle->statusMessage.messageUpdated){
		checkUartMessage(BLEHandle,NO_SEEK_TO_END);
		if(BLEHandle->CMDTimeoutCounter >= timeout) {
			BLEHandle->statusMessageReceived = 0;
			return BLE_ERROR;		
		}
	}
	return BLE_OK;
}
RN4871_UARTStatusTypeDef BLE_ParseStatusMessage(BLE_TypeDef* BLEHandle, uint8_t* startPointer, uint8_t size){
	uint8_t i;
	
	for(i=0;i<size;i++){
		BLEHandle->uartParseBuffer[i] = *startPointer;
		startPointer++;
		if(startPointer >= (BLEHandle->uartReceiveBuffer + 200)){
			startPointer = BLEHandle->uartReceiveBuffer;
		}
	}
	BLEHandle->uartParseBuffer[i] = 0;

	if(BLEHandle->uartParseBuffer[0] == '%'){
		if(BLEHandle->uartParseBuffer[1] == 'A'){
			if(mByteCmp(BLEHandle->uartParseBuffer,"%ADV_TIMEOUT#,",13)==0){
				BLEHandle->statusMessage.message = ADV_TIMEOUT;
				BLEHandle->statusMessage.numOfParams = 0;
				BLEHandle->statusMessage.messageUpdated = 1;
			}
			
		}
		else if(BLEHandle->uartParseBuffer[1] == 'C'){
			if(mByteCmp(BLEHandle->uartParseBuffer,"%CONN_PARAM,",12)==0){
				BLEHandle->statusMessage.message = CONN_PARAM;
				BLEHandle->statusMessage.numOfParams = 3;
				BLEHandle->statusMessage.param1 =  hex2int(BLEHandle->uartParseBuffer + 12, 4);
				BLEHandle->statusMessage.param2 =  hex2int(BLEHandle->uartParseBuffer + 17, 4);
				BLEHandle->statusMessage.param3 =  hex2int(BLEHandle->uartParseBuffer + 22, 4);
				BLEHandle->statusMessage.messageUpdated = 1;
			}
			else if(mByteCmp(BLEHandle->uartParseBuffer,"%CONNECT,",9)==0){
				BLEHandle->statusMessage.message = CONNECT;
				BLEHandle->statusMessage.numOfParams = 2;
				BLEHandle->statusMessage.param1 =  hex2int(BLEHandle->uartParseBuffer + 9, 1);
				BLEHandle->statusMessage.messageUpdated = 1;
				//BLEHandle->statusMessage.param2 =  hex2int(BLEHandle->uartReceiveBuffer + 14, 4);		//too long MAC address		
			}
		}
		else if(BLEHandle->uartParseBuffer[1] == 'D'){
			if(mByteCmp(BLEHandle->uartParseBuffer,"%DISCONNECT#",12)==0){
				BLEHandle->statusMessage.message = DISCONNECT;
				BLEHandle->statusMessage.numOfParams = 0;
				BLEHandle->statusMessage.messageUpdated = 1;
			}
			
		}
		else if(BLEHandle->uartParseBuffer[1] == 'R'){
			if(mByteCmp(BLEHandle->uartParseBuffer,"%REBOOT#",8)==0){
				BLEHandle->statusMessage.message = REBOOT;
				BLEHandle->statusMessage.numOfParams = 0;
				BLEHandle->statusMessage.messageUpdated = 1;
				
			}
		}
		else if(BLEHandle->uartParseBuffer[1] == 'S'){
			if(mByteCmp(BLEHandle->uartParseBuffer,"%STREAM_OPEN#",12)==0){
				BLEHandle->statusMessage.message = STREAM_OPEN;
				BLEHandle->statusMessage.numOfParams = 0;
				BLEHandle->statusMessage.messageUpdated = 1;
			}
		}
	}
	else{
		BLEHandle->bleStatus = BLE_ERROR;
		BLEHandle->ErrorNumber = BLE_STATUS_MSG_ERROR;
		return BLE_ERROR;
	}
	return BLE_OK;
	
}
RN4871_UARTStatusTypeDef BLE_ParseAckOrAppMessage(BLE_TypeDef* BLEHandle, uint8_t* startPointer, uint8_t size){
	uint8_t i;
	
	for(i=0;i<size;i++){
		BLEHandle->uartParseBuffer[i] = *startPointer;
		startPointer++;
		if(startPointer >= (BLEHandle->uartReceiveBuffer + 200)){
			startPointer = BLEHandle->uartReceiveBuffer;
		}
	}
	BLEHandle->uartParseBuffer[i] = 0;
	
	if(BLEHandle->uartParseBuffer[0] == 'A'){
		if(mByteCmp(BLEHandle->uartParseBuffer,"AOK\r\n",5)==0){
			BLEHandle->ackOrAppMessage.message = AOK;
			BLEHandle->ackOrAppMessage.messageUpdated = 1;
		}	
		else if(mByteCmp(BLEHandle->uartParseBuffer,"APP_DOK,",8)==0) {
				BLEHandle->ackOrAppMessage.message = APP_DOK;
				BLEHandle->ackOrAppMessage.param1 =  hex2int(BLEHandle->uartParseBuffer + 8, 4);				
				BLEHandle->ackOrAppMessage.messageUpdated = 1;
		}
		else if(mByteCmp(BLEHandle->uartParseBuffer,"APP_DNOK,",9)==0) {
				BLEHandle->ackOrAppMessage.message = APP_DNOK;
				BLEHandle->ackOrAppMessage.param1 =  hex2int(BLEHandle->uartParseBuffer + 9, 4);				
				BLEHandle->ackOrAppMessage.messageUpdated = 1;
		}
		else if(mByteCmp(BLEHandle->uartParseBuffer,"APP_START_ACQ\r\n",13)==0){
			BLEHandle->ackOrAppMessage.message = APP_START_ACQ;
			BLEHandle->ackOrAppMessage.messageUpdated = 1;
		}
		else if(mByteCmp(BLEHandle->uartParseBuffer,"APP_STOP_ACQ\r\n",12)==0){
			BLEHandle->ackOrAppMessage.message = APP_STOP_ACQ;
			BLEHandle->ackOrAppMessage.messageUpdated = 1;
		}
		else if(mByteCmp(BLEHandle->uartParseBuffer,"APP_OFF\r\n",7)==0){
			BLEHandle->ackOrAppMessage.message = APP_OFF;
			BLEHandle->ackOrAppMessage.messageUpdated = 1;
		}
		else if(mByteCmp(BLEHandle->uartParseBuffer,"APP_STAY\r\n",8)==0){
			BLEHandle->ackOrAppMessage.message = APP_STAY;
			BLEHandle->ackOrAppMessage.messageUpdated = 1;
		}
	}
	else if(BLEHandle->uartParseBuffer[0] == 'C'){
		if(mByteCmp(BLEHandle->uartParseBuffer,"CMD\r\n#,",5)==0){
			BLEHandle->ackOrAppMessage.message = CMD;
			BLEHandle->ackOrAppMessage.messageUpdated = 1;
		}	
	}
	else if(BLEHandle->uartParseBuffer[0] == 'E'){
		if(mByteCmp(BLEHandle->uartParseBuffer,"Err\r\n#,",5)==0){
			BLEHandle->ackOrAppMessage.message = ERR;
			BLEHandle->ackOrAppMessage.messageUpdated = 1;
		}	
	}
	else{
		BLEHandle->bleStatus = BLE_ERROR;
		BLEHandle->ErrorNumber = BLE_ACKAPP_MSG_ERROR;
		return BLE_ERROR;
	}
	return BLE_OK;
	
}
uint8_t compareUartMessage(BLE_TypeDef* BLEHandle, char* string, uint8_t size, BLE_UARTSeekTypeDef seekType){	
	uint8_t* pA = BLEHandle->uartBufferReadPointer;
	uint8_t origSize = size;
	char* pB = string;	
	
	if((BLEHandle->bleStatus == BLE_ERROR) && (BLEHandle->ErrorNumber == BLE_UART_ERROR) && (!BLEHandle->uartErrorRecover)){
		BLEHandle->uartBufferForward = 1;
		BLEHandle->uartDataAvailable = 0;
		BLEHandle->uartErrorRecover = 1;
		BLEHandle->uartBufferReadPointer = BLEHandle->uartReceiveBuffer;
		BLEHandle->uartHandle->RxState =HAL_UART_STATE_READY;
		mUART_Config(BLEHandle->uartHandle,BLEHandle->uartBaudRate,BLEHandle->uartHWControl);
		mUART_Receive_IT(BLEHandle->uartHandle,BLEHandle->uartReceiveBuffer,200);
	}
	
	if(BLEHandle->uartDataAvailable >= size){

		while(size!=0){
			if(*pA!=*pB){
				BLEHandle->uartBufferReadPointer = pA;
				BLEHandle->uartDataAvailable -= (origSize - size);
				if(seekType == SEEK_TO_END){
					BLEHandle->uartDataAvailable = 0;
					if(BLEHandle->uartHandle->pRxBuffPtr < BLEHandle->uartBufferReadPointer){
						BLEHandle->uartBufferForward ^=0x01;
					}
					BLEHandle->uartBufferReadPointer = BLEHandle->uartHandle->pRxBuffPtr;
				}
				return 1;
			}
			pA++;
			if(pA>=(BLEHandle->uartReceiveBuffer + 200)){
				pA = BLEHandle->uartReceiveBuffer;
				BLEHandle->uartBufferForward ^=0x01;
			}
			pB++;
			size--;
		}
		BLEHandle->uartBufferReadPointer = pA;
		BLEHandle->uartDataAvailable -= (origSize - size);
		if(seekType == SEEK_TO_END){
			BLEHandle->uartDataAvailable = 0;
			if(BLEHandle->uartHandle->pRxBuffPtr < BLEHandle->uartBufferReadPointer){
				BLEHandle->uartBufferForward ^=0x01;
			}
			BLEHandle->uartBufferReadPointer = BLEHandle->uartHandle->pRxBuffPtr;
		}
		return 0;
		
		
	}
	else{
		if(seekType == SEEK_TO_END){
			BLEHandle->uartDataAvailable = 0;
			if(BLEHandle->uartHandle->pRxBuffPtr < BLEHandle->uartBufferReadPointer){
				BLEHandle->uartBufferForward ^=0x01;
			}
			BLEHandle->uartBufferReadPointer = BLEHandle->uartHandle->pRxBuffPtr;
		}
		return 1;			//no comparable data
	}
	
}

uint8_t checkUartMessage(BLE_TypeDef* BLEHandle, BLE_UARTSeekTypeDef seekType){
	uint8_t* pA = BLEHandle->uartBufferReadPointer;
	uint8_t size = 0;
	if((BLEHandle->bleStatus == BLE_ERROR) && (BLEHandle->ErrorNumber == BLE_UART_ERROR) && (!BLEHandle->uartErrorRecover)){
		BLEHandle->uartBufferForward = 1;
		BLEHandle->uartDataAvailable = 0;
		BLEHandle->uartErrorRecover = 1;
		BLEHandle->uartBufferReadPointer = BLEHandle->uartReceiveBuffer;
		BLEHandle->uartHandle->RxState =HAL_UART_STATE_READY;
		mUART_Config(BLEHandle->uartHandle,BLEHandle->uartBaudRate,BLEHandle->uartHWControl);
		mUART_Receive_IT(BLEHandle->uartHandle,BLEHandle->uartReceiveBuffer,200);
	}
	
	if(BLEHandle->uartDataAvailable == 0) return 0;
	
	while(pA != BLEHandle->uartHandle->pRxBuffPtr){
		if(*pA == '#'){
			BLEHandle->statusMessageReceived = 1;			
		}
		else if(*pA == 0x0A){
			BLEHandle->ackOrAppMessageReceived = 1;			
		}
		pA++;
		size++;
		if(pA>=(BLEHandle->uartReceiveBuffer + 200)){
			pA = BLEHandle->uartReceiveBuffer;
			BLEHandle->uartBufferForward ^=0x01;
		}
		if(BLEHandle->statusMessageReceived){
			BLEHandle->statusMessageReceived = 0;
			if(!BLEHandle->statusMessage.messageUpdated){				
				BLE_ParseStatusMessage(BLEHandle, BLEHandle->uartBufferReadPointer, size);
				BLEHandle->uartBufferReadPointer = pA;
				BLEHandle->uartDataAvailable -= size;
			}	
			break;
		}
		else if(BLEHandle->ackOrAppMessageReceived){
			BLEHandle->ackOrAppMessageReceived = 0;
			if(!BLEHandle->ackOrAppMessage.messageUpdated){				
				BLE_ParseAckOrAppMessage(BLEHandle, BLEHandle->uartBufferReadPointer, size);
				BLEHandle->uartBufferReadPointer = pA;
				BLEHandle->uartDataAvailable -= size;
			}	
			break;
		}
		
	}
	if(seekType == SEEK_TO_END){
		BLEHandle->uartDataAvailable = 0;
		if(BLEHandle->uartHandle->pRxBuffPtr < BLEHandle->uartBufferReadPointer){
			BLEHandle->uartBufferForward ^=0x01;
		}
		BLEHandle->uartBufferReadPointer = BLEHandle->uartHandle->pRxBuffPtr;
	}
	
	return 1;
}

uint8_t mByteCmp(uint8_t* source, char* string, uint8_t size){
	uint8_t* pA = source;
	char* pB = string;
	while(size!=0){
		if(*pA!=*pB){
			return 1;
		}
		pA++;
		pB++;
		size--;
	}
	return 0;
}
void string2hexString(char* outputString, char* inputString){
	char* pO1 = outputString;
	char* pO2 = outputString;
	char* pI = inputString;
		
	while(*pI!=0){
		*pO2 = (*pI) >> 4;
		pO2++;
		*pO2 = (*pI) & 0x0F;
		pO2++;
		pI++;
	}
	*pO2 = 0;
	
	while(pO1<pO2){
		if(*pO1<10){
			*pO1+=48;
		}
		else{
			*pO1+=55;
		}
		pO1++;
	}
}
void value2DecimalString(char* outputString, uint8_t value){
	uint8_t a,b,c;
	a = value / 100;
	value = value % 100;
	b = value / 10;
	c = value % 10;	
	outputString[0] = a + 48;
	outputString[1] = b + 48;
	outputString[2] = c + 48;
	outputString[3] = 0x0D;
	outputString[4] = 0x0A;
	outputString[5] = 0x00;
}
uint32_t hex2int(uint8_t* charArray, uint8_t size){
	uint8_t i, character;
	uint32_t result = 0;
	for(i=0;i<size;i++){
		if(charArray[i]>57){
			character = charArray[i] - 55;
		}
		else{
			character = charArray[i] - 48;
		}
		result<<=4;
		result |= (character & 0x00FF) ; 
		
	}
	return result;
}

#pragma push
#pragma O0
void mDelay(uint32_t halfUsDelay){		
	uint32_t temp = 0;	
	while(temp<=halfUsDelay){
		temp++;
	}
}
#pragma pop



