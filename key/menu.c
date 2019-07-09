#include "conf.h"
#include "menu.h"
#include "main.h"
#include "display.h"
#include "key.h"
#include "modbus.h"
#include "eeprom.h"
#ifndef EDITVALUELINE
#define EDITVALUELINE 4
#endif
extern void Short_Error_Pulse_Count(uint32 c,uchar ); //显示短路故障脉冲个数
  //显示接地故障脉冲个数
extern void Error_Reset_Time(uint32 c,uchar);   //显示接地故障脉冲个数
extern void dispBaundRate(uint32 c,uchar); //显示波特率
extern void dispCount(uint32 absdat,uchar digits);
extern void dispParity(uint32 c,uchar ); //显示485数据格式 
void dispStandard(uint32 c,uchar);  //  显示信号制式 3P4L 3P3L
void dispModel(uint32 c,uchar);   //  显示 型号
uint16 PassWord=0x0000;
uint8 CeShi_Led_ONflag=0;
uchar clearPowerFun(uchar key);

												// 		数据指针	 				数据类型	最大值   编辑位数 	转为计数值 转为数据	  显示计数值
EDITDATA code editFREQ={(void*)&config.freq,				2,		9999,			4,					0,				0,				0};
EDITDATA code editCT={(void*)&config.ct,						2,		9999,			4,					0,				0,				0};
EDITDATA code editPT={(void*)&config.pt,						2,		9999,			4,					0,				0,				0};
EDITDATA code editUH={(void*)&config.uh,						1,		199,			3,					0,				0,				0};
EDITDATA code editUL={(void*)&config.ul,						1,		199,			3,					0,				0,				0};
EDITDATA code editIH={(void*)&config.ih,						1,		199,			3,					0,				0,				0};
EDITDATA code editIL={(void*)&config.il,						1,		199,			3,					0,				0,				0};
EDITDATA code editADDR={(void*)&config.addr,				1,		247,			3,					0,				0,				0};		
EDITDATA code editBAUD={(void*)&config.baundrate,		1,		4,				1,					0,				0,				&dispBaundRate};		
EDITDATA code editBTS={(void*)&config.parity,				1,		2,				1,					0,				0,				&dispParity};		
EDITDATA code editPASS={(void*)&PassWord,						2,		9999,			4,					0,				0,				0};
EDITDATA code editStand={(void*)&config.standard,		1,		1,				1	,					0,				0,				&dispStandard};
EDITDATA code editModel={(void*)&config.model,			1,		11,				1	,					0,				0,				&dispModel};
#ifdef THD_ENABLE
EDITDATA code editTHD={(void*)&config.thd,			1,		19,				2	,					0,				0,				0};
#endif







uint32  Dat2Count(struct EDITDATA *ed)
{
	uint32 data count;
	switch(ed->type)
	{
		case 1:count=*((uint8*)(ed->dat));break;
		case 2:count=*((uint16*)(ed->dat));break;
		case 3:count=*((uint32*)(ed->dat));count=count>>8;break;
		case 4:count=*((uint32*)(ed->dat));break;
		default: count=0;break; 
		
	}	
	if(count>ed->max)count=ed->max;
	return(count);
	
}
void Count2Dat(struct EDITDATA *ed,uint32 count)
{
	if(count>ed->max)count=ed->max;
	switch(ed->type)
	{
		case 1:*((uint8*)(ed->dat))=count;break;
		case 2:*((uint16*)(ed->dat))=count;break;
		case 3:*((uint8*)(ed->dat))=count>>16;*((uint8*)(ed->dat)+1)=count>>8;*((uint8*)(ed->dat)+2)=count;break;
		case 4:*((uint32*)(ed->dat))=count;break;
		default: break; 
	}	
}


uchar passwordFun(uchar key);  // password  处理函数
 
//struct MENU code menuRoot;


  //设置菜单中设置的变量
