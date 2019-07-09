#ifndef __DS18B20_H
#define __DS18B20_H

#include "conf.h"
#include "stm32f0xx_tim.h"

//#include "bsp_SysTick.h"   //精确延时函数----http://blog.csdn.net/xuxuechen/article/details/40783209?????
#define Delay_Us(a)  {}

#define HIGH  1 
#define LOW   0

#define DS18B20_CLK     RCC_AHBPeriph_GPIOA
#define DS18B20_PIN      GPIO_Pin_5                  
#define DS18B20_PORT GPIOA                  //DS18B20 DATA PORT 

//Macro data out
#define DS18B20_DATA_OUT(a)		{ \
																	if (a) GPIO_SetBits(DS18B20_PORT,DS18B20_PIN);\
																	else GPIO_ResetBits(DS18B20_PORT,DS18B20_PIN); \
															}
 //Macro data in 
#define  DS18B20_DATA_IN()  (DS18B20_PORT->IDR & DS18B20_PIN)

uint8_t DS18B20_Init(void);
int16_t  DS18B20_Get_Temp(void);
void read_serial(uint8_t *serial);
#endif /* __DS18B20_H */

// Serial NO 
uint8_t serial_1[8]={0x28,0x2d,0x9a,0xdd,0x02,0x00,0x00,0x3b}; 
uint8_t serial_2[8]={0x28,0x3b,0x2b,0xbc,0x02,0x00,0x00,0x4f};
uint8_t serial_3[8]={0x28,0x00,0x49,0x1b,0x03,0x00,0x00,0x4c};  

// DS18B20-DATA 输入模式
void DS18B20_Mode_IPU(void) 
{
	uint32_t ts=((uint32_t)DS18B20_PIN * DS18B20_PIN);
	DS18B20_PORT->MODER  &= ~(ts*GPIO_MODER_MODER0);
	DS18B20_PORT->MODER |= ts*GPIO_Mode_IN;

      /* Pull-up Pull down resistor configuration */
	DS18B20_PORT->PUPDR &= ~( ts*GPIO_PUPDR_PUPDR0);
	DS18B20_PORT->PUPDR |= ts*GPIO_PuPd_UP;
}

// DS18B20-DATA 输出模式
void DS18B20_Mode_OD(void) 
{
	uint32_t ts=((uint32_t)DS18B20_PIN * DS18B20_PIN);
	
	DS18B20_PORT->OSPEEDR &= ~(ts*GPIO_OSPEEDER_OSPEEDR0 );
	DS18B20_PORT->OSPEEDR |= ts*GPIO_Speed_50MHz;

	DS18B20_PORT->OTYPER &= ~((uint32_t)DS18B20_PIN*GPIO_OTYPER_OT_0);
	DS18B20_PORT->OTYPER |= (uint32_t)DS18B20_PIN*GPIO_OType_OD;

	DS18B20_PORT->MODER  &= ~(ts*GPIO_MODER_MODER0);
	DS18B20_PORT->MODER |= ts*GPIO_Mode_OUT;

}


void DS18B20_GPIO_Config(void)
{

	
	
//	{
//	/* PA5 -> */
//		GPIO_InitTypeDef  GPIO_InitStructure;
//		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE); 
// 	
//		GPIO_InitStructure.GPIO_Pin =GPIO_Pin_5;                 
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
//		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
//		GPIO_Init(GPIOA, &GPIO_InitStructure); 
//	/*-----------------打开复用功能------------------*/				
//		GPIO_SetBits(GPIOA, GPIO_Pin_5);   
//	}
	
	{
		GPIO_InitTypeDef GPIO_InitStructure;
  
		RCC_AHBPeriphClockCmd(DS18B20_CLK, ENABLE); 
		GPIO_InitStructure.GPIO_Pin = DS18B20_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;  
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(DS18B20_PORT, &GPIO_InitStructure);
 // GPIO_SetBits(DS18B20_PORT, DS18B20_PIN);  
	}
}
 

// //复位DS18B20
//static void DS18B20_Rst(void) 
//{
//  DS18B20_Mode_Out_PP();
//  DS18B20_DATA_OUT(LOW);
//  Delay_Us(750);  //延时 750 uS
//  DS18B20_DATA_OUT(HIGH); 
//  Delay_Us(15);  
//}

