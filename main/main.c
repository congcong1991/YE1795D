#include "conf.h"
#include "cpu.h"
#include "dsp.h"
#include "main.h"
#include "ModBus.h"
#include "eeprom.h"

struct PARAMETER  parameter={
0,//	int32_t up,un;   // ���˵�ѹ  mV
0,//	int32_t ip,in;		// ���˵�����uA
0,	//uint32_t ubase;   // ƫ�õ�ѹ  mV; 
0, //	float i;   // ����
0, //	float fi;  // ���
0, //	float f;  // Ƶ��
0, //	uint8_t locked;
0, //	uint16_t currDC;   //ֱ����ѹ
200, //	uint16_t ext_temp; // �ⲿ�¶� 0.1 ���϶�  0-150
6000, //	uint16_t curr_freq;  // ��ǰƵ��
//	uint32_t kk; // ϵͳУ��ϵ��
//	uint32_t t0; // ����Ƶ�ʶ�Ӧ����ֵ 200��Ƶ��
//	uint32_t v_t; // ��ѹ����������
	
//	uint16_t currI;			// ��ǰ��������ֵ
//	int16_t currInterTemp; // ��ǰ�ڲ��¶Ȳ���ֵ
//	uint8_t sampled; // ����ֵ��Ч��־
//	uint16_t lastI;  // ��һ�ε�������ֵ
//	uint16_t maxValue;  //���ټ���ֵ 
//	uint16_t currV;  // ��ǰ��Դ��ѹ 
//	uint16_t currZ; // �迹 0.01K
//	int32_t 	currP;  // 
//	int32_t currQ;  // �޹�����
//	int32_t 	currPF;
//	int32_t currS; // �ܹ��� 
	
//	uint8_t model;
//	uint32_t disc_l_k;  // �ŵ�������ϵ�� 
//	int32_t cc; 			// �������
//	uint32_t hv_set;  // ���õ�ѹ  mV ;
	
//	uint32_t LLL;
	
};		 



void  Delay(uint16 count)
{
	uint16 idata t=count;

	while(--t!=0){watchdog();};
}





uint8 ms1=0;

void On1ms()	   // 1ms ����һ�Ρ������ж��е���
{
	ms1=1;
}
#define MAX_FREQ_RATE  106
#define MIN_FREQ_RATE  94

void AFC_KK(void){
	static uint32_t last_i;
	static uint8_t dir=1;  // !=0 up,  =0 down;
	uint32_t d_freq=1; //1/1000
	uint32_t freq;
	
	if(isSearch()){ //����״̬
		if(isMin()){  // ������
			if(parameter.kk>config.i_gate){
				parameter.locked=1; // ����Ƶ��
			}else if(parameter.kk<((uint32_t)config.i_gate*9/10)){
				parameter.locked=0; // δ����
			}
		}else{  // ��С����
			if(parameter.kk<config.i_gate){
				parameter.locked=1; // ����Ƶ��
			}else if(parameter.kk>((uint32_t)config.i_gate*11/10)){
				parameter.locked=0; // δ����
			}
		}			
		if(parameter.locked==0)d_freq=15;
		else d_freq=2; 
	}		
	else {
		d_freq=2;  //0.002
	}
	
	
	if(isAFC()){  //�Զ�Ƶ�ʸ���
		
		if(isMin() ){// ���ٷ�ʽ ���迹���
			if(parameter.kk>last_i) dir=!dir;  // ����
			if(parameter.kk<parameter.maxValue)parameter.maxValue=parameter.kk;
		}else{  // ��������迹
			if(parameter.kk<last_i) dir=!dir; // ����
			if(parameter.kk>parameter.maxValue)parameter.maxValue=parameter.kk;
		}
		
		if(dir!=0)freq=parameter.curr_freq*(1024+d_freq)/1024;
		else freq=parameter.curr_freq*(1024-d_freq)/1024;
		
		if(freq>((uint32_t)config.freq*115/100))freq=(uint32_t)config.freq*87/100;
		if(freq<((uint32_t)config.freq*85/100))freq=(uint32_t)config.freq*113/100;
		
		parameter.curr_freq=freq;
		
	}else{
		freq=config.freq;
	}
	last_i=parameter.kk;
}

