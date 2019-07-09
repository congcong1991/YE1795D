
//在TIM1 CH1,CH2 产生一个互补的PWM波，频率为48KHz,
#include "conf.h"
#include "main.h"
extern void tab_sqrt_trig(int32_t angle,int32_t *pdu,int32_t *psqrt_div,int32_t *psqrt_mul,int32_t *ndu,int32_t *nsqrt_div,int32_t *nsqrt_mul);


int32_t pdu0=0;
int32_t psqrt_div0=0;   // 1L<<16位表示1.0
int32_t psqrt_mul0=0; 
int32_t ndu0=0;
int32_t nsqrt_div0=0;   // 1L<<16位表示1.0
int32_t nsqrt_mul0=0;

int32_t dphase=0;
int32_t vsin0,vcos0;
int32_t va=1200;   // mV 50V
//int32_t pw0=15;

struct dds_s dds_p={0,0,0,30000,1000,0,0,0,0,0,0,0};
struct dds_s dds_n={0,0,0,30000,1000,0,0,0,0,0,0,0};



int32_t dds_v_gate=10;

#define MAX_HPW  1000
#define MAX_LPW  1200
int32_t kk;
extern int32_t u_100mV;

//泄露电阻 K欧 
#define RS   (1000)
// PWM 重复周期 ns
#define PWMT  (41666)
// 低压电源电压
#define LOWU 5000
// 自举电感 uH
#define BOOSTL 100
// 放电电阻 欧
#define DISR  1000
// MOS管最小导通宽度
#define MIN_PW0 0
// 二极管漏电 uA
#define I_DIS 30
// mos最大导通宽度 (ns)
#define MAX_PW0 4000

uint32_t max_pw=4200;
int32_t du=0;
int32_t tempt=0;

uint32_t temprate=0;
uint32_t dds_start=0; 
uint32_t kk2=0;
void next_dds(int32_t cur_v){
	if(dds_start==0){
		TIM1->CCR1=0;
		TIM1->CCR2=0;
		TIM1->CCR3=0;
		TIM1->CCR4=0;
		return;
	}


	// 计算最大脉宽，  10V  4.5us 
	max_pw=45000000L/parameter.currDC;
	if(max_pw>MAX_PW0)max_pw=MAX_PW0;
	
		temprate=(uint32_t)parameter.hv_set*125L/parameter.currDC;  //  放大了256倍

		kk2=(uint32_t)parameter.kk;		//*temprate>>8;

	{	
		tab_sqrt_trig(dds_p.phase,&pdu0,&psqrt_div0,&psqrt_mul0,&ndu0,&nsqrt_div0,&nsqrt_mul0);

		if(pdu0>0)
			{  // 充电
				dds_p.tns1=((int64_t)psqrt_mul0*kk2*temprate*config.outp_k>>32);

				dds_p.tns1+=MIN_PW0;   //  nS
				if(dds_p.tns1>max_pw)dds_p.tns1=max_pw;
				if((parameter.up>165000)){	TIM1->CCR3=0;}  // 低压侧
				else{				TIM1->CCR3=((uint32_t)dds_p.tns1*197>>12);}
				TIM1->CCR1=0;  // 高压侧
		}else if(pdu0<0){  // 放电
				dds_p.tns2=((int64_t)psqrt_div0*kk2*parameter.disc_l_k>>20)+MIN_PW0;   //  nS
				if(dds_p.tns2>max_pw)dds_p.tns2=max_pw;			// nS
				TIM1->CCR3=0;		// 低压侧
			
				TIM1->CCR1=(uint32_t)dds_p.tns2*197>>12;  //　高压侧

		}else{
			TIM1->CCR1=0;
			TIM1->CCR3=0;
		}
	}
//	while(0)
	{
//		tab_sqrt_trig(dds_n.phase,&cos0,&sqrt_div0,&sqrt_mul0);
	
		if(ndu0>0)
			{  // 充电
				dds_n.tns1=((int64_t)nsqrt_mul0*kk2*temprate*config.outn_k>>32)+MIN_PW0;   //  sqrt(10-18 J* uH)/ mV = nS
				if(dds_n.tns1>max_pw)dds_n.tns1=max_pw;
				if((parameter.un>165000)){	TIM1->CCR4=0;}
				else {		TIM1->CCR4=((uint32_t)dds_n.tns1*197>>12); }  // 低压侧
				TIM1->CCR2=0;			// 高压侧
		}else if(ndu0<0){  // 放电
			dds_n.tns2=((int64_t)(nsqrt_div0)*kk2*parameter.disc_l_k>>20)+MIN_PW0; 
			
			if(dds_n.tns2>max_pw)dds_n.tns2=max_pw;
			TIM1->CCR4=0;				// 低压侧
			
			TIM1->CCR2=(uint32_t)dds_n.tns2*197>>12;		// 高压侧

		}else{
			TIM1->CCR4=0;
			TIM1->CCR2=0;
		}
	}
	dds_p.phase+=dphase;
	if(dds_p.phase>=PI2){dds_p.phase=dds_p.phase-PI2;}



}


