#include "mPWR.h"

extern BLE_TypeDef	BLEHandle;
extern ECG_TypeDef ECGHandle;
extern BATT_TypeDef BATTHandle;

extern TIM_HandleTypeDef    TimHandle;
extern mTimerHandler_TypeDef  mTimHandle;

void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /* MSI is enabled after System reset, activate PLL with MSI as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 4;								//Number between 1 and 8.
  RCC_OscInitStruct.PLL.PLLN = 16;							//Number between 8 and 86.
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;		//PLLCLK = ((MSICLK/M)*N)/R = ((4MHz/4)*16)/2 = 8MHz
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;		//PLL48M1CLK = ((MSICLK/M)*N)/Q = ((4MHz/4)*16)/2 = 8MHz
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV8;		//PLLSAI3CLK = ((MSICLK/M)*N)/P = ((4MHz/4)*16)/8 = 2MHz
  
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    //while(1);
  }
  
  // Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers 
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    /* Initialization Error */
    //while(1);
  }
	
	//Disable HSI Oscillator
  RCC_OscInitStruct.OscillatorType  = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState        = RCC_HSI_OFF;
  RCC_OscInitStruct.PLL.PLLState    = RCC_PLL_NONE;  /* No update on PLL */
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    //while(1);
  }
}


void EnterLowEnergyMODE(void){
	SystemClockMSI_Config();			//Configure 8MHz clock
	
	BLEHandle.uartBufferForward = 1;
	BLEHandle.uartDataAvailable = 0;
	BLEHandle.uartBufferReadPointer = BLEHandle.uartReceiveBuffer;
	BLEHandle.uartHandle->RxState =HAL_UART_STATE_READY;
	mUART_Config(BLEHandle.uartHandle,BLEHandle.uartBaudRate,BLEHandle.uartHWControl);
	mUART_Receive_IT(BLEHandle.uartHandle,BLEHandle.uartReceiveBuffer,200);
	
	mTimer_Config(&TimHandle,&mTimHandle);
	
	
	BATTHandle.adcValuesCounter = 0;
	BATTHandle.adcBufferFull = 0;
	//BATTHandle.lowLevelBattery = 0;
	//BATT_StartMeasure(&BATTHandle);	
}
void EnterHighEnergyMODE(void){
	SystemClockHSI_Config();			//Configure 48MHz clock
	
	BLEHandle.uartBufferForward = 1;
	BLEHandle.uartDataAvailable = 0;
	BLEHandle.uartBufferReadPointer = BLEHandle.uartReceiveBuffer;
	BLEHandle.uartHandle->RxState =HAL_UART_STATE_READY;
	mUART_Config(BLEHandle.uartHandle,BLEHandle.uartBaudRate,BLEHandle.uartHWControl);
	mUART_Receive_IT(BLEHandle.uartHandle,BLEHandle.uartReceiveBuffer,200);
	
	mTimer_Config(&TimHandle,&mTimHandle);	
	//BATT_StartMeasure(&BATTHandle);	
	BATTHandle.adcValuesCounter = 0;
	BATTHandle.adcBufferFull = 0;
	//BATTHandle.lowLevelBattery = 0;
}

void EnterStandByMODE(void){
	
	HAL_UART_DeInit(BLEHandle.uartHandle);
	HAL_ADC_MspDeInit(BATTHandle.adcHandle);
	HAL_SPI_MspDeInit(ECGHandle.spiHandle);
	__HAL_RCC_DMA1_CLK_DISABLE();
	__HAL_RCC_GPIOA_CLK_DISABLE();	
	__HAL_RCC_GPIOB_CLK_DISABLE();
	__HAL_RCC_GPIOC_CLK_DISABLE();
			
	HAL_PWREx_EnablePullUpPullDownConfig();	

#ifdef RN4871_Nucleo_Test_Board	
	HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_A,PWR_GPIO_BIT_11);		// BT_UART_RX_IND
	
	 HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN2);		// Disable all used wakeup sources: WKUP pin 

  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF2);					// Clear wake up Flag

	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN2_LOW);	// Enable wakeup pin WKUP2 
	
