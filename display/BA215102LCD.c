#include "conf.h"
#ifdef BA215102

#include "display.h"
#include "HT1623.h"
#include "main.h"

uint8 xdata	DispBuf[DISPBUFLEN];   //  显示缓冲区	  =1  显示
uint8 xdata DispFlashBuf[DISPBUFLEN];  // 闪烁标志	  =1  闪烁
void DisplayON(void)    // 打开显示
{
	bg_lcd_set();
}
void DisplayOFF(void)   // 关闭显示 
{		
bg_lcd_clr();
}

void clearUnit(void)     //  清除 所有单位显示
{
	DispBuf[37]=0;
	DispBuf[39]&= 0x08;
}
void clearType(void)    //  清除 所有测量类型
{	
	DispBuf[16]=0;
	DispBuf[18]=0;
}
void displayLoop()
{
	static uint16 idata  flashcount=0;
	static uint8 idata i=1;
	uint8  idata ds0=DispBuf[i];
	uint8 idata ds1=DispBuf[(i+1)%DISPBUFLEN]; 
	uint8 idata fds0=DispFlashBuf[i] ;  
	uint8 idata fds1=DispFlashBuf[(i+1)%DISPBUFLEN];
	lcd_cs_clr();//Delay(2);
	SendBit_1623(0xa0,3);			//写入标志码"101"
	SendBit_1623((i<<2),7);			//写入addr的高6位
	
	if(flashcount<300)					  // 闪烁控制
	{									  
		SendDataBit_1623(ds0,4);
		SendDataBit_1623(ds1,4);
		SendDataBit_1623(ds0>>4,4);
		SendDataBit_1623(ds1>>4,4);

	}
	else
	{
		SendDataBit_1623((ds0 & ~fds0),4);		  
		SendDataBit_1623((ds1 & ~fds1),4);		  
		SendDataBit_1623((ds0 & ~fds0)>>4,4);		  
		SendDataBit_1623((ds1 & ~fds1)>>4,4);		  

	}																
																  
	lcd_cs_set();												 
	i+=2;
	if(i>=(DISPBUFLEN))i=1;

	flashcount++;
	if(flashcount>=600)flashcount=0;
}
	
//			 /--4-- /			  位 21 23 25 27
//			 0	  5					 22 24 26 28 
//			/-1 - /					 29 31 33 35
//		   2	 6
//		3./--7--/   大数字 段码表
const code uint8    BigLCDTab[] ={		//D C B A dop	E G F
//0		 1				2		  	3			4		  	5			6		  7			  8			9
0xF5,  	0x60,		0xB6,		0xF2,		0x63,		0xD3,		0xD7,		0x70,		0xF7,		0xF3,
//A		 b		  	   C		   d		  	E			F		  g		      L		 	  n			o		 		 
0x77,  	0xC7,		0x95,		0xE6,		0x97,		0x17,		0xF3,		0x85,		0x46,		0xC6,
//p	     q		  	   S		   t	  		U			H	  		y		  				 		 
0x37,  	0x73,		0xD3,		0x87,		0xE5,		0x67,		0xE3,	    0x00
};
//
//		  /--3--/		  //  小数字，前段 字符 段表
//		 7	   2
//		 /--6--/
//		5	  1			  位 11				14,12,10,8,6,3,2,1,0
//	 4./--0--/
const code uint8	SmallLCDTab1[]={		//B C D dop A F G E 
//0		 1			2		  	3			4		  	5			6		  	7			8			9
0xAF,  	0x06,		0x6D,		0x4F,		0xC6,		0xCB,		0xEB,		0x0E,		0xEF,		0xCF,
//A		 b		  	C			d		  	E			F		  	g		    H			I		  	J 		K		L		   m	 	n			o		 		 
0xEE,  	0xE3,		0xA9,		0x67,		0xE9,		0xE8,		0xAB,		0xE6,		0xA0,		0x07,	0xE5,	0xA1,		0xA7,	0x62,		0x63,
//p		 q		  	r			S			t	  		U			V	  		W			x			y			z		-	 	_		' ' 
0xEC,  	0xCE,		0x60,		0xCB,		0xE1,		0xA7,		0x21,		0xE1,		0xC4,		0xC7,	   0x41, 	0x40,	0x01,	0x00, 
}; 

	

