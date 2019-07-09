#include "FFT.h"
#include "conf.h"
#include <math.h>

COMPLEX FFTData[N];

//寻找对应当前下标的反转下标
uint32_t FindIndex11bit(uint16_t k)
{
	uint32_t j;
	k=k&(0x7ff);
	j=((k&0x001)<<10)+((k&0x002)<<8)+((k&0x004)<<6)+((k&0x008)<<4)+((k&0x010)<<2)\
		+((k&0x020))+((k&0x040)>>2)+((k&0x080)>>4)+((k&0x100)>>6)+((k&0x200)>>8)+((k&0x400)>>10);
	return j;
}

uint32_t FindIndex10bit(uint16_t k)
{
	uint16_t j;
	k=k&(0x3ff);
	j=((k&0x001)<<9)+((k&0x002)<<7)+((k&0x004)<<5)+((k&0x008)<<3)+((k&0x010)<<1)+\
		((k&0x020)>>1)+((k&0x040)>>3)+((k&0x080)>>5)+((k&0x100)>>7)+((k&0x200)>>9);
	return j;
}

uint32_t FindIndex9bit(uint16_t k)
{
	uint32_t j;
	k=k&(0x1ff);
	j=((k&0x001)<<8)+((k&0x002)<<6)+((k&0x004)<<4)+((k&0x008)<<2)+((k&0x010))\
		+((k&0x020)>>2)+((k&0x040)>>4)+((k&0x080)>>6)+((k&0x100)>>8);
	return j;
}

uint8_t FindIndex8bit(uint8_t k)
{
	uint8_t j;
	k=k&(0xff);
	j=((k&0x001)<<7)+((k&0x002)<<5)+((k&0x004)<<3)+((k&0x008)<<1)+((k&0x010)>>1)\
		+((k&0x020)>>3)+((k&0x040)>>5)+((k&0x080)>>7);
	return j;
}

uint8_t FindIndex7bit(uint8_t k)
{
	uint8_t j;
	k=k&(0x7f);
	j=((k&0x001)<<6)+((k&0x002)<<4)+((k&0x004)<<2)+((k&0x008))+((k&0x010)>>2)+((k&0x020)>>4)+((k&0x040)>>6);
	return j;
}

uint8_t FindIndex6bit(uint8_t k)
{
	uint8_t j;
	k=k&(0x7f);
	j=((k&0x001)<<5)+((k&0x002)<<3)+((k&0x004)<<1)+((k&0x008)>>1)+((k&0x010)>>3)+((k&0x020)>>5);
	return j;
}


//反转下标
void ReverseIndex()
{
	uint32_t i,j;
	COMPLEX tmp;
	for (i=0;i<N;i++)
	{
		j=FindIndex(i);
		if (j>i)
		{
			tmp.real=FFTData[i].real;
			tmp.imag=FFTData[i].imag;
			FFTData[i].real =FFTData[j].real;
			FFTData[i].imag =FFTData[j].imag;
			FFTData[j].real=tmp.real;
			FFTData[j].imag=tmp.imag;
		}
	}
}
#define ONE	(1L<<12)

COMPLEX wn[N/2],XkWn;	
//按时间抽取法的fft变换，输入反序，输出正序
void fft()//x为欲变换数组，也当作输出数组，N为点数，支持1024（其它的点数请自行修改反转下标函数即可）
{					
	uint32_t j,k,u=0,l=0,wi=0;				//j第二层循环（子块中的每个蝶形的循环计数）
	//k第一层循环（横向fft变换阶数，为log2（N）
	//u 蝶形上标x[upper],l 蝶形下标x[lower]，wi旋转因子下标wn[wi]
	uint32_t SubBlockNum,SubBlockStep=1;		//SubBlockNum当前k层子块数量，SubBlockStep当前k层不同子块的相同位置元素间间隔
				//wn为旋转因子数组，XkWn为临时存储当前蝶形的旋转因子的临时变量			

	ReverseIndex();					//输入反序
//	 selftest_set();
	for(k=N;k>1;k=(k>>1))				//第一个循环，代表log2(k)阶的变换
	{			
		SubBlockNum=k>>1;				//子块个数为所做点数的一半
		SubBlockStep=SubBlockStep<<1;	//子块间同等地位的元素间隔以2为倍数递增
		wi=0;							//旋转因子初始化
		for(j=0;j<SubBlockStep>>1;j++)	//第二层循环，更新j值，j表示各个子块的第j个蝶形。因为每个子块的同地位蝶形具有相同的wn，所以用第二层循环控制wn
		{
			for(u=j;u<N;u+=SubBlockStep)//第三层循环，循环于各个子块间的第j个蝶形，计算所有蝶形。直到下标u越界。(u>N)
			{				
				l=u+(SubBlockStep>>1);	//下标l计算
				XkWn.real=FFTData[l].real*wn[wi].real/ONE-FFTData[l].imag*wn[wi].imag/ONE;//蝶形x[u]=x[u]+x[l]*Wn,x[l]=x[u]-x[l]*Wn的复数计算
				XkWn.imag=FFTData[l].imag*wn[wi].real/ONE+FFTData[l].real*wn[wi].imag/ONE;
				FFTData[l].real=FFTData[u].real-XkWn.real;
				FFTData[l].imag=FFTData[u].imag-XkWn.imag;
				FFTData[u].real+=XkWn.real;
				FFTData[u].imag+=XkWn.imag;
			}
			wi+=SubBlockNum;			//第二层循环更新wi值
		}
	}
}

void initFFT()
{
   int k;		
	for (k=0;k<N/2;k++)
	{				//初始化wn数组
		wn[k].real=(int)(cos(2*3.14159265358979*k/N)*ONE);
		wn[k].imag=(int)(sin(-2*3.14159265358979*k/N)*ONE);
	}

}



int beginFFT(unsigned int pa)   // pa adc startaddress
{
		unsigned int i;
		for(i=0;i<N;i++)
		{
				FFTData[i].real=Adc_Data[pa+i*ADCHS]*2;
				FFTData[i].imag=0;
		}
	fft();

    return 0;
}
float getMod(struct Complex *dat)
{
	return(sqrt((float)dat->real*dat->real+(float)dat->imag*dat->imag));
}
float getAngle(struct Complex *dat)
{
		float a;
		float mod=getMod(dat);
		if((mod)<(5*N))return(0);
		a=acos(fabs((float)dat->real)/mod);
		if(dat->real>=0 && dat->imag>=0) return(a);
		if(dat->real<=0 && dat->imag>=0) return(PI-a);
		if(dat->real<=0 && dat->imag<=0) return(PI+a);
		if(dat->real>=0 && dat->imag<=0) return(2.0*PI-a);
		return(0);
			
}
float getInvAngle(struct Complex *dat)     // 求输入信号反相时的相角
{
		float a;
		float mod=getMod(dat);
		if((mod)<(5*N))return(0);
		a=acos(fabs((float)dat->real)/mod);
		if(dat->real>=0 && dat->imag>=0) return(PI+a);
		if(dat->real<=0 && dat->imag>=0) return(2*PI-a);
		if(dat->real<=0 && dat->imag<=0) return(a);
		if(dat->real>=0 && dat->imag<=0) return(PI-a);
		return(0);
			
}
