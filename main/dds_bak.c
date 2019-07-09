
//在TIM3 CH1,CH2 产生一个互补的PWM波，频率为48KHz,
#include "conf.h"

extern void tab_sin_cos(int32_t angle,int32_t *psin,int32_t *pcos);
#define MAX_AMP  480
#define HALF_T   250
#define PWM_T    720
#define PI2			411775	/* (2L<<16)*3.1415926 */
#define PI			205887	/* (1L<<16)*3.1415926 */
int32_t sin0=0;
int32_t cos0=0;   // 1L<<16位表示1.0


int32_t dphase=0;
int32_t vsin0,vcos0;
int32_t va=15000;   // mV 50V
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

uint32_t sqrt_64_2(uint64_t  x){
	uint32_t last = 0;
	uint32_t  res = 16;
	uint8_t i;
	x<<=4;
	if (x == 0) return 0;

	while((res>>1)!=last){
		last = res>>1;
		res = (res + x / res)>>1;
	}
	return ((res+2)>>2);
}

uint32_t  sqrt_32_fast(unsigned int number)
{
    float f;
    unsigned int u;

    f = number;
    u = *(unsigned int *)&f;
    u = (u + 0x3f7a63d7) >> 1;
    f = *(float *) &u;
    u = f;
    u = (u + number/u) >> 1;
    u = (u + number/u) >> 1;

    return u;
}


uint16_t sqrt_32(uint32_t M)
{
	unsigned char i;
	uint16_t N;
	uint32_t tmp, ttp; // 结果、循环计数

	if(M == 0) // 被开方数，开方结果也为0
		return 0;
	N = 0;
	tmp = (M >> 30); // 获取最高位：B[m-1]
	M <<= 2;
	if(tmp) // 最高位为1
	{
		N = 1; // 结果当前位为1，否则为默认的0
		tmp -= 1;// -= N;
	}
	for(i = 15; i > 0; i--) // 求剩余的15位
	{
		N <<= 1; // 左移一位
		tmp <<= 2;
		tmp += (M >> 30); // 假设
		ttp = ((unsigned long)N << 1) + 1;
		//ttp++;//对于长整数不可用自加或自减，而是用+1来取代
		M <<= 2;
		if (tmp >= ttp) // 假设成立
		{
			tmp -= ttp;
			N += 1;
		}
	}
	return N;
}


uint32_t sqrt_64(uint64_t M){
	uint32_t N, i;
	uint64_t tmp, ttp; // 结果、循环计数

	if (M == 0)return 0; // 被开方数，开方结果也为0
		
	N = 0;
	tmp = (M >> 62); // 获取最高位：B[m-1]
	M <<= 2;
	if (tmp > 1) // 最高位为1
	{
		N ++; // 结果当前位为1，否则为默认的0
		tmp -= N;
	}
	
	for (i=31; i>0; i--) // 求剩余的15位
	{
		N <<= 1; // 左移一位
		tmp <<= 2;
		tmp += (M >> 62); // 假设
		ttp = N;
		ttp = (ttp<<1)+1;
		M <<= 2;
		if (tmp >= ttp) // 假设成立
		{
			tmp -= ttp;
			N ++;
		}
	}
	return N;
}


int32_t dds_v_gate=10;
int32_t dds_v_kk=60000; // 1<<16=1.0
int32_t dds_i_kk=100;
int32_t tar_v=0;
int32_t dv=0;
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
#define BUCKL 100
// 放电电阻 欧
#define DISR  1000
// MOS管最小导通宽度
#define MIN_PW0 100
// 二极管漏电 uA
#define I_DIS 30



