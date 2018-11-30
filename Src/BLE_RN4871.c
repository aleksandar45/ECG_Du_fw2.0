#include "BLE_RN4871.h"
char advertisingNameCMD[30] = "IA,09,524E2D";
char advertisingBattCMD[] = "IA,08,303030";
extern BLE_DFU_Typedef BLE_DFUHandle;
extern Log_TypeDef  LogHandle;
	
void BLE_Init(UART_HandleTypeDef* uartHandle, BLE_TypeDef* BLEHandle,BLE_DFU_Typedef* bleDfuHandle,uint8_t onlyUARTInit){
	uint16_t i;
	
	BLEHandle->uartHandle = uartHandle;
	BLEHandle->dfuHandle = bleDfuHandle;
	BLEHandle->connectionStatus = DISCONNECTED_ADVERTISING;
	BLEHandle->bleStatus = BLE_OK;
	BLEHandle->uartErrorExpected = 0;
	BLEHandle->uartBufferForward = 1;
	BLEHandle->uartDataAvailable = 0;
	BLEHandle->uartErrorRecover = 0;
	BLEHandle->uartBufferReadPointer = BLEHandle->uartReceiveBuffer;
	BLEHandle->cmdMode = 1;
#ifdef BLE_DATA_DOK_RESPONSE
	BLEHandle->dataOKRespEnabled = 1;
#else
	BLEHandle->dataOKRespEnabled = 0;
#endif	
	
	//=================Check BLE UART status============//	
	if(onlyUARTInit) {
		uartHandle->RxState =HAL_UART_STATE_READY;
		mUART_Config(uartHandle,460800,UART_HWCONTROL_CTS);//UART_HWCONTROL_CTS		
		mUART_Receive_IT(uartHandle,BLEHandle->uartReceiveBuffer,200);
		Log_WriteData(&LogHandle, "BLE/Init/Stage_0_0_x");	
		
		BLEHandle->uartBaudRate = 460800;
		BLEHandle->uartHWControl = UART_HWCONTROL_CTS;
		return;
	}
	//HAL_UART_Receive_DMA(uartHandle, BLEHandle->uartReceiveBuffer,200);

	BLEHandle->uartErrorExpected = 1;
	HAL_GPIO_WritePin(BT_RESET_PORT,BT_RESET_PIN,GPIO_PIN_RESET);				//Hardware Reset					
	HAL_Delay(1);
	HAL_GPIO_WritePin(BT_RESET_PORT,BT_RESET_PIN,GPIO_PIN_SET);
	HAL_Delay(50);	
	
	uartHandle->RxState =HAL_UART_STATE_READY;
	mUART_Config(uartHandle,460800,UART_HWCONTROL_CTS);//UART_HWCONTROL_CTS		
	mUART_Receive_IT(uartHandle,BLEHandle->uartReceiveBuffer,200);	
	Log_WriteData(&LogHandle, "BLE/Init/Stage_1_0_x");	
	HAL_Delay(450);
	
	if(compareUartMessage(BLEHandle,"%REBOOT",7,SEEK_TO_END)==0){
	//if(mByteCmp(BLEHandle->uartReceiveBuffer,"%REBOOT",7)==0){					//check BLE UART with BaudRate = 115200 and hardware control off
		BLEHandle->uartBaudRate = 460800;
		BLEHandle->uartHWControl = UART_HWCONTROL_CTS;
		Log_WriteData(&LogHandle, "BLE/Init/Stage_1_1_x");	
	}
	else{								
			
			BLEHandle->uartErrorExpected = 1;
			HAL_GPIO_WritePin(BT_RESET_PORT,BT_RESET_PIN,GPIO_PIN_RESET);				//Hardware Reset					
			HAL_Delay(1);
			HAL_GPIO_WritePin(BT_RESET_PORT,BT_RESET_PIN,GPIO_PIN_SET);
			HAL_Delay(50);
			BLEHandle->uartErrorExpected = 0;
		
			BLEHandle->uartBufferForward = 1;
			BLEHandle->uartDataAvailable = 0;
			BLEHandle->uartBufferReadPointer = BLEHandle->uartReceiveBuffer;
			uartHandle->RxState =HAL_UART_STATE_READY;
			mUART_Config(uartHandle,115200,UART_HWCONTROL_NONE);
			mUART_Receive_IT(uartHandle,BLEHandle->uartReceiveBuffer,200);			
			HAL_Delay(450);
			
			if(compareUartMessage(BLEHandle,"%REBOOT",7,SEEK_TO_END)==0){				//check BLE UART with BaudRate = 460800 and hardware control	off
				BLEHandle->uartBaudRate = 115200;
				BLEHandle->uartHWControl = UART_HWCONTROL_NONE;
				Log_WriteData(&LogHandle, "BLE/Init/Stage_1_2_x");	
			}
			else{
				BLEHandle->bleStatus = BLE_ERROR;
				BLEHandle->ErrorNumber = BLE_REBOOT_ERROR;
				Log_WriteData(&LogHandle, "BLE/Init/Stage_1_3_x");	
				return;			
				
			}
	}
	BLEHandle->uartErrorExpected = 0;																					//When checking BLE UART with wrong speed BLE_ERROR is always generated. 
	//=================Check BLE UART status============//
	
	//=================Initialize BLE module with necessary parameters============//	
	BLE_EnterCMDMode(BLEHandle, NO_WAIT_CMD_RESP,ERROR_IGNORE);	
	HAL_Delay(20);
	if(compareUartMessage(BLEHandle,"CMD",3,SEEK_TO_END)!=0){
		uartHandle->RxState =HAL_UART_STATE_READY;
		mUART_Config(uartHandle,BLEHandle->uartBaudRate,UART_HWCONTROL_NONE);
		mUART_Receive_IT(uartHandle,BLEHandle->uartReceiveBuffer,200);	
		Log_WriteData(&LogHandle, "BLE/Init/Stage_2_1_x");	
	}

#ifdef UART_BAUD_RATE_460800
	BLE_SendCMD(BLEHandle,"K,1",NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);					//Klll connection if exists
	HAL_Delay(60);
	BLE_SendCMD(BLEHandle,"V",NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);						//Klll connection if exists
	HAL_Delay(60);
	//BLE_SendCMD(BLEHandle,"NB,09,524E42",NO_WAIT_CMD_RESP);		//Set non-connectable beacon advertisement payload (Complete local name = "RNB")
	//HAL_Delay(60);
	BLE_SendCMD(BLEHandle,"NA,Z",NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);						//Clear advertisement payload
	HAL_Delay(60);
	//BLE_SendCMD(BLEHandle,"NA,01,06",NO_WAIT_CMD_RESP);				//Set advertisement payload (flag)
	//HAL_Delay(60);
	BLE_SendCMD(BLEHandle,"SC,0",NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);						//Enable connectable advertisement
	HAL_Delay(60);
	BLE_SendCMD(BLEHandle,"S%,%,#",NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);					//Set pre and post delimiter
	HAL_Delay(60);
	BLE_SendCMD(BLEHandle,"SB,01",NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);					//Set baud rate to 460800
	HAL_Delay(60);
	BLE_SendCMD(BLEHandle,"SR,C000",NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);				//Set UartFlowControl (mask 0x8000), Set No Prompt (0x4000)
	HAL_Delay(100);
	
	compareUartMessage(BLEHandle,"CMD",3,SEEK_TO_END);				//dummy clear
	
	BLE_SendCMD(BLEHandle,"D",NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);							//Take BLE module information	
	HAL_Delay(350);
	
	if(compareUartMessage(BLEHandle,"BTA=",4,NO_SEEK_TO_END)==0){
		Log_WriteData(&LogHandle, "BLE/Init/Stage_3_1_x");
		for(i=0;i<12;i++){
			BLEHandle->macAddressString[i] = *BLEHandle->uartBufferReadPointer;
			BLEHandle->uartBufferReadPointer++;
			if(BLEHandle->uartBufferReadPointer>= (BLEHandle->uartReceiveBuffer + 200)){
				BLEHandle->uartBufferReadPointer = BLEHandle->uartReceiveBuffer;
			}
		}
	}
	else{
		Log_WriteData(&LogHandle, "BLE/Init/Stage_3_2_x");
		BLEHandle->macAddressString[0]=0;
		strcpy(BLEHandle->macAddressString,"D0D0D0D0D0D0");
	}
	compareUartMessage(BLEHandle,"Name",4,SEEK_TO_END);				//dummy clear
	
	BLEHandle->uartBaudRate = 460800;
	BLEHandle->uartHWControl = UART_HWCONTROL_CTS;
#else
	BLE_SendCMD(BLEHandle,"K,1",NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);						//Klll connection if exists
	HAL_Delay(60);
	BLE_SendCMD(BLEHandle,"NB,09,524E42",NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);		//Set non-connectable beacon advertisement payload (Complete local name = "RNB")
	HAL_Delay(60);
	BLE_SendCMD(BLEHandle,"NA,Z",NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);						//Clear advertisement payload
	HAL_Delay(60);
	BLE_SendCMD(BLEHandle,"SC,0",NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);						//Enable connectable advertisement
	HAL_Delay(60);
	BLE_SendCMD(BLEHandle,"S%,%,#",NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);					//Set pre and post delimiter
	HAL_Delay(60);
	BLE_SendCMD(BLEHandle,"SB,03",NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);					//Set baud rate to 115200
	HAL_Delay(60);
	BLE_SendCMD(BLEHandle,"SR,4000",NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);				//Reset UartFlowControl (mask 0x8000), Set No Prompt (0x4000)
	HAL_Delay(60);

	compareUartMessage(BLEHandle,"CMD",3,SEEK_TO_END);				//dummy clear
		
	BLE_SendCMD(BLEHandle,"D",NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);							//Take BLE module information
	HAL_Delay(350);
	
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
	
	BLEHandle->uartErrorExpected = 1;
	HAL_GPIO_WritePin(BT_RESET_PORT,BT_RESET_PIN,GPIO_PIN_RESET);				//Hardware Reset	
	Log_WriteData(&LogHandle, "BLE/Init/Stage_4_0_0");
	HAL_Delay(1);
	HAL_GPIO_WritePin(BT_RESET_PORT,BT_RESET_PIN,GPIO_PIN_SET);
	HAL_Delay(50);
	BLEHandle->uartErrorExpected = 0;
	
	BLEHandle->uartBufferForward = 1;
	BLEHandle->uartDataAvailable = 0;
	BLEHandle->uartBufferReadPointer = BLEHandle->uartReceiveBuffer;
	uartHandle->RxState =HAL_UART_STATE_READY;
	mUART_Config(uartHandle,BLEHandle->uartBaudRate,BLEHandle->uartHWControl);
	mUART_Receive_IT(uartHandle,BLEHandle->uartReceiveBuffer,200);
	HAL_Delay(450);
	
	Log_WriteData(&LogHandle, "BLE/Init/Stage_4_0_1");

	if(compareUartMessage(BLEHandle,"%REBOOT#",8,SEEK_TO_END)!=0){			
		BLEHandle->bleStatus = BLE_ERROR;
		BLEHandle->ErrorNumber = BLE_REBOOT_ERROR;
		Log_WriteData(&LogHandle, "BLE/Init/Stage_4_1_x");
		return;
	}
	
	BLEHandle->statusMessage.messageUpdated =0;
	//=================Initialize BLE module with necessary parameters============//
	
	
	//=================Initialize BLE module============//
	BLE_EnterCMDMode(BLEHandle, WAIT_CMD_RESP,ERROR_IGNORE);
	HAL_Delay(20);	
	Log_WriteData(&LogHandle, "BLE/Init/Stage_4_0_2");
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
		BLE_SendCMD(BLEHandle,"SR,C000",WAIT_CMD_RESP,NO_ERROR_IGNORE);		
		Log_WriteData(&LogHandle, "BLE/Init/Stage_4_0_3");
	}
	else{																												//No HW control
		BLE_SendCMD(BLEHandle,"SR,4000",WAIT_CMD_RESP,NO_ERROR_IGNORE);
		Log_WriteData(&LogHandle, "BLE/Init/Stage_4_0_4");
		
	}
	
	//---------------SS cmd - Set supported feature-----------//
	//0x80 Device Information
	//0x40 UART Transparent
	//0x20 Beacon
	//0x10 Reserved
	//--------------------------------------------------------//
	BLE_SendCMD(BLEHandle,"SS,E0",WAIT_CMD_RESP,NO_ERROR_IGNORE);
	BLE_SendCMD(BLEHandle,"SO,1",WAIT_CMD_RESP,NO_ERROR_IGNORE);
	//BLE_SendCMD(BLEHandle,"SGA,0",WAIT_CMD_RESP,NO_ERROR_IGNORE);						//Set advertisement power to highest
	BLE_SendCMD(BLEHandle,"SW,0A,07",WAIT_CMD_RESP,NO_ERROR_IGNORE);				//Route Status 1 to CFG1 pin (P1_2)
	BLE_SendCMD(BLEHandle,"SW,0B,08",WAIT_CMD_RESP,NO_ERROR_IGNORE);				//Route Status 2 to CFG2 pin (P1_3)
	
	//comment two lines when using RN4871 with wrong firmware
	BLE_SendCMD(BLEHandle,"SW,0C,04",WAIT_CMD_RESP,NO_ERROR_IGNORE);				//Route UART_RX_INDICATION to CFG1 pin (P1_2)
	BLE_SendCMD(BLEHandle,"SW,0D,00",WAIT_CMD_RESP,NO_ERROR_IGNORE);				//Route None to pin (P1_3)	
	
	BLE_SendCMD(BLEHandle,"ST,0006,000C,0000,0200",WAIT_CMD_RESP,NO_ERROR_IGNORE);	//Set preferable connection parameters
	BLE_SendCMD(BLEHandle,"S-,RN",WAIT_CMD_RESP,NO_ERROR_IGNORE);						//Set serialized device name (RN_xxxx, where xxxx is last 2 bytes of MAC address)
	BLE_SendCMD(BLEHandle,"SDF,2.0",WAIT_CMD_RESP,NO_ERROR_IGNORE);					//Set device firmware revision
	BLE_SendCMD(BLEHandle,"SDH,v2.2",WAIT_CMD_RESP,NO_ERROR_IGNORE);				//Set device hardware revision
	BLE_SendCMD(BLEHandle,"SDM,RN4871",WAIT_CMD_RESP,NO_ERROR_IGNORE);			//Set device model name (BLE module name)
	BLE_SendCMD(BLEHandle,"SDN,Heart Beam",WAIT_CMD_RESP,NO_ERROR_IGNORE);	//Set device manufacturer name
	BLE_SendCMD(BLEHandle,"SDR,N/A",WAIT_CMD_RESP,NO_ERROR_IGNORE);					//Set device software revision name
	BLE_SendCMD(BLEHandle,"SDS,1000-00",WAIT_CMD_RESP,NO_ERROR_IGNORE);			//Set device serial number name
	
	BLEHandle->uartErrorExpected = 1;
	HAL_GPIO_WritePin(BT_RESET_PORT,BT_RESET_PIN,GPIO_PIN_RESET);				//Hardware Reset					
	HAL_Delay(1);
	HAL_GPIO_WritePin(BT_RESET_PORT,BT_RESET_PIN,GPIO_PIN_SET);
	HAL_Delay(50);
	BLEHandle->uartErrorExpected = 0;
	
	BLEHandle->uartBufferForward = 1;
	BLEHandle->uartDataAvailable = 0;
	BLEHandle->uartBufferReadPointer = BLEHandle->uartReceiveBuffer;
	uartHandle->RxState =HAL_UART_STATE_READY;
	mUART_Config(uartHandle,BLEHandle->uartBaudRate,BLEHandle->uartHWControl);
	mUART_Receive_IT(uartHandle,BLEHandle->uartReceiveBuffer,200);
	HAL_Delay(450);

	Log_WriteData(&LogHandle, "BLE/Init/Stage_5_0_x");
	if(compareUartMessage(BLEHandle,"%REBOOT#",8,SEEK_TO_END)!=0){				
		BLEHandle->bleStatus = BLE_ERROR;
		BLEHandle->ErrorNumber = BLE_REBOOT_ERROR;
		Log_WriteData(&LogHandle, "BLE/Init/Stage_5_1_x");
		return;
	}	
	
	BLEHandle->statusMessage.messageUpdated =0;
	//=================Initialize BLE module============//

	
	BLE_EnterCMDMode(BLEHandle, WAIT_CMD_RESP,NO_ERROR_IGNORE);
	HAL_Delay(20);
	BLE_SendCMD(BLEHandle,"IA,Z",WAIT_CMD_RESP,NO_ERROR_IGNORE);			
	BLE_SendCMD(BLEHandle,"A",WAIT_CMD_RESP,NO_ERROR_IGNORE);
	BLE_SendCMD(BLEHandle,"IA,01,06",WAIT_CMD_RESP,NO_ERROR_IGNORE);
	BLE_SendCMD(BLEHandle,"IA,08,303030",WAIT_CMD_RESP,NO_ERROR_IGNORE);
	string2hexString(advertisingNameCMD + 12,BLEHandle->macAddressString + 8);	
	BLE_SendCMD(BLEHandle,advertisingNameCMD,WAIT_CMD_RESP,NO_ERROR_IGNORE);	
	Log_WriteData(&LogHandle, "BLE/Init/Stage_6_0_x");
	
}

