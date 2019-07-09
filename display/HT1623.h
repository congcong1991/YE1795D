#ifndef HT1623MAINH
#define HT1623MAINH
#include "datatype.h"
#include "conf.h"
#define SYSEN 	0x02
#define LCDON 	0x06
#define LCDOFF 	0x04
#define BIAS    0X52               //LCD 1/3 bias option 1/3 bais . 4 duty
#define RC256   0X30               //System clock source, on-chip RC oscillator



extern void SendBit_1623(uint8 tmp_data,uint8 tmp_cnt);

extern void SendDataBit_1623(uint8 tmp_data,uint8 tmp_cnt);

extern void SendCmd(uint8 lcd_command);

extern void WriteAll_1623(uint8 tmp_addr,uint8 *tmp_p,uint8 tmp_cnt);

#endif
