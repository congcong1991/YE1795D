#include "conf.h"
#include "main.h"
#include "modbus.h"

#include "eeprom.h"

#ifndef DISABLE_MODBUS

uint8_t TxdBuf[TX_BUFFER_SIZE];
uint8_t RxdBuf[RX_BUFFER_SIZE];
uint8_t RxdTelBuf[RX_BUFFER_SIZE];
uint8_t TxdBytes=0;
uint8_t RxdBytes=0;
uint8_t TxdTelBytes=0;

uint8_t ReceivedTel=0;

void    OnGetData03(void);
void    OnGetData05(void);
uint16_t CRC16ISR0(uint8 * puchMsg, uint8 usDataLen);

uint16_t CRC16ISR1(uint8 * puchMsg, uint8 usDataLen);

uint8_t  isVaildTel(void)
{
	if(RxdBytes>=1)if(RxdBuf[0]!=config.addr)return(0);
	if(RxdBytes>=2)if(RxdBuf[1]!=0x03 && RxdBuf[1]!=0x05 && RxdBuf[1]!=0x06	&& RxdBuf[1]!=0x10)return(0);

	return(1);				 // 合法的
}

uint8_t getTelLength(void)
{
					
	switch(RxdBuf[1])
	{
		case 0x03: return(6);
		case 0x05: return(6);
		case 0x06: return(6);
		case 0x10: return(RxdBuf[6]+7);
		default:
				return(0);
	}					
}
uint8_t isTelComplete(void)	   // =0 不完整  =1 CRC Error =2 正确
{
	uint16_t  temp16;
	uint8_t  dal, dah, dat_len;

	if(RxdBytes<8)return(0);
  ////////////////
	dat_len=getTelLength();	//给数据长度
	if(dat_len==0)return(0);
	if(RxdBytes<(dat_len+2))return(0);

	temp16=CRC16ISR0(&RxdBuf[0],dat_len);
 
   	dal=temp16>>8;
	dah=temp16;

	if ((RxdBuf[dat_len]==dal)&&(RxdBuf[dat_len+1]==dah))
		return(2); 
	else
	{
		return(1);
	}	
}						 

uint8 leftRxdTel(void)		//数组左移一位
{
	uint8 i;
	if(RxdBytes<1)return(0);     // 无法左移
	for	(i=1;i<RxdBytes;i++)
	{
		RxdBuf[i-1]=RxdBuf[i];		
	}
	RxdBytes--;
	return(1);					 // 丢弃一个字节成功

}

 void RxdByte(uint8 c)
{	
	uint8 	i;
	RxdBuf[RxdBytes]=c;
	RxdBytes++;

	switch(RxdBytes)
	{
		case 0:	break;
		case 1:
		case 2:
				while(!isVaildTel())	//如果不合法			 
				{
					if(!leftRxdTel())break;	  // 丢弃首字节
				}
				break;
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:	break;
		default:		
				i=isTelComplete();
				if(i==2)
				{
					//do some thing
					for(i=0;i<RxdBytes;i++)	RxdTelBuf[i]=RxdBuf[i];
					ReceivedTel=1;
					RxdBytes=0;
				}
				else if(i==1)	 // CRC error
				{
					leftRxdTel();
					while(!isVaildTel())	//如果不合法			 
					{
						if(!leftRxdTel())break;
					}	
				}
				else if(i==0) //没收完继续收
				{
				
				}
				else
				{
				}
				break;

				
		}
	}


uint8_t hasByteToTxd()		 // =1 有字节待发
{
	if(TxdBytes<TxdTelBytes)return(1);
	TxdTelBytes=0;
	TxdBytes=0;
	return(0);	
}

uint8_t  getTxdByte()   //
{
	uint8 re;
	if(TxdBytes <TxdTelBytes)
	{
		re=TxdBuf[TxdBytes];
		TxdBytes++;
		return(re);
	}
	else
	{
		TxdTelBytes=0;
		TxdBytes=0;
		return(0);
	}
}
		