uint8_t DS18B20_Presence(void)  // test 18B10 exist? 
{
  uint8_t pulse_time = 0;

  DS18B20_Mode_IPU(); // input mode


  while( DS18B20_DATA_IN() && pulse_time<100 )
  {
    pulse_time++;
    Delay_Us(1);
  }
  if( pulse_time >=100 )   //>100us, no 18B20
        return 1;
  else
        pulse_time = 0;
  while( !DS18B20_DATA_IN() && pulse_time<240 )  //存在, 240us 
  {
    pulse_time++;
    Delay_Us(1);
  }
  if( pulse_time >=240 )
    return 1;
  else
    return 0;
}
//// read DS18B20 a bit
//static uint8_t DS18B20_Read_Bit(void)  //
//{
//  uint8_t dat;

//  DS18B20_Mode_Out_PP();  
//  DS18B20_DATA_OUT(LOW); 
//  Delay_Us(10);
//  DS18B20_Mode_IPU();
//  if( DS18B20_DATA_IN() == SET )
//      dat = 1;
//  else
//     dat = 0;
//  Delay_Us(45);   

//  return dat;
//}
//读 DS18B20 Byte
//uint8_t DS18B20_Read_Byte(void)  
//{
//  uint8_t i, j, dat = 0;

//  for(i=0; i<8; i++) 
//  {
//      j = DS18B20_Read_Bit();
//		dat = (dat) | (j<< i); 
//	}

//  return dat;
//}
////write DS18B20,byte
//void DS18B20_Write_Byte(uint8_t dat)  
//{
//  uint8_t i, testb;

//  DS18B20_Mode_Out_PP();
//  for( i=0; i<8; i++ )
//  {
//    testb = dat&0x01;
//    dat = dat>>1;
//    if (testb) 
//    {
//        DS18B20_DATA_OUT(LOW);
//        Delay_Us(8);
//        DS18B20_DATA_OUT(HIGH);
//        Delay_Us(58);
//    }
//  else
//    {
//        DS18B20_DATA_OUT(LOW);
//        Delay_Us(70);
//        DS18B20_DATA_OUT(HIGH);
//        Delay_Us(2);
//      }
//  }
//}

//void DS18B20_Start(void)
//{
//  DS18B20_Rst();   
//  DS18B20_Presence();  
//  DS18B20_Write_Byte(0XCC); //?? ROM  
//  DS18B20_Write_Byte(0X44); //???? 
//}

//uint8_t DS18B20_Init(void)
//{
//  DS18B20_GPIO_Config();
//  DS18B20_Rst();

//  return DS18B20_Presence();
//}

//void DS18B20_Match_Serial(uint8_t a)    //?????
//{
//  uint8_t i;
//  DS18B20_Rst();
//  DS18B20_Presence();
//  DS18B20_Write_Byte(0X55); //???????
//  if(a==1)
//  {
//    for(i=0;i<8;i++)
//       DS18B20_Write_Byte(serial_1[i]);
//  }
//  else if(a==2)
//  {
//    for(i=0;i<8;i++)
//       DS18B20_Write_Byte(serial_2[i]);
//  }
//  else if(a==3)
//  {
//    for(i=0;i<8;i++)
//       DS18B20_Write_Byte(serial_3[i]);
//  }       
//}

//??????16 ???????????????
//????12?????,??5????,7????,4????

//         |---------??----------|-----?? ??? 1/(2^4)=0.0625----|
//???  | 2^3 | 2^2 | 2^1 | 2^0 | 2^(-1) | 2^(-2) | 2^(-3) | 2^(-4) |

//        |-----???:0->?  1->?-------|-----------??-----------|
//???  |  s  |  s  |  s  |  s  |    s   |   2^6  |   2^5  |   2^4  |
//?? = ??? + ?? + ??*0.0625

//int16_t DS18B20_Get_Temp(void)
//{
//  uint8_t tpmsb, tplsb;
//  short s_tem;
// // float f_tem;
////  int temp_num;

//  DS18B20_Rst();   
//  DS18B20_Write_Byte(0XCC); // ?? ROM 
//  DS18B20_Write_Byte(0XBE); // ???? 

//  tplsb = DS18B20_Read_Byte();  
//  tpmsb = DS18B20_Read_Byte(); 