void	dds_gpio_init()
{
	GPIO_InitTypeDef GPIO_InitStructure; //声明端口结构 
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);//开启相应端口时钟 

	//***************端口A6,A7是TIM3_PWM波的输出端******** 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_2);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_2);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_2);
}

//****TIM1_PWM波1通道初始化***************************************** 
void dds_pwm_init(uint16_t CCR1_Val,uint16_t CCR2_Val,uint16_t CCR3_Val,uint16_t CCR4_Val){
	TIM_OCInitTypeDef	TIM_OCInitStructure;

	
	
// USE TIM1 生成 PWM
  TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//   选择定时器模式        TIM脉冲宽度调制模式1 
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //选择输出比较状态 
 	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable; //选择互补输出比较状态
 	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //选择输出极性  
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High; //选择互补输出极性 
	TIM_OCInitStructure.TIM_Pulse =CCR1_Val;//设置待装入捕获比较寄存器的脉冲值 
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);//捕获比较匹配器结构1通道赋值 
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable); //使能TIM3在CCR1上的预装载寄存器 

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//   选择定时器模式        TIM脉冲宽度调制模式1 
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //选择输出比较状态 
 	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable; //选择互补输出比较状态
 	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //选择输出极性  
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High; //选择互补输出极性 
	TIM_OCInitStructure.TIM_Pulse = CCR2_Val;  //设置待装入捕获比较寄存器的脉冲值 
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);//捕获比较匹配器结构2通道赋值 
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable); //使能TIM3在CCR2上的预装载寄存器 

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//   选择定时器模式        TIM脉冲宽度调制模式1 
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //选择输出比较状态 
 	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable; //选择互补输出比较状态
 	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //选择输出极性  
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High; //选择互补输出极性 
	TIM_OCInitStructure.TIM_Pulse = CCR3_Val;  //设置待装入捕获比较寄存器的脉冲值 
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);//捕获比较匹配器结构2通道赋值 
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable); //使能TIM3在CCR2上的预装载寄存器 

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//   选择定时器模式        TIM脉冲宽度调制模式1 
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //选择输出比较状态 
 	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable; //选择互补输出比较状态
 	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //选择输出极性  
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High; //选择互补输出极性 
	TIM_OCInitStructure.TIM_Pulse = CCR4_Val;  //设置待装入捕获比较寄存器的脉冲值 
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);//捕获比较匹配器结构2通道赋值 
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable); //使能TIM3在CCR2上的预装载寄存器 
	
	TIM_CtrlPWMOutputs(TIM1, ENABLE); 
	
//	{  /*  TIM3 IT setup */ 
//		NVIC_InitTypeDef NVIC_InitStructure;
//		NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
//		NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
//		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//		NVIC_Init(&NVIC_InitStructure);	
//		TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
//	}
//	

//	TIM_Cmd(TIM3,ENABLE);	//使能或者失能指定的TIM3  
//	TIM_CtrlPWMOutputs(TIM3, ENABLE); 
//使能或者失能TIM1外设的主输出 
}
	uint8_t  dfreq=0;   // 5%
void update_freq(void){

//	if(isAFC())	dphase=((uint64_t)1<<16)*2*3.1415926*parameter.curr_freq*0.01/24000; // 每次输出相移
//	else 	dphase=((uint64_t)1<<16)*2*3.1415926*config.freq*0.01/24000; // 每次输出相移
		if((isAFC())||(is18B20AFC()))	dphase=((uint64_t)411775L)*parameter.curr_freq/2400000L; // 每次输出相移
		else 	dphase=((uint64_t)411775L)*config.freq/2400000L; // 每次输出相移

}

void init_dds(void){
//	config.freq=31400;
//	config.hv_set=120;
	update_freq();
	dds_gpio_init();
		
	dds_pwm_init(0,0,0,0);

}
extern uint32_t loops;
void start_dds(void){
	dds_start=1;
	parameter.kk=2;
}
void stop_dds(void){
	dds_start=0;
}
uint8_t  is_dds_start(void){
	return(dds_start>10);
}
uint8_t is_dds_end(void){
	return(dds_start>1024);
}
uint32_t rate=0;
void AGC(){  //  u  0.1V
//	int32_t dv;
	if(dds_start==0){parameter.kk=0;return;}
	if(dds_start<2048)dds_start++;
	
	if(!isAGC()){
		parameter.kk=config.agc_level;
		return;
	}
//	dv=(int32_t)config.hv_set*1000L-parameter.ubase;
	parameter.kk=((uint32_t)parameter.hv_set)*parameter.kk/parameter.ubase;
//	parameter.kk=parameter.kk+(dv>>10);
	if(parameter.kk<1)parameter.kk=1;
	if(parameter.kk>1024)parameter.kk=1024;
	
	if(parameter.kk>dds_start)parameter.kk=dds_start;
	
}