void OnGetData05()			   // 设置 dout;
{
	uint8	i;
	for(i=0;i<8;i++)	TxdBuf[i]=RxdTelBuf[i];
 	
	if(TxdBuf[4]==0xff)
	{
//		if(TxdBuf[3]==0)	{Dout1 = 1;RelayTimeCount[0]= 200	;flag1|=0x01;}
//		if(TxdBuf[3]==1)	{Dout2 = 1;RelayTimeCount[1]= 200	;flag1|=0x02;}
//		if(TxdBuf[3]==2)	yk3=0;
//		if(TxdBuf[3]==3)	yk4=0;
	}
	TxdTelBytes = 8;
	//YkDoingflag=1;	
}
uint16 float2uint16(float f){
	int16 t=f;
	return(*(uint16*)&t);
}
extern int32_t pf;
void OnGetData03()
{	
	uint8 	i;
	uint16 startaddr=(uint16)RxdTelBuf[2]*256+RxdTelBuf[3];
	uint16 datanum=(uint16)RxdTelBuf[4]*256+RxdTelBuf[5];
	uint16 tt;
	uint8 *pb;
	TxdBuf[0] = config.addr;
	TxdBuf[1] = 3;//0x03;
	TxdBuf[2] = datanum*2;
	pb=&TxdBuf[3];

	for(i=0;i<datanum;i++)	{
		switch(startaddr+i){
				case 0x05FA:	tt=parameter.up/100;	break; // Up 0.1V
				case 0x05FB:	tt=parameter.un/100;	break; // Un 0.1V
				case 0x05FC:	tt=parameter.ip/100;	break; // Ip 0.1mA
				case 0x05FD:	tt=parameter.in/100;	break; // In 0.1mA
				case 0x05FE:	tt=parameter.kk;	break; // In 0.1mA
	//			case 0x05FF:	tt=parameter.in/100;	break; // In 0.1mA
			
				case 0x0600:  tt=CurrentStatus;	break; // 状态字
				case 0x0601: 	tt=parameter.ubase/100;break;  
				case 0x0602:	tt=parameter.ext_temp;break; // 外部温度
				case 0x0603:	tt=parameter.curr_freq; break; // 当前频率
				case 0x0604:  tt=parameter.currI; break; // 当前电流
				case 0x0605: 	tt=parameter.currV/100; break; // 当前电压
				case 0x0606:  tt=parameter.maxValue; break; // 极限值
				case 0x0607:  tt=parameter.currZ; break; // 阻抗
				case 0x0608:  tt=parameter.currQ; break; // 功耗
				case 0x0609:  tt=parameter.currP; break; // 功耗
			  case 0x060a: 	tt=parameter.currS; break;	
				case 0x060b: 	tt=parameter.currPF; break; // 有功/总功
				case 0x060c:	tt=parameter.currDC/100; break; // 电源电压
				case 0x060d: 	tt=parameter.cc;break;
				case 0x060e:	tt=parameter.currInterTemp;break; // 当前内部温度 CPU
		

				case 0x0610:  tt=config.addr; break;
				case 0x0611:  tt=config.freq; break;
				case 0x0612:  tt=config.max_hv; break;
				case 0x0613:	tt=config.temp_start;break;
				case 0x0614:  tt=config.temp_stop;break;
				case 0x0615:  tt=config.i_gate; break;  // 电流阈值
				case 0x0616:  tt=config.config; break; // 配置
				case 0x0617:	tt=config.agc_level; break; // AGC 幅度
				
				case 0x0618:	tt=config.up_sub; break; // 电压采样修正  1L<<8 =1.0
				case 0x0619:	tt=config.un_sub; break; 
				case 0x061a:	tt=config.ip_sub; break; // 电流修正  1L<<8 =1.0 
				case 0x061b:	tt=config.in_sub; break; 
				case 0x061c:	tt=config.outp_k; break;  // 输出修正 1L<<8 =1.0
				case 0x061d:	tt=config.outn_k; break; 
				case 0x061e:	tt=config.udc_sub; break;  // 电源电压修正 1L<<8 1.0
				case 0x061f:	tt=config.max_q; break;  // 最大无功
				
				default:			tt=0x00;break;
			}
			*pb=tt>>8; pb++;
			*pb=tt&0xff; pb++;
	} 
	tt=CRC16ISR1(&TxdBuf[0],3+TxdBuf[2]);
	*pb=tt/256; *(pb+1)=tt%256; 
	TxdBytes=0;
	TxdTelBytes=5+datanum*2;
	
	startTxd();			   // 产生一个中断 ，启动发送

}
void OnSetData05(void){  /* 设置线圈 */
	uint8_t	i;
	for(i=0;i<8;i++)	TxdBuf[i]=RxdTelBuf[i];	
	TxdTelBytes = 8;	TxdBytes=0;
	startTxd();	

	{
//		if(RxdTelBuf[2]==0xBA && RxdTelBuf[3]==0x10&& RxdTelBuf[4]== 0xff ){ //??????
//			resetRunning();						/*????*/
//		}
//		else if(RxdTelBuf[2]==0xBA && RxdTelBuf[3]==0x11&& RxdTelBuf[4]== 0xff ) { /* ??485 */
//			clr485();
//		}else if(RxdTelBuf[2]==0xBA && RxdTelBuf[3]==0x12&& RxdTelBuf[4]== 0xff ) { /* ??485 */
//			set485();
//		}else if(RxdTelBuf[2]==0xBA && RxdTelBuf[3]==0x13&& RxdTelBuf[4]== 0xff ) { /* ?? */
//			if(is485()&&isElecMode()&&!isRunning())		{
//				startOpen();
//			}
//		}	else if(RxdTelBuf[2]==0xBA && RxdTelBuf[3]==0x14&& RxdTelBuf[4]== 0xff ) { /* ?? */
//			if(is485()&&isElecMode()&&!isRunning())		{
//				startShort();
//			}
//		}
	}
}

