#ifndef _DISPLAYH
#define _DISPLAYH
#include "datatype.h"
extern uint8 xdata	DispBuf[];   //  ��ʾ������	  =1  ��ʾ
extern uint8 xdata DispFlashBuf[];

extern void dispUINT32(uint8 lines,uint32 i,uint8 sign,uint8 deci);

#define L_0 0x0
#define L_1 0x1
#define L_2 0x2
#define L_3 0x3
#define L_4 0x4
#define L_5 0x5
#define L_6 0x6
#define L_7 0x7
#define L_8 0x8
#define L_9 0x9
#define L_A 0xa
#define L_B 0xb
#define L_C 0xc
#define L_D 0xd
#define L_E 0xe
#define L_F 0xf
#define L_G 0x10
#define L_H 0x11
#define L_I 0x12
#define L_J 0x13
#define L_K 0x14
#define L_L 0x15
#define L_M 0x16
#define L_N 0x17
#define L_O 0x18
#define L_P 0x19
#define L_Q 0x1a
#define L_R 0x1b
#define L_S 0x1c
#define L_T 0x1d
#define L_U 0x1e
#define L_V 0x1f
#define L_W 0x20
#define L_X 0x21
#define L_Y 0x22
#define L_Z 0x23
#define L_fu 0x24
#define L__ 0x25
#define L_	0x26
#define L_END 0xff

extern void clearDisplay(void);
extern void fillDisplay(void);
extern void clearUnit(void);			// �����λ��ʾ
extern void clearType(void);			// ���������ʾ
extern void clearLine(uint8 lines);	    // ������ʾ��
extern void echoChar(uint8 c,uint8 pos);  // ��ָ����MENULINE����ʾһ����  pos ������������ 	
extern void dispStr(uint8 lines,const uchar* s);
extern void initDisplay(void);
extern void DisplayON(void);
extern void DisplayOFF(void);
extern void	displayLoop(void);
extern void dispChar(uint8 pos,uint8 c);   // ��ʾһ���ַ���һ��ָ����λ�ã� pos ��ȫ��Ψһ��λ��	
//extern void dispByte(uint8 line,uint8 pos,uint8 c);  // ��ָ������ʾ�ַ��� pos �������� 0,1,2,3
#endif
