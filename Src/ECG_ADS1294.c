#include "ECG_ADS1294.h"

extern ECG_TypeDef ECGHandle;
extern mTimerHandler_TypeDef  mTimHandle;

void ECG_Init(SPI_HandleTypeDef* spiHandle, ECG_TypeDef* ECGHandle){
	uint32_t read_value;
	uint8_t error_ecg;
	
	//-----------Electrodes connection----------------//
	// LF-> CH1P
	// RF-> CH1N
	// LCH-> CH2P, CH3N
	// UCH-> CH3P
	// CH1 = (LF-RF), CH2 = (LCH - WCT), CH3 = (UCH-LCH)
	
	ECGHandle->spiHandle = spiHandle;
	ECGHandle->spiDMAReceiveBufferPointer =ECGHandle->spiDMAReceiveBuffer1;
	ECGHandle->acqStarted = 0;
	ECGHandle->writeFIFOPointer = 0;
	ECGHandle->readFIFOPointer = 0;
	ECGHandle->dataFIFOAvailable = 0;
	ECGHandle->forwardFIFO = 1;
	ECGHandle->ecgStatus = ECG_OK;

#ifdef ECG_Du_v2_Board
	mSPI_Config(spiHandle);
	
	//HAL_Delay(200);																									//After power supply is turned on it is necessary to wait 200ms 
  HAL_GPIO_WritePin(ECG_RESET_PORT,ECG_RESET_PIN,GPIO_PIN_RESET);		//Reset ECG
	HAL_Delay(1);																											//Wait at least 2Tclk = 2*500ns = 1us
	HAL_GPIO_WritePin(ECG_RESET_PORT,ECG_RESET_PIN,GPIO_PIN_SET);
	HAL_Delay(1);																											//Wait reset command at least 18Tclk = 18*500 = 9us
	Send_Comand_ECG(spiHandle,ECG_SDATAC_CMD);   											// SDATAC command
	
	HAL_Delay(1);
	
	//------------------ID Register----------------------------//
	//|7:5| 	Device ID (100 = ADS129x family,110 = ADS129xR family)
	//|4:3|		Reserved = always read back 2h			
	//|2:0|		Channel ID = (000 = 4channel, 001 = 6channel, 002 = 8channel)
	read_value=Read_Register_ECG(spiHandle,ECG_ID);										
	
	//----------------CONFIG1 Register----------------------------//
	//|7|			High resolution or low power mode(0 = LP mode, 1=HR mode)
	//|6|			Daisy chain or multiple read back mode (0 = Daisy chain mode, 1 = multiple readback mode)
	//|5|			Clock connection (0 = Oscillator clock output disabled, 1 = Oscillator clock output enabled)
	//|4:3|		Reserved
	//|2:0|		Output data rate (110 = HR mode 500SPS(LP mode 250SPS), 101 = HR mode 1000SPS(LP mode 500SPS),...000 = HR mode 32kSPS (LP mode 16kSPS))	
	Write_Register_ECG(spiHandle,ECG_CONFIG1,0x86); 									//CONFIG1 = HR, 500SPS					
	read_value=Read_Register_ECG(spiHandle,ECG_CONFIG1);
	if(read_value!=0x86) error_ecg = 0x01;
	
	//----------------CONFIG2 Register----------------------------//
	//|7:6|		Reserved = always write 0
	//|5|			WCT chopping scheme (0 = Chopping frequency varies, 1 = chopping frequency constant at fMOD/16)
	//|4|			TEST source (0 = Test signals are driven externaly, 1 = Test signals are generated internally)
	//|3|			Reserved = always write 0
	//|2|			Test signal amplitude (0 = 1x*(VREFP-VREFN)/2400V, 2x(VREFP-VREFN)/2400V)
	//|1:0|		Test signal frequency (00 = fclk/2^21, 01 = fclk/2^20)
	Write_Register_ECG(spiHandle,ECG_CONFIG2,0x10);										//CONFIG2 = TEST internall 
	read_value=Read_Register_ECG(spiHandle,ECG_CONFIG2);
	if(read_value!=0x10) error_ecg = 0x01;
	
	//----------------CONFIG3 Register----------------------------//
	//|7|			Power down reference buffer (0 = Power down internal reference buffer, 1 = Enable internal reference buffer)
	//|6|			Reserved = always write 1
	//|5|			Reference voltage (0 = VREFP set to 2.4V, 1 = VREFP set to 4V (only with 5V analog supply))
	//|4|			RLD measurement (0 = Open, 1 = RLD_IN signal is routed to channel with MUX_Setting 010)
	//|3|			RLDREF signal (0 = RLDREF signal fed externally, 1 = RLDREF signal (AVDD-AVSS)/2 generated internally)
	//|2|			RLD buffer power (0 = RLD buffer is powered down, 1 = RLD buffer is enabled)
	//|1|			RLD sense function (0 = RLD sense is disabled, 1 = RLD sense is enabled)
	//|0| 		RLD lead off status (0 = RLD is connected, 1 = RLD is not connected)
	Write_Register_ECG(spiHandle,ECG_CONFIG3,0xDC);										//CONFIG3 =  Internal reference enabled, RLD_IN routed, RLDREF generated internally, RLD enabled
	read_value=Read_Register_ECG(spiHandle,ECG_CONFIG3);
	if(read_value!=0xDC) error_ecg = 0x01;
	
	//-----------------CHnSET Register----------------------------//
	//|7|			Power down (0 = Normal operation, 1 = Channel power down)
	//|6:4|		Gain (000 = 6, 001 = 1, 010 = 2, 011 = 3, 100 = 4, 101 = 8, 110 = 12)
	//|3|			Reserved = always write 0
	//|2:0|		Channel input (000 = Normal electrode, 001 = Input shorted (for noise or offset), 010 = RLD measurement, 011 = MVDD, 100 = Temperature, 101 = Test, 110 = RLD_DRP, 111 = RLD_DRN)

#ifdef ECG_TEST_DATA
	Write_Register_ECG(spiHandle,ECG_CH1SET,0x25);    								//CH1SET  = Gain 2, Test electrode	  
	read_value=Read_Register_ECG(spiHandle,ECG_CH1SET);
	if(read_value!=0x25) error_ecg = 0x01;
	
	Write_Register_ECG(spiHandle,ECG_CH2SET,0x25);    								//CH2SET  = Gain 2, Test electrode
	read_value=Read_Register_ECG(spiHandle,ECG_CH2SET);
	if(read_value!=0x25) error_ecg = 0x01;
	
  Write_Register_ECG(spiHandle,ECG_CH3SET,0x25);    								//CH3SET  = Gain 2, Test electrode
	read_value=Read_Register_ECG(spiHandle,ECG_CH3SET);
	if(read_value!=0x25) error_ecg = 0x01;
	
  Write_Register_ECG(spiHandle,ECG_CH4SET,0x80);    								//CH4SET  = Powered down
	read_value=Read_Register_ECG(spiHandle,ECG_CH4SET);
	if(read_value!=0x80) error_ecg = 0x01;
#else
	Write_Register_ECG(spiHandle,ECG_CH1SET,0x20);    								//CH1SET  = Gain 2, Normal electrode	  
	read_value=Read_Register_ECG(spiHandle,ECG_CH1SET);
	if(read_value!=0x20) error_ecg = 0x01;
	
	Write_Register_ECG(spiHandle,ECG_CH2SET,0x20);    								//CH2SET  = Gain 2, Normal electrode
	read_value=Read_Register_ECG(spiHandle,ECG_CH2SET);
	if(read_value!=0x20) error_ecg = 0x01;
	
  Write_Register_ECG(spiHandle,ECG_CH3SET,0x20);    								//CH3SET  = Gain 2, Normal electrode
	read_value=Read_Register_ECG(spiHandle,ECG_CH3SET);
	if(read_value!=0x20) error_ecg = 0x01;
	
  Write_Register_ECG(spiHandle,ECG_CH4SET,0x80);    								//CH4SET  = Powered down
	read_value=Read_Register_ECG(spiHandle,ECG_CH4SET);
	if(read_value!=0x80) error_ecg = 0x01;
#endif
	
	//-----------------RLD_SENSP Register----------------------------//
	//|x|			Route channel x to positive signal derivation (0 = disabled, 1 = enabled)	
	Write_Register_ECG(spiHandle,ECG_RLD_SENSP,0x03);									//RLD_SENSP = sense from channel 1 and 2
	read_value=Read_Register_ECG(spiHandle,ECG_RLD_SENSP);
	if(read_value!=0x03) error_ecg = 0x01;
	
	//-----------------RLD_SENSN Register----------------------------//
	//|x|			Route channel x to negative signal derivation (0 = disabled, 1 = enabled)	
	Write_Register_ECG(spiHandle,ECG_RLD_SENSN,0x00);									//RLD_SENSN = sense from chanel 1 and 2
	read_value=Read_Register_ECG(spiHandle,ECG_RLD_SENSN);
	if(read_value!=0x00) error_ecg = 0x01;
	
	//-----------------CONFIG4 Register----------------------------//
	//|7:5|		Respiration frequency (000 = 64kHz modulation clock,...)
	//|4|			Reserved  =  always write 0
	//|3|			Single shot conversion (0 = continuous conversion mode, 1 = single shot mode)
	//|2|			Connect the WCT to RLD (0 = WCT to RLD off, 1 = WCT to RLD on)
	//|1|			Lead off comparator power down (0 = lead off comparators disabled, 1 = lead off comparators enabled)
	//|0|			Reserved = always write 0
	Write_Register_ECG(spiHandle,ECG_CONFIG4,0x00);										//CONFIG4  
	read_value=Read_Register_ECG(spiHandle,ECG_CONFIG4);
	if(read_value!=0x00) error_ecg = 0x01;
	
	//-----------------WCT1 Register----------------------------//
	//|7|			Enable (WCTA+WCTB)/2 to the negative input of ch 6 (0 = Disabled, 1 = Enabled)
	//|6|			Enable (WCTA+WCTC)/2 to the negative input of ch 5 (0 = Disabled, 1 = Enabled)
	//|5|			Enable (WCTB+WCTC)/2 to the negative input of ch 7 (0 = Disabled, 1 = Enabled)
	//|4|			Enable (WCTB+WCTC)/2 to the negative input of ch 4 (0 = Disabled, 1 = Enabled)
	//|3|			Power down WCTA (0 = Powered down, 1 = Powered on)
	//|2:0|		WCTA channel selection (000 = CH1P,001 = CH1N, 010 = CH2P, 011 = CH2N, 100 = CH3P, 101 = CH3N, 110 = CH4P, 111 = CH4N)				
	Write_Register_ECG(spiHandle,ECG_WCT1,0x08);										//WCT1 = WCTA enabled, CH1P(LF) routed to WCTA
	read_value=Read_Register_ECG(spiHandle,ECG_WCT1);
	if(read_value!=0x08) error_ecg = 0x01;
	
	//-----------------WCT2 Register----------------------------//
	//|7|			Power down WCTC (0 = Power down, 1 = Powered on)
	//|6|			Power down WCTB (0 = Power down, 1 = Powered on)
	//|5:3|		WCTB channel selection (000 = CH1P,001 = CH1N, 010 = CH2P, 011 = CH2N, 100 = CH3P, 101 = CH3N, 110 = CH4P, 111 = CH4N)	
	//|2:0|		WCTC channel selection (000 = CH1P,001 = CH1N, 010 = CH2P, 011 = CH2N, 100 = CH3P, 101 = CH3N, 110 = CH4P, 111 = CH4N)
	Write_Register_ECG(spiHandle,ECG_WCT2,0x48);									//WCT2 = WCTB enabled, CH1N (RF) routed to WCTB
	read_value=Read_Register_ECG(spiHandle,ECG_WCT2);
	if(read_value!=0x48) error_ecg = 0x01;
	
	Send_Comand_ECG(ECGHandle->spiHandle,ECG_STANDBY_CMD);			// STANDBY command	
	
	if(error_ecg){
		ECGHandle->ecgStatus = ECG_ERROR;
		ECGHandle->ErrorNumber = ECG_INIT_ERROR;
	}
#endif
}
void ECG_Start_Acquisition(ECG_TypeDef* ECGHandle){	
	ECGHandle->spiDMAReceiveBufferPointer =ECGHandle->spiDMAReceiveBuffer1;
	ECGHandle->acqStarted = 0;
	ECGHandle->writeFIFOPointer = 0;
	ECGHandle->readFIFOPointer = 0;
	ECGHandle->dataFIFOAvailable = 0;
	ECGHandle->forwardFIFO = 1;
	ECGHandle->ecgStatus = ECG_OK;
	
	Send_Comand_ECG(ECGHandle->spiHandle,ECG_WAKEUP_CMD);			// WAKEUP command
	HAL_Delay(15);	
	Send_Comand_ECG(ECGHandle->spiHandle,ECG_START_CMD);			// START command
	Send_Comand_ECG(ECGHandle->spiHandle,ECG_RDATAC_CMD);			// RDATAC command
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4, GPIO_PIN_RESET);			//ECG CS = LOW

