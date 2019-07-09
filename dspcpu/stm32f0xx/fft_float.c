#include "stm32f0xx.h"
#include "fft_float.h"
#include "math.h"

#define ADCHS 6
#define FFTPOINTS 64

#define PI 3.14159265358979 
#define N  FFTPOINTS 

#define SWAP(a,b) { tempr=(a);(a)=(b);(b)=tempr ;}


struct complex f1[FFTPOINTS];

/*-------------------------复数数组相加----------------------*/
void c_plus(complex a,complex b,complex *c)  
{  
    c->real = a.real + b.real;  
    c->imag = a.imag + b.imag;  
} 


/*-------------------------复数数组相减----------------------*/
void c_sub(complex a,complex b,complex *c)  
{  
    c->real = a.real - b.real;  
    c->imag = a.imag - b.imag;     
}

/*-------------------------复数数组相乘--------------------*/
void c_mul(complex a,complex b,complex *c)  
{  
    c->real = a.real * b.real - a.imag * b.imag;  
    c->imag = a.real * b.imag + a.imag * b.real;   
} 

/*-------------------------复数数组相除---------------------*/ 
void c_div(complex a,complex b,complex *c)  
{  
    c->real = (a.real * b.real + a.imag * b.imag)/(b.real * b.real +b.imag * b.imag);  
    c->imag = (a.imag * b.real - a.real * b.imag)/(b.real * b.real +b.imag * b.imag);  
}  
 
  
/*----------------------------旋转因子-------------------------*/  
void Wn_i(uint16_t n,uint16_t i,complex *Wn,uint8_t flag)  
{  
    Wn->real = cos(2*PI*i/n);  
    if(flag == 1)  
    Wn->imag = -sin(2*PI*i/n);  
    else if(flag == 0)  
    Wn->imag = -sin(2*PI*i/n);  
}  
//////////////////////////////////////////   
void fft()  
{  
    complex t,wn;//中间变量   
    uint16_t i,j,k,m,n,l,r,M;  
    uint16_t la,lb,lc;  
  
      
    /*----计算分解的级数M=log2(N)----*/  
    for(i=N,M=1;(i=i/2)!=1;M++);   
      
    /*----按照倒位序重新排列原信号----*/ 
    for(i=1,j=N/2;i<=N-2;i++)  
    {  
        if(i<j)  
        {  
            t=f1[j];  
            f1[j]=f1[i];  
            f1[i]=t;  
        }  
        k=N/2;  
        while(k<=j)  
        {  
            j=j-k;  
            k=k/2;  
        }  
        j=j+k;  
    }  
  
    /*----FFT算法---- */
    for(m=1;m<=M;m++)  
    {  
        la=pow(2,m); //la=2^m代表第m级每个分组所含节点数        
        lb=la/2;    //lb代表第m级每个分组所含碟形单元数   
                     //同时它也表示每个碟形单元上下节点之间的距离   
        /*----碟形运算----  */
        for(l=1;l<=lb;l++)  
        {  
            r=(l-1)*pow(2,M-m);   
            for(n=l-1;n<N-1;n=n+la) //遍历每个分组，分组总数为N/la   
            {  
                lc=n+lb;  //n,lc分别代表一个碟形单元的上、下节点编号        
                Wn_i(N,r,&wn,1);//wn=Wnr   
                c_mul(f1[lc],wn,&t);//t = f[lc] * wn复数运算   
                c_sub(f1[n],t,&(f1[lc]));//f[lc] = f[n] - f[lc] * Wnr   
                c_plus(f1[n],t,&(f1[n]));//f[n] = f[n] + f[lc] * Wnr   
            }  
  
        }  
    }  
}  
void initFFT(void)
{
}

int beginFFT(unsigned char page,unsigned char ch)   // page =0  或 =1    ch =0: =1: =2: =3: =4: =5:
{
	unsigned int pa=(unsigned int )page*FFTPOINTS*6;
	unsigned int i;

	for(i=0;i<N;i++)
	{
//				float_in[i].real= 0.8*sin(2.0*PI*i/N)+0.6*sin(4.0*PI*i/N)+3.3/2;
//				float_in[i].img=0.0;
//				in[i].real=float_in[i].real/3.3*4096;
//				in[i].img=float_in[i].img/3.3*4096;			
				
				f1[i].real=Adc_Data[pa+i*ADCHS+ch];
				f1[i].imag=0;
	}

	fft();
	for(i=0;i<N;i++)
	{
//		float_in[i].real = (float) ((float) in[i].real*3.3/4096);
//		float_in[i].img = (float) ((float) in[i].img*3.3/4096);

		
	}

    return 0;
}
				
				
 
