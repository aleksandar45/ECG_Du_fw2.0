#include "mADC.h"



void mADC_Config(ADC_HandleTypeDef* AdcHandle)
{
	ADC_ChannelConfTypeDef   sConfig;
  
  AdcHandle->Instance = ADC1;

  AdcHandle->Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV4;
  AdcHandle->Init.Resolution            = ADC_RESOLUTION12b;
  AdcHandle->Init.DataAlign             = ADC_DATAALIGN_RIGHT;
  AdcHandle->Init.ScanConvMode          = ENABLE;                       // Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) 
  AdcHandle->Init.EOCSelection          = EOC_SINGLE_CONV;
  AdcHandle->Init.LowPowerAutoWait      = ENABLE;                        // Enable the dynamic low power Auto Delay: new conversion start only when the previous conversion (for regular group) or previous sequence (for injected group) has been treated by user software.//

  AdcHandle->Init.ContinuousConvMode    = DISABLE;                       // Continuous mode disabled to have only 1 conversion at each conversion trig //
  AdcHandle->Init.NbrOfConversion       = 2;                             // Parameter discarded because sequencer is disabled //
  AdcHandle->Init.DiscontinuousConvMode = DISABLE;                       // Parameter discarded because sequencer is disabled //
  AdcHandle->Init.NbrOfDiscConversion   = 1;                             // Parameter discarded because sequencer is disabled //
  AdcHandle->Init.ExternalTrigConv      = ADC_EXTERNALTRIG_T3_TRGO;  		 // Trig of conversion start done by external event //
  AdcHandle->Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_RISING;
  AdcHandle->Init.DMAContinuousRequests = ENABLE;
  AdcHandle->Init.Overrun               = ADC_OVR_DATA_PRESERVED;        /* Overrun set to data preserved to trig overrun event as an error in this exemple */
  AdcHandle->Init.OversamplingMode      = DISABLE;                       /* No oversampling */

  if (HAL_ADC_Init(AdcHandle) != HAL_OK)
  {   
    //Error_Handler();
  }
 
	sConfig.Channel      = ADC_CHANNEL_15;                // Sampled channel number 
  sConfig.Rank         = ADC_REGULAR_RANK_1;          	// Rank of sampled channel number  
  sConfig.SamplingTime = ADC_SAMPLETIME_47CYCLES_5;    	// Sampling time (number of clock cycles unit) 
  sConfig.SingleDiff   = ADC_SINGLE_ENDED;            	// Single-ended input channel 
  sConfig.OffsetNumber = ADC_OFFSET_NONE;             	// No offset subtraction 
  sConfig.Offset = 0;         
  
  if (HAL_ADC_ConfigChannel(AdcHandle, &sConfig) != HAL_OK)
  {    
    //Error_Handler();
  }
	
	sConfig.Channel      = ADC_CHANNEL_VREFINT;           // Vrefint channel
  sConfig.Rank         = ADC_REGULAR_RANK_2;          	// Rank of sampled channel number  
  sConfig.SamplingTime = ADC_SAMPLETIME_47CYCLES_5;    	// Sampling time (number of clock cycles unit) 
  sConfig.SingleDiff   = ADC_SINGLE_ENDED;            	// Single-ended input channel 
  sConfig.OffsetNumber = ADC_OFFSET_NONE;             	// No offset subtraction 
  sConfig.Offset = 0;         
  
  if (HAL_ADC_ConfigChannel(AdcHandle, &sConfig) != HAL_OK)
  {    
    //Error_Handler();
  }
	if (HAL_ADCEx_Calibration_Start(AdcHandle, sConfig.SingleDiff) != HAL_OK)
  {    
    //Error_Handler();
  }
}
