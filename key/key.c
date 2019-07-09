#include "conf.h"
#include "key.h"
//#include "main_stm8s103.h"
extern uint8 xdata brightness;
void OnKeyUp10s(uchar key)
{
	brightness=5;
}

void  initKey()
{
	InitKeyInputMode();

}
uchar readkey(void)										   //  返回键值 并消抖
{  	
	static uchar idata last=0;    // 上次键值
	uchar ckey=0;				//当前键值
	if(isEscKey())ckey=ESCKEY;
	else if(isUpKey()) ckey=UPKEY;
	else if(isDownKey()) ckey=DOWNKEY;	 
	else if(isEnterKey())ckey=ENTERKEY;
	else;
	if(last==ckey)return(ckey);
	last=ckey;
	return(0);
}

uchar keyLoop(void)								 //50ms 调用一次
{
	static uchar idata currentkey=0,lastkey=0;
	static uchar idata KeyPressTimes=0;
	static uchar idata NoKeyPressTimes=0;

	currentkey=readkey();
	
	if((lastkey==0) && (currentkey==0))
	{
		if(NoKeyPressTimes<250)NoKeyPressTimes++;
		if(NoKeyPressTimes==200)OnKeyUp10s(currentkey);
		return(currentkey);         //  无键按下
	}
	else if((lastkey==0) && (currentkey!=0))
	{
		KeyPressTimes=0;
		OnKeyDown(currentkey);       // 有键按下

	}
	else if((lastkey!=0) && (currentkey==0)) 
	{
		if(KeyPressTimes<10);
		OnKeyPress(lastkey);				//  击键事件
		OnKeyUp(lastkey);						//  按键弹起
		NoKeyPressTimes=0;
	}
	if((lastkey!=0) && (currentkey==lastkey))
	{
		if(KeyPressTimes<250)	KeyPressTimes++;

		if((KeyPressTimes%5)==0)	OnKeyPress(currentkey);	  // 0.5秒一次
		if(KeyPressTimes==30)OnKeyDown3s(currentkey);
		if(KeyPressTimes==50)OnKeyDown5s(currentkey);
		if(KeyPressTimes==200)OnKeyDown10s(currentkey);
	}
	lastkey=currentkey;
	return(currentkey);
}