#define STR_SET	 {L_S,L_E,L_T,L_END} 
#define STR_CT   {L_C,L_T,L_END}
#define STR_PT   {L_P,L_T,L_END}
#define STR_RS485 {4,8,5,L_END}
#define STR_BTS {L_B,L_T,L_S,L_END}
#define STR_ADD   {L_A,L_D,L_D,L_END}	
#define STR_BAUD  {L_B,L_A,L_U,L_D,L_END}
#define STR_BOSS  {L_B,L_O,L_S,L_S,L_END}
#define STR_ADJUST1 {L_A,L_D,L_J,1,L_END}
#define STR_ADJUST2 {L_A,L_D,L_J,2,L_END}
#define STR_CLEAR {L_C,L_L,L_E,L_A,L_R,L_END}
#define STR_TEST  {L_T,L_E,L_S,L_T,L_END}
#define STR_UH		{L_U,L_H,L_END}
#define STR_UL	    {L_U,L_L,L_END}
#define STR_IH	    {L_I,L_H,L_END}
#define STR_IL		{L_I,L_L,L_END}
#define	 STR_DOUT	{L_D,L_O,L_U,L_T,L_END}
#define	 STR_DO1	{L_D,L_O,1,L_END}
#define	 STR_DO2	{L_D,L_O,1,L_END}
#define	 STR_DO3	{L_D,L_O,1,L_END}
#define	 STR_DO4	{L_D,L_O,1,L_END}
#define STR_STANDARD {L_S,L_T,L_D,L_END}
#define STR_MODEL {L_M,L_O,L_D,L_END}
#define STR_THD {L_T,L_H,L_D,L_END}
#define STR_FREQ {L_F,L_R,L_E,L_Q,L_END}

const code struct MENUITEM    menu[]=
	{	
		SubItem(),								//	 必须是SubItem;
		MenuItem(STR_SET,0,0),			  // set		 
			SubItem(),
#ifdef THD_ENABLE
			MenuItem(STR_THD,&editValue,&editTHD),	    // CT
			
#endif
			MenuItem(STR_CT,&editValue,&editCT),	    // CT
			MenuItem(STR_PT,&editValue,&editPT),		// PT
			MenuItem(STR_CLEAR,&clearPowerFun,0),
			
			MenuItem(STR_UH,&editValue,&editUH),
			MenuItem(STR_UL,&editValue,&editUL),
			MenuItem(STR_IH,&editValue,&editIH),
			MenuItem(STR_IL,&editValue,&editIL),
			EndItem(),
		MenuItem(STR_RS485,0,0),	    // CONN  
			SubItem(),
			MenuItem(STR_ADD,&editValue,&editADDR),	   // 地址
			MenuItem(STR_BAUD,&editValue,&editBAUD),   // 波特率 
			MenuItem(STR_BTS,&editValue,&editBTS),	  // 数据格式
			EndItem(),
		MenuItem(STR_DOUT,0,0),
			SubItem(),
			MenuItem(STR_DO1,0,0),
			MenuItem(STR_DO2,0,0),
			MenuItem(STR_DO3,0,0),
			MenuItem(STR_DO4,0,0),
			EndItem(),
		MenuItem(STR_BOSS,&passwordFun,&editPASS),     // boss
			SubItem(),
			MenuItem(STR_ADJUST1,&adjustFun,0),         // 220V,5A校正
			MenuItem(STR_ADJUST2,&adjustFun,(struct EDITDATA*)1), 				// 100V,2.5A校正
			MenuItem(STR_MODEL,&editValue,&editModel),
			MenuItem(STR_STANDARD,&editValue,&editStand),   // 制式  三相三线，三相四线
			MenuItem(STR_TEST,0,0),
			MenuItem(STR_FREQ,&editValue,&editFREQ),
			EndItem(),
		EndItem(),
	};
#define MENUITEMS (sizeof(menu)/sizeof(menu[0]))

uint8 currentMenu=1;   // 当前菜单的数组编号