void AFC_P(void){
	static uint32_t last_i;
	static uint8_t dir=1;  // !=0 up,  =0 down;
	uint32_t d_freq=1; //1/1000
	uint32_t freq;
	
	if(isSearch()){ //����״̬
		if(isMin()){  // ��Сֵ
			if(parameter.currP<config.i_gate){
				parameter.locked=1; // ����Ƶ��
			}else if(parameter.currP>((uint32_t)config.i_gate*11/10)){
				parameter.locked=0; // δ���� 
			}
		}else{  // ���ֵ
			if(parameter.currP>config.i_gate){
				parameter.locked=1; // ����Ƶ��
			}else if(parameter.currP<((uint32_t)config.i_gate*9/10)){
				parameter.locked=0; // δ����
			}
		}			
		if(parameter.locked==0){d_freq=15;}
		else {d_freq=2; } 
	}		
	else {
		d_freq=2;  //0.002
	}
	
	if(isAFC() ){  //�Զ�Ƶ�ʸ���
		if(isMin() ){// ���ٷ�ʽ ����Сֵ
			if(parameter.currP>last_i) dir=!dir;  // ����
			if(parameter.currP<parameter.maxValue)parameter.maxValue=parameter.currP;
		}else{  // �������ֵ
			if(parameter.currP<last_i) dir=!dir; // ����
			if(parameter.currP>parameter.maxValue)parameter.maxValue=parameter.currP;
		}
		if(isSearch()&& parameter.locked==0)dir=0; // ����ʱ ��������
		if(dir!=0)freq=parameter.curr_freq*(1024+d_freq)/1024;
		else freq=parameter.curr_freq*(1024-d_freq)/1024;
		
		if(freq>((uint32_t)config.freq*MAX_FREQ_RATE/100))freq=(uint32_t)config.freq*(MIN_FREQ_RATE+2)/100;
		if(freq<((uint32_t)config.freq*MIN_FREQ_RATE/100))freq=(uint32_t)config.freq*(MAX_FREQ_RATE-2)/100;
	
		parameter.curr_freq=freq;
		
	}else{
		freq=config.freq;
	}
	last_i=parameter.currP;
}
void Search_PF_AFC_P(void){
	static uint32_t last_i;
	static uint8_t dir=1;  // !=0 up,  =0 down;
	uint32_t d_freq=1; //1/1000
	uint32_t freq;
	
	if(isSearch()){ //����״̬
		if(isMin()){  // ��Сֵ
			if(parameter.currPF<config.i_gate){
				parameter.locked=1; // ����Ƶ��
			}else if(parameter.currPF>((uint32_t)config.i_gate*11/10)){
				parameter.locked=0; // δ���� 
			}
		}else{  // ���ֵ
			if(parameter.currPF>config.i_gate){
				parameter.locked=1; // ����Ƶ��
			}else if(parameter.currPF<((uint32_t)config.i_gate*9/10)){
				parameter.locked=0; // δ����
			}
		}			
		if(parameter.locked==0){d_freq=15;}
		else {d_freq=2; } 
	}		
	else {
		d_freq=2;  //0.002
	}
	
	if(isAFC() ){  //�Զ�Ƶ�ʸ���
		if(isMin() ){// ���ٷ�ʽ ����Сֵ
			if(parameter.currP>last_i) dir=!dir;  // ����
			if(parameter.currP<parameter.maxValue)parameter.maxValue=parameter.currP;
		}else{  // �������ֵ
			if(parameter.currP<last_i) dir=!dir; // ����
			if(parameter.currP>parameter.maxValue)parameter.maxValue=parameter.currP;
		}
		if(isSearch()&& parameter.locked==0)dir=0; // ����ʱ ��������
		if(dir!=0)freq=parameter.curr_freq*(1024+d_freq)/1024;
		else freq=parameter.curr_freq*(1024-d_freq)/1024;
		
		if(freq>((uint32_t)config.freq*MAX_FREQ_RATE/100))freq=(uint32_t)config.freq*(MIN_FREQ_RATE+2)/100;
		if(freq<((uint32_t)config.freq*MIN_FREQ_RATE/100))freq=(uint32_t)config.freq*(MAX_FREQ_RATE-2)/100;
		
		parameter.curr_freq=freq;
		
	}else{
		freq=config.freq;
	}
	last_i=parameter.currP;
}
void AFC_PF(void){
	static uint32_t last_i;
	static uint8_t dir=1;  // !=0 up,  =0 down;
	uint32_t d_freq=1; //1/1000
	uint32_t freq;
	
	if(isSearch()){ //����״̬
		if(isMin()){  // ��Сֵ
			if(parameter.currPF<config.i_gate){
				parameter.locked=1; // ����Ƶ��
			}else if(parameter.currPF>((uint32_t)config.i_gate*11/10)){
				parameter.locked=0; // δ���� 
			}
		}else{  // ���ֵ
			if(parameter.currPF>config.i_gate){
				parameter.locked=1; // ����Ƶ��
			}else if(parameter.currPF<((uint32_t)config.i_gate*9/10)){
				parameter.locked=0; // δ����
			}
		}			
		if(parameter.locked==0){d_freq=15;}
		else {d_freq=2; } 
	}		
	else {
		d_freq=2;  //0.002
	}
	
	if(isAFC() ){  //�Զ�Ƶ�ʸ���
		if(isMin() ){// ���ٷ�ʽ ����Сֵ
			if(parameter.currPF>last_i) dir=!dir;  // ����
			if(parameter.currPF<parameter.maxValue)parameter.maxValue=parameter.currPF;
		}else{  // �������ֵ
			if(parameter.currPF<last_i) dir=!dir; // ����
			if(parameter.currPF>parameter.maxValue)parameter.maxValue=parameter.currPF;
		}
		if(isSearch()&& parameter.locked==0)dir=0; // ����ʱ ��������
		if(dir!=0)freq=parameter.curr_freq*(1024+d_freq)/1024;
		else freq=parameter.curr_freq*(1024-d_freq)/1024;
		
		if(freq>((uint32_t)config.freq*115/100))freq=(uint32_t)config.freq*87/100;
		if(freq<((uint32_t)config.freq*85/100))freq=(uint32_t)config.freq*113/100;
		
		parameter.curr_freq=freq;
		
	}else{
		freq=config.freq;
	}
	last_i=parameter.currPF;
}
const int32_t df_temp_table[]=		{0, 		150,		250,			600,		800,		1000};
const int32_t df_freq_table[]=		{65868,	65647,	65447,	64318,	63986,	63875}; 
const int32_t df_basev_table[]=		{0,			0, 				0			-2000,	-5000,	-25000};
uint32_t df_table_num=sizeof(df_temp_table)/sizeof(int32_t);