//  s_tem = tpmsb<<8;
//  s_tem = s_tem | tplsb;
//	
//	return(s_tem);
 /*
	if( s_tem < 0 ) //???
  {
    f_tem = (~s_tem+1) * 0.0625;
    temp_num = (~s_tem+1) * 0.0625*10;
    go_temp= temp_num;
    if(temp_num>=1000)
    {
       a[0]='-';
       a[1]= temp_num/1000+'0';
       a[2]= temp_num%1000/100+'0';
       a[3]= temp_num%100/10+'0';
       a[4]='.';
       a[5]= temp_num%10+'0';
       a[6]= '\0';
    }
    else
    {
       a[0]='-';
       a[1]= temp_num/100+'0';
       a[2]= temp_num%100/10+'0';
       a[3]='.'; 
       a[4]=temp_num%10+'0';
       a[5]= '\0';
    }
  }
  else
  {
    f_tem = s_tem * 0.0625;
    temp_num = s_tem * 0.0625*10;
    go_temp= temp_num;
    if(temp_num>=1000)
    {
       a[0]='+';
       a[1]= temp_num/1000+'0';
       a[2]= temp_num%1000/100+'0';
       a[3]= temp_num%100/10+'0';
       a[4]='.';
       a[5]= temp_num%10+'0';
       a[6]= '\0';
    }
   else
   {
       a[0]='+';
       a[1]= temp_num/100+'0';
       a[2]= temp_num%100/10+'0';
       a[3]='.'; 
       a[4]=temp_num%10+'0';
       a[5]= '\0';
    }
  }
  return f_tem; 
	*/
//}

//void read_serial(uint8_t *serial)  //?????
//{
//  uint8_t i;
//  DS18B20_Rst();
//  DS18B20_Presence();
//  DS18B20_Write_Byte(0X33); //???????
//  for(i=0;i<8;i++)
//     serial[i] = DS18B20_Read_Byte(); 
//}

uint32_t Status18B20=0;
uint16_t temp=0;
uint8_t h_temp=0;
uint8_t l_temp=0;
uint8_t h_alarm_temp_user=0;
uint8_t l_alarm_temp_user=0;
uint8_t cfg_18b20=0;
uint8_t Reserved1=0;
uint8_t Reserved2=0;
uint8_t Reserved3=0;
uint8_t CRC_18B20=0;
#define setTimer(t) {TIM16->CNT=(64000-t);}

uint8_t DS18B20_BIT_WR=0;
uint8_t *DS18B20_RXD_POINT;
//uint8_t DS18B20_RXDBUF=0;
void startReadByte(uint8_t *tt){
	DS18B20_RXD_POINT=tt;
	DS18B20_BIT_WR=0;
}
uint8_t DS18B20_TXDBUF=0;
void startWriteByte(uint8_t b){
	DS18B20_TXDBUF=b;
	DS18B20_BIT_WR=0;
}

uint8_t readByte(void){
	uint8_t bs=DS18B20_BIT_WR>>1;
	uint8_t bb=DS18B20_BIT_WR&1;

	if(bb==0){  // 输出电平
		DS18B20_Mode_OD(); 	 
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);		
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);		
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);		
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);		
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
//		setTimer(1); 
		DS18B20_Mode_IPU();	
//		DS18B20_Mode_IPU();	

//	}else{  // 读电平

		if(DS18B20_DATA_IN()){		
			
			*DS18B20_RXD_POINT|=(1<<bs);
		}else{		
			*DS18B20_RXD_POINT&=~(1<<bs);
		}
	
		setTimer(20);

	}
	DS18B20_BIT_WR+=2;
	if(DS18B20_BIT_WR==16)return(1);
	return(0);
}

uint8_t writeByte(void){   // return(1) end;
	uint8_t bs=DS18B20_BIT_WR>>1;
	uint8_t bb=DS18B20_BIT_WR&1;
	DS18B20_Mode_IPU();	
	if(bb==0){  // 输出电平
		DS18B20_Mode_OD(); 	 
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);  
		if((1<<bs)&DS18B20_TXDBUF){		
			GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN); 
			GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN); 			
			 
			DS18B20_BIT_WR+=2;
			setTimer(10);
			DS18B20_Mode_IPU();				
		}else{
			setTimer(6);//输出0 >60us
			DS18B20_BIT_WR++;

		}
	}else{
			DS18B20_Mode_IPU();	
			setTimer(10);
		DS18B20_BIT_WR++;

	}
	if(DS18B20_BIT_WR==16)return(1);
	return(0);
}



