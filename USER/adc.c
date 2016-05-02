#include"include_all.h"
#define ADC1_DR_Address    ((u32)0x40012400+0x4c)

volatile u16 ADC_Running_ConvertedValue[4];
volatile u16 ADC_PhaseDet_ConvertedValue;
void ADC_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void ADC_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	/* DMA channel1 configuration */
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;  //ADC��ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_Running_ConvertedValue; //�ڴ��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 4;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //�����ַ�̶�
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַ����
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //����
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;     //ѭ������
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
	DMA_ClearITPendingBit(DMA1_IT_GL1 | DMA1_IT_TC1 | DMA1_IT_HT1);
	/* Enable DMA channel1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	/* ADC1 configuration */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //��ϵ�ͬ������+ע��ͬ��ģʽ
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;         //ʹ��ɨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;                  //����ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T4_CC4;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;              //�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 4; //����ͨ�����г��� ��8��ת��ͨ��
	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_7Cycles5); // SENSE_A																			//
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 2, ADC_SampleTime_7Cycles5); // SENSE_B
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 3, ADC_SampleTime_7Cycles5); // SENSE_C
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 4, ADC_SampleTime_7Cycles5);  // VIN


	ADC_InjectedSequencerLengthConfig(ADC1, 4);
	ADC_InjectedChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_7Cycles5);
	ADC_InjectedChannelConfig(ADC1, ADC_Channel_4, 2, ADC_SampleTime_7Cycles5);
	ADC_InjectedChannelConfig(ADC1, ADC_Channel_5, 3, ADC_SampleTime_7Cycles5);
	ADC_InjectedChannelConfig(ADC1, ADC_Channel_5, 4, ADC_SampleTime_7Cycles5);
	ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_None);
	ADC_ITConfig(ADC1,ADC_IT_JEOC,ENABLE);
	//ADC_ExternalTrigInjectedConvCmd(ADC1, DISABLE);
	ADC_Cmd(ADC1, ENABLE); 
	ADC_ResetCalibration(ADC1);
	while (ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while (ADC_GetCalibrationStatus(ADC1));
	ADC_ExternalTrigConvCmd(ADC1, ENABLE);
	ADC_DMACmd(ADC1, ENABLE);
	
}
void ADC_PhaseDectectMode_Config()
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	ADC_DeInit(ADC1);
	/* ADC1 configuration */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //��ϵ�ͬ������+ע��ͬ��ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;         //ʹ��ɨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;                  //����ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T4_CC4;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;              //�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1; //����ͨ�����г��� ��8��ת��ͨ��
	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_7Cycles5); // SENSE_A
	ADC_Cmd(ADC1, ENABLE); 
	ADC_ResetCalibration(ADC1);
	while (ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while (ADC_GetCalibrationStatus(ADC1));
	ADC_ExternalTrigConvCmd(ADC1, ENABLE); 
}
void ADC_SetDetTime(int time)
{
	TIM_SetCompare4(FET_H_TIMER, time);
}
void DMA1_Channel1_IRQHandler(void)
{
	if ((DMA1->ISR & DMA1_FLAG_TC1) != RESET)
	{
		DMA_ClearFlag(DMA1_FLAG_TC1);
		LED_POWER_ON;
		Voltage.Phase_A = ADC_Running_ConvertedValue[0] *3300 / 4096;
		Voltage.Phase_B = ADC_Running_ConvertedValue[1] *3300 / 4096;
		Voltage.Phase_C = ADC_Running_ConvertedValue[2] *3300 / 4096;
		Voltage.Battery = ADC_Running_ConvertedValue[3] *3300 / 4096;
		//�����жϵ�ǰ��ȡ����3���ѹֵ
        if (Motor_Status == Running)
		{
#if DEMAGNETIC
			if (Demagnetic_Delay_Cnt == 0)
			{
#if BREAK
				if (TIM_GetCapture3(FET_H_TIMER)<=300) Commutation_Detected_PWM_Off();
				else Commutation_Detected_PWM_On();
#else
				Commutation_Detected_PWM_On();
#endif
			}
			else Demagnetic_Delay_Cnt--; 
			Demagnetic_Cnt++;
#else
#if BREAK
			if (TIM_GetCapture3(FET_H_TIMER) <= 300) Commutation_Detected_PWM_Off();
			else Commutation_Detected_PWM_On();
#else
			Commutation_Detected_PWM_On();
#endif
#endif
			/*
			if (TIM_GetITStatus(TIM1,TIM_IT_CC1)!=RESET)
			{
				TIM2->SR = (uint16_t)~TIM_IT_CC1;
				TIM2->DIER &= (uint16_t)~TIM_IT_CC1;
				Time_Delay1_Switch = 0;
				Motor_Switch_Step(Motor_Config.NextStep_Detect);
			}
			if (TIM_GetITStatus(TIM1, TIM_IT_CC2) != RESET)
			{
				TIM2->SR = (uint16_t)~TIM_IT_CC2;
				TIM2->DIER &= (uint16_t)~TIM_IT_CC2;
				Time_Delay2_Switch = 0;
				Motor_Switch_Step(Motor_Config.Next_Step);
			}
			*/
		}
		LED_POWER_OFF;

	}
}