void AFC_18B20(void){
	int32_t t=parameter.ext_temp;
//	int32_t t=850; 
	int32_t dv=0;
	int32_t df=0;
	uint32_t i;
	if(t<df_temp_table[0]){
			t=df_temp_table[0];
	}
	if(t>df_temp_table[df_table_num-1]){
		t=df_temp_table[df_table_num-1];
	}
	for(i=0;i<df_table_num-1;i++){
		if(t>=df_temp_table[i] && t<=df_temp_table[i+1]){
			df=df_freq_table[i]+(t-df_temp_table[i])*(df_freq_table[i+1]-df_freq_table[i])/(df_temp_table[i+1]-df_temp_table[i]);
			dv=df_basev_table[i]+(t-df_temp_table[i])*(df_basev_table[i+1]-df_basev_table[i])/(df_temp_table[i+1]-df_temp_table[i]);
			break;
		}
	}
	//df=df+(parameter.currInterTemp-250)*200/800; // �ڲ���������
	
	parameter.curr_freq=(int32_t)config.freq*df>>16;
//	parameter.hv_set=config.max_hv*1000+dv;
	parameter.hv_set=config.max_hv*1000;

}

void AFC_I(void){
	static uint32_t last_i;
	static uint8_t dir=1;  // !=0 up,  =0 down;
	uint32_t d_freq=1; //1/1000
	uint32_t freq;
	
	if(isSearch()){ //����״̬
		if(isMin()){  // ������
			if(parameter.currI>config.i_gate){
				parameter.locked=1; // ����Ƶ��
			}else if(parameter.currI<((uint32_t)config.i_gate*9/10)){
				parameter.locked=0; // δ����
			}
		}else{  // ��С����
			if(parameter.currI<config.i_gate){
				parameter.locked=1; // ����Ƶ��
			}else if(parameter.currI>((uint32_t)config.i_gate*11/10)){
				parameter.locked=0; // δ����
			}
		}			
		if(parameter.locked==0)d_freq=15;
		else d_freq=2; 
	}		
	else {
		d_freq=2;  //0.002
	}
	
	
	if(isAFC()){  //�Զ�Ƶ�ʸ���
		
		if(isMin() ){// ���ٷ�ʽ ���迹���
			if(parameter.currI>last_i) dir=!dir;  // ����
			if(parameter.currI<parameter.maxValue)parameter.maxValue=parameter.currI;
		}else{  // ��������迹
			if(parameter.currI<last_i) dir=!dir; // ����
			if(parameter.currI>parameter.maxValue)parameter.maxValue=parameter.currI;
		}
		
		if(dir!=0)freq=parameter.curr_freq*(1024+d_freq)/1024;
		else freq=parameter.curr_freq*(1024-d_freq)/1024;
		
		if(freq>((uint32_t)config.freq*115/100))freq=(uint32_t)config.freq*87/100;
		if(freq<((uint32_t)config.freq*85/100))freq=(uint32_t)config.freq*113/100;
		
		parameter.curr_freq=freq;
		
	}else{
		freq=config.freq;
	}
	last_i=parameter.currI;
}

