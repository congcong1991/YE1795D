#include <math.h>
#include <fft_fix.h>
#define ADCHS 6
#define FFTPOINTS 64

#define PI 3.14159265358979 
#define N  FFTPOINTS 

typedef struct comp_float
{
	float real;
	float img;
}comp_float;

typedef struct comp_int
{
	int real;
	int img;
}comp_int;

//struct comp_float  float_in[N]; // out[N];
 //__packed  struct comp_int  hex_in[N],hex_out[N];
 struct comp_int  w[N/2],in[N];

unsigned short int size_x = N;
unsigned short int m = 10;        /*级数*/  // 128点
	int comput_m(int size_x);	
	void fft(void);
	void initw(void);
void change(struct comp_int *z);
int beginFFT(unsigned char page,unsigned char ch)   // page =0  或 =1    ch =0: =1: =2: =3: =4: =5:
{
	unsigned int pa=(unsigned int )page*FFTPOINTS*6;
	unsigned int i;

	for(i=0;i<size_x;i++)
	{
//				float_in[i].real= 0.8*sin(2.0*PI*i/N)+0.6*sin(4.0*PI*i/N)+3.3/2;
//				float_in[i].img=0.0;
//				in[i].real=float_in[i].real/3.3*4096;
//				in[i].img=float_in[i].img/3.3*4096;			
				
				in[i].real=Adc_Data[pa+i*ADCHS+ch];
				in[i].img=0;
	}

	fft();
	for(i=0;i<size_x;i++)
	{
//		float_in[i].real = (float) ((float) in[i].real*3.3/4096);
//		float_in[i].img = (float) ((float) in[i].img*3.3/4096);

		
	}

    return 0;
}


void initFFT()
{
   int i=0;		
	m=comput_m(size_x);

//   w=(struct comp_int *)calloc( size_x/2, sizeof(struct comp_int) );
   for(i=0;i<size_x/2;i++)
   {   
       w[i].real = (int)(cos(2*(PI)/size_x*i)*32768);
       w[i].img = (int)(-1*sin(2*(PI)/size_x*i)*32768);
	   
   }         
}

void change(struct comp_int *z)
{  
		struct comp_int temp;
		int i=0,j=0,k=0,t;
		for(i=0;i<size_x;i++)
		{
			k=i;j=0;
			t=m;
			while(t--)
			{
					j=j<<1;
					j|=(k & 1);
					k=k>>1;
			}
			if(j>i)
			{
					temp=z[i];
					z[i]=z[j];
					z[j]=temp;
			}
   }
}

int comput_m(int n)
{
	int i=0,number;
	number=n;
	do
	{
		number>>=1;
		i++;
	}while(number!=1);
	return i;
}
	void func_die(int ,int ,int , int ); 
void fft()
{


	int  i=0,j=0,k=0;
	int  L,c_num,beg;   //组数,组内元素个数,组内计算时的起始位置
	change(in);
	for(i=0;i<m;i++)
	{
		L = pow(2,m-i-1);		    	

		for(j=0;j<L;j++)
		{
			c_num = pow(2,i+1);
			beg = j*c_num;
			for(k=0;k<c_num/2;k++)
			{
				
				func_die(beg,c_num,k,i);
			}
		}
	}
}
	void add(struct comp_int ,struct comp_int ,struct comp_int *);
	void sub(struct comp_int ,struct comp_int ,struct comp_int *);
	void mul(struct comp_int ,struct comp_int ,struct comp_int *);
void func_die(int beg,int c_num,int k,int i)
{


	int temp_real =0 ,temp_img = 0;
	struct comp_int temp1,up,down;
	
	int a,b,p;
	a = beg + k;
	b = beg + k + c_num/2;
	p = k*pow(2,m-1-i);
	mul(in[b], w[p], &up);
	add(in[a], up, &up);
	mul(in[b], w[p], &down);
	sub(in[a], down, &down);
	in[a]=up;
	in[b]=down;

	/*temp_real =  (((w[p].real*in[b].real)>>15)-((w[p].img*in[b].img)>>15))>>1;
	temp_img  =  (((w[p].real*in[b].img)>>15) + ((w[p].img*in[b].real)>>15))>>1;
	temp1.real =  (in[a].real + temp_real)>>1;
	temp1.img  =  (in[a].img +temp_img)>>1;
	temp_real =  (((w[p].real*in[b].real)>>15)-((w[p].img*in[b].img)>>15))>>1;
	temp_img  = (((w[p].real*in[b].img)>>15) + ((w[p].img*in[b].real)>>15))>>1;
	in[b].real = (in[a].real - temp_real)>>1;
	in[b].img  = (in[a].img - temp_img)>>1;
	in[a].real =temp1.real;
	in[a].img =temp1.img;*/	
	
}


void add(struct comp_int a,struct comp_int b,struct comp_int *c)//(16,15)+(16,15)->(16,14)
{
	c->real=((a.real+b.real)>>1);
	c->img=((a.img+b.img)>>1);
}

void mul(struct  comp_int a,struct comp_int b,struct comp_int *c)//(16,15)*(16,15)->(16,15)
{
	c->real=((a.real*b.real)>>15) -((a.img*b.img)>>15);
	c->img= ((a.real*b.img)>>15) +((a.img*b.real)>>15);
}

void sub(struct comp_int a,struct comp_int b,struct comp_int *c)//(16,15)+(16,15)->(16,14)
{
	c->real=((a.real-b.real)>>1);
	c->img=((a.img-b.img)>>1);
}
