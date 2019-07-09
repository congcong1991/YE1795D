#include "datatype.h"  
#include "conf.h"
#include "main.h"
#include "eeprom.h"
//#include "fft.h"
//#include "stm32f0xx_dac.h"


//#define PI 3.14159265358979 


#define ADCHS 6

extern  void next_dds(int32_t v);  // dds 合成波形

//  资源占用  
//  Timer15 用于定时采样，

uint16_t   Adc_Data[ADCHS*2];  //  

#define ADC1_DR_Address                0x40012440

// TIMER1 used to trig the adc 
// sample rate 24K , 输出频率 30~500 Hz
// 500Hz 时，每个周期48个点


void emu_init_timer() {   
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure; 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	{  /*  TIM1 中断优先级 */ 
		NVIC_InitTypeDef NVIC_InitStructure;
		NVIC_InitStructure.NVIC_IRQChannel = TIM1_BRK_UP_TRG_COM_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);  //打开中断 
	}
	


  TIM_TimeBaseStructure.TIM_ClockDivision = 0; // 不分频
	  TIM_TimeBaseStructure.TIM_Prescaler =0;    //时钟预分频值(1分频成48MHz)
	TIM_TimeBaseStructure.TIM_Period =250-1;       //定时器计数值  (2000分频成48K)
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //计数模式(向上计数)
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 8-1;   // 中断计数为24K 
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	
	TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Update);     // 设置 Tim1 输出触发ADC
  TIM_Cmd(TIM1,ENABLE);    // 开启定时器
}

void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{

	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update); //清除中断等待标志位  
						next_dds(0); 
	}
}
// PA0 ADC0 高压电源
// PA1 ADC1 I
// PA2 ADC2 U
// PA5 ADC5 电源电压

void ADC_Port_Init(void){
	GPIO_InitTypeDef    GPIO_InitStruct; 
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA|RCC_AHBPeriph_GPIOB, ENABLE);
	//   U2, I2,U1,I1 采样   , U2 201 倍
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStruct);				// 不用设置速率
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;    // ADC8  电源电压采样，   21倍
	GPIO_Init(GPIOB, &GPIO_InitStruct);				// 不用设置速率
}


void ADC1_DMA_Init(void)
{
  ADC_InitTypeDef     ADC_InitStruct;
  DMA_InitTypeDef     DMA_InitStruct;
 
	NVIC_InitTypeDef    NVIC_InitStructure;
  ADC_DeInit(ADC1);

  /* ADC1 Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	{
		NVIC_InitStructure.NVIC_IRQChannel = ADC1_IRQn;   // ADC1 开中断
		NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);  
	}
  /* DMA1 clock enable */
	{
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);

  	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;   // DMA1 开中断
		NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);  
  
  /* DMA1 Channel1 Config */
		DMA_DeInit(DMA1_Channel1);
		DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_Address;    //配置外设地址
		DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)&Adc_Data;  //配置内存映射地址
		DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;  //外设作为传输的来源
		DMA_InitStruct.DMA_BufferSize =ADCHS;    //DMA缓存大小
		DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;   //外设地址寄存器不变
		DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;    //内存地址寄存器递增
		DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;   //外设数据宽度
		DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;   //内存数据宽度
		DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;   //DMA工作在循环缓冲模式
		DMA_InitStruct.DMA_Priority = DMA_Priority_VeryHigh;   //DMA通道优先级 （高）   
		DMA_InitStruct.DMA_M2M = DMA_M2M_Disable; //关闭DMA通道内存到内存传输
		DMA_Init(DMA1_Channel1, &DMA_InitStruct);
	
		DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);   
  /* DMA1 Channel1 enable */
		DMA_Cmd(DMA1_Channel1, ENABLE);
  
   /* ADC DMA request in circular mode */
		ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);//ADC的DMA工作在循环模式下
  }
  
  /* Initialize ADC structure */
  ADC_StructInit(&ADC_InitStruct);
  
  /* Configure the ADC1 in continous mode withe a resolutuion equal to 12 bits  */
  ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;   //ADC转换精度 12位
  ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;    //ADC工作在连续模式
 // ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; //ADC由软件触发
	ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;   // 上升沿触发
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_TRGO;  //由T1触发
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;  //数据右对齐
  ADC_InitStruct.ADC_ScanDirection = ADC_ScanDirection_Upward;  //扫描方式为向前扫描
  ADC_Init(ADC1, &ADC_InitStruct); 
 
   /*------------ADC转换通道配置----------------------*/
//	 	ADC_VrefintCmd(ENABLE);  // 打开内部基准
  

	ADC_ChannelConfig(ADC1,ADC_Channel_TempSensor, ADC_SampleTime_7_5Cycles);  
	
	ADC_TempSensorCmd(ENABLE);

  /* ADC Calibration */
  ADC_GetCalibrationFactor(ADC1);