RN4871_UARTStatusTypeDef BLE_EnterCMDMode(BLE_TypeDef* BLEHandle, BLE_CMDWaitRespTypeDef waitCMDResponse,BLE_ErrorIgnoreTypeDef ignoreError){
	uint8_t message[5] = {'$','$','$'};
	
	BLEHandle->transmitCMDCompleted = 0;
	BLEHandle->ackOrAppMessage.messageUpdated = 0;
	
	HAL_Delay(3);
	HAL_UART_Transmit_DMA(BLEHandle->uartHandle,message,1);	
	HAL_Delay(3);
	HAL_UART_Transmit_DMA(BLEHandle->uartHandle,message,1);	
	HAL_Delay(3);
	HAL_UART_Transmit_DMA(BLEHandle->uartHandle,message,1);	
	HAL_Delay(3);
	
	
	if(waitCMDResponse == WAIT_CMD_RESP){
		BLEHandle->CMDTimeoutCounter = 0;
		while(BLEHandle->ackOrAppMessage.messageUpdated == 0){
			checkUartMessage(BLEHandle,NO_SEEK_TO_END);
			if(BLEHandle->CMDTimeoutCounter >= 300){			//receive timeout occured
				if(ignoreError==NO_ERROR_IGNORE){
					BLEHandle->bleStatus = BLE_ERROR;
					BLEHandle->ErrorNumber =  BLE_ENTER_CMD_MODE_ERROR;
					Log_WriteData(&LogHandle, "BLE/SendCMD/timeout_");
				}
				else{
					BLE_SendCMD(BLEHandle,"K,1",WAIT_CMD_RESP,ERROR_IGNORE);		//send dummy command (means BLE already in CMD mode)
				}
				return BLE_ERROR;			
			}
		}
		if(BLEHandle->ackOrAppMessage.message != CMD){
			Log_WriteData(&LogHandle, "BLE/SendCMD/resp_err");
			if(ignoreError==NO_ERROR_IGNORE){
				BLEHandle->bleStatus = BLE_ERROR;
				BLEHandle->ErrorNumber =  BLE_ENTER_CMD_MODE_ERROR;
			}
			return BLE_ERROR;
		}
		BLEHandle->ackOrAppMessage.messageUpdated = 0;
		
		BLEHandle->cmdMode = 1;
	}	
	return BLE_OK;
}
RN4871_UARTStatusTypeDef BLE_EnterLPMode(BLE_TypeDef* BLEHandle){
	uint8_t i;
  char batteryString [] = "000";
	value2DecimalString(batteryString, BLEHandle->batteryAdvPercentage);
	string2hexString(advertisingBattCMD + 6,batteryString);
	
	Log_WriteData(&LogHandle, "BLE/LP_Ent/Stage_0_0");
	if(HAL_GPIO_ReadPin(BT_CFG1_PORT,BT_CFG1_PIN) == GPIO_PIN_RESET){
		BLE_SendCMD(BLEHandle,"K,1",WAIT_CMD_RESP,NO_ERROR_IGNORE);													//Kill active BLE connection	
		if(BLE_ERROR != BLE_CheckStatusMessage(BLEHandle,5000)){
			Log_WriteData(&LogHandle, "BLE/LP_Ent/Stage_0_1");
			if(BLEHandle->statusMessage.messageUpdated){
				if(BLEHandle->statusMessage.message == DISCONNECT){				
				}
				BLEHandle->statusMessage.messageUpdated = 0;
			}
		}
		/*else{			
			BLEHandle->uartBufferForward = 1;
			BLEHandle->uartDataAvailable = 0;
			BLEHandle->uartBufferReadPointer = BLEHandle->uartReceiveBuffer;
			BLEHandle->uartHandle->RxState =HAL_UART_STATE_READY;
			mUART_Config(BLEHandle->uartHandle,460800,UART_HWCONTROL_CTS);//UART_HWCONTROL_CTS		
			mUART_Receive_IT(BLEHandle->uartHandle,BLEHandle->uartReceiveBuffer,200);		

			HAL_GPIO_WritePin(BT_RESET_PORT,BT_RESET_PIN,GPIO_PIN_RESET);				//Hardware Reset					
			HAL_Delay(1);
			HAL_GPIO_WritePin(BT_RESET_PORT,BT_RESET_PIN,GPIO_PIN_SET);
			HAL_Delay(100);

			if(compareUartMessage(BLEHandle,"%REBOOT",7,SEEK_TO_END)==0){						
				BLEHandle->uartBaudRate = 460800;
				BLEHandle->uartHWControl = UART_HWCONTROL_CTS;
				
			}
			else{						
				BLEHandle->uartBufferForward = 1;
				BLEHandle->uartDataAvailable = 0;
				BLEHandle->uartBufferReadPointer = BLEHandle->uartReceiveBuffer;
				BLEHandle->uartHandle->RxState =HAL_UART_STATE_READY;
				mUART_Config(BLEHandle->uartHandle,115200,UART_HWCONTROL_NONE);
				mUART_Receive_IT(BLEHandle->uartHandle,BLEHandle->uartReceiveBuffer,200);
			
				HAL_GPIO_WritePin(BT_RESET_PORT,BT_RESET_PIN,GPIO_PIN_RESET);				//Hardware Reset					
				HAL_Delay(1);
				HAL_GPIO_WritePin(BT_RESET_PORT,BT_RESET_PIN,GPIO_PIN_SET);
				HAL_Delay(500);
			
				if(compareUartMessage(BLEHandle,"%REBOOT",7,SEEK_TO_END)==0){				//check BLE UART with BaudRate = 460800 and hardware control	off
					BLEHandle->uartBaudRate = 115200;
					BLEHandle->uartHWControl = UART_HWCONTROL_NONE;
				}
				else{
					HAL_NVIC_SystemReset();	
				}					
			}
			BLE_EnterCMDMode(BLEHandle, WAIT_CMD_RESP,ERROR_IGNORE);
			BLE_SendCMD(BLEHandle,"D",NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);																	//Take BLE module information
			HAL_Delay(350);
						
			if(compareUartMessage(BLEHandle,"BTA=",4,NO_SEEK_TO_END)==0){
				Log_WriteData(&LogHandle, "BLE/LP_Ent/Stage_0_2");
				for(i=0;i<12;i++){
					BLEHandle->macAddressString[i] = *BLEHandle->uartBufferReadPointer;
					BLEHandle->uartBufferReadPointer++;
					if(BLEHandle->uartBufferReadPointer>= (BLEHandle->uartReceiveBuffer + 200)){
						BLEHandle->uartBufferReadPointer = BLEHandle->uartReceiveBuffer;
					}
				}
			}
			else{
				Log_WriteData(&LogHandle, "BLE/LP_Ent/Stage_0_3");
				BLEHandle->macAddressString[0]=0;
				strcpy(BLEHandle->macAddressString,"D0D0D0D0D0D0");
			}
			compareUartMessage(BLEHandle,"Name",4,SEEK_TO_END);														//dummy clear	
			
			BLE_SendCMD(BLEHandle,"IA,Z",NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);
			HAL_Delay(60);
			BLE_SendCMD(BLEHandle,"IA,01,06",NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);
			HAL_Delay(60);
			string2hexString(advertisingNameCMD + 12,BLEHandle->macAddressString + 8);	
			BLE_SendCMD(BLEHandle,advertisingNameCMD,NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);
			HAL_Delay(60);
			BLE_SendCMD(BLEHandle,advertisingBattCMD,NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);
			//BLE_SendCMD(BLEHandle,"IA,FF,10",NO_WAIT_CMD_RESP);
			HAL_Delay(60);
			BLE_SendCMD(BLEHandle,"A,3E8",NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);													//Start LP advertising after 30s timeout
			HAL_Delay(60);
			compareUartMessage(BLEHandle,"AOK",3,SEEK_TO_END);												//dummy clear
			
			return BLE_OK;
		}*/
	}	

	BLEHandle->uartErrorExpected = 1;
	HAL_GPIO_WritePin(BT_RESET_PORT,BT_RESET_PIN,GPIO_PIN_RESET);									//Hardware Reset					
	HAL_Delay(1);
	HAL_GPIO_WritePin(BT_RESET_PORT,BT_RESET_PIN,GPIO_PIN_SET);
	HAL_Delay(50);
	
	
	BLEHandle->uartBufferForward = 1;
	BLEHandle->uartDataAvailable = 0;
	BLEHandle->uartBufferReadPointer = BLEHandle->uartReceiveBuffer;
	BLEHandle->uartHandle->RxState =HAL_UART_STATE_READY;
	mUART_Config(BLEHandle->uartHandle,460800,UART_HWCONTROL_CTS);	
	mUART_Receive_IT(BLEHandle->uartHandle,BLEHandle->uartReceiveBuffer,200);	
	HAL_Delay(450);

	if(compareUartMessage(BLEHandle,"%REBOOT",7,SEEK_TO_END)==0){						
		BLEHandle->uartBaudRate = 460800;
		BLEHandle->uartHWControl = UART_HWCONTROL_CTS;
		
	}
	else{						
		BLEHandle->uartBufferForward = 1;
		BLEHandle->uartDataAvailable = 0;
		BLEHandle->uartBufferReadPointer = BLEHandle->uartReceiveBuffer;
		BLEHandle->uartHandle->RxState =HAL_UART_STATE_READY;
		mUART_Config(BLEHandle->uartHandle,115200,UART_HWCONTROL_NONE);
		mUART_Receive_IT(BLEHandle->uartHandle,BLEHandle->uartReceiveBuffer,200);
	
		BLEHandle->uartErrorExpected = 1;
		HAL_GPIO_WritePin(BT_RESET_PORT,BT_RESET_PIN,GPIO_PIN_RESET);				//Hardware Reset					
		HAL_Delay(1);
		HAL_GPIO_WritePin(BT_RESET_PORT,BT_RESET_PIN,GPIO_PIN_SET);
		HAL_Delay(50);
		BLEHandle->uartErrorExpected = 0;
		
		BLEHandle->uartBufferForward = 1;
		BLEHandle->uartDataAvailable = 0;
		BLEHandle->uartBufferReadPointer = BLEHandle->uartReceiveBuffer;
		BLEHandle->uartHandle->RxState =HAL_UART_STATE_READY;
		mUART_Config(BLEHandle->uartHandle,460800,UART_HWCONTROL_CTS);	
		mUART_Receive_IT(BLEHandle->uartHandle,BLEHandle->uartReceiveBuffer,200);	
		HAL_Delay(450);
	
		if(compareUartMessage(BLEHandle,"%REBOOT",7,SEEK_TO_END)==0){				//check BLE UART with BaudRate = 460800 and hardware control	off
			BLE_EnterCMDMode(BLEHandle, NO_WAIT_CMD_RESP,ERROR_IGNORE);
			HAL_Delay(60);
			BLE_SendCMD(BLEHandle,"SB,01",NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);	//Set baud rate to 460800
			HAL_Delay(60);
			BLE_SendCMD(BLEHandle,"SC,0",NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);						//Enable connectable advertisement
			HAL_Delay(60);
			BLE_SendCMD(BLEHandle,"S%,%,#",NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);					//Set pre and post delimiter
			HAL_Delay(60);
			BLE_SendCMD(BLEHandle,"SR,C000",NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);				//Set UartFlowControl (mask 0x8000), Set No Prompt (0x4000)
			HAL_Delay(60);
			BLE_SendCMD(BLEHandle,"SS,E0",NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);
			HAL_Delay(60);
			BLE_SendCMD(BLEHandle,"SO,1",NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);
			HAL_Delay(60);
			BLE_SendCMD(BLEHandle,"SW,0A,07",NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);				//Route Status 1 to CFG1 pin (P1_2)
			HAL_Delay(60);
			BLE_SendCMD(BLEHandle,"SW,0B,08",NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);				//Route Status 2 to CFG2 pin (P1_3)
			HAL_Delay(60);
	
			//comment two lines when using RN4871 with wrong firmware
			BLE_SendCMD(BLEHandle,"SW,0C,04",NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);				//Route UART_RX_INDICATION to CFG1 pin (P1_2)
			HAL_Delay(60);
			BLE_SendCMD(BLEHandle,"SW,0D,00",NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);				//Route None to pin (P1_3)	
			HAL_Delay(60);
			
			BLE_SendCMD(BLEHandle,"ST,0006,000C,0000,0200",NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);	//Set preferable connection parameters
			HAL_Delay(60);
			BLE_SendCMD(BLEHandle,"S-,RN",NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);						//Set serialized device name (RN_xxxx, where xxxx is last 2 bytes of MAC address)
			HAL_Delay(60);
			BLE_SendCMD(BLEHandle,"SDF,2.0",NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);					//Set device firmware revision
			HAL_Delay(60);
			BLE_SendCMD(BLEHandle,"SDH,v2.2",NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);				//Set device hardware revision
			HAL_Delay(60);
			BLE_SendCMD(BLEHandle,"SDM,RN4871",NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);			//Set device model name (BLE module name)
			HAL_Delay(60);
			BLE_SendCMD(BLEHandle,"SDN,Heart Beam",NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);	//Set device manufacturer name
			HAL_Delay(60);
			BLE_SendCMD(BLEHandle,"SDR,N/A",NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);					//Set device software revision name
			HAL_Delay(60);
			BLE_SendCMD(BLEHandle,"SDS,1000-06",NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);			//Set device serial number name
			HAL_Delay(60);					
			
			BLEHandle->uartErrorExpected = 1;
			HAL_GPIO_WritePin(BT_RESET_PORT,BT_RESET_PIN,GPIO_PIN_RESET);				//Hardware Reset					
			HAL_Delay(1);
			HAL_GPIO_WritePin(BT_RESET_PORT,BT_RESET_PIN,GPIO_PIN_SET);
			HAL_Delay(50);
			BLEHandle->uartErrorExpected = 0;
			
			BLEHandle->uartBufferForward = 1;
			BLEHandle->uartDataAvailable = 0;
			BLEHandle->uartBufferReadPointer = BLEHandle->uartReceiveBuffer;
			BLEHandle->uartHandle->RxState =HAL_UART_STATE_READY;
			mUART_Config(BLEHandle->uartHandle,460800,UART_HWCONTROL_CTS);	
			mUART_Receive_IT(BLEHandle->uartHandle,BLEHandle->uartReceiveBuffer,200);	
			HAL_Delay(450);		
			
			if(compareUartMessage(BLEHandle,"%REBOOT",7,SEEK_TO_END)!=0){	
				//HAL_NVIC_SystemReset();
				BLEHandle->shorterWakeupTimeout = 1;
				return BLE_ERROR;
			}						
		}
		else{						
			//HAL_NVIC_SystemReset();	
			BLEHandle->shorterWakeupTimeout = 1;
			return BLE_ERROR;
		}	
	}
	BLEHandle->uartErrorExpected = 0;
	
	BLE_EnterCMDMode(BLEHandle, WAIT_CMD_RESP,ERROR_IGNORE);
	HAL_Delay(20);
	
	BLE_SendCMD(BLEHandle,"D",NO_WAIT_CMD_RESP,NO_ERROR_IGNORE);								//Take BLE module information
	HAL_Delay(350);
	
	Log_WriteData(&LogHandle, "BLE/LP_Ent/Stage_1_0");
	if(compareUartMessage(BLEHandle,"BTA=",4,NO_SEEK_TO_END)==0){
		Log_WriteData(&LogHandle, "BLE/LP_Ent/Stage_1_1");
		for(i=0;i<12;i++){
			BLEHandle->macAddressString[i] = *BLEHandle->uartBufferReadPointer;
			BLEHandle->uartBufferReadPointer++;
			if(BLEHandle->uartBufferReadPointer>= (BLEHandle->uartReceiveBuffer + 200)){
				BLEHandle->uartBufferReadPointer = BLEHandle->uartReceiveBuffer;
			}
		}
	}
	else{
		Log_WriteData(&LogHandle, "BLE/LP_Ent/Stage_1_2");
		BLEHandle->macAddressString[0]=0;
		strcpy(BLEHandle->macAddressString,"D0D0D0D0D0D0");
		BLEHandle->shorterWakeupTimeout = 1;
	}
	compareUartMessage(BLEHandle,"Name",4,SEEK_TO_END);														//dummy clear
	
	Log_WriteData(&LogHandle, "BLE/LP_Ent/Stage_2_0");
	
	BLE_SendCMD(BLEHandle,"IA,Z",WAIT_CMD_RESP,NO_ERROR_IGNORE);	
	BLE_SendCMD(BLEHandle,"IA,01,06",WAIT_CMD_RESP,NO_ERROR_IGNORE);
	BLE_SendCMD(BLEHandle,advertisingBattCMD,WAIT_CMD_RESP,NO_ERROR_IGNORE);
	string2hexString(advertisingNameCMD + 12,BLEHandle->macAddressString + 8);	
	BLE_SendCMD(BLEHandle,advertisingNameCMD,WAIT_CMD_RESP,NO_ERROR_IGNORE);
	//BLE_SendCMD(BLEHandle,"IA,FF,10",WAIT_CMD_RESP,NO_ERROR_IGNORE);
	BLE_SendCMD(BLEHandle,"A,3E8",WAIT_CMD_RESP,NO_ERROR_IGNORE);																	//Start LP advertising after 30s timeout																
			
	if(BLEHandle->bleStatus == BLE_ERROR){
		Log_WriteData(&LogHandle, "BLE/LP_Ent/Stage_2_1");
		BLE_Init(BLEHandle->uartHandle,BLEHandle,&BLE_DFUHandle,0);
		BLE_SendCMD(BLEHandle,"IA,Z",WAIT_CMD_RESP,NO_ERROR_IGNORE);	
		BLE_SendCMD(BLEHandle,"IA,01,06",WAIT_CMD_RESP,NO_ERROR_IGNORE);
		string2hexString(advertisingNameCMD + 12,BLEHandle->macAddressString + 8);	
		BLE_SendCMD(BLEHandle,advertisingNameCMD,WAIT_CMD_RESP,NO_ERROR_IGNORE);
		BLE_SendCMD(BLEHandle,advertisingBattCMD,WAIT_CMD_RESP,NO_ERROR_IGNORE);
		//BLE_SendCMD(BLEHandle,"IA,FF,10",WAIT_CMD_RESP,NO_ERROR_IGNORE);
		BLE_SendCMD(BLEHandle,"A,3E8",WAIT_CMD_RESP,NO_ERROR_IGNORE);													//Start LP advertising after 30s timeout		
		BLEHandle->shorterWakeupTimeout = 1;
	}
	BLEHandle->connectionStatus = DISCONNECTED_LPADVERTISING;
	
	return BLE_OK;
}
RN4871_UARTStatusTypeDef BLE_ExitLPMode(BLE_TypeDef* BLEHandle){
	BLEHandle->bleStatus = BLE_OK;

	
	BLE_SendCMD(BLEHandle,"K,1",WAIT_CMD_RESP,NO_ERROR_IGNORE);												//Kill active BLE connection	
	if(BLEHandle->bleStatus == BLE_ERROR){
		BLEHandle->bleStatus = BLE_OK;																	//If peer device is not connected Err message will be received	
	}
	BLE_CheckStatusMessage(BLEHandle,2000);														//Wait for BLE module to disconnect
	if(BLEHandle->statusMessage.message == DISCONNECT){
		BLEHandle->connectionStatus = DISCONNECTED_ADVERTISING;
		BLE_SendCMD(BLEHandle,"Y",WAIT_CMD_RESP,NO_ERROR_IGNORE);												//Disable advertising	
	}		
	else{
		BLEHandle->bleStatus = BLE_ERROR;
		return BLE_OK;
	}	

	return BLE_OK;
}