void initDS18B20(void){
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure; 	
	DS18B20_GPIO_Config();
	

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE);

	{  /*  TIM15 中断优先级 */ 
		NVIC_InitTypeDef NVIC_InitStructure;
		NVIC_InitStructure.NVIC_IRQChannel = TIM16_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		TIM_ITConfig(TIM16, TIM_IT_Update, ENABLE);  //打开中断 
	}
	


  TIM_TimeBaseStructure.TIM_ClockDivision = 0; // 不分频
	  TIM_TimeBaseStructure.TIM_Prescaler =480-1;    //时钟预分频值(479分频成0.1MHz)
	TIM_TimeBaseStructure.TIM_Period =64000-1;       //定时器计数值  (480uS)
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //计数模式
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;   
  TIM_TimeBaseInit(TIM16, &TIM_TimeBaseStructure);
	
  TIM_Cmd(TIM16,ENABLE);    // 开启定时器
}



uint32_t DS18B20_INDEX=0;
void DS18B20_INIT(void){
	static uint32_t l=0;
	switch(l){
		case 0:	
			DS18B20_Mode_OD(); 	  
			GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);  
			setTimer(75);	
			l++;
			break;
		case 1:
			DS18B20_Mode_IPU();		
			setTimer(7);			
			l++;
			break;     
		case 2:
			if(Bit_SET==(DS18B20_PORT->IDR & DS18B20_PIN))
			{
				setTimer(60000);
				
				l=0;	
				DS18B20_INDEX=0;
			}else{
					setTimer(50); // 500uS
					l=0;
					DS18B20_INDEX++;
					break;
			}
		default:
			DS18B20_INDEX=0;
			l=0;
			break;
			
	}
}
void DS18B20_DELAY2S(void){
		static uint32_t l=0;				
		setTimer(50000);	
		l++;
		if(l>4){
			l=0;
			DS18B20_INDEX++;
		}
}
void DS18B20_WRITE_CC(void){
	static uint32_t l=0;
	static uint8_t cc=0xCC;
	

//	GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);  
	if(l&1){ // 延时
		setTimer(10);
		DS18B20_Mode_IPU();
		l++;

	}else{	// 输出脉冲
		DS18B20_Mode_OD(); 	  
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);  
		if(cc&(1<<(l>>1))){  // 输出1
			l+=2;
			setTimer(10);	
			DS18B20_Mode_IPU();

		}else{
			setTimer(7);	 // 70uS;
			l++;
		}
	}
	if(		l==16)	{l=0;	DS18B20_INDEX++;}
}
void DS18B20_WRITE_44(void){
	static uint32_t l=0;
	static uint8_t cc=0x44;
	

//	GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);  
	if(l&1){ // 延时
		setTimer(10);
		DS18B20_Mode_IPU();
		l++;

	}else{	// 输出脉冲
		DS18B20_Mode_OD(); 	  
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);  
		if(cc&(1<<(l>>1))){  // 输出1
			l+=2;
			setTimer(10);	
			DS18B20_Mode_IPU();

		}else{
			setTimer(7);	 // 70uS;
			l++;
		}
	}
	if(		l==16)	{l=0;	DS18B20_INDEX++;}
}
void DS18B20_WRITE_BE(void){
	static uint32_t l=0;
	static uint8_t cc=0xBE;

	if(l&1){ // 延时
		setTimer(10);
		DS18B20_Mode_IPU();
		l++;

	}else{	// 输出脉冲
		DS18B20_Mode_OD(); 	  
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);  
		if(cc&(1<<(l>>1))){  // 输出1
			l+=2;
			setTimer(10);	
			DS18B20_Mode_IPU();

		}else{
			setTimer(7);	 // 70uS;
			l++;
		}
	}
	if(		l==16)	{l=0;	DS18B20_INDEX++;}
}
uint8_t DS18B20_READ_DATA[9]={0x00};

