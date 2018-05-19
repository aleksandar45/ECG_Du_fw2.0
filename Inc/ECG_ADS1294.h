#ifndef __ECG_ADS1294_H
#define __ECG_ADS1294_H

// Includes ------------------------------------------------------------------//
#include "stm32l4xx_hal.h"
#include "mSPI.h"
#include "main.h"

// Defines ------------------------------------------------------------------//
#define ECG_FIFO_SIZE    4000

//=========ADS1298 gpio pins=======//
#define ECG_CS_PIN  		GPIO_PIN_4
#define ECG_CS_PORT  		GPIOA

#define ECG_RESET_PIN  	GPIO_PIN_9
#define ECG_RESET_PORT  GPIOA
//=========ADS1298 gpio pins=======//


//=========ADS1298 register addresses======//
#define ECG_ID										0x00
#define ECG_CONFIG1								0x01
#define ECG_CONFIG2								0x02
#define ECG_CONFIG3								0x03
#define ECG_LOFF									0x04
#define ECG_CH1SET								0x05
#define ECG_CH2SET								0x06
#define ECG_CH3SET								0x07
#define ECG_CH4SET								0x08
#define ECG_CH5SET								0x09
#define ECG_CH6SET								0x0A
#define ECG_CH7SET								0x0B
#define ECG_CH8SET								0x0C
#define ECG_RLD_SENSP							0x0D
#define ECG_RLD_SENSN							0x0E
#define ECG_LOFF_SENSP						0x0F
#define ECG_LOFF_SENSN						0x10
#define ECG_LOFF_FLIP							0x11
#define ECG_LOFF_STATP						0x12
#define ECG_LOFF_STATN						0x13
#define ECG_GPIO									0x14
#define ECG_PACE									0x15
#define ECG_RESP									0x16
#define ECG_CONFIG4								0x17
#define ECG_WCT1									0x18
#define ECG_WCT2									0x19
//=========ADS1298 register addresses======//


//=========ADS1298 commands======//
#define ECG_WAKEUP_CMD						0x02
#define ECG_STANDBY_CMD						0x04
#define ECG_RESET_CMD							0x06
#define ECG_START_CMD							0x08
#define ECG_STOP_CMD							0x0A
#define ECG_RDATAC_CMD						0x10
#define ECG_SDATAC_CMD						0x11
#define ECG_RDATA_CMD							0x12
#define ECG_RREG_CMD							0x20
#define ECG_WREG_CMD							0x40
//=========ADS1298 commands======//

//=========ECG error numbers=====//
#define ECG_INIT_ERROR 						1
#define ECG_FIFO_ERROR					  2
#define ECG_SPI_ERROR					  	3
//=========ECG error numbers=====//

// Types ------------------------------------------------------------------//
typedef enum {
	ECG_OK = 0,					//ECG perifery is ok
	ECG_ERROR = 1,			//ECG perifery is not ok
}ECG_StatusTypeDef;

typedef struct
{	
			SPI_HandleTypeDef* spiHandle;									//SPI periphery connected to ECG module.

			uint8_t spiDMAReceiveBuffer1[20];							//SPI receive buffer 1 for DMA controller.
			uint8_t spiDMAReceiveBuffer2[20];							//SPI receive buffer 2 for DMA controller.
__IO	uint8_t* spiDMAReceiveBufferPointer;					//Pointer to one of two spiDMA receive buffers.
			uint8_t acqStarted;														//Flag that specifies whether continuous acquisition is started or not
			
			uint8_t ecgFIFOBufferCH1[ECG_FIFO_SIZE];			//FIFO buffer of channel 1; received data is circulary stored.
			uint8_t ecgFIFOBufferCH2[ECG_FIFO_SIZE];			//FIFO buffer of channel 2; received data is circulary stored.
			uint8_t ecgFIFOBufferCH3[ECG_FIFO_SIZE];			//FIFO buffer of channel 3; received data is circulary stored.
__IO	uint16_t writeFIFOPointer;										//Pointer that points to write location of FIFO buffer.
			uint16_t readFIFOPointer;											//Pointer that points to read location of FIFO buffer.
__IO	uint16_t dataFIFOAvailable;										//Available data in FIFO buffer.
__IO	uint8_t forwardFIFO;													//Flag that specifies whether data is stored in front of writeFIFOPointer or behind writeFIFOPointer
	
			ECG_StatusTypeDef ecgStatus;									//ECG module status.
			uint8_t ErrorNumber;													//ECG error number.												
		
}ECG_TypeDef;

// Functions ------------------------------------------------------------------//

void ECG_Init(SPI_HandleTypeDef* spiHandle, ECG_TypeDef* ECGHandle);
void ECG_Start_Acquisition(ECG_TypeDef* ECGHandle);
void ECG_Stop_Acquisition(ECG_TypeDef* ECGHandle);

void Send_Comand_ECG(SPI_HandleTypeDef* spiHandle,  uint8_t comand);
void Write_Register_ECG(SPI_HandleTypeDef* spiHandle,uint8_t address, uint8_t data);
uint8_t Read_Register_ECG(SPI_HandleTypeDef* spiHandle,uint8_t address);

ECG_StatusTypeDef ECG_WriteFIFOData(ECG_TypeDef* ECGHandle, uint8_t* inputDataBuffer, uint8_t chNum, uint16_t size);


// Macros ---------------------------------------------------------------------//
#define ECG_ReadFIFODataCH1(__HANDLE__) ((__HANDLE__)->ecgFIFOBufferCH1[(__HANDLE__)->readFIFOPointer])
#define ECG_ReadFIFODataCH2(__HANDLE__) ((__HANDLE__)->ecgFIFOBufferCH2[(__HANDLE__)->readFIFOPointer])
#define ECG_ReadFIFODataCH3(__HANDLE__) (((__HANDLE__)->readFIFOPointer + 1 >= ECG_FIFO_SIZE ?(__HANDLE__)->readFIFOPointer = 0 : (__HANDLE__)->readFIFOPointer++), (__HANDLE__)->dataFIFOAvailable--, ((__HANDLE__)->readFIFOPointer == 0? (__HANDLE__)->ecgFIFOBufferCH3[ECG_FIFO_SIZE-1] : (__HANDLE__)->ecgFIFOBufferCH3[(__HANDLE__)->readFIFOPointer-1]) )



#endif