void AFC_Z(void){
	static uint32_t last_z;
	static uint8_t dir=1;  // !=0 up,  =0 down;
	uint32_t d_freq=1; //1/1000
	uint32_t freq;
	
	if(isSearch()){ //����״̬
		if(isMin()){  // ������
			if(parameter.currI>config.i_gate){
				parameter.locked=1; // ����Ƶ��
			}else if(parameter.currI<((uint32_t)config.i_gate*9/10)){
				parameter.locked=0; // δ����
			}
		}else{  // ��С����
			if(parameter.currI<config.i_gate){
				parameter.locked=1; // ����Ƶ��
			}else if(parameter.currI>((uint32_t)config.i_gate*11/10)){
				parameter.locked=0; // δ����
			}
		}			
		if(parameter.locked==0)d_freq=15;
		else d_freq=2; 
	}		
	else {
		d_freq=2;  //0.002
	}
	
	
	if(isAFC()){  //�Զ�Ƶ�ʸ���
		
		if(isMin() ){// ���ٷ�ʽ ���迹���
			if(parameter.currZ>last_z) dir=!dir;  // ����
			if(parameter.currZ<parameter.maxValue)parameter.maxValue=parameter.currZ;
		}else{  // ��������迹
			if(parameter.currZ<last_z) dir=!dir; // ����
			if(parameter.currZ>parameter.maxValue)parameter.maxValue=parameter.currZ;
		}
		
		if(dir!=0)freq=parameter.curr_freq*(1024+d_freq)/1024;
		else freq=parameter.curr_freq*(1024-d_freq)/1024;
		
		if(freq>((uint32_t)config.freq*115/100))freq=(uint32_t)config.freq*87/100;
		if(freq<((uint32_t)config.freq*85/100))freq=(uint32_t)config.freq*113/100;
		
		parameter.curr_freq=freq;
		
	}else{
		freq=config.freq;
	}
	last_z=parameter.currZ;
}