//	ADC_ITConfig(ADC1,ADC_IT_EOC,ENABLE);  // ADC开中断
	
  ADC_DMACmd(ADC1, ENABLE);
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);  

  
  /* Wait the ADCEN falg */
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADEN)); 
  
  /* ADC1 regular Software Start Conv */ 
	ADC_StartOfConversion(ADC1);

}
extern 	void Power_OnSample(uint16_t dc);  // 电源电压调整



#define tao_zero (1<<16)
uint64_t zero_i=2048*tao_zero;
uint64_t zero_u=613*tao_zero;
#define backlash 10
uint8_t last_u_z=0;  // =0 未过零
uint8_t last_i_z=0;  // =0 未过零 
uint8_t is_u_up(uint16_t uu){  // 电压正向过零 
	if((uu>(zero_u/tao_zero+backlash)) && (last_u_z==0)){
		last_u_z=1;
		return(1);
	}
	if((uu<(zero_u/tao_zero-backlash))&&(last_u_z==1)){
		last_u_z=0; // 负向过零
	}
	return(0);
}
uint8_t is_i_up(uint16_t ii){  // 电压正向过零 
	if((ii>(zero_i/tao_zero+backlash)) && (last_i_z==0)){
		last_i_z=1;
		return(1);
	}
	if((ii<(zero_i/tao_zero-backlash))&&(last_i_z==1)){
		last_i_z=0; // 负向过零
	}
	return(0);
}
extern void AGC(void); 
//平均电流
#define I_TAO		1024
int32_t last_i=0*I_TAO;
// 功率 
#define S_TAO 1024
int32_t last_s=3000*S_TAO;
// 有功功率 
#define P_TAO 1024
int32_t last_p=1000*P_TAO;
//无功功率 
#define Q_TAO 1024
int32_t last_q=2000*Q_TAO;
// AC电压
#define V_TAO 1024
int32_t last_v=0*V_TAO;
// 偏置电压
#define UBASE_TAO 512
uint32_t last_ubase=0*UBASE_TAO; 
// 阻抗
#define Z_TAO	64
uint32_t last_z=10*Z_TAO;

#define TEMP_TAO 4096
uint32_t last_temp=1500*TEMP_TAO;



/* Temperature sensor calibration value address */
#define TEMP110_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1FFFF7C2))
#define TEMP30_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1FFFF7B8))
#define VDD_CALIB ((uint16_t) (330))
#define VDD_APPLI ((uint16_t) (300))

//  变压器电感量修正
//  
// 增高60度   增大一倍

int32_t getTemp(void){
	int32_t temperature; /* will contain the temperature in degree Celsius */
//	temperature = (((int32_t) (last_temp>>12) * VDD_APPLI / VDD_CALIB)- (int32_t) *TEMP30_CAL_ADDR );
	temperature = (((int32_t) (last_temp) )- (int32_t) *TEMP30_CAL_ADDR );
	temperature = temperature * (int32_t)(1100 - 300);
	temperature = temperature / (int32_t)(*TEMP110_CAL_ADDR- *TEMP30_CAL_ADDR);
	temperature = temperature + 300;
	
	//  变压器电感量修正
	// 增高60度   增大一倍
//	if(isYE1795D1()){
//		parameter.LLL=18000+((parameter.currInterTemp-250)*2);
//	}else if(isYE1795D3()){
//		parameter.LLL=5436+(parameter.currInterTemp-250)*5;
//	}else if(isYE1795D6()){
//		parameter.LLL=5436+(parameter.currInterTemp-250)*5;
//	}else if(isYE1795D3_2()){
//		parameter.LLL=5436+(parameter.currInterTemp-250)*5;
//	}
	return(temperature);
}


