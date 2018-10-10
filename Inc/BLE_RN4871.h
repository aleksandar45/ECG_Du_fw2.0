#ifndef __BLE_RN4871_H
#define __BLE_RN4871_H


// Includes ------------------------------------------------------------------//
#include "stm32l4xx_hal.h"
#include "string.h"
#include "mUART.h"
#include "main.h"


// Defines ------------------------------------------------------------------//
#define UART_BAUD_RATE_460800
//#define BLE_DATA_DOK_RESPONSE

#define BLE_REBOOT_ERROR 					1
#define BLE_ENTER_CMD_MODE_ERROR  2
#define BLE_SEND_CMD_ERROR				3
#define BLE_DNOK_OVF_ERROR				4
#define BLE_STATUS_MSG_ERROR			5
#define BLE_ACKAPP_MSG_ERROR			6
#define BLE_UART_ERROR						7
#define BLE_ALL_MSG_ERROR					8

// Types ------------------------------------------------------------------//
typedef enum {
	DISCONNECTED_ADVERTISING = 0,
	DISCONNECTED_LPADVERTISING = 1,
	CONNECTED = 2,

}BLE_ConnectionTypeDef;

typedef enum {
	ADV_TIMEOUT = 0,  			//(%ADV_TIMEOUT#) Advertisement timeout, if advertisement time is specified by command A
	BONDED = 1,							//(%BONDED#) Security materials such as Link Key are saved into NVM
	CONN_PARAM = 2,					//(%CONN_PARAM,<Interval>,<Latency>,<Timeout>#) Update connection parameters of connection interval, slave latency and supervision timeout.
	CONNECT = 3,						//(%CONNECT,<0-1>,<Addr>#) Connect to BLE device with address <Addr>
	DISCONNECT = 4,					//(%DISCONNECT#) BLE connection lost
  ERR_CONNPARAM = 5,			//(%ERR_CONNPARAM#) Failed to update connection parameters
	ERR_MEMORY = 6,					//(%ERR_MEMORY#) Running out of dynamic memory
	ERR_READ = 7,						//(%ERR_READ#) Failed to read characteristic value
	ERR_RMT_CMD = 8,				//(%ERR_RMT_CMD#) Failed to start remote command, due to insecure BLE link or mismatch pin code
	ERR_SEC = 9,						//(%ERR_SEC#) Failed to secure the BLE link
	KEY = 10,								//(%KEY:<Key>#) Display the 6-digit security key
	KEY_REQ = 11,						//(%KEY_REQ#) Request input security key
	INDI = 12,							//(%INDI,<hdl>,<hex>#) Received value indication <hex> for characteristic handle <hdl>
	NOTI = 13,							//(%NOTI,<hdl>,<hex>#) Received value notification <hex> for characteristic handle <hdl>
	PIO1H = 13,							//(%PIO1H#) PIO1 rising edge event
	PIO1L = 14,							//(%PIO1L#) PIO1 falling edge event
	PIO2H = 15,							//(%PIO2H#) PIO2 rising edge event
	PIO2L = 16,							//(%PIO2L#) PIO2 falling edge event
	PIO3H = 17,							//(%PIO3H#) PIO3 rising edge event
	PIO3L = 18,							//(%PIO3L#) PIO3 falling edge event
	RE_DISCV = 19,					//(%RE_DISCV#) Received data indication of service changed, redo service discovery
	REBOOT = 20,						//(%REBOOT#) Reboot finished
	RMT_CMD_OFF = 21,				//(%RMT_CMD_OFF#) End of Remote Command mode
	RMT_CMD_ON = 22,				//(%RMT_CMD_ON#) Start of Remote Command mode;		
	RV = 23,								//(%RV,<hdl>,<hex># Read value <hex> for characteristic handle <hdl>
	S_RUN = 24,							//(%S_RUN:<CMD>#) Debugging information when running script. CMD is the command called by script
	SECURED = 25,						//(%SECURED#) BLE link is secured
	STREAM_OPEN = 26,				//(%STREAM_OPEN#) UART Transparent data pipe is established
	TMR1 = 27,							//(%TMR1#) Timer 1 expired event
	TMR2 = 28,							//(%TMR2#) Timer 2 expired event
	TMR3 = 29,							//(%TMR3#) Timer 3 expired event
	WC = 30,								//(%WC,<hdl>,<hex>#) Received start/end notification/indication request <hex> for characteristic configuration handle <hdl>
	WV = 31,								//(%WV,<hdl>,<hex>#) Received write request <hex> for characteristic handle <hdl>
	CONN_ADV = 32,					//(%<Addr>,<0-1>,<name>,<UUIDs>,<RSSI>#) Received connectable advertisement
	NONCONN_ADV = 33,				//(%<Addr>,<0-1>,<RSSI>,Brcst,<hex>#)
	
	NOT_VALID_MESSAGE = 34,	//Not valid status mesage received
	
}RN4871_RespStatusTypeDef;

