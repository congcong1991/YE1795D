#ifndef CONFH
#define CONFH

///   ����  dsp and cpu 


#define STM32EMU

//#define DISABLE_MODBUS

#include "stm32f030.h"


//#include "display.h"


extern void  Delay(uint16_t count);
#include "main.h"

//#define CH423
//#define HC164
// �Լ���ʾ�˿� led
#define selftest_set() {GPIO_SetBits(GPIOC,GPIO_Pin_4);}
#define selftest_clr()	{GPIO_ResetBits(GPIOC,GPIO_Pin_4);}
extern void initSelfTestPort(void);
#define INITSELFTEST() {initSelfTestPort();}



//#define DEBUG

// WATCHDOG
#ifdef DEBUG
#define watchdog()	 { }
#else
#define watchdog()	 { IWDG_ReloadCounter();}
#endif

// 485
// PA11=0 send  PA11=1  ���� 
//#define Enable485TXD() { }
//#define Disable485TXD() {}
extern void InitRS485Port(void);
#define INITRS485PORT() {}
#endif		     //  end of file 

//emu
//������ƽ��ֵ�ĵ���
#define AVRNUMBER 10