void DS18B20_READ_DATA0(void){
	static uint32_t l=0;
	static uint8_t cc=0;
	
	if((l&1)==0){  // 输出电平
		DS18B20_Mode_OD(); 	 
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);		
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);	
		DS18B20_Mode_IPU();
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);		
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);

		if(DS18B20_DATA_IN()){		
			cc|=(1<<(l>>1));
		}else{		
			cc&=~(1<<(l>>1));
		}
		setTimer(20);
		l+=2;
	}		
	
	if(	l==16)	{l=0; l_temp=cc;	DS18B20_INDEX++;}
	DS18B20_READ_DATA[0]=cc;
}
void DS18B20_READ_DATA1(void){
	static uint32_t l=0;
	static uint8_t cc=0;
	
	if((l&1)==0){  // 输出电平
		DS18B20_Mode_OD(); 	 
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);		
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);	
		DS18B20_Mode_IPU();
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);		
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
		if(DS18B20_DATA_IN()){		
			cc|=(1<<(l>>1));
		}else{		
			cc&=~(1<<(l>>1));
		}
		setTimer(20);
		l+=2;
	}		
	
	if(	l==16)	{l=0; h_temp=cc;	DS18B20_INDEX++;}
	DS18B20_READ_DATA[1]=cc;
}

void DS18B20_READ_DATA2(void){
	static uint32_t l=0;
	static uint8_t cc=0;
	
	if((l&1)==0){  // 输出电平
		DS18B20_Mode_OD(); 	 
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);		
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);	
		DS18B20_Mode_IPU();
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);		
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
		if(DS18B20_DATA_IN()){		
			cc|=(1<<(l>>1));
		}else{		
			cc&=~(1<<(l>>1));
		}
		setTimer(20);
		l+=2;
	}		
	
	if(	l==16)	{l=0; h_alarm_temp_user=cc;	DS18B20_INDEX++;}
	DS18B20_READ_DATA[2]=cc;
}


void DS18B20_READ_DATA3(void){
	static uint32_t l=0;
	static uint8_t cc=0;
	
	if((l&1)==0){  // 输出电平
		DS18B20_Mode_OD(); 	 
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);		
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);	
		DS18B20_Mode_IPU();
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);		
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
		if(DS18B20_DATA_IN()){		
			cc|=(1<<(l>>1));
		}else{		
			cc&=~(1<<(l>>1));
		}
		setTimer(20);
		l+=2;
	}		
	
	if(	l==16)	{l=0; l_alarm_temp_user=cc;	DS18B20_INDEX++;}
	DS18B20_READ_DATA[3]=cc;
}
void DS18B20_READ_DATA4(void){
	static uint32_t l=0;
	static uint8_t cc=0;
	
	if((l&1)==0){  // 输出电平
		DS18B20_Mode_OD(); 	 
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);		
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);	
		DS18B20_Mode_IPU();
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);		
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
		if(DS18B20_DATA_IN()){		
			cc|=(1<<(l>>1));
		}else{		
			cc&=~(1<<(l>>1));
		}
		setTimer(20);
		l+=2;
	}		
	
	if(	l==16)	{l=0; cfg_18b20=cc;	DS18B20_INDEX++;}
	DS18B20_READ_DATA[4]=cc;
}

void DS18B20_READ_DATA5(void){
	static uint32_t l=0;
	static uint8_t cc=0;
	
	if((l&1)==0){  // 输出电平
		DS18B20_Mode_OD(); 	 
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);		
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);	
		DS18B20_Mode_IPU();
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);		
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
		if(DS18B20_DATA_IN()){		
			cc|=(1<<(l>>1));
		}else{		
			cc&=~(1<<(l>>1));
		}
		setTimer(20);
		l+=2;
	}		
	
	if(	l==16)	{l=0; Reserved1=cc;	DS18B20_INDEX++;}
	DS18B20_READ_DATA[5]=cc;
}

void DS18B20_READ_DATA6(void){
	static uint32_t l=0;
	static uint8_t cc=0;
	
	if((l&1)==0){  // 输出电平
		DS18B20_Mode_OD(); 	 
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);		
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);	
		DS18B20_Mode_IPU();
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);		
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
		if(DS18B20_DATA_IN()){		
			cc|=(1<<(l>>1));
		}else{		
			cc&=~(1<<(l>>1));
		}
		setTimer(20);
		l+=2;
	}		
	
	if(	l==16)	{l=0; Reserved2=cc;	DS18B20_INDEX++;}
	DS18B20_READ_DATA[6]=cc;
}