typedef enum {
	AOK = 0,			//(AOK+<CRLF>)	//Indicates a positive or successful response
	ERR = 1,			//(ERR+<CRLF>)	//Indicates an error or negative response
	CMD_CP = 2,			//(CMD>+<CRLF>)	//Ready to receive next command. Command prompt is enabled.
	CMD = 3,			//(CMD+<CRLF>)	//Ready to receive next command. Command prompt is disabled.
	END = 4,			//(END+<CRLF>)	//End Command mode
	
	APP_START_ACQ = 5,		//(APP_START_ACQ+<CRLF>)				//App command - start acquisition
//	APP_START_STR = 6,		//(APP_START_STR+<CRLF>)				//App command - start storing in ram memory	
	APP_STOP_ACQ = 7,			//(APP_STOP_ACQ+<CRLF>)					//App command - stop acquisition
	APP_MISS_RANGE = 8,		//(APP_MISS_RANGE,)							//App command - missing blocks range
	APP_END_BLOCK = 9,		//(APP_END_BLOCK,)							//App command - final block of session
	APP_STAY = 10,					//(APP_STAY+<CRLF>)						//App command - stay turned on, reset timeout timer
	APP_OFF = 11,					//(APP_OFF+<CRLF>)							//App command - turn off device
	APP_DOK = 12,					//(APP_DOK,xxxx+<CRLF>)					//App command - data packet received correctly, parameter is packetNumber(hex in ascii) 
	APP_DNOK = 13,				//(APP_DNOK,xxxx+<CRLF>)				//App command - data packet not received correctly, parameter is packetNumber(hex in ascii)
	APP_DFU = 14,					//(APP_DFU<CRLF>)								//App command - device firmware upgrade
}RN4871orApp_RespCMDTypeDef;

typedef struct{
	RN4871_RespStatusTypeDef message;									//Received status message. This parameter can be one of @RN4871_RespStatusTypeDef
	uint32_t param1;																	//If message contains parameters this is parameter 1.
	uint32_t param2;																	//If message contains parameters this is parameter 2.
	uint32_t param3;																	//If message contains parameters this is parameter 3.
	uint32_t param4;																	//If message contains parameters this is parameter 4.
	uint32_t param5;																	//If message contains parameters this is parameter 5.	
	uint8_t numOfParams;															//Number of parameters received
	uint8_t	messageUpdated;														//If this parameter is 1 that means message is not read. After reading status message clear flag.
}RN4871_RespStatusParamsTypeDef;

typedef struct{
	RN4871orApp_RespCMDTypeDef message;								//Received status message. This parameter can be one of @RN4871orApp_RespCMDTypeDef
	uint32_t param1;																	//If message contains parameters this is parameter 1.	
	uint32_t param2;																	//If message contains parameters this is parameter 2.	
	uint8_t	messageUpdated;														//If this parameter is 1 that means message is not read. After reading status message clear this flag.
}RN4871orApp_RespCMDParamsTypeDef;



typedef enum {
	BLE_OK = 0,					//BLE UART communication is OK. 
	BLE_ERROR = 1,			//BLE UART communication is not OK.
}RN4871_UARTStatusTypeDef;

typedef enum{
	NO_WAIT_CMD_RESP = 0,
	WAIT_CMD_RESP = 1,
}BLE_CMDWaitRespTypeDef;

typedef enum{
	SEEK_TO_END = 0,
	NO_SEEK_TO_END = 1,
}BLE_UARTSeekTypeDef;