void emu(){  // 电压 0.1V  电流  1uA  // 测量功率因数
	// 通过测量电流电压过零点时差 确定功率因数
	static uint32_t i=0;
	static uint32_t ii=0;
	uint32_t putt,nutt;
	if(ii<4){ii++;}
	else{
		ii=0;
//		last_ubase=((uint32_t)(parameter.up+parameter.un))+((uint64_t)last_ubase*511>>9);  // mV		
////		last_i=((uint32_t)(parameter.ip+parameter.in))+((uint64_t)last_i*1023>>10);  // 电流  uA
//		if(parameter.up>parameter.un){
//			last_v=((parameter.up-parameter.un))+((int64_t)last_v*1023>>10);  // 电压  mv
//		}else{
//			last_v=((parameter.un-parameter.up))+((int64_t)last_v*1023>>10);  // 电压  mV
//		}
//		last_q=((parameter.in*parameter.un+parameter.ip*parameter.up)>>15)+((int64_t)last_q*1023>>10);
//		
//		// 计算 电流平均值
//		if(isYE1795D3_2()){
//			if(parameter.in>parameter.ip){
//					last_i=(((int32_t)parameter.in-parameter.ip)>>1)+((int64_t)last_i*1023>>10);
//			}else{
//					last_i=(((int32_t)parameter.ip-parameter.in)>>1)+((int64_t)last_i*1023>>10);
//			}
//			/* mW */
//			last_p=((int64_t)last_p*1023>>10)+(((int64_t)parameter.up*parameter.ip+(int64_t)parameter.un*parameter.in)*2147>>31);
//			
//		}else{
//			if(dds_p.phase>PI1){
//				last_i=(((int32_t)parameter.in-parameter.ip))+((int64_t)last_i*1023>>10);
//			}else{
//				last_i=(((int32_t)parameter.ip-parameter.in))+((int64_t)last_i*1023>>10);
//			}
//		}
//		
//		putt=dds_p.tns1*parameter.currDC>>12;// nS*mV/uH = uW
//		nutt=dds_n.tns1*parameter.currDC>>12;
//		last_s=((putt*putt+nutt*nutt)>>8)+((int64_t)last_s*1023>>10);
//		
	//  温度采样	
		last_temp=Adc_Data[5]+((uint64_t)last_temp*4095>>12);
		last_temp=last_temp>>12;
	}
		
	

//	if(i<256)i++;
//	else {


//		i=0;

//		
//		parameter.ubase=last_ubase>>9;   // 偏置电压*2
//		parameter.currV=last_v>>10;
//		parameter.currI=last_i>>10;


//		parameter.currS=((last_s>>10)*parameter.LLL>>16);
//		
//		if(isYE1795D3_2()){
//			parameter.currP=last_p>>10;
//			parameter.currQ=parameter.currS-parameter.currP;
//		}else{
//			parameter.currQ=last_q>>10;
//			parameter.currP=parameter.currS-parameter.currQ;
//		}
//		parameter.currPF=parameter.currP*10000/parameter.currS;
//		
//		parameter.cc=(uint64_t)parameter.ubase*parameter.ubase/parameter.currQ/parameter.curr_freq;
//		
//		AGC();  // 自动调整增益 幅度
//		
//	}
	
}
// 电源电压
#define EV_TAO	4096L
uint32_t last_ev=28000*EV_TAO;

#define  ZeroTao 4096L

uint32_t zeroIn=2048;
uint32_t zeroIp=2048;



void DMA1_Channel1_IRQHandler(void) 
{
		if(DMA_GetITStatus(DMA1_IT_TC1) != RESET)		{
				DMA_ClearITPendingBit(DMA1_IT_TC1);     //  全部采样结束
//				if(isYE1795D3_2()){				
//					zeroIp=((uint64_t)zeroIp*4095>>12)+Adc_Data[3];
//					zeroIn=((uint64_t)zeroIn*4095>>12)+Adc_Data[1];
//					parameter.ip=(int64_t)((int32_t)(zeroIp>>12)-Adc_Data[3])*660000L*config.ip_sub>>20;   // uA
//					parameter.in=(int64_t)((int32_t)(zeroIn>>12)-Adc_Data[1])*660000L*config.in_sub>>20;   // uA		
//				}else{
//					parameter.ip=((uint64_t)Adc_Data[3])*66000L*config.ip_sub>>20;   // uA
//					parameter.in=((uint64_t)Adc_Data[1])*66000L*config.in_sub>>20;   // uA		
//				}
//			
//				parameter.up=(uint64_t)Adc_Data[2]*663300L*config.up_sub>>20;   // mV
//				parameter.un=(uint64_t)Adc_Data[0]*663300L*config.un_sub>>20;   // mV

//				last_ev=((uint64_t)Adc_Data[4]*69300L*config.udc_sub>>20)+((uint64_t)last_ev*4095>>12);  // mV
//				parameter.currDC=last_ev>>12;  // 电源电压  mV				

//			

				emu();		

		}
}

void ADC1_COMP_IRQHandler(void) 
{
//	static float rxh,rxl;
	if (ADC_GetITStatus(ADC1, ADC_IT_EOC) != RESET){
		ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);

	}
}

void PVD_IRQHandler(void)
{
	selftest_clr();
  EXTI_ClearITPendingBit(EXTI_Line6);
	selftest_set();
	while(1);
}
void initLVD()
{
		NVIC_InitTypeDef NVIC_InitStructure; 
		EXTI_InitTypeDef EXTI_InitStructure;   
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); 
	

		EXTI_DeInit();  
		EXTI_StructInit(&EXTI_InitStructure);  
		EXTI_InitStructure.EXTI_Line = EXTI_Line16; 
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;  
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;   //电压低于阀值时产生中断  
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;  
		EXTI_Init(&EXTI_InitStructure);
		
		NVIC_InitStructure.NVIC_IRQChannel = PVD_IRQn; 	
		NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);    
//		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);  
 
//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
//		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
	
	
		PWR_PVDLevelConfig(PWR_PVDLevel_7); // 设定监控阀值    2.9V
		PWR_PVDCmd(ENABLE); // 使能PVD  

}

void initDSP(){
			emu_init_timer();  //  32KHz  TIM1
//			ADC_Port_Init();
//			initLVD();  // 开掉电中断
			startEMU();
}
void startEMU(){
				ADC1_DMA_Init();
}


void emuLoop(void){   

}


