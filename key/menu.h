#ifndef MENUH
#define MENUH
#include "datatype.h"

typedef struct EDITDATA
{
	void *dat;   // ���༭����ָ��
	uint8 type;  // =0   =1 uint8  =2 uint16 =3 uint 24  =4 uint16 
	uint32 max;  // ����ֵ ���ֵ
	uint8 digits;  // �༭λ��
	uint32 (*readData)(struct EDITDATA *ed);  // ������ת��Ϊ����ֵ�Ļص�����   =0 ����Ĭ�ϴ�����
	void  (*writeData)(struct EDITDATA *ed,uint32 dat)  ; // ������ֵת��Ϊ���ݵĻص�����   =0 ����Ĭ�ϴ�����
	void   (*dispCount) (uint32 count,uchar dispDigits)  ; // ��ʾ����ֵ  =0 ����Ĭ�ϴ�����
}EDITDATA;

#define MenuMode	0
#define EditMode	1
#define ExeMode		2
#define NormalMode  3
#define ControlMode1 4
#define ControlMode2 5

extern uint8 currentMode;
extern struct SETDATAFORMAT code databyid[];


typedef struct MENUITEM
{
	uint8 type;	   // ���� =0  ���˵�  =250 �˵���  =254 ���� =252 �����Ӳ˵�  =253 �����Ӳ˵� 
	uint8 caption[6];
	uint8   (*Fun)(uchar key);  // ���������� 
	struct EDITDATA  *ed;
}MENUITEM;

#define STR__ {L_fu,L_fu,L_fu,L_END}
#define ROOTITEM 251
#define NONEITEM 254
#define NORMALITEM 250
#define SUBITEM	 252
#define PARENTITEM 253
#define ENDITEM 249


#define MenuItem(t,proc,DataFormatPoint)  {NORMALITEM,t,proc,DataFormatPoint}
#define SubItem() {SUBITEM,STR__,0,0}
#define ParentItem(t,proc,dfId)  {PARENTITEM,t,proc,dfId}
#define EndItem() {ENDITEM,STR__,0,0}

extern  uint8 keyProc(uchar key);
extern uint8 editValue(uchar key);
extern uint8 showValue(uchar key);

extern  uint8 CeShi_Led_ONflag;
extern const struct MENUITEM code  menu[];
extern uint8 currentMenu;
void menuLoop(void);
      // �ް�������ʱ 100ms ����һ��  ,����ִ�к�ʱ����

uchar  OnAdjust(uchar status);


extern uchar password(uchar key);
extern uchar adjustFun(uchar key);
extern uchar agingFun(uchar key);
extern void echoChar(uchar c,uint8 pos);		/////////////////////////////
extern uint16 currentPassword;				  /////////////
//extern void FlickerChar(uint8 ccoms);		///////////////
//extern void unFlickerChar(uint8 ccoms);		///////////////
extern void writeOptionToFlash(void); 
extern void LCDLoop(void);

extern bit canDisplayPar(void);   // =1 Ϊ���Բ���ģʽ  // =0 Ϊ�˵�������ʽ


#endif