typedef struct
{	
		UART_HandleTypeDef* uartHandle;								//UART periphery connected to BLE module
		uint32_t uartBaudRate;												//UART baud rate
		uint32_t uartHWControl;												//Enable or disable UART hardware control
		uint8_t	uartErrorExpected;										//If UART error is expected set this flag
		uint8_t uartErrorRecover;											//???
		uint8_t uartTransmitBuffer[500];							//UART transmit buffer for BLE communication
		uint8_t uartReceiveBuffer[200];								//UART receive buffer for BLE communication
	  uint8_t uartParseBuffer[200];									//???
		uint8_t uartBufferForward;										//???
		uint32_t uartDataAvailable;										//???
		uint8_t *uartBufferReadPointer;								//???
		
		BLE_ConnectionTypeDef connectionStatus;       //Specifies BLE module connection status.
		RN4871_UARTStatusTypeDef bleStatus;						//BLE module status.
		uint8_t ErrorNumber;													//BLE error number.
		RN4871_RespStatusParamsTypeDef statusMessage;	//Received status message from RN4871 with or without parameters
		RN4871orApp_RespCMDParamsTypeDef ackOrAppMessage;		//Received ack message from RN4871 or message from App.
		char macAddressString[13];										//MAC address string
		
	//	uint8_t responseDelimiter;									//Delimiter character for responded messages from RN4871.
		uint32_t CMDTimeoutCounter;										//Timeout counter used for measuring CMD response time
		uint8_t transmitCMDCompleted;									//Flag that specifies whether CMD message transmit is completed 
	//	uint8_t receiveCMDCompleted;								//Flag that specifies whether CMD message receive is completed 
		uint8_t statusMessageReceived;								//Flag that specifies whether status message from RN4871 is received
		uint8_t ackOrAppMessageReceived;							//Flag that specifies whether ack message from RN4871 or App message is received	
		uint8_t cmdMode;															//Specifies whether BLE module is in CMD or Data mode
		uint8_t dataOKRespEnabled;										//Flag that enables data transfering acknowledge from App
		uint8_t dataOKWaiting;												//Flag that is enabled when waiting for data transfering acknowledge 
		uint8_t repeatDataPacket;											//Flag that is enabled when data transfering error acknowledge is received (APP_DNOK)
		uint16_t repeatDataPacketNumber;							//Number of packet that needs to be repeated
		uint8_t repeatDataPacketCounter;							//This parameter counts number of received data transfering error acknowledges (APP_DNOK)
		uint8_t connParamsUpdateCounter;							//This parameter counts number of connection parameter updates
	
}BLE_TypeDef;


// Functions ------------------------------------------------------------------//
void BLE_Init(UART_HandleTypeDef* uartHandle, BLE_TypeDef* BLEHandle);
RN4871_UARTStatusTypeDef BLE_EnterCMDMode(BLE_TypeDef* BLEHandle, BLE_CMDWaitRespTypeDef waitCMDResponse);
RN4871_UARTStatusTypeDef BLE_EnterLPMode(BLE_TypeDef* BLEHandle);
RN4871_UARTStatusTypeDef BLE_ExitLPMode(BLE_TypeDef* BLEHandle);
RN4871_UARTStatusTypeDef BLE_SendCMD(BLE_TypeDef* BLEHandle, char* cmdString, BLE_CMDWaitRespTypeDef waitCMDResponse);
RN4871_UARTStatusTypeDef BLE_SendData(BLE_TypeDef* BLEHandle, uint8_t* dataBuffer, uint16_t size);

RN4871_UARTStatusTypeDef BLE_CheckStatusMessage(BLE_TypeDef* BLEHandle, uint32_t timeout);
//RN4871_UARTStatusTypeDef BLE_ParseStatusMessage(BLE_TypeDef* BLEHandle);
//RN4871_UARTStatusTypeDef BLE_ParseAckOrAppMessage(BLE_TypeDef* BLEHandle);
uint8_t mByteCmp(uint8_t* pA, char* string, uint8_t size);
void string2hexString(char* outputString, char* inputString);
uint32_t hex2int(uint8_t* charArray, uint8_t size);
void value2DecimalString(char* outputString, uint8_t value);
void mDelay(uint32_t msDelay);


uint8_t compareUartMessage(BLE_TypeDef* BLEHandle, char* string, uint8_t size, BLE_UARTSeekTypeDef seekType);
uint8_t checkUartMessage(BLE_TypeDef* BLEHandle, BLE_UARTSeekTypeDef seekType);
RN4871_UARTStatusTypeDef BLE_ParseStatusMessage(BLE_TypeDef* BLEHandle, uint8_t* startPointer, uint8_t size);
RN4871_UARTStatusTypeDef BLE_ParseAckOrAppMessage(BLE_TypeDef* BLEHandle, uint8_t* startPointer, uint8_t size);

#endif
