
//在TIM1 CH1,CH2 产生一个互补的PWM波，频率为48KHz,
#include "conf.h"

extern void tab_sqrt_trig(int32_t angle,int32_t *pcos,int32_t *psqrt_div,int32_t *psqrt_mul);
#define MAX_AMP  480
#define HALF_T   250
#define PWM_T    720
#define PI2			411775	/* (2L<<16)*3.1415926 */
#define PI1			205887	/* (1L<<16)*3.1415926 */
int32_t cos0=0;
int32_t sqrt_div0=0;   // 1L<<16位表示1.0
int32_t sqrt_mul0=0; 

int32_t dphase=0;
int32_t vsin0,vcos0;
int32_t va=1200;   // mV 50V
//int32_t pw0=15;
struct dds_s{
	int32_t phase; // 当前相位  /* 弧度 1L<<16表示1.0 */
	int32_t u0; // 当前电压  mV
	int32_t u1; // 目标电压  mV
	int32_t udc; //直流偏置电压 mV
	int32_t cc;  // 负载电容 pF
	int64_t e0; // 电容贮能 10-18 J
	int64_t e1; // 电容目标贮能 10-18J 
	int64_t er;  // 电阻泄漏能量 10-18J
	int64_t ei;  // 二极管泄漏能量 10-18J
	int64_t de; // 能量变化
	int32_t tns1; // 电感充电时间 nS
	int32_t tns2; // 电容放电时间 ns
};

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
#define MIN_PW0 10
// 二极管漏电 uA
#define I_DIS 30
// mos最大导通宽度 (ns)
#define MAX_PW0 4000
uint32_t max_pw=4000;
int32_t du=0;
int32_t tempt=0;

uint32_t temprate=0;
uint8_t dds_start=0; 
void next_dds(int32_t cur_v){
	if(dds_start==0){
		TIM1->CCR1=0;
		TIM1->CCR2=0;
		TIM1->CCR3=0;
		TIM1->CCR4=0;
		return;
	}
	//保护
	if((parameter.up>165000)||(parameter.un>165000)){
		TIM1->CCR3=0;
		TIM1->CCR4=0;
	}
	// 计算最大脉宽，  10V  4us 
	max_pw=40000000L/parameter.currV;
	if(max_pw>MAX_PW0)max_pw=MAX_PW0;
	//	temprate=config.hv_set*100000/(205*parameter.currV)
	temprate=(uint32_t)config.hv_set*124878L/parameter.currV;  //  放大了256倍

	{	
		tab_sqrt_trig(dds_p.phase,&cos0,&sqrt_div0,&sqrt_mul0);

		if(cos0>0)
			{  // 充电
				dds_p.tns1=((int64_t)sqrt_mul0*parameter.kk*temprate*config.outp_k>>32)+MIN_PW0;   //  nS
				if(dds_p.tns1>max_pw)dds_p.tns1=max_pw;
				TIM1->CCR3=((uint32_t)dds_p.tns1*197>>12);
				TIM1->CCR1=0;
		}else if(cos0<0){  // 放电
				dds_p.tns2=((int64_t)sqrt_div0*parameter.kk*20>>20)+MIN_PW0;   //  nS
				if(dds_p.tns2>max_pw)dds_p.tns2=max_pw;			// nS
				TIM1->CCR3=0;
				TIM1->CCR1=(uint32_t)dds_p.tns2*197>>12;

		}else{
			TIM1->CCR1=0;
			TIM1->CCR3=0;
		}
	}
//	while(0)
	{
		tab_sqrt_trig(dds_n.phase,&cos0,&sqrt_div0,&sqrt_mul0);
	
		if(cos0>0)
			{  // 充电
				dds_n.tns1=((int64_t)sqrt_mul0*parameter.kk*temprate*config.outn_k>>32)+MIN_PW0;   //  sqrt(10-18 J* uH)/ mV = nS
				if(dds_n.tns1>max_pw)dds_n.tns1=max_pw;
				TIM1->CCR4=((uint32_t)dds_n.tns1*197>>12);
				TIM1->CCR2=0;
		}else if(cos0<0){  // 放电
			dds_n.tns2=((int64_t)(sqrt_div0)*parameter.kk*20>>20)+MIN_PW0; 
			if(dds_n.tns2>max_pw)dds_n.tns2=max_pw;
			TIM1->CCR4=0;
			TIM1->CCR2=(uint32_t)dds_n.tns2*197>>12;

		}else{
			TIM1->CCR4=0;
			TIM1->CCR2=0;
		}
	}
	dds_p.phase+=dphase;
	if(dds_p.phase>=PI2){dds_p.phase=dds_p.phase-PI2;}
	dds_n.phase=dds_p.phase+PI1;
	if(dds_n.phase>=PI2){dds_n.phase=dds_n.phase-PI2;}
	dds_p.u0=dds_p.u1;
	dds_p.e0=dds_p.e1;
	dds_n.u0=dds_n.u1;
	dds_n.e0=dds_n.e1;
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
		if(isAFC())	dphase=((uint64_t)411775L)*parameter.curr_freq/2400000L; // 每次输出相移
		else 	dphase=((uint64_t)411775L)*config.freq/2400000L; // 每次输出相移

}

void init_dds(void){
//	config.freq=31400;
//	config.hv_set=120;
	update_freq();
	dds_gpio_init();
		
	dds_pwm_init(0,0,0,0);

}

void start_dds(void){
	dds_start=1;
}
void stop_dds(void){
	dds_start=0;
}

uint32_t rate=0;
void AGC(){  //  u  0.1V
	static int32_t i=0;
	if(!isAGC()){
		parameter.kk=config.agc_level;
		return;
	}
	parameter.kk=((uint32_t)1000L*config.hv_set)*parameter.kk/parameter.ubase;
	if(parameter.kk<1)parameter.kk=1;
	if(parameter.kk>1024)parameter.kk=1024;
}