uint8 getNextMenu()	    //  寻找下一菜单项　 =254  NONEITEM 无下一菜单
{
	uint8 i=currentMenu;
	uint8 lastsub=i;
	bit back=0;
	int8 layer=0;		 // 菜单层次
	while(1)
	{
		i++;  
		if(i>=MENUITEMS)i=0;
		if(i==currentMenu)return(lastsub);
		switch(menu[i].type)
		{
			case SUBITEM:layer++;if(layer==0)lastsub=i+1;break;
			case ENDITEM:	layer--;if(layer<0)back=1;break;
			case NORMALITEM:if((layer==0)&&(back==0))return(i);break;     
			default: return(NONEITEM);
		}	
		
	}	
}
uint8 getLastMenu()//   寻找上一菜单项　=254  NONEITEM 无下一菜单
{
	int8 i=currentMenu; 
	uint8 lastend=i; 
	uint8 back=0;
	int8 layer=0;		 // 菜单层次
	while(1)
	{
		i--;	 
		if(i<0)i=MENUITEMS-1;
		if(i==currentMenu)return(lastend);
		switch(menu[i].type)		{
			case SUBITEM: layer--; if(layer<0)back=1;if((layer==0)&&(back==2)){lastend=i-1;back=3;}break;
			case ENDITEM:							layer++; 
							if(layer==0)						    // 找到　对应的 endline
							{
								if(menu[i-1].type==NORMALITEM)		  // 找到　对应的 menuitem 
								{
									lastend=i-1;
								}
								else
								{
									back=2;					 // 找到　对应的 menuitem 有subitem
								}
							}break;	
			case NORMALITEM:if((layer==0)&&(back==0))return(i);break;     
			default: return(NONEITEM);
		}
	}	
}

uint8 getEnterMenu(){
	uint8 i=currentMenu;
	i++;
	if(menu[i].type!=SUBITEM)return(currentMenu);
	i++;
	if(menu[i].type!=NORMALITEM)return(currentMenu);
	return(i);
}


uint8 getParentMenu()
{
	uint8 i=currentMenu;
	int8 j;			
	int8 layer=0;
	for(j=i-1;j>=0;j--)
	{
		switch(menu[j].type)
		{
			case SUBITEM: 	layer--; 
							if(layer==-1)
							{
								if(j==0)return(ROOTITEM);
								if(menu[j-1].type==NORMALITEM)return(j-1);
							}
		  					break;
			case ENDITEM:  	layer++; 
							break;	
			case NORMALITEM:break;     
			default: return(NONEITEM);
		}
	}
	return(currentMenu);
}


uchar currentMode=NormalMode ;  //	
//uchar currentMode=MenuMode ;  //	
								   
uchar currentKey=0;

			  
uchar code  space6[]={L_,L_,L_,L_,L_,L_END};

// 菜单显示在哪一行
#ifndef MENULINE 
#define MENULINE 4
#endif

#ifndef BA215102
void dispStr(uint8 lines,const unsigned char* s){			 // 菜单显示在第二排
	uchar idata c;uint8 idata i;
	clearLine( MENULINE );
	for(i=0;i<4;i++)	{
		c=s[i];	
		if(c!=L_END)		{
			echoChar(c,3-i);
		}		else		{
			return;
		}
	}
}
#endif


void dispMenu(uint8 cm)
{
	if(cm==0){hideMENU();dispStr(MENULINE,space6);return;}
	dispMENU();		// 显示Menu 标记
	dispStr(MENULINE,space6);
	dispStr(MENULINE,menu[cm].caption);
//	if(menu[cm].dfId!=0 )dispValue(cm);
}






uchar passwordFun(uchar key)
{
	uchar re;
   	//return(0);
	re=editValue(key);

	if(re==0 && key==ENTERKEY)
	{
		if(PassWord==9999)
		{
			PassWord=0;  // 清除密码
			return(0);
		}
		else return(2);
	}
	return(re);
}
extern void clearPower(void);

uchar clearPowerFun(uchar key)
{	
	if(key==ENTERKEY )
	{
			clearPower();
			
	}
	return(0);							 // 
}
uchar adjustFun(uchar key)
{	
	static uchar idata status=0;
	uchar re;
	if(key!=ENTERKEY && currentMode==MenuMode)
	{										
		return(0);		// 菜单模式， 
	}
	if(key==ENTERKEY && currentMode==MenuMode)
	{
		
		status=0;
		currentMode=ExeMode;
		clearLine(MENULINE);   // 清除菜单行
		echoChar(0,0);		echoChar(0,1);		echoChar(0,2);	  // 显示000

		return(3);							 // EnterKey  开始执行程序
	}

	if(key==ESCKEY && currentMode==ExeMode)  
	{
		currentMode=MenuMode;
		dispMenu(currentMenu);
		status=0;							   
		return(0);							// ESC key 中止程序执行. 
	}
	
	re=OnAdjust(status);					// status 表示调用的次数 
	status++;
	if(re==0)
	{
		currentMode=MenuMode;	// 结束程序
		dispMenu(currentMenu);    
		status=0;
	}
	return(re);

}