uint8_t getModel(void){
	uint8_t t=0;
	{ // init port PB7,PB6 
		GPIO_InitTypeDef  GPIO_InitStructure;
		RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOB, ENABLE);
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_6;                 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_Init(GPIOB, &GPIO_InitStructure);   
	
	}
	Delay(1);
	if(GPIO_ReadInputDataBit( GPIOB, GPIO_Pin_7)!=Bit_RESET)t|=0x02;
	if(GPIO_ReadInputDataBit( GPIOB, GPIO_Pin_6)!=Bit_RESET)t|=0x01;
	
	return(t);

}

void initModel(void){
	parameter.model=getModel();
	if(isYE1795D1()){
		parameter.disc_l_k=20;
		parameter.LLL=11000;
	}else if(isYE1795D3()){
		parameter.disc_l_k=18;
		parameter.LLL=5436;
	}else if(isYE1795D6()){
		parameter.disc_l_k=16;
		parameter.LLL=5436;
	}else if(isYE1795D3_2()){
		parameter.disc_l_k=15;
		parameter.LLL=5436;
	}

}

extern void start_dds(void);
extern void stop_dds(void);


void updateEnable(void){
	static uint32_t hcount=0;
	static uint32_t lcount=0;
	
	{ // init port PB2 
		GPIO_InitTypeDef  GPIO_InitStructure;
		RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOB, ENABLE);
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;                 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOB, &GPIO_InitStructure);   
	}
	if(GPIO_ReadInputDataBit( GPIOB, GPIO_Pin_2)!=Bit_RESET){ if(hcount<8) hcount++; lcount=0; }
	else {if(lcount<8)lcount++; hcount=0; }
	
	if(	hcount==8 && !isEnable()){ setEnable(); start_dds(); };
	if( lcount==8 && isEnable()) { clrEnable(); stop_dds(); };
	
}


void AutoTempMode(void){
	if((parameter.ext_temp>=config.temp_start) && !isTempOver() ){
		setTempOver();
		start_dds();
	}
	if((parameter.ext_temp<=config.temp_stop)) {
	//	&& isTempOver() ){
		clrTempOver();
		stop_dds();
	}
}
void q_control(void){   // 0.5S����һ�Σ� �˲�����Ϊ8s
	static int32_t last_q=2000<<4;
	int32_t curr_q;
	int32_t hv;
	last_q=parameter.currQ+(last_q*15>>4); 
	curr_q=last_q>>4;
	
	hv=(int32_t)parameter.hv_set-(curr_q-config.max_q)*5000/config.max_q;
		
	if(hv>((uint32_t)config.max_hv*1000)){parameter.hv_set=(uint32_t)config.max_hv*1000;}
	else if(hv<50000){ parameter.hv_set=50000; }
	else { parameter.hv_set=hv;}
}

