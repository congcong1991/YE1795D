#ifndef _FFT_H
#define _FFT_H
typedef struct complex //¸´ÊıÀàĞÍ   
{  
    float real;  
    float imag;  
}complex;  


extern int beginFFT(unsigned char page,unsigned char ch);
extern 	void initFFT(void);
extern  void	fft(void);
extern  unsigned short int Adc_Data[];

#endif