uchar agingFun( uchar key)
{	
	static uchar idata status=0;	
	if(key!=ENTERKEY && currentMode==MenuMode)
	{
		return(0);							 // EnterKey 执行程序
	}

	if(key==ESCKEY && currentMode==ExeMode)
	{
		status=0;							   
		return(0);							// ESC key 中止程序执行. 
	}
	
	switch(status)
	{
		case 0:
			currentMode=ExeMode;
			status++;
			break;
		case 1:
			status++;
			break;
		case  200:
			status++;
			currentMode=MenuMode;
			status=0;
			return(0);	
		default:
			status++;
			break;
	}	
	return(0);

}

void dispBaundRate(uint32 c,uchar digits) //显示波特率
{
	c=c%5;
	echoChar(0,0);echoChar(0,1);
	switch(c)
	{
		case 0:	echoChar(L_,4);echoChar(L_,3),echoChar(6,2);  break;    // 1200;
		case 1:	echoChar(L_,4);echoChar(1,3),echoChar(2,2);  break;
		case 2:	echoChar(L_,4);echoChar(2,3),echoChar(4,2);  break;
		case 3:	echoChar(L_,4);echoChar(4,3),echoChar(8,2);  break;	   //9600
		case 4:	echoChar(L_,4);echoChar(9,3),echoChar(6,2);  break;
		default:break;	
	}	
}
void dispParity(uint32 c,uchar digits)		// =0 : n,8,1   =1: o,8,1  =2: e,8,1	 数据格式
{
	c=c%3;
	echoChar(L_ ,3);echoChar(1,0),echoChar(8,2);
	switch(c)
	{
		case 0: echoChar(L_N,1);break;
		case 1:	echoChar(L_O,1);break;
		case 2: echoChar(L_E,1);break;
		default:break;
	}
}

void dispStandard(uint32 c,uchar digits)  // =0  =1 
{
		c=c%2;
		 echoChar(3,3);echoChar(L_P,2); echoChar(L_L,0);
		switch(c)
		{
				case 0:echoChar(4,1); break; // 3P4L
				case 1:echoChar(3,1); break;  // 3P3L
				default:break;
		}
}
void dispModel(uint32 c,uchar digits)  // =0  =1 
{
		c=c%12;
		echoChar(4,3);echoChar(L_,2); echoChar(L_,1);echoChar(L_,0);
		switch(c)
		{
				case BAM4E33:echoChar(4,3);echoChar(L_E,2);echoChar(3,1);echoChar(3,0); break; 
				case BAM4E31:echoChar(4,3);echoChar(L_E,2);echoChar(3,1);echoChar(1,0); break;
				case BAM4U33:echoChar(4,3);echoChar(L_U,2);echoChar(3,1);echoChar(3,0); break;
				case BAM4U31:echoChar(4,3);echoChar(L_U,2);echoChar(3,1);echoChar(1,0); break;
				case BAM4I33:echoChar(4,3);echoChar(L_I,2);echoChar(3,1);echoChar(3,0); break;
				case BAM4I31:echoChar(4,3);echoChar(L_I,2);echoChar(3,1);echoChar(1,0); break;
				case BAM4H33:echoChar(4,3);echoChar(L_H,2);echoChar(3,1);echoChar(3,0); break;
				case BAM4H31:echoChar(4,3);echoChar(L_H,2);echoChar(3,1);echoChar(1,0); break;
				case BAM4P33:echoChar(4,3);echoChar(L_P,2);echoChar(3,1);echoChar(3,0); break;
				case BAM4P31:echoChar(4,3);echoChar(L_P,2);echoChar(3,1);echoChar(1,0); break;
				case BAM4Q33:echoChar(4,3);echoChar(L_Q,2);echoChar(3,1);echoChar(3,0); break;
				case BAM4Q31:echoChar(4,3);echoChar(L_Q,2);echoChar(3,1);echoChar(1,0); break;
			
				default:break;
		}
}