void p_control(void){   // 0.5S����һ�Σ� �˲�����Ϊ8s
	static int32_t last_p=2000<<4;
	int32_t curr_p;
	int32_t hv;
	last_p=parameter.currP+(last_p*15>>4); 
	curr_p=last_p>>4;
	
	hv=(int32_t)parameter.hv_set-(curr_p-config.max_q)*5000/config.max_q;
		
	if(hv>((uint32_t)config.max_hv*1000)){parameter.hv_set=(uint32_t)config.max_hv*1000;}
	else if(hv<50000){ parameter.hv_set=50000; }
	else { parameter.hv_set=hv;}
}
extern uint8_t is_dds_end(void);
#define ERRORCOUNT 10
void testError(void){
	static  uint8_t ViLowCount=0;
	static uint8_t  VoLowCount=0;
	static uint8_t  IoHighCount=0;
	static uint8_t  TiHighCount=0;
	static uint8_t  TeHighCount=0;
	
	uint8_t error=0;
	/* �����ѹ���͡�*/
	if(parameter.currDC<1000){if(ViLowCount<255)ViLowCount++;} /* <10V */
	else {ViLowCount=0; }
	/* �����ѹ���� */
	if(is_dds_end()&&(parameter.ubase<(parameter.hv_set-30000))) {if(VoLowCount<255)VoLowCount++; } /* <100V */
	else{VoLowCount=0;}
	
	/* �����������*/
	if(parameter.currI>40000) {if(IoHighCount<255)IoHighCount++; }
	else {IoHighCount=0;}
	
	/* �ڲ��¶ȹ��ߡ�*/
	if(parameter.currInterTemp>1200){if(TiHighCount<255)TiHighCount++;}
	else {TiHighCount=0;}
	
	/* �ⲿ�¶ȹ��ߡ�*/
	if(parameter.ext_temp>1200){if(TeHighCount<255)TeHighCount++;}
	else {TeHighCount=0;}
	
	if((ViLowCount>ERRORCOUNT)||
		  (VoLowCount>ERRORCOUNT)||
		  (IoHighCount>ERRORCOUNT)||
		  (TiHighCount>ERRORCOUNT)||
		  (TeHighCount>ERRORCOUNT)	){  /* ���ϡ�*/
		GPIO_InitTypeDef GPIO_InitStructure; //�����˿ڽṹ?
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);//������Ӧ�˿�ʱ��?

		/***************�˿�A7�ǹ��������********/
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
		GPIO_Init(GPIOA, &GPIO_InitStructure); 
		
		GPIO_SetBits(GPIOA, GPIO_Pin_7);
				
				
	}else{ /* ������*/
		
		GPIO_InitTypeDef GPIO_InitStructure; //�����˿ڽṹ?
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);//������Ӧ�˿�ʱ��?

		/***************�˿�A7�ǹ��������********/
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
		GPIO_Init(GPIOA, &GPIO_InitStructure); 
		
		GPIO_ResetBits(GPIOA, GPIO_Pin_7);
				
		
	}
	
}

extern void init_power(void);
extern void init_dds(void);
extern void update_freq(void);
extern void initDS18B20(void);
void start_dds(void);
extern uint8_t  is_dds_start(void);
extern int32_t getTemp(void);

uint32_t last_record_freq=0;
uint32_t emu_period;
void updat_freq_by_temp(void)
{
		if(parameter.ext_temp>=config.temp_stop)
		{
			setHaltMode();
			parameter.curr_freq=0;
			return;
		}
		if(parameter.ext_temp<=0)
		{
			setHaltMode();
			parameter.curr_freq=0;
		}
		else if(parameter.ext_temp>0&parameter.ext_temp<=300)
		{
			 clrHaltMode();
			 parameter.curr_freq=config.freq;
		}
		else if(parameter.ext_temp>300&parameter.ext_temp<=350)
		{
			 clrHaltMode();
			 parameter.curr_freq=config.freq;
		}
		else if(parameter.ext_temp>350&parameter.ext_temp<=400)
		{
			 clrHaltMode();
			 parameter.curr_freq=config.freq-30;
		}
		else if(parameter.ext_temp>400&parameter.ext_temp<=450)
		{
			 clrHaltMode();
			 parameter.curr_freq=config.freq-50;
		}
		else if(parameter.ext_temp>450&parameter.ext_temp<=500)
		{
			 clrHaltMode();
			 parameter.curr_freq=config.freq-100;
		}
		else if(parameter.ext_temp>500&parameter.ext_temp<=550)
		{
			 clrHaltMode();
			 parameter.curr_freq=config.freq-140;
		}
		else if(parameter.ext_temp>550&parameter.ext_temp<=600)
		{
			 clrHaltMode();
			 parameter.curr_freq=config.freq-160;
		}
		else if(parameter.ext_temp>600&parameter.ext_temp<=650)
		{
			 clrHaltMode();
			 parameter.curr_freq=config.freq-170;
		}
		else if(parameter.ext_temp>650&parameter.ext_temp<=700)
		{
			 clrHaltMode();
			 parameter.curr_freq=config.freq-180;
		}
		else if(parameter.ext_temp>700&parameter.ext_temp<=750)
		{
			 clrHaltMode();
			 parameter.curr_freq=config.freq-200;
		}
		else if(parameter.ext_temp>750&parameter.ext_temp<=800)
		{
			 clrHaltMode();
			 parameter.curr_freq=config.freq-210;
		}
		else if(parameter.ext_temp>800&parameter.ext_temp<=1000)
		{
			 clrHaltMode();
			 parameter.curr_freq=config.freq-240;
		}
		else
		{
			 clrHaltMode();
			parameter.curr_freq=config.freq;
		}
		emu_period=(float)100000/parameter.curr_freq*100;
	
}

