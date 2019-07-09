#include "datatype.h"  
#include "conf.h"
#include "main.h"
#include "eeprom.h"

void readProtect(void)
{
         // ���϶�����

        FLASH_Unlock();
    FLASH_OB_Unlock();

    if(FLASH_OB_GetRDP() != SET)
     {  
  
          FLASH_OB_RDPConfig(OB_RDP_Level_1);//����������
					FLASH_OB_Launch();
					FLASH_OB_Lock();
					FLASH_Lock();        
     }

						// (2)�����������   �ϵ�ʱ���ĸ���ťȫ���¡��������������������Ҳ��������������ճ���
//		if (isEscKey() && isUpKey() && isDownKey() && isEnterKey())
//		{	
//				if(FLASH_OB_GetRDP() == SET)
//				{
//						FLASH_Unlock();
//						FLASH_OB_Unlock();

//               FLASH_OB_RDPConfig(OB_RDP_Level_0);//
//					FLASH_OB_Launch();
//							FLASH_OB_Lock();
//								FLASH_Lock(); 
//				}
//		}
}
void initPLL()
{
#ifndef DEBUG
	readProtect();
#endif
}

void initWatchdog(void)
{
		IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);//�ر�IWDG_PR��IWDG_RLR��д����
		IWDG_SetReload(0xfff);//������װ��ֵΪ0xfff
		IWDG_SetPrescaler(IWDG_Prescaler_32);//����Ԥ��Ƶϵ��Ϊ32
		IWDG_ReloadCounter();
		IWDG_Enable();//ʹ�ܿ��Ź� 
}



void CalGS()
{

}
void CalPS()
{

}

#include "modbus.h"

void setCom(uint8 baudrate,uint8 parity)
{
		uint16_t br;
		USART_InitTypeDef USART_InitStructure;

	
		USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);
		USART_ITConfig(USART1,USART_IT_TC,DISABLE);
		USART_Cmd(USART1, DISABLE);	 
		/*-------------------------USART�����趨------------------------*/
		if(baudrate==0x0)br=600;		    //  600
		else if(baudrate==0x1)br=1200;	// 1200
		else if(baudrate==0x2)br=2400;	// 2400
		else if(baudrate==0x3)br=4800;	// 4800
		else if(baudrate==0x4)br=9600;	//9600
		
		USART_InitStructure.USART_BaudRate = br;//���ô��ڲ�����
		
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//����ֹͣλ
		if(parity==0){USART_InitStructure.USART_WordLength = USART_WordLength_8b;	USART_InitStructure.USART_Parity = USART_Parity_No;}
		if(parity==1){USART_InitStructure.USART_WordLength = USART_WordLength_9b; USART_InitStructure.USART_Parity = USART_Parity_Odd ;}// ��У��
		if(parity==2){USART_InitStructure.USART_WordLength = USART_WordLength_9b; USART_InitStructure.USART_Parity = USART_Parity_Even ;}// żУ��		
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//����������
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//���ù���ģʽ
	
		USART_Init(USART1, &USART_InitStructure); //������ṹ��
		USART_ClearFlag(USART1,USART_FLAG_TC);
		USART_ITConfig(USART1,USART_IT_TC,ENABLE);
		USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
		USART_Cmd(USART1, ENABLE);//ʹ�ܴ���1

}
void setBaudrate(uint8 baudrate)
{
		setCom(baudrate,config.parity);
}
void setParity(uint8 parity)
{		
	setCom(config.baundrate,parity);
}
void Enable485TXD(void) {
	/* PA14 -> TXD */
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_14;                 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);   
	/*-----------------�򿪸��ù���------------------*/				
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource14,GPIO_AF_1);    
}
void Disable485TXD(void) 
{
		/* PA14 -> TXD */
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_14;                 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);   
	/*-----------------�رո��ù���------------------*/				
//	GPIO_PinAFConfig(GPIOA,GPIO_PinSource14,GPIO_AF_0);   
	
}
void init485()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;		
	

// 	INITRS485PORT();	
// �رմ���

	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE );
	
	Disable485TXD();
	/*
	*  USART1_TX -> PA9 , USART1_RX ->  PA10
	*/ 
 /*------------------����USART�˿�-----------------------------*/	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;                 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);   
	/*-----------------�򿪸��ù���------------------*/				
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_1);      
	/*-----------------------�ж���������----------------------*/
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	/*-------------------------USART�����趨------------------------*/
	setCom(config.baundrate,config.parity);
}
unsigned char CheckEven(unsigned char becheck) 
{
		return(0);
}
void startTxd()
{

#ifndef DISABLE_MODBUS
	Enable485TXD();
	USART_SendData(USART1,getTxdByte()); 
#endif
}

void USART1_IRQHandler(void)
{	
	uint8 c;
	if(USART_GetITStatus(USART1,USART_IT_RXNE)!= RESET)  //�����ж�
	{
//		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
#ifndef DISABLE_MODBUS
			RxdByte((uint8_t)(USART_ReceiveData(USART1)));		   // �յ�һ���ֽ�
#endif
	}
	else if(USART_GetITStatus(USART1,USART_IT_TC)!= RESET)   // ���ͻ����
	{			
		USART_ClearITPendingBit(USART1,USART_IT_TC);
//			USART_SendData(USART1,0x55);
#ifndef DISABLE_MODBUS
		if(hasByteToTxd())		{
			Enable485TXD();
			c=getTxdByte();
			USART_SendData(USART1,c);
		}
		else	{
			Disable485TXD();
		}
#endif
	}	else 	{
		USART1->ICR=0xffffffff;
	}

}	

//-----------------------------------------------------|
//��ʱ��0�ж��ӳ���	  1ms һ���ж� ������ϵͳ��ʱ�͵���
//-----------------------------------------------------|
//************************************************
//***********************************************/
extern void On1ms(void);   // main.c�ж���
void initSysTick1ms()
{
		NVIC_InitTypeDef NVIC_InitStructure;
		NVIC_InitStructure.NVIC_IRQChannel = TIM16_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		
		TIM_ITConfig(TIM16, TIM_IT_Update, ENABLE);  //���ж� 
		if (SysTick_Config(SystemCoreClock / 1000))//1����
		{ 
			/* Capture error */ 
			while (1);
		}
}
//void Init_Time16(uint16_t num)    //  num ��ʱʱ����  1uS �ı���
//{
//	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure; 
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE);

//	TIM_ITConfig(TIM16, TIM_IT_Update, DISABLE);
//	TIM_DeInit(TIM16);
//	
//	  /*  TIM16 �ж����ȼ� */
//  TIM_TimeBaseStructure.TIM_Period = num;       //��ʱ������ֵ 
//  TIM_TimeBaseStructure.TIM_Prescaler =47;    //ʱ��Ԥ��Ƶֵ(1uS)
//  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
//  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //����ģʽ(���ϼ���)
//	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
//  TIM_TimeBaseInit(TIM16, &TIM_TimeBaseStructure);
//  
//	TIM_ClearITPendingBit(TIM16, TIM_IT_Update);
//	TIM_ITConfig(TIM16, TIM_IT_Update, ENABLE);
//	TIM_Cmd(TIM16,ENABLE);    // ������ʱ��
//}
/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */

void SysTick_Handler(void)
{
	On1ms();
}


//void TIM16_IRQHandler(void)
//{
//	if (TIM_GetITStatus(TIM16, TIM_IT_Update) != RESET)
//	{
//		TIM_ClearITPendingBit(TIM16, TIM_IT_Update); //����жϵȴ���־λ  
//		TIM_Cmd(TIM16,DISABLE); 

//	}
//}

