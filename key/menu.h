#ifndef MENUH
#define MENUH
#include "datatype.h"

typedef struct EDITDATA
{
	void *dat;   // 待编辑数据指针
	uint8 type;  // =0   =1 uint8  =2 uint16 =3 uint 24  =4 uint16 
	uint32 max;  // 计数值 最大值
	uint8 digits;  // 编辑位数
	uint32 (*readData)(struct EDITDATA *ed);  // 将数据转换为计数值的回调函数   =0 调用默认处理函数
	void  (*writeData)(struct EDITDATA *ed,uint32 dat)  ; // 将计数值转换为数据的回调函数   =0 调用默认处理函数
	void   (*dispCount) (uint32 count,uchar dispDigits)  ; // 显示计数值  =0 调用默认处理函数
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
	uint8 type;	   // 类型 =0  根菜单  =250 菜单项  =254 结束 =252 进入子菜单  =253 结束子菜单 
	uint8 caption[6];
	uint8   (*Fun)(uchar key);  // 按键处理函数 
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
      // 无按键操作时 100ms 调用一次  ,用于执行耗时操作

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

extern bit canDisplayPar(void);   // =1 为轮显参数模式  // =0 为菜单控制显式


#endif
