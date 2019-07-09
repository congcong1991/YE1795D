#ifndef DATATYPEH
#define DATATYPEH

#if defined(STM32F0XX ) || defined(STM8S105) || defined(STM8S103)
#define xdata
#define idata
#define bdata 
#define code  
#define bit uint8
#define pdata  
#define data
#else
#define __packed 
#define const
#endif

#define  uint8 unsigned char
#define int8 signed char
#define uchar unsigned char
#define  uint16 unsigned short int
#define  int16 short  int
#define  uint32 unsigned long  int
#define  int32 long int

#ifndef SetBit
	#define SetBit(addr,bit) {addr|=(1<<bit);}
#endif
#ifndef ClrBit
	#define ClrBit(addr,bit) {addr&=~(1<<bit);}
#endif
#ifndef CplBit
	#define CplBit(addr,bit) {addr^= (1<<bit);}
#endif
#ifndef TestBit
	#define TestBit(p,bit)  ((p & (1<<bit))!=0)
#endif
#endif 