#ifndef MCU_TEST_DATA
	ECGHandle->acqStarted = 1;
#endif
}
void ECG_Stop_Acquisition(ECG_TypeDef* ECGHandle){
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4, GPIO_PIN_SET);				//ECG CS = HIGH
	Send_Comand_ECG(ECGHandle->spiHandle,ECG_STOP_CMD);				// STOP command
	Send_Comand_ECG(ECGHandle->spiHandle,ECG_SDATAC_CMD);   	// SDATAC command
	
	Send_Comand_ECG(ECGHandle->spiHandle,ECG_STANDBY_CMD);		// STANDBY command	
	ECGHandle->acqStarted = 0;
}


void Send_Comand_ECG(SPI_HandleTypeDef* spiHandle,  uint8_t comand){
	HAL_GPIO_WritePin(ECG_CS_PORT,ECG_CS_PIN,GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_SPI_Transmit(spiHandle, &comand, 1, 5);
	HAL_Delay(1);
	HAL_GPIO_WritePin(ECG_CS_PORT,ECG_CS_PIN,GPIO_PIN_SET);
}

void Write_Register_ECG(SPI_HandleTypeDef* spiHandle,uint8_t address, uint8_t data){
	uint8_t transmitBuffer[2]={0,0};
	
	transmitBuffer[0]=(address & 0x1F) | 0x40;
	transmitBuffer[1]=0x00;
	
	HAL_GPIO_WritePin(ECG_CS_PORT,ECG_CS_PIN,GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_SPI_Transmit(spiHandle, (uint8_t*)transmitBuffer, 1, 5);
	HAL_SPI_Transmit(spiHandle, transmitBuffer+1, 1, 5);
	HAL_SPI_Transmit(spiHandle, &data, 1, 5);
	HAL_Delay(1);
	HAL_GPIO_WritePin(ECG_CS_PORT,ECG_CS_PIN,GPIO_PIN_SET);
}
uint8_t Read_Register_ECG(SPI_HandleTypeDef* spiHandle,uint8_t address){
	uint8_t data=0;
	uint8_t transmitBuffer[2]={0,0};
	
	transmitBuffer[0]=(address & 0x1F) | 0x20;
	transmitBuffer[1]=0x00;
	
	HAL_GPIO_WritePin(ECG_CS_PORT,ECG_CS_PIN,GPIO_PIN_RESET);
	HAL_SPI_Transmit(spiHandle, &transmitBuffer[0], 1, 5);
	//HAL_Delay(1);
	HAL_SPI_Transmit(spiHandle, transmitBuffer+1, 1, 5);
	HAL_SPI_Receive(spiHandle, &data, 1, 1);
	
	HAL_GPIO_WritePin(ECG_CS_PORT,ECG_CS_PIN,GPIO_PIN_SET);

	return data;
}

ECG_StatusTypeDef ECG_WriteFIFOData(ECG_TypeDef* ECGHandle, uint8_t* inputDataBuffer, uint8_t chNum, uint16_t size){
	uint16_t i;
	uint8_t * pFIFOBuffer;
	uint16_t writeFIFOPointer;
	uint8_t forwardFIFO;
	if(size>ECG_FIFO_SIZE) return ECG_ERROR;
	
	switch(chNum){
				case 1:
					pFIFOBuffer = ECGHandle->ecgFIFOBufferCH1;
					writeFIFOPointer = ECGHandle->writeFIFOPointer;
					forwardFIFO = ECGHandle->forwardFIFO;
					break;
				case 2:
					pFIFOBuffer = ECGHandle->ecgFIFOBufferCH2;
					writeFIFOPointer = ECGHandle->writeFIFOPointer;
					forwardFIFO = ECGHandle->forwardFIFO;
					break;
				case 3:
					pFIFOBuffer = ECGHandle->ecgFIFOBufferCH3;
					break;
				default:
					return ECG_ERROR;
	}
	
	if(chNum==3){
		for(i=0;i<size;i++){
			if(ECGHandle->writeFIFOPointer >= ECG_FIFO_SIZE){
				ECGHandle->forwardFIFO ^= 0x01;
				ECGHandle->writeFIFOPointer = 0;
			}
			if((!ECGHandle->forwardFIFO) && (ECGHandle->writeFIFOPointer == ECGHandle->readFIFOPointer)) {
				ECGHandle->ecgStatus = ECG_ERROR;
				ECGHandle->ErrorNumber = ECG_FIFO_ERROR;
				return ECG_ERROR;
			}
			pFIFOBuffer[ECGHandle->writeFIFOPointer] = inputDataBuffer[i];
			ECGHandle->writeFIFOPointer++;
			ECGHandle->dataFIFOAvailable++;			
		}
	}
	else{
		for(i=0;i<size;i++){
			if(writeFIFOPointer >= ECG_FIFO_SIZE){
				forwardFIFO ^= 0x01;
				writeFIFOPointer = 0;
			}
			if((!forwardFIFO) && (writeFIFOPointer == ECGHandle->readFIFOPointer)) {
				ECGHandle->ecgStatus = ECG_ERROR;
				ECGHandle->ErrorNumber = ECG_FIFO_ERROR;				
				return ECG_ERROR;
			}
			pFIFOBuffer[writeFIFOPointer] = inputDataBuffer[i];
			writeFIFOPointer++;			
		}
	}
	return ECG_OK;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == GPIO_PIN_3){
		mTimer_LBlinkStatus_Stop(&mTimHandle);
		mTimer_LBlinkError_Stop(&mTimHandle);
		mTimHandle.lblinkUSBCharge = 1;
	}
	else if(GPIO_Pin == GPIO_PIN_8){
		if(ECGHandle.acqStarted){			
			HAL_SPI_Receive_DMA(ECGHandle.spiHandle,(uint8_t*) ECGHandle.spiDMAReceiveBufferPointer,12);					
		}
	}
	else if(GPIO_Pin == GPIO_PIN_12){
		ACC_Interrupt_Callback();
	}
}
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi){
	uint8_t i;
	if(ECGHandle.acqStarted){
#ifndef MCU_TEST_DATA
		ECG_WriteFIFOData(&ECGHandle,(uint8_t*) ECGHandle.spiDMAReceiveBufferPointer+3,1,3);
		ECG_WriteFIFOData(&ECGHandle,(uint8_t*) ECGHandle.spiDMAReceiveBufferPointer+6,2,3);
		ECG_WriteFIFOData(&ECGHandle,(uint8_t*) ECGHandle.spiDMAReceiveBufferPointer+9,3,3);
		for(i=0;i<12;i++){
			ECGHandle.spiDMAReceiveBufferPointer[i] = 0;
		}
#endif		
		if(ECGHandle.spiDMAReceiveBufferPointer == ECGHandle.spiDMAReceiveBuffer1){
			ECGHandle.spiDMAReceiveBufferPointer = ECGHandle.spiDMAReceiveBuffer2;
		}
		else{
			ECGHandle.spiDMAReceiveBufferPointer = ECGHandle.spiDMAReceiveBuffer1;
		}		
	}
}
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
	ECGHandle.ecgStatus = ECG_ERROR;
	ECGHandle.ErrorNumber = ECG_SPI_ERROR;
}
