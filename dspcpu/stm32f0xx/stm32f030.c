#include "datatype.h"  
#include "conf.h"
#include "main.h"
#include "eeprom.h"

void readProtect(void)
{
         // 加上读保护

        FLASH_Unlock();
    FLASH_OB_Unlock();

    if(FLASH_OB_GetRDP() != SET)
     {  
  
          FLASH_OB_RDPConfig(OB_RDP_Level_1);//开启读保护
					FLASH_OB_Launch();
					FLASH_OB_Lock();
					FLASH_Lock();        
     }

						// (2)解除读保护　   上电时，四个按钮全按下　，解除读保护，但程序也被清除，必须重烧程序。
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
		IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);//关闭IWDG_PR和IWDG_RLR的写保护
		IWDG_SetReload(0xfff);//设置重装载值为0xfff
		IWDG_SetPrescaler(IWDG_Prescaler_32);//设置预分频系数为32
		IWDG_ReloadCounter();
		IWDG_Enable();//使能看门狗 
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
		/*-------------------------USART参数设定------------------------*/
		if(baudrate==0x0)br=600;		    //  600
		else if(baudrate==0x1)br=1200;	// 1200
		else if(baudrate==0x2)br=2400;	// 2400
		else if(baudrate==0x3)br=4800;	// 4800
		else if(baudrate==0x4)br=9600;	//9600
		
		USART_InitStructure.USART_BaudRate = br;//设置串口波特率
		
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//设置停止位
		if(parity==0){USART_InitStructure.USART_WordLength = USART_WordLength_8b;	USART_InitStructure.USART_Parity = USART_Parity_No;}
		if(parity==1){USART_InitStructure.USART_WordLength = USART_WordLength_9b; USART_InitStructure.USART_Parity = USART_Parity_Odd ;}// 奇校验
		if(parity==2){USART_InitStructure.USART_WordLength = USART_WordLength_9b; USART_InitStructure.USART_Parity = USART_Parity_Even ;}// 偶校验		
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//设置流控制
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//设置工作模式
	
		USART_Init(USART1, &USART_InitStructure); //配置入结构体
		USART_ClearFlag(USART1,USART_FLAG_TC);
		USART_ITConfig(USART1,USART_IT_TC,ENABLE);
		USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
		USART_Cmd(USART1, ENABLE);//使能串口1

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
	/*-----------------打开复用功能------------------*/				
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
	/*-----------------关闭复用功能------------------*/				
//	GPIO_PinAFConfig(GPIOA,GPIO_PinSource14,GPIO_AF_0);   
	
}
void init485()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;		
	

// 	INITRS485PORT();	
// 关闭串口

	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE );
	
	Disable485TXD();
	/*
	*  USART1_TX -> PA9 , USART1_RX ->  PA10
	*/ 
 /*------------------配置USART端口-----------------------------*/	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;                 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);   
	/*-----------------打开复用功能------------------*/				
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_1);      
	/*-----------------------中断向量配置----------------------*/
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	/*-------------------------USART参数设定------------------------*/
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
	if(USART_GetITStatus(USART1,USART_IT_RXNE)!= RESET)  //接收中断
	{
//		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
#ifndef DISABLE_MODBUS
			RxdByte((uint8_t)(USART_ReceiveData(USART1)));		   // 收到一个字节
#endif
	}
	else if(USART_GetITStatus(USART1,USART_IT_TC)!= RESET)   // 发送缓冲空
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
//定时器0中断子程序	  1ms 一次中断 ，用于系统计时和调度
//-----------------------------------------------------|
//************************************************
//***********************************************/
extern void On1ms(void);   // main.c中定义
void initSysTick1ms()
{
		NVIC_InitTypeDef NVIC_InitStructure;
		NVIC_InitStructure.NVIC_IRQChannel = TIM16_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		
		TIM_ITConfig(TIM16, TIM_IT_Update, ENABLE);  //打开中断 
		if (SysTick_Config(SystemCoreClock / 1000))//1毫秒
		{ 
			/* Capture error */ 
			while (1);
		}
}
//void Init_Time16(uint16_t num)    //  num 定时时长，  1uS 的倍数
//{
//	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure; 
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE);

//	TIM_ITConfig(TIM16, TIM_IT_Update, DISABLE);
//	TIM_DeInit(TIM16);
//	
//	  /*  TIM16 中断优先级 */
//  TIM_TimeBaseStructure.TIM_Period = num;       //定时器计数值 
//  TIM_TimeBaseStructure.TIM_Prescaler =47;    //时钟预分频值(1uS)
//  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
//  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //计数模式(向上计数)
//	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
//  TIM_TimeBaseInit(TIM16, &TIM_TimeBaseStructure);
//  
//	TIM_ClearITPendingBit(TIM16, TIM_IT_Update);
//	TIM_ITConfig(TIM16, TIM_IT_Update, ENABLE);
//	TIM_Cmd(TIM16,ENABLE);    // 开启定时器
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
//		TIM_ClearITPendingBit(TIM16, TIM_IT_Update); //清除中断等待标志位  
//		TIM_Cmd(TIM16,DISABLE); 

//	}
//}