void TIM3_IRQHandler(void)
{
 if(TIM_GetITStatus(TIM3,TIM_IT_Update) != RESET) //����ж�
	{
				
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //����жϱ�־λ
	if(isHaltMode())
	{
		TIM3->ARR=0;
		return;
	}
	if(!isDS18B20fine())
	{
		TIM3->ARR=0;
		return;
	}
	TIM3->ARR=emu_period-1;
	TIM3->CCR1=emu_period>>1;
	
}
void TIM2_IRQHandler(void)
{
 if(TIM_GetITStatus(TIM2,TIM_IT_Update) != RESET) //����ж�
	{
				
	}
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //����жϱ�־λ
	if(isHaltMode())
	{
		TIM2->ARR=TIM2->ARR+1; 
		return;
	}
	if(!isDS18B20fine())
	{
		TIM2->CCR2=TIM2->ARR+1;
		return;
	}
	TIM2->ARR=emu_period-1;
	TIM2->CCR2=emu_period>>1;
	
}
//void updata_pwm(void)
//{
//	
//	if(isHaltMode())
//	{
////		TIM_CtrlPWMOutputs(TIM2, DISABLE); 
////		TIM_CtrlPWMOutputs(TIM3, DISABLE); 
//		TIM2->CCR2=TIM2->ARR+1;
//		TIM3->CCR1=0;
//		return;
//	}else if(!isDS18B20fine())
//	{
//		TIM2->CCR2=TIM2->ARR+1;
//		TIM3->CCR1=0;
//		return;	
//	}else 
//	{
//		if(last_record_freq!=parameter.curr_freq)
//		{
//			emu_period=(float)100000/parameter.curr_freq*100;
//			TIM2->ARR=emu_period-1;
//			TIM3->ARR=emu_period-1;
//			TIM2->CCR2=(emu_period>>1)-1;
//			TIM3->CCR1=(emu_period>>1)-1;
//			last_record_freq=parameter.curr_freq;
//		}
//		
//	}
//	
//}

int main(void){
	uint32_t  loops=0;      
	uint16 idata ttt;
#ifndef DEBUG
	initWatchdog();
#endif
//	INITSELFTEST();	  // �Լ�
//	selftest_set();	
//	initKey();    // �ȳ�ʼ������
//	initPLL();		//�������ر�
	CloseAllInt();   // �ر������ж�	
//	initModel();
	clrHaltMode();
	initEEPROM();
	parameter.curr_freq=config.freq;
//	last_record_freq=parameter.curr_freq;  //�ϴμ�¼��Ƶ��
	parameter.locked=0;  // ��ʼ��δ��������Ƶ��
	

	init_power();  // ��DC/DC

	
#ifndef DEBUG
	init485();
#endif
	initDSP();
//	init_dds();
	initT0();
	initDS18B20();

	OpenAllInt(); //�������ж�

	while(1)									  
 	{	
		watchdog();
		modbusLoop();	    // modbus 
		if(ms1==0)continue;			// 1ms A LOOP
		ms1=0;
		loops++;
		if((loops%500)==0){	// about 500ms  ͨ�����������ѹ���ȶ��������
				
		updat_freq_by_temp();
		}
		if((loops%2000)==0){	// about 500ms  ͨ�����������ѹ���ȶ��������
		parameter.currInterTemp=getTemp(); 
		}
		if(loops>5000)
			loops=0;
	}
}//main




