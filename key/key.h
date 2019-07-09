#ifndef KEYH
#define  KEYH
#include "DataType.h"


#define ESCKEY	  1
#define UPKEY	  2
#define DOWNKEY	  3
#define ENTERKEY  4



extern void OnKeyPress(uchar key);
extern void OnKeyDown(uchar key);
extern void OnKeyUp(uchar key);
extern void OnKeyDown3s(uchar key);
extern void OnKeyDown5s(uchar key);
extern void OnKeyDown10s(uchar key);

extern void OnKeyUp10s(uchar key);
extern uchar keyLoop(void);	   // 50ms 调用一次
extern void initKey(void);


#endif