void next_dds(int32_t cur_v){

	tab_sin_cos(dds_p.phase,&sin0,&cos0);
	dds_p.udc=(uint32_t)config.hv_set*500;   // 
	{
		dds_p.u1=(((int64_t)sin0*va)>>16)	+	dds_p.udc;   // mV
//		dds_p.e0=(int64_t)dds_p.cc*dds_p.u0*dds_p.u0>>1;  // mV*mV*pF = 10-18 J 
		dds_p.e1=(int64_t)dds_p.cc*dds_p.u1*dds_p.u1>>1;
 		dds_p.er=(int64_t)dds_p.u0*dds_p.u0*PWMT/RS;     // mV*mV*nS/K欧= 10-18J
//		dds_p.ei=(int64_t)dds_p.u0*I_DIS*PWMT;  // mV*uA*ns =  10-18 J
		dds_p.de=dds_p.e1-dds_p.e0+dds_p.er;//+dds_p.ei;

		if(dds_p.de>0){  // 电感充电
			dds_p.tns1=sqrt_32_fast(dds_p.de*2*BUCKL/LOWU/LOWU)+MIN_PW0;   //  sqrt(10-18 J* uH)/ mV = nS
//			dds_p.tns1=sqrt_64(dds_p.de*2*BUCKL)/LOWU+MIN_PW0;   //  sqrt(10-18 J* uH)/ mV = nS
	
//			TIM1->CCR1=(uint32_t)dds_p.tns1*48/1000;
			TIM1->CCR1=(uint32_t)dds_p.tns1*197>>12;
			TIM1->CCR2=0;
		}else if(dds_p.de<0){  // 电阻放电
			//			tns2=(-de)*DISR/u0/u0/1000;  //  10-18J*欧/mV/mV/1000=nS
			dds_n.tns2=(-dds_p.de)*DISR/dds_p.u0/1750/1000;   // 10-18J*欧/mV/mV/1000=nS
			TIM1->CCR1=0;
//			TIM1->CCR2=(uint32_t)dds_p.tns2*48/1000;
			TIM1->CCR2=(uint32_t)dds_p.tns2*197>>12;

		}else{
			TIM1->CCR1=0;
			TIM1->CCR2=0;
		}
	}
	while(0)
	{
		tab_sin_cos(dds_n.phase,&sin0,&cos0);
		dds_n.udc=(uint32_t)config.hv_set*500;   // 
		dds_n.u1=(((int64_t)sin0*va)>>16)	+	dds_n.udc;   // mV
//		dds_n.e0=(int64_t)dds_n.cc*dds_n.u0*dds_n.u0/2;  // mV*mV*pF = 10-18 J 
		dds_n.e1=(int64_t)dds_n.cc*dds_n.u1*dds_n.u1>>1;
 		dds_n.er=(int64_t)dds_n.u0*dds_n.u0*PWMT/RS;     // mV*mV*nS/K欧= 10-18J
		dds_n.ei=(int64_t)dds_n.u0*I_DIS*PWMT;  // mV*uA*ns =  10-18 J
		dds_n.de=dds_n.e1-dds_n.e0+dds_n.er+dds_n.ei;

		if(dds_n.de>0){  // 电感充电
			dds_n.tns1=sqrt_64(dds_n.de*2*BUCKL)/LOWU+MIN_PW0;   //  sqrt(10-18 J* uH)/ mV = nS
			TIM1->CCR3=(uint32_t)dds_n.tns1*48/1000;
			TIM1->CCR4=0;
		}else if(dds_n.de<0){  // 电阻放电
			//			tns2=(-de)*DISR/u0/u0/1000;  //  10-18J*欧/mV/mV/1000=nS
			dds_n.tns2=(-dds_n.de)*DISR/dds_n.u0/1750/1000;   // 10-18J*欧/mV/mV/1000=nS
			TIM1->CCR3=0;
			TIM1->CCR4=(uint32_t)dds_n.tns2*48/1000;
		}else{
			TIM1->CCR3=0;
			TIM1->CCR4=0;
		}
	}
	dds_p.phase+=dphase;
	if(dds_p.phase>=PI2){dds_p.phase=dds_p.phase-PI2;}
	dds_n.phase=dds_p.phase+PI;
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

	while(0){
		TIM_TimeBaseInitTypeDef	TIM_TimeBaseStructure;
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
		TIM_TimeBaseStructure.TIM_Period=1000-1;  // 48K
		TIM_TimeBaseStructure.TIM_Prescaler=0;   // 不分频
		TIM_TimeBaseStructure.TIM_ClockDivision=0; // 
		TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_CenterAligned1;  // 中心对齐模式1
		TIM_TimeBaseStructure.TIM_RepetitionCounter=0; 
		TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 
		TIM_ARRPreloadConfig(TIM3,ENABLE); //使能TIM3在ARR上的预装载寄存器 
	}
	
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

void TIM3_IRQHandler(void)
{
	static uint8_t i=0;
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update); 
		if((i%2)==1)		next_dds(u_100mV);
		i++;
	}
}


uint32_t rate=0;
void AGC(){  //  u  0.1V
	static int32_t i=0;
	rate=parameter.currV*1000/((uint32_t)config.hv_set*5); //  1000 表示 1.0  
	if(i<(1L<<16))i++;
	else {
		i=0;
		va=va*config.agc_level/rate;  // rate=0.5
	}
	if(va<1)va=1;
}