void dispCount(uint32 absdat,uchar digits)
{
	clearLine(EDITVALUELINE);
	echoChar(absdat%10,0);
	if(digits==1){echoChar(L_,1);echoChar(L_,2);echoChar(L_,3);echoChar(L_,4);return;}
	echoChar((absdat%100)/10,1);
	if(digits==2){echoChar(L_,2);echoChar(L_,3);echoChar(L_,4);return;}
	echoChar((absdat%1000)/100,2);
	if(digits==3){echoChar(L_,3);echoChar(L_,4);return;}
	echoChar((absdat%10000)/1000,3);
	if(digits==4){echoChar(L_,4);return;}
	echoChar((absdat%100000)/10000,4);
	
}
struct EDITDATA *ped=0;
uchar getDigits(uint32 m)
{
		if(m<10)return(1);
		if(m<100)return(2);
		if(m<1000)return(3);
		if(m<10000)return(4);
		return(5);
}
extern void setParity(uint8 parity);
extern void setBaudrate(uint8 baudrate);
uchar editValue(uchar key)   // =0 返回当前菜单, =1返回root(auto) 菜单, =2 任务尚未完成
									// 最大显示格式  -xx xxx
{
 //  	static uchar xdata bytes[6];
	static uchar idata dispDigits;     // 显示位数	 最大为5		 0x.x   3     0x.xx 4 
	static uchar idata  editDigits;
	static uint32 idata  absdat;
	static uint16 idata  delta_table[]={1,10,100,1000,10000};
	static uchar idata  ccoms=0;  //  当前编辑位   
	uint16 delta;

	ped=menu[currentMenu].ed;
	editDigits=ped->digits;          // 可编辑位数
	dispDigits=getDigits(ped->max);  //  显示位数
	if(currentMode==MenuMode)
	{	
		if(key!=ENTERKEY)return(0);						  /////////////////////// 如果不是 enter 直接返回
		currentMode=EditMode;
	  if(ped->readData!=0){absdat=ped->readData(ped);}	  // 将数据转换为计数值
		else{		absdat=Dat2Count(ped); }

		if(ped->dispCount!=0)		{			ped->dispCount(absdat,dispDigits);		} 
		else		{ dispCount(absdat,dispDigits);		}
		
		ccoms=editDigits-1;
		flashByte(EDITVALUELINE  ,ccoms);
		return(3);
	}

	delta=delta_table[ccoms];
 	switch(key)
	{
		
		case ESCKEY	:
			echoChar(L_,0);	
			echoChar(L_,1);
			echoChar(L_,2);
			unFlashByte(EDITVALUELINE,0);unFlashByte(EDITVALUELINE,1);unFlashByte(EDITVALUELINE,2);unFlashByte(EDITVALUELINE,3);
			currentMode=MenuMode;
			return(1);

		case UPKEY:
			
			if(editDigits==1)
			{
					if(absdat>ped->max)absdat=0;
					else absdat+=delta;
			}
			else 
			{
					if(((absdat%(delta*10) )/delta)==9)absdat-=delta*9;
					else	absdat+=delta;   /////////////////
			}
			if(ped->dispCount!=0){if(absdat>ped->max)absdat=0; ped->dispCount(absdat,dispDigits);}   // 显示转换
			else { dispCount(absdat,dispDigits);}
			
			return(3);

		case DOWNKEY:
			if(editDigits==1)
			{
					if(absdat==0)absdat=ped->max;
					else absdat-=delta;
			}
			else 
			{
					if(((absdat%(delta*10) )/delta)==0)absdat+=delta*9;
					else absdat-=delta;
			}
			if(ped->dispCount!=0){if(absdat>ped->max)absdat=ped->max; ped->dispCount(absdat,dispDigits);}   // 显示转换
			else { dispCount(absdat,dispDigits);}

			return(3);

		case ENTERKEY:
			unFlashByte(EDITVALUELINE,ccoms);

			if(ccoms==0)
			{
//				if(absdat>ped->max)absdat=ped->max;
				if(absdat>ped->max)absdat=absdat%(ped->max+1);
				if(ped->dispCount!=0){ped->dispCount(absdat,dispDigits);} 
				else { dispCount(absdat,dispDigits);}

				if(ped->writeData!=0){ped->writeData(ped,absdat);}	  // 将计数值转换为数据
				else {Count2Dat(ped,absdat);}

 			saveConfig();            // 写入 flash
	#ifndef DISABLE_MODBUS	
  #if defined(STM32F030) || defined(STM32F051) || defined(STM8S103) || defined(STM8S105)
 				CloseAllInt(); ///////////////////////
 				setCom(config.baundrate,config.parity);
 				OpenAllInt();   /////////////
  #else
				CloseAllInt(); ///////////////////////
 //				init485();   // 重新初始化串口
				setBaudrate(config.baundrate);
				setParity(config.parity);
				OpenAllInt();   /////////////
	#endif
	#endif
				currentMode=MenuMode;
						   //  确定返回
				return(0);
			}			
			ccoms--;
			flashByte(EDITVALUELINE,ccoms);
			break;
		default:
			break;
	}		
	return(3);			    // 编程模式,尚未结束
}





