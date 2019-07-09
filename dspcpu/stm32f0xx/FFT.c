#include "FFT.h"
#include "conf.h"
#include <math.h>

COMPLEX FFTData[N];

//Ѱ�Ҷ�Ӧ��ǰ�±�ķ�ת�±�
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


//��ת�±�
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
//��ʱ���ȡ����fft�任�����뷴���������
void fft()//xΪ���任���飬Ҳ����������飬NΪ������֧��1024�������ĵ����������޸ķ�ת�±꺯�����ɣ�
{					
	uint32_t j,k,u=0,l=0,wi=0;				//j�ڶ���ѭ�����ӿ��е�ÿ�����ε�ѭ��������
	//k��һ��ѭ��������fft�任������Ϊlog2��N��
	//u �����ϱ�x[upper],l �����±�x[lower]��wi��ת�����±�wn[wi]
	uint32_t SubBlockNum,SubBlockStep=1;		//SubBlockNum��ǰk���ӿ�������SubBlockStep��ǰk�㲻ͬ�ӿ����ͬλ��Ԫ�ؼ���
				//wnΪ��ת�������飬XkWnΪ��ʱ�洢��ǰ���ε���ת���ӵ���ʱ����			

	ReverseIndex();					//���뷴��
//	 selftest_set();
	for(k=N;k>1;k=(k>>1))				//��һ��ѭ��������log2(k)�׵ı任
	{			
		SubBlockNum=k>>1;				//�ӿ����Ϊ����������һ��
		SubBlockStep=SubBlockStep<<1;	//�ӿ��ͬ�ȵ�λ��Ԫ�ؼ����2Ϊ��������
		wi=0;							//��ת���ӳ�ʼ��
		for(j=0;j<SubBlockStep>>1;j++)	//�ڶ���ѭ��������jֵ��j��ʾ�����ӿ�ĵ�j�����Ρ���Ϊÿ���ӿ��ͬ��λ���ξ�����ͬ��wn�������õڶ���ѭ������wn
		{
			for(u=j;u<N;u+=SubBlockStep)//������ѭ����ѭ���ڸ����ӿ��ĵ�j�����Σ��������е��Ρ�ֱ���±�uԽ�硣(u>N)
			{				
				l=u+(SubBlockStep>>1);	//�±�l����
				XkWn.real=FFTData[l].real*wn[wi].real/ONE-FFTData[l].imag*wn[wi].imag/ONE;//����x[u]=x[u]+x[l]*Wn,x[l]=x[u]-x[l]*Wn�ĸ�������
				XkWn.imag=FFTData[l].imag*wn[wi].real/ONE+FFTData[l].real*wn[wi].imag/ONE;
				FFTData[l].real=FFTData[u].real-XkWn.real;
				FFTData[l].imag=FFTData[u].imag-XkWn.imag;
				FFTData[u].real+=XkWn.real;
				FFTData[u].imag+=XkWn.imag;
			}
			wi+=SubBlockNum;			//�ڶ���ѭ������wiֵ
		}
	}
}

void initFFT()
{
   int k;		
	for (k=0;k<N/2;k++)
	{				//��ʼ��wn����
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
float getInvAngle(struct Complex *dat)     // �������źŷ���ʱ�����
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