#endif
#ifdef ECG_Du_v1_Board
		
	HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_A,PWR_GPIO_BIT_15);		// BT_UART_RX_IND
	HAL_PWREx_EnableGPIOPullDown(PWR_GPIO_B,PWR_GPIO_BIT_1);	// EN_BAT
	
  HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN2);		// Disable all used wakeup sources: WKUP pin 

  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF2);					// Clear wake up Flag

	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN2_HIGH);	// Enable wakeup pin WKUP2 
  
#endif
	
  HAL_PWR_EnterSTANDBYMode();										// Request to enter STANDBY mode 
	
	//HAL_PWR_EnterSHUTDOWNMode();
	//HAL_PWREx_EnterSHUTDOWNMode();
	HAL_NVIC_SystemReset();
}

void SystemClockHSI_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  // Select MSI as system clock source to allow modification of the PLL configuration 
  RCC_ClkInitStruct.ClockType       = RCC_CLOCKTYPE_SYSCLK;
  RCC_ClkInitStruct.SYSCLKSource    = RCC_SYSCLKSOURCE_MSI;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    /* Initialization Error */
    //Error_Handler();
  }
	

  //Enable HSI Oscillator, select it as PLL source and finally activate the PLL //
  RCC_OscInitStruct.OscillatorType       = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState             = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue  = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState         = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource        = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 4;								//Number between 1 and 8.
  RCC_OscInitStruct.PLL.PLLN = 24;							//Number between 8 and 86.
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;		//PLLCLK = ((HSICLK/M)*N)/R = ((16MHz/4)*24)/2 = 48MHz
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;		//PLL48M1CLK = ((HSICLK/M)*N)/Q = ((16MHz/4)*24)/2 = 48MHz
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV8;		//PLLSAI3CLK = ((HSICLK/M)*N)/P = ((16MHz/4)*24)/8 = 12MHz
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    // Initialization Error 
    //Error_Handler();
  }

  //Select the PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers
  RCC_ClkInitStruct.ClockType       = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource    = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider   = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider  = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider  = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    // Initialization Error 
    //Error_Handler();
  }

  //Optional: Disable MSI Oscillator 
  RCC_OscInitStruct.OscillatorType  = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState        = RCC_MSI_OFF;
  RCC_OscInitStruct.PLL.PLLState    = RCC_PLL_NONE;  // No update on PLL 
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    // Initialization Error 
    //Error_Handler();
  }
}

void SystemClockMSI_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /* -1- Select HSI as system clock source to allow modification of the PLL configuration */
  RCC_ClkInitStruct.ClockType       = RCC_CLOCKTYPE_SYSCLK;
  RCC_ClkInitStruct.SYSCLKSource    = RCC_SYSCLKSOURCE_HSI;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    //Error_Handler();
  }

  /* -2- Enable MSI Oscillator, select it as PLL source and finally activate the PLL */
  RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState            = RCC_MSI_ON;
  RCC_OscInitStruct.MSIClockRange       = RCC_MSIRANGE_6;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 4;								//Number between 1 and 8.
  RCC_OscInitStruct.PLL.PLLN = 16;							//Number between 8 and 86.
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;		//PLLCLK = ((MSICLK/M)*N)/R = ((4MHz/4)*16)/2 = 8MHz
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;		//PLL48M1CLK = ((MSICLK/M)*N)/Q = ((4MHz/4)*16)/2 = 8MHz
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV8;		//PLLSAI3CLK = ((MSICLK/M)*N)/P = ((4MHz/4)*16)/8 = 2MHz
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    //Error_Handler();
  }

  /* -3- Select the PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
  RCC_ClkInitStruct.ClockType       = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource    = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider   = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider  = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider  = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    //Error_Handler();
  }

  /* -4- Optional: Disable HSI Oscillator (if the HSI is no more needed by the application)*/
  RCC_OscInitStruct.OscillatorType  = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState        = RCC_HSI_OFF;
  RCC_OscInitStruct.PLL.PLLState    = RCC_PLL_NONE;  /* No update on PLL */
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    //Error_Handler();
  }
}