void DS18B20_READ_DATA7(void){
	static uint32_t l=0;
	static uint8_t cc=0;
	
	if((l&1)==0){  // 输出电平
		DS18B20_Mode_OD(); 	 
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);		
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);	
		DS18B20_Mode_IPU();
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);		
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
		if(DS18B20_DATA_IN()){		
			cc|=(1<<(l>>1));
		}else{		
			cc&=~(1<<(l>>1));
		}
		setTimer(20);
		l+=2;
	}		
	
	if(	l==16)	{l=0; Reserved3=cc;	DS18B20_INDEX++;}
	DS18B20_READ_DATA[7]=cc;
}
void DS18B20_READ_DATA8(void){
	static uint32_t l=0;
	static uint8_t cc=0;
	
	if((l&1)==0){  // 输出电平
		DS18B20_Mode_OD(); 	 
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);		
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);	
		DS18B20_Mode_IPU();
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);		
		GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
		if(DS18B20_DATA_IN()){		
			cc|=(1<<(l>>1));
		}else{		
			cc&=~(1<<(l>>1));
		}
		setTimer(20);
		l+=2;
	}		
	
	if(	l==16)	{l=0; CRC_18B20=cc;	DS18B20_INDEX++;}
	DS18B20_READ_DATA[8]=cc;
}

static unsigned char CRC8Calculate(void *pBuf ,unsigned pBufSize)
{ unsigned char retCRCValue=0x00;
	unsigned char *pData; 
	int i=0; 
	unsigned char pDataBuf=0; //  retCRCValue=0x01;
	pData=(unsigned char *)pBuf; // pDataBuf=pData[0]; // cout<<hex<<pDataBuf<<endl;
	while(pBufSize--) 
	{ 
		pDataBuf=*pData++;
		for(i=0;i<8;i++) { 
			if((retCRCValue^(pDataBuf))&0x01) 
				{ retCRCValue^=0x18; 
					retCRCValue>>=1; 
					retCRCValue|=0x80; //    printf("i=%d;retCRCValue=%x\n",i,retCRCValue);
					} else { retCRCValue>>=1; //     printf("i=%d;retCRCValue=%x\n",i,retCRCValue); 
						} pDataBuf>>=1; 
			} 
		} 
		return retCRCValue; 
}
uint8_t sssss;
void DS18B20_LOOP(void){
	uint16_t t=h_temp<<8;
	t|=l_temp;
	sssss=CRC8Calculate(DS18B20_READ_DATA,8);
	if(DS18B20_READ_DATA[8]==sssss)
	{
		DS18B20fine();		
		parameter.ext_temp=(int32_t)(*(int16_t*)(&t))*10>>4;
	}
	else
	{
		DS18B20error();
		parameter.curr_freq=0;
	}
	DS18B20_INDEX=0;
}

void (*DS18B20_OP_TAB[])(void)={ 
	&DS18B20_INIT, 
//	&DS18B20_DELAY2S,
	&DS18B20_WRITE_CC, 
	&DS18B20_WRITE_44,
	&DS18B20_DELAY2S,
	&DS18B20_INIT, 
	&DS18B20_WRITE_CC, 
	&DS18B20_WRITE_BE,
	&DS18B20_READ_DATA0,
	&DS18B20_READ_DATA1,
	&DS18B20_READ_DATA2,
	&DS18B20_READ_DATA3,
	&DS18B20_READ_DATA4,
	&DS18B20_READ_DATA5,
	&DS18B20_READ_DATA6,
	&DS18B20_READ_DATA7,
	&DS18B20_READ_DATA8,
	&DS18B20_LOOP,
};

void OnTimer18B20_1(void){
	DS18B20_OP_TAB[DS18B20_INDEX]();
}

static uint8_t toggle_flag=0;
void TIM16_IRQHandler(void)
{

	if (TIM_GetITStatus(TIM16, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM16, TIM_IT_Update); //清除中断等待标志位  
		OnTimer18B20_1();
//		if(toggle_flag)
//		{
//		GPIO_SetBits(GPIOA, GPIO_Pin_5); 
//			toggle_flag=0;
//		}else
//		{
//			GPIO_ResetBits(GPIOA, GPIO_Pin_5); 
//			toggle_flag=1;
//		}
	}
}
