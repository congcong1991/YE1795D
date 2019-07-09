#ifndef _FFT_H_
#define _FFT_H_  
#include "conf.h"
#define ADCHS 3


#define PI 3.14159265358979 
#define N  64



					//FFT����

#if (N==2048)
#define FindIndex FindIndex11bit
#elif (N==1024)
#define FindIndex FindIndex10bit
#elif  (N==512)
#define FindIndex FindIndex9bit
#elif  (N==256)
#define FindIndex FindIndex8bit
#elif  (N==128)
#define FindIndex FindIndex7bit
#elif (N==64)
#define FindIndex FindIndex6bit
#endif


struct Complex
{							//���츴���ṹ
	signed int real;
	signed int imag;     //   ���㣬 8λС��  24λ����
};

typedef struct Complex COMPLEX;
extern COMPLEX FFTData[];
//����FFT���㻺����
//FFT_EXT COMPLEX FFTData[];
//extern FFT_EXT uint32_t y[N];

uint32_t FindIndex11bit(uint16_t k);
uint32_t FindIndex10bit(uint16_t k);
uint32_t FindIndex9bit(uint16_t k);
uint8_t FindIndex8bit(uint8_t k);
uint8_t FindIndex7bit(uint8_t k);
uint8_t FindIndex6bit(uint8_t k);
extern void ReverseIndex(void);
extern void fft(void);

extern int beginFFT(unsigned int pa);
extern 	void initFFT(void);
extern float getMod(struct Complex *data);
extern float getAngle(struct Complex *dat);   //  ����
extern float getInvAngle(struct Complex *dat) ;    // �������źŷ���ʱ�����
extern  unsigned short int Adc_Data[];
extern COMPLEX wn[N/2];
#endif

