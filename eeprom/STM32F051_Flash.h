#ifndef _STM32F051FLASHH
#define _STM32F051FLASHH
#include "conf.h"
#include "DataType.h"


#ifndef XBYTE
#define XBYTE 		((unsigned char volatile xdata *) 0)
#endif
extern void WRABYTE(uint16 addr,uint8 dat);
extern uint8 RDABYTE(uint16 addr);
extern void initEEPROM(void);
extern void saveConfig(void);
extern void loadConfig(void);
extern void saveAdjustData(void);

#endif