void menuLoop()      // 无按键操作时 100ms 调用一次  ,用于执行耗时操作
{
	if(currentMode!=ExeMode)return;
	if(menu[currentMenu].Fun==0)return;
	menu[currentMenu].Fun(currentKey);

}
extern void OnNormalModeKey(uint8_t key);
void OnKeyDown(uchar key)
{
			
	static uint8 idata nextMenu=0;			
//	if(currentMenu==&menuAuto && key==ENTERKEY)return;  // 自动状态 不响应Enter按键.
	brightness=10;    //   显示屏亮度调节 
  if(currentMode==NormalMode)        
	{
		OnNormalModeKey(key);
	}
	else if(currentMode==MenuMode)
	{ 						//上次操作完成,菜单响应按键
		currentKey=key;
		switch(key)
		{
			case ESCKEY	:	nextMenu=getParentMenu();break;
			case UPKEY	:	nextMenu=getLastMenu();break;	   
			case DOWNKEY:	nextMenu=getNextMenu();break;
			case ENTERKEY:	nextMenu=getEnterMenu();break;
			default		:	break;
		}

		if(nextMenu==ROOTITEM)
		{
			currentMenu=0;
			currentMode=NormalMode;
			dispMenu(0);
			parameterLoop();  // 更新显示
			return;
		}

		if(menu[currentMenu].Fun !=0)				   // 设置了执行程序
		{
			switch(menu[currentMenu].Fun(key))
			{
				case 0:
					currentMenu=nextMenu;
					dispMenu(currentMenu);

						 // =0 返回当前菜单, =1退出菜单, =3 任务尚未完成
					break;
				case 1:     // 返回正常 模式
						currentMenu=0;
						currentMode=NormalMode;
						dispMenu(0);
						parameterLoop();  // 更新显示
					break;
				case 2:	   // 异常返回,不能进入子菜单
	//				if(key!=ENTERKEY)currentMenu=nm;	 
					dispMenu(currentMenu);
					break;
				case 3:  
					break; // 未结束
					
				default:
					break;	
			}   
		}
		else
		{
				if(nextMenu==NONEITEM) return;
				currentMenu=nextMenu;
				dispMenu(currentMenu);
		}

		
	}
	else if(currentMode==EditMode)
	{
		switch(menu[currentMenu].Fun(key))
		{
			case 0:	
						if(currentMode==MenuMode)           // 退出编辑 模式
						{
								currentMenu=nextMenu;
						}
						dispMenu(currentMenu);
						break;
			case 1:
						dispMenu(currentMenu);
						break;
						
			default:break;
		};		
		
	}
	else if(currentMode==ExeMode)
	{
		menu[currentMenu].Fun(key);			
	}
}


void OnKeyPress(uchar key){}

void OnKeyUp(uchar key){}

void OnKeyDown3s(uchar key)				 //  按在自动状态下 按Enter 3s 进入设置参数状态.
{
	if(key!=ENTERKEY)return;
	if(currentMode==MenuMode)
	{
		currentMode=NormalMode;
	}
	else if(currentMode==NormalMode)
	{
		currentMode=MenuMode;
		currentMenu=1;
		clearDisplay();
		dispMenu(currentMenu);
	}
	else
	{
	}

}
void OnKeyDown5s(uchar key){}
void OnKeyDown10s(uchar key){}

bit canDisplayPar()   // =1 为轮显参数模式  // =0 为菜单控制显式
{
	return(currentMode==NormalMode);
}