extern void update_freq(void);
void OnSetData06(void)	// 设置保持寄存器
{	
	uint8_t 	i; int16_t tt;
	for(i=0;i<8;i++)	TxdBuf[i]=RxdTelBuf[i];
	TxdTelBytes = 8;	TxdBytes=0;
	if(TxdBuf[0]!=0)startTxd();	
	tt=RxdTelBuf[4];
	tt<<=8;
	tt+=RxdTelBuf[5];
	{
		
		if(RxdTelBuf[2]==0x06 ){
			if(RxdTelBuf[3]==0x10){
				if(tt<1)tt=1;
				if(tt>247)tt=247;
				config.addr=tt;
			}else if(RxdTelBuf[3]==0x11){
				if(tt<2000)tt=2000;
				if(tt>15000)tt=15000;
				config.freq=tt;
				parameter.curr_freq=tt;
				update_freq();
			}	else if(RxdTelBuf[3]==0x12){
				if(tt<30)tt=30;
				if(tt>150)tt=150;
				config.max_hv=tt;
	//			update_hv();
			} else if(RxdTelBuf[3]==0x13){
				if(tt<0)tt=0;
				else if(tt>1500)tt=1500;
				config.temp_start=tt;
			}	else if(RxdTelBuf[3]==0x14){
				if(tt<0)tt=0;
				else if(tt>1500)tt=1500;
				config.temp_stop=tt;
			}	else if(RxdTelBuf[3]==0x15){	//电流阈值
//				if(tt<300)tt=300;	
//				else if(tt>2048)tt=2048;
				config.i_gate=tt;
			} else if(RxdTelBuf[3]==0x16){
				config.config=tt;
			}	else if(RxdTelBuf[3]==0x17){
				config.agc_level=tt;
				
			}	
			else if(RxdTelBuf[3]==0x18){				config.up_sub=tt;				}	
			else if(RxdTelBuf[3]==0x19){				config.un_sub=tt;			}
			else if(RxdTelBuf[3]==0x1a){				config.ip_sub=tt;			}
			else if(RxdTelBuf[3]==0x1b){				config.in_sub=tt;			}
			else if(RxdTelBuf[3]==0x1c){				config.outp_k=tt;			}	
			else if(RxdTelBuf[3]==0x1d){				config.outn_k=tt;			}	
			else if(RxdTelBuf[3]==0x1e){				config.udc_sub=tt;			} 
			else if(RxdTelBuf[3]==0x1f){				config.max_q=tt;			} 
			

			else {
			}
			saveConfig();
		}
		
		return;
	}
}


const     uint8_t  auchCRCHi[] = {
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40
};


const    	 uint8_t  auchCRCLo[] = {
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
0x40
};

uint16_t CRC16ISR0(uint8_t * puchMsg, uint8_t usDataLen) 	//80109
{
   uint8_t uchCRCHi=0xFF; // ??????
   uint8_t uchCRCLo=0xFF; // ??????
   uint8_t uIndex ;      //?CRC?
   while (usDataLen--) //???????
   {
	uIndex = uchCRCHi^*puchMsg++ ; //??CRC 
	uchCRCHi=uchCRCLo^auchCRCHi[uIndex] ;
	uchCRCLo=auchCRCLo[uIndex] ;
   }
	return ((((uint16_t)uchCRCHi )<< 8 )| uchCRCLo);
}
uint16_t CRC16ISR1(uint8_t * puchMsg, uint8_t usDataLen)
{
   uint8_t uchCRCHi = 0xFF ; // ??????
   uint8_t uchCRCLo = 0xFF ; // ??????
   uint8_t uIndex ;          //?CRC?
   while (usDataLen--)     //???????
   {
    uIndex = uchCRCHi ^ *puchMsg++ ; //??CRC 
    uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex] ;
    uchCRCLo = auchCRCLo[uIndex] ;
   }
	return ((((uint16_t)uchCRCHi )<< 8 )| uchCRCLo);
}

void modbusLoop()
{	
	static uint8 idata comflag_count=0;   

	if(comflag_count>0)
	{
		comflag_count--;
		if(comflag_count==0)	{
		}
	}	
	
	CloseAllInt();
	if(ReceivedTel)
	{
		ReceivedTel=0;
		OpenAllInt();
 		
		switch(getRxdTelCommand())
		{
          case 0x03:
                OnGetData03();		// 读保持寄存器
                break;
          case 0x05:                   //写线圈                 
 //               OnSetData05();
  //              break;	
					case 0x06:
								OnSetData06();				//写寄存器
                break;
		}
	}
	else
	{	
		OpenAllInt();
	}
}

#endif
