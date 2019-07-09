
#ifndef STM32F030H
#define STM32F030H
#include  "stm32f0xx.h"

#ifdef STM32F030
#define xdata
#define idata
#define bdata 
#define code  
#define bit uint8
#define pdata  
#define data
#define reentrant
#else
#define XDATA()    xdata
#define IDATA() 	 idata
#define BDATA()			bdata
#define CODE()			code
#define BIT() uint8
#define PDATA()   pdata
#define DATA()		data
#endif

#include  "stm32f0xx_conf.h"
// 关闭所有中断
#define CloseAllInt()      __set_PRIMASK(1) 

// 打开所有中断
#define OpenAllInt()      __set_PRIMASK(0)  

#define initT0  initSysTick1ms
extern void initDSP(void);
extern void initSysTick1ms(void);
extern void initPLL(void);
extern void emuLoop(void);
extern void startEMU(void);
extern void setCom(unsigned char  baudrate,unsigned char parity);

extern void initWatchdog(void);

#endif

