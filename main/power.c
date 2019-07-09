//升压电源的PWM 
// 在PB4  TIM3_CH1 上产生频率为100KHz,可调占空比PWM脉冲。
// 在PB5	TIM3_CH2 
#include "conf.h"




//#define POWER_MAX_W  1900
//#define POWER_MIN_W  25
//#define POWER_INIT_W    150

void	power_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; //声明端口结构 
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);//开启相应端口时钟 


	//***************端口B4,B5是TIM3_CH1,TIM3_CH2 AF1的输出端******** 
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_6; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //PWM波设为复用推挽输出  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//AF1
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_1);

		//***************************************************************************
}
#define POWER_PWM_T 	240
#define POWER_PWM_H		110
#define POWER_PWM_L 	110

//****TIM3_PWM波1,2通道初始化***************************************** 
void power_pwm_init(void){
		uint32_t pwm_total=(float)100000/parameter.curr_freq*100;
		TIM_OCInitTypeDef	TIM_OCInitStructure;
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure; 
		NVIC_InitTypeDef                NVIC_InitStructure;
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
//		{  /*  TIM3 中断优先级 */ 
//			NVIC_InitTypeDef NVIC_InitStructure;
//			NVIC_InitStructure.NVIC_IRQChannel = TIM3_BRK_UP_TRG_COM_IRQn;
//			NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
//			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//			NVIC_Init(&NVIC_InitStructure);
//			TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);  //打开中断 
//		}
	
	{

		TIM_TimeBaseStructure.TIM_ClockDivision = 0; // 不分频
	  TIM_TimeBaseStructure.TIM_Prescaler =480-1;    //时钟预分频值(1分频成48MHz)
		TIM_TimeBaseStructure.TIM_Period =pwm_total-1;       //定时器计数值  (480分频成100K)
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //计数模式(中间对齐)
		TIM_TimeBaseStructure.TIM_RepetitionCounter = 1-1;   // 0 
		TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
//		TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Update);     // 设置 Tim1 输出触发ADC
//		TIM_Cmd(TIM1,ENABLE);    // 开启定时器
	}
	

	{
	
		TIM_OCStructInit(&TIM_OCInitStructure);
	
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//   选择定时器模式        TIM脉冲宽度调制模式1 
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //选择输出比较状态 
		TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable; //选择互补输出比较状态
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //选择输出极性  
		TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High; //选择互补输出极性 
		TIM_OCInitStructure.TIM_Pulse =pwm_total>>1;//设置待装入捕获比较寄存器的脉冲值 
		TIM_OC1Init(TIM3, &TIM_OCInitStructure);//捕获比较匹配器结构2通道赋值 
		TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable); //使能TIM3在CCR1上的预装载寄存器 
	
		
		
		
	}
	{
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure; 
		TIM_TimeBaseStructure.TIM_ClockDivision = 0; // 不分频
	  TIM_TimeBaseStructure.TIM_Prescaler =480-1;    //时钟预分频值(1分频成48MHz)
		TIM_TimeBaseStructure.TIM_Period =pwm_total-1;       //定时器计数值  (480分频成100K)
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //计数模式(中间对齐)
		TIM_TimeBaseStructure.TIM_RepetitionCounter = 1-1;   // 0 
		TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
		

		TIM_OCStructInit(&TIM_OCInitStructure);
	
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//   选择定时器模式        TIM脉冲宽度调制模式1 
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //选择输出比较状态 
		TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable; //选择互补输出比较状态
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //选择输出极性  
		TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low; //选择互补输出极性 
		TIM_OCInitStructure.TIM_Pulse =pwm_total>>1;//设置待装入捕获比较寄存器的脉冲值 
		TIM_OC2Init(TIM2, &TIM_OCInitStructure);//捕获比较匹配器结构2通道赋值 
		TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable); //使能TIM3在CCR1上的预装载寄存器 
	
	
		
	}
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
		TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //允许定时器3更新中断
		TIM_Cmd(TIM3,ENABLE); //使能定时器3        

		
		NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; //定时器3中断
		NVIC_InitStructure.NVIC_IRQChannelPriority = 2; //优先级0
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
		TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); //允许定时器3更新中断
		TIM_Cmd(TIM2,ENABLE); //使能定时器3        

		
		NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; //定时器3中断
		NVIC_InitStructure.NVIC_IRQChannelPriority = 2; //优先级0
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	
		TIM_CtrlPWMOutputs(TIM3, ENABLE); 
		TIM_CtrlPWMOutputs(TIM2, ENABLE); 
		TIM_Cmd(TIM3,ENABLE);	//使能或者失能指定的TIM3
		TIM_Cmd(TIM2,ENABLE);	//使能或者失能指定的TIM3
}


void init_power(void){

	power_gpio_init();
	power_pwm_init();
//	while(1);

}
//// 电源电压调整
//int32_t last_pw=POWER_INIT_W;
//void Power_OnSample(uint16_t dc){
//	int32_t pw;
//	int32_t v,v0,delta;
//	v=((uint32_t)dc*3300/4096);  // 0.1V为单位
//	v0=(uint32_t)config.hv_set*10; // 设定电压
//	
//	if(isCompRelay()){  // 比较形式
//		delta=(v-v0)*12;
//		pw=-delta;
//		if(pw>POWER_MAX_W)pw=POWER_MAX_W;				
//		if(v>v0)pw=0;
//	
//	}else{
//		delta=(v-v0)*2;
//		pw=last_pw-delta;
//		if(pw<POWER_MIN_W)pw=POWER_MIN_W;
//		if(pw>POWER_MAX_W)pw=POWER_MAX_W;
//	}
////	if(v<10)v=10;
////	pw=v0*last_pw/v;
//	

//	
//	last_pw=pw;
//	TIM_SetCompare3(TIM1,pw);
//}