void flashChar(uint8 pos,uint8 c)
{
	uint8 t;
	if(pos==0)
	{
		DispFlashBuf[0]&=0x0f;
		DispFlashBuf[0]|=(SmallLCDTab1[c]<<4) ;
		DispFlashBuf[2]&=0xf0; 
		DispFlashBuf[2]|=(SmallLCDTab1[c]>>4) ;
		return;
	}
	if(pos==1)
	{
		DispFlashBuf[39]&=~0x08;
		DispFlashBuf[39]|=(SmallLCDTab1[c]&0x08) ;
		DispFlashBuf[38]&=0x1f; 
		DispFlashBuf[38]|=(SmallLCDTab1[c]&0xE0) ;
		t=SmallLCDTab1[c]<<1;
		DispFlashBuf[40]&=0xf0;
		DispFlashBuf[40]|=(t&0x0f) ;
		return;

	}
	if(pos==2)
	{
		t=SmallLCDTab1[c];
		DispFlashBuf[38]&=0xf0; 
		DispFlashBuf[38]|=(t&0x0f) ;
		DispFlashBuf[2]&=0x0f;
		DispFlashBuf[2]|=(t&0xf0) ;
		return;

	}
	if(pos==3)
	{
		t=SmallLCDTab1[c];
		DispFlashBuf[3]&=~0x08; 
		DispFlashBuf[3]|=(t&0x08) ;
		DispFlashBuf[4]&=0x1f;
		DispFlashBuf[4]|=(t&0xE0) ;
		DispFlashBuf[4]&=0xf1;
		DispFlashBuf[4]|=((t<<1)&0x0E) ;

		return;

	}

	if(pos<21)
		DispFlashBuf[pos]=SmallLCDTab1[c];	
	else
		DispFlashBuf[pos]=BigLCDTab[c];

}
void dispChar(uint8 pos,uint8 c)
{
	uint8 t;
	if(pos==0)
	{
		DispBuf[0]&=0x0f;
		DispBuf[0]|=(SmallLCDTab1[c]<<4) ;
		DispBuf[2]&=0xf0; 
		DispBuf[2]|=(SmallLCDTab1[c]>>4) ;
		return;
	}
	if(pos==1)
	{
		DispBuf[39]&=~0x08;
		DispBuf[39]|=(SmallLCDTab1[c]&0x08) ;
		DispBuf[38]&=0x1f; 
		DispBuf[38]|=(SmallLCDTab1[c]&0xE0) ;
		t=SmallLCDTab1[c]<<1;
		DispBuf[40]&=0xf0;
		DispBuf[40]|=(t&0x0f) ;
		return;

	}
	if(pos==2)
	{
		t=SmallLCDTab1[c];
		DispBuf[38]&=0xf0; 
		DispBuf[38]|=(t&0x0f) ;
		DispBuf[2]&=0x0f;
		DispBuf[2]|=(t&0xf0) ;
		return;

	}
	if(pos==3)
	{
		t=SmallLCDTab1[c];
		DispBuf[3]&=~0x08; 
		DispBuf[3]|=(t&0x08) ;
		DispBuf[4]&=0x1f;
		DispBuf[4]|=(t&0xE0) ;
		DispBuf[4]&=0xf1;
		DispBuf[4]|=((t<<1)&0x0E) ;

		return;

	}

	if(pos<21)
		DispBuf[pos]=SmallLCDTab1[c];	
	else
		DispBuf[pos]=BigLCDTab[c];

}

void echoChar(uint8 c,uint8 pos)
{
	static xdata uint8 ptab[]={0,1,2,3,6,8,10,12,14};
	dispChar(ptab[pos],c);	
}
void clearLine(uint8 lines)
{
	switch(lines)   
	{
		case 1:		  // 21 23 25 27
				dispChar(21,L_);
				dispChar(23,L_);
				dispChar(25,L_);
				dispChar(27,L_);
				break;
		case 2:	dispChar(22,L_);
				dispChar(24,L_);
				dispChar(26,L_);
				dispChar(28,L_);
				break;
		case 3: dispChar(29,L_);
				dispChar(31,L_);
				dispChar(33,L_);
				dispChar(35,L_);
				break;
		case 4:							   //14,12,10,8,6,3,2,1,0
				dispChar(14,L_);
				dispChar(12,L_);
				dispChar(10,L_);
				dispChar(8,L_);
				dispChar(6,L_);
				dispChar(3,L_);
				dispChar(2,L_);
				dispChar(1,L_);
				dispChar(0,L_);
				break;
		default:
				break;
	}
	
}