RN4871_UARTStatusTypeDef BLE_SendCMD(BLE_TypeDef* BLEHandle, char* cmdString, BLE_CMDWaitRespTypeDef waitCMDResponse,BLE_ErrorIgnoreTypeDef ignoreError){
	char cr[1] = {0x0D};
	char messageTimeout[] =  "BLE/SendCmd/xxx_Time";
	char messageErrAck[] =   "BLE/SendCmd/xxx_Err_";
	
	uint8_t i;
	
	for(i=0;i<3;i++){
		messageTimeout[12+i] = cmdString[i];
		messageErrAck[12+i] = cmdString[i];
	}
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
				if(ignoreError == NO_ERROR_IGNORE){
					BLEHandle->bleStatus = BLE_ERROR;
					BLEHandle->ErrorNumber = BLE_SEND_CMD_ERROR;					
					Log_WriteData(&LogHandle, messageTimeout);
				}
				return BLE_ERROR;
			}
		}
		
		if(BLEHandle->ackOrAppMessage.message != AOK){
			if(ignoreError == NO_ERROR_IGNORE){
				BLEHandle->bleStatus = BLE_ERROR;
				BLEHandle->ErrorNumber = BLE_SEND_CMD_ERROR;
				Log_WriteData(&LogHandle, messageErrAck);
			}
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
			BLEHandle->uartErrorExpected = 0;
			BLEHandle->uartBufferForward = 1;
			BLEHandle->uartDataAvailable = 0;
			BLEHandle->uartErrorRecover = 0;
			BLEHandle->uartBufferReadPointer = BLEHandle->uartReceiveBuffer;
		
#ifdef BLE_DATA_DOK_RESPONSE
			BLEHandle->dataOKRespEnabled = 1;
#else
			BLEHandle->dataOKRespEnabled = 0;
#endif			

			BLEHandle->uartErrorExpected = 1;
			HAL_GPIO_WritePin(BT_RESET_PORT,BT_RESET_PIN,GPIO_PIN_RESET);				//Hardware Reset					
			HAL_Delay(1);
			HAL_GPIO_WritePin(BT_RESET_PORT,BT_RESET_PIN,GPIO_PIN_SET);
			HAL_Delay(50);
			BLEHandle->uartErrorExpected = 0;
			
			BLEHandle->uartHandle->RxState =HAL_UART_STATE_READY;
			mUART_Config(BLEHandle->uartHandle,BLEHandle->uartBaudRate,BLEHandle->uartHWControl);
			mUART_Receive_IT(BLEHandle->uartHandle,BLEHandle->uartReceiveBuffer,200);
			HAL_Delay(450);
			
			if(compareUartMessage(BLEHandle,"%REBOOT",7,SEEK_TO_END)==0){
				BLEHandle->uartBaudRate = 460800;
				BLEHandle->uartHWControl = UART_HWCONTROL_CTS;
				
			}
			
			BLEHandle->statusMessage.messageUpdated =0;

			BLE_EnterCMDMode(BLEHandle, WAIT_CMD_RESP, ERROR_IGNORE);
			HAL_Delay(20);
			BLE_SendCMD(BLEHandle,"IA,Z",WAIT_CMD_RESP,NO_ERROR_IGNORE);			
			BLE_SendCMD(BLEHandle,"A",WAIT_CMD_RESP,NO_ERROR_IGNORE);	
			BLE_SendCMD(BLEHandle,"IA,01,06",WAIT_CMD_RESP,NO_ERROR_IGNORE);
			string2hexString(advertisingNameCMD + 12,BLEHandle->macAddressString + 8);	
			BLE_SendCMD(BLEHandle,advertisingNameCMD,WAIT_CMD_RESP,NO_ERROR_IGNORE);	
			BLE_SendCMD(BLEHandle,"IA,FF,01",WAIT_CMD_RESP,NO_ERROR_IGNORE);
			
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
	
	if(BLEHandle->uartParseBuffer[0] == ':'){
		for(i=0;i<size;i++){
			BLEHandle->dfuHandle->parseBuffer[i] = BLEHandle->uartParseBuffer[i];
		}
		BLEHandle->dfuHandle->parseBuffer[i] = 0;
		BLEHandle->dfuHandle->messageSize = size;
		BLEHandle->ackOrAppMessage.message = APP_DFU_PACKET;
		BLEHandle->ackOrAppMessage.messageUpdated = 1;
	}
	else if(BLEHandle->uartParseBuffer[0] == 'A'){
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
		/*else if(mByteCmp(BLEHandle->uartParseBuffer,"APP_START_STR\r\n",13)==0){
			BLEHandle->ackOrAppMessage.message = APP_START_STR;
			BLEHandle->ackOrAppMessage.messageUpdated = 1;
		}*/
		else if(mByteCmp(BLEHandle->uartParseBuffer,"APP_STOP_ACQ\r\n",12)==0){
			BLEHandle->ackOrAppMessage.message = APP_STOP_ACQ;
			BLEHandle->ackOrAppMessage.messageUpdated = 1;
		}
		else if(mByteCmp(BLEHandle->uartParseBuffer,"APP_END_BLOCK,",14)==0){
			BLEHandle->ackOrAppMessage.message = APP_END_BLOCK;
			BLEHandle->ackOrAppMessage.param1 =  hex2int(BLEHandle->uartParseBuffer + 14, 4);				
			BLEHandle->ackOrAppMessage.messageUpdated = 1;
		}
		else if(mByteCmp(BLEHandle->uartParseBuffer,"APP_MISS_RANGE,",15)==0){
			BLEHandle->ackOrAppMessage.message = APP_MISS_RANGE;
			BLEHandle->ackOrAppMessage.param1 =  hex2int(BLEHandle->uartParseBuffer + 15, 4);				
			BLEHandle->ackOrAppMessage.param2 =  hex2int(BLEHandle->uartParseBuffer + 20, 4);				
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
		else if(mByteCmp(BLEHandle->uartParseBuffer,"APP_DFU\r\n",9)==0){
			BLEHandle->ackOrAppMessage.message = APP_DFU;
			BLEHandle->ackOrAppMessage.messageUpdated = 1;
		}
		else if(mByteCmp(BLEHandle->uartParseBuffer,"APP_DFU_OTA\r\n",13)==0){
			BLEHandle->ackOrAppMessage.message = APP_DFU_OTA;
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
RN4871_UARTStatusTypeDef BLE_DFU_Init(BLE_TypeDef* BLEHandle){
	FLASH_Erase(TEMP_PROGRAM_ADDRESS,TEMP_PROGRAM_ADDRESS + PROGRAM_SIZE - 1);
	HAL_FLASH_Unlock();	
	BLEHandle->dfuHandle->totalByteCount = 0;
	BLEHandle->dfuHandle->newAddressH = 0;
	BLEHandle->dfuHandle->dataRowIndex = 0;
	BLEHandle->dfuHandle->flashAddress = TEMP_PROGRAM_ADDRESS;
	BLEHandle->dfuHandle->dfuError = 0;	
	BLEHandle->dfuHandle->dfuCompleted = 0;
	return BLE_OK;
}
RN4871_UARTStatusTypeDef BLE_DFU_Process(BLE_TypeDef* BLEHandle){
	uint32_t sumCRC;
	uint16_t byteCount;
	uint8_t tempByte;
	uint16_t addressL;
	uint32_t currAbsAddress;
	uint8_t recordType;
	uint16_t checkSum;
	uint8_t i;
	BLE_DFU_Typedef* pDFU = BLEHandle->dfuHandle;
	uint64_t* src_addr = (uint64_t *) pDFU->dataRowBuffer;
	
	sumCRC = 0;
	byteCount = char2byte(pDFU->parseBuffer+1);
	sumCRC += byteCount;
	
	tempByte = char2byte(pDFU->parseBuffer+3);
	sumCRC += tempByte;
	addressL  = tempByte;
	tempByte = char2byte(pDFU->parseBuffer+5);
	sumCRC += tempByte;
	addressL <<= 8;
	addressL |= tempByte;
	
	recordType = char2byte(pDFU->parseBuffer+7);
	sumCRC += recordType;
	
	if((pDFU->messageSize - 2)!= byteCount*2 + 11) {
		pDFU->dfuError = 1;
		return BLE_ERROR;
	}
	
	switch (recordType){
		case 0: 
			if(!pDFU->newAddressH){							
				currAbsAddress = pDFU->addressH;
				currAbsAddress <<= 16;
				currAbsAddress |= addressL;
				if((pDFU->prevAbsAddress + pDFU->prevByteCount) != currAbsAddress){
					pDFU->dfuError = 2;
					return BLE_ERROR;
				}
			}
			pDFU->prevByteCount = byteCount;
			pDFU->prevAbsAddress = pDFU->addressH;
			pDFU->prevAbsAddress <<= 16;
			pDFU->prevAbsAddress |= addressL;
			pDFU->totalByteCount += byteCount;
			for(i=0;i<byteCount;i++){
				pDFU->dataRowBuffer[pDFU->dataRowIndex] = char2byte(pDFU->parseBuffer + 9 + i*2);
				sumCRC += pDFU->dataRowBuffer[pDFU->dataRowIndex++];
				if(pDFU->dataRowIndex==8){
					pDFU->dataRowIndex = 0;		
					if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, pDFU->flashAddress, *src_addr) == HAL_OK)
						pDFU->flashAddress = pDFU->flashAddress + 8;
					else{
						if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, pDFU->flashAddress, *src_addr) == HAL_OK)
							pDFU->flashAddress = pDFU->flashAddress + 8;
						else {
							pDFU->dfuError = 3;
							return BLE_ERROR;
						}
					}
				}
			}
			pDFU->newAddressH = 0;
			break;
		case 1:
			if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, pDFU->flashAddress, *src_addr) == HAL_OK)
				pDFU->flashAddress = pDFU->flashAddress + 8;
			else{
				if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, pDFU->flashAddress, *src_addr) == HAL_OK)
					pDFU->flashAddress = pDFU->flashAddress + 8;
				else {
					pDFU->dfuError = 4;
					return BLE_ERROR;
				}
			}
			//-----check total size with address field--//
			if(pDFU->totalByteCount != (pDFU->prevAbsAddress + pDFU->prevByteCount - PROGRAM_ADDRESS)){
			//if(totalByteCount != (prevAbsAddress + prevByteCount - 0x08000000)){
				pDFU->dfuError = 5;
				return BLE_ERROR;
			}
			//-----program DFU_STATUS_SECTOR---------//
			FLASH_Erase(DFU_STATUS_SECTOR_ADDRESS,DFU_STATUS_SECTOR_ADDRESS + 2048 - 1);
			HAL_FLASH_Unlock();
			if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, DFU_STATUS_SECTOR_ADDRESS, (uint64_t) DFU_STATUS_FLAG) == HAL_OK){}
			else{
				if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, DFU_STATUS_SECTOR_ADDRESS, (uint64_t) DFU_STATUS_FLAG) == HAL_OK){}
				else {
					pDFU->dfuError = 6;
					return BLE_ERROR;
				}
			}
			if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, DFU_STATUS_SECTOR_ADDRESS + 8, (uint64_t) pDFU->totalByteCount) == HAL_OK){}							
			else{
				if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, DFU_STATUS_SECTOR_ADDRESS + 8, (uint64_t) pDFU->totalByteCount) == HAL_OK){}								
				else {
					pDFU->dfuError = 7;
					return BLE_ERROR;
				}
			}			
			HAL_FLASH_Lock();
			
			pDFU->dfuCompleted=1;			
			
			break;
		case 4:			
			pDFU->addressH = char2byte(pDFU->parseBuffer + 9);
			sumCRC += pDFU->addressH;
			pDFU->addressH <<=8;
			pDFU->addressH |= char2byte(pDFU->parseBuffer + 11);
			sumCRC += (pDFU->addressH & 0xFF);
			pDFU->newAddressH = 1;
			break;
		case 5:	
			for(i=0;i<byteCount;i++){
				sumCRC += char2byte(pDFU->parseBuffer + 9 + i*2);
			}
			break;
		default:
			pDFU->dfuError = 8;
			return BLE_ERROR;
	}						
	checkSum = char2byte(pDFU->parseBuffer + 9 + byteCount*2);
	sumCRC = ~sumCRC;
	sumCRC +=1;
	if((sumCRC & 0xFF) != checkSum) {
		pDFU->dfuError = 9;
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
	/*outputString[3] = 0x0D;
	outputString[4] = 0x0A;
	outputString[5] = 0x00;*/
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

uint16_t char2byte(uint8_t* string){
	uint8_t out;
	string[0] &= 0x7F;
	string[1] &= 0x7F;

	if((string[0]<='9') && (string[0]>='0')){
		out <<=4;
		out |= string[0] - '0';
	}
	else{
		out <<=4;
		out |= string[0] - 'A' + 10;
	}
	if((string[1]<='9') && (string[1]>='0')){
		out <<=4;
		out |= string[1] - '0';
	}
	else{
		out <<=4;
		out |= string[1] - 'A' + 10;
	}
	return out;
		
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



