#ifndef DSPH
#define DSPH
#include "datatype.h"

extern void initDSP(void);
extern void readEMU(void);
extern uchar OnAdjust(uchar status);    //
extern void startEMU(void);
extern void clearPower(void);
#ifdef STM32F0XX
extern uint64_t EEP;    // �й�����Ͱ��   1LSB= 1Wh/(1<<16)   ��48λ����ʾ wh
extern uint64_t EEQ;
extern uint64_t EES;
#endif

#endif