void dispStr(uint8 lines,const uchar *s)
{
	
	uchar c;uint8 i;
	clearLine(4);
	for(i=0;i<9;i++)
	{
		c=s[i];	
		if(c!=L_END)
		{
			echoChar(c,8-i);
		}
		else
		{
			return;
		}
	}
}
void dispDP(uint8 lines,uint8 deci)    //高字节 对应缓冲区位置,低字节,对应小数点位置
{
	switch(lines)
	{
		case 1:	if(deci==1)		{ SetBit(DispBuf[27],3);  }
				else if(deci==2)	{ SetBit(DispBuf[25],3);}
				else if(deci==3)	{ SetBit(DispBuf[23],3);}
				else if(deci==4) 	{}
				else   {}
				return;
		case 2:	if(deci==1)		{ SetBit(DispBuf[28],3);  }
				else if(deci==2)	{ SetBit(DispBuf[26],3);}
				else if(deci==3)	{ SetBit(DispBuf[24],3);}
				else if(deci==4) 	{}
				else   {}
				return;
		case 3:	if(deci==1)		{ SetBit(DispBuf[35],3);  }
				else if(deci==2)	{ SetBit(DispBuf[33],3);}
				else if(deci==3)	{ SetBit(DispBuf[31],3);}
				else if(deci==4) 	{}
				else   {}
				return;
		case 4:	
					if(deci==1)			{ SetBit(DispBuf[0],4);  }
					else if(deci==2){ SetBit(DispBuf[40],0); }
					else if(deci==3){ SetBit(DispBuf[2],4); }
					else if(deci==4){}
					else   {}
					return;
		default:break;
	}
	
} 
void flashByte(uint8 lines,uint8 pos)	   // 显示整型值   lines=1 第1行，第2行，第3行，第4行，
{					
	uint8 code line1PosTab[]={27,25,23,21};
	uint8 code line2PosTab[]={28,26,24,22};
	uint8 code line3PosTab[]={29,31,33,35};
	uint8 code line4PosTab[]={0,1,2,3,6,8,10,12,14};

	switch(lines)   
	{
		case 1:	flashChar(line1PosTab[pos],8);
				break;
		case 2:	flashChar(line2PosTab[pos],8);
				break;
		case 3: flashChar(line3PosTab[pos],8);
				break;
		case 4:	flashChar(line4PosTab[pos],8);
				break;
		default:
				break;
	}

}
void unFlashByte(uint8 lines,uint8 pos)	   // 显示整型值   lines=1 第1行，第2行，第3行，第4行，
{					
	uint8 code line1PosTab[]={27,25,23,21};
	uint8 code line2PosTab[]={28,26,24,22};
	uint8 code line3PosTab[]={29,31,33,35};
	uint8 code line4PosTab[]={0,1,2,3,6,8,10,12,14};

	switch(lines)   
	{
		case 1:	flashChar(line1PosTab[pos],L_);
				break;
		case 2:	flashChar(line2PosTab[pos],L_);
				break;
		case 3: flashChar(line3PosTab[pos],L_);
				break;
		case 4:	flashChar(line4PosTab[pos],L_);
				break;
		default:
				break;
	}

}
void dispTHDU(uint16 i){
	clearLine(4);
	if(i>1){
		dispChar(0,i%10);
		dispChar(1,(i%100)/10);
	}
	dispChar(14,L_T);dispChar(12,L_H);dispChar(10,L_D);
	dispChar(8,L_U);
}

void dispTHDI(uint16 i){
		clearLine(4);
	if(i>1){
		dispChar(0,i%10);
		dispChar(1,(i%100)/10);
	}
	dispChar(14,L_T);dispChar(12,L_H);dispChar(10,L_D);
	dispChar(8,L_I);
}
extern void dispTHDI(uint16);

void dispUINT32(uint8 lines,uint32 i,uint8 sign,uint8 deci)	   // 显示整型值   lines=1 第1行，第2行，第3行，第4行，
{															  // deci 小数位数 =0 整数 
	uchar dispZero=0;uchar j;
	switch(lines)   
	{
		case 1:		  // 21 23 25 27
				dispChar(21,(i%10000)/1000);
				dispChar(23,(i%1000)/100);
				dispChar(25,(i%100)/10);
				dispChar(27,(i%10));
				if(sign==1) {dispSignLine1();}else{unDispSignLine1();}
				break;
		case 2:	dispChar(22,(i%10000)/1000);
				dispChar(24,(i%1000)/100);
				dispChar(26,(i%100)/10);
				dispChar(28,(i%10));
				if(sign==1){dispSignLine2();}else{unDispSignLine2();}
				break;
		case 3: dispChar(29,(i%10000)/1000);
				dispChar(31,(i%1000)/100);
				dispChar(33,(i%100)/10);
				dispChar(35,(i%10));
 				if(sign==1){dispSignLine3();}else{unDispSignLine3();}
				break;
		case 4:							   //14,12,10,8,6,3,2,1,0
//				dispChar(14,(i%1000000000)/100000000);
				i=(i%100000000);j=(i/10000000); if(j==0 && dispZero==0){dispChar(12,L_);}else{dispChar(12,j);dispZero=1;};
				i=(i%10000000);j=i/1000000;if(j==0 && dispZero==0){dispChar(10,L_);}else{dispChar(10,j);dispZero=1;}
				i=(i%1000000);j=i/100000;if(j==0 && dispZero==0){dispChar(8,L_);}else{dispChar(8,j);dispZero=1;}
				i=(i%100000);j=i/10000;if(j==0 && dispZero==0){dispChar(6,L_);}else{dispChar(6,j);dispZero=1;}
				dispChar(3, (i%10000)/1000);
				dispChar(2, (i%1000)/100);
				dispChar(1, (i%100)/10);
				dispChar(0, (i%10));
				break;
		default:
				break;
	}

	dispDP(lines,deci);
}




#endif
