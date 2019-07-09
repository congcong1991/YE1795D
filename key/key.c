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
uchar readkey(void)										   //  ���ؼ�ֵ ������
{  	
	static uchar idata last=0;    // �ϴμ�ֵ
	uchar ckey=0;				//��ǰ��ֵ
	if(isEscKey())ckey=ESCKEY;
	else if(isUpKey()) ckey=UPKEY;
	else if(isDownKey()) ckey=DOWNKEY;	 
	else if(isEnterKey())ckey=ENTERKEY;
	else;
	if(last==ckey)return(ckey);
	last=ckey;
	return(0);
}

uchar keyLoop(void)								 //50ms ����һ��
{
	static uchar idata currentkey=0,lastkey=0;
	static uchar idata KeyPressTimes=0;
	static uchar idata NoKeyPressTimes=0;

	currentkey=readkey();
	
	if((lastkey==0) && (currentkey==0))
	{
		if(NoKeyPressTimes<250)NoKeyPressTimes++;
		if(NoKeyPressTimes==200)OnKeyUp10s(currentkey);
		return(currentkey);         //  �޼�����
	}
	else if((lastkey==0) && (currentkey!=0))
	{
		KeyPressTimes=0;
		OnKeyDown(currentkey);       // �м�����

	}
	else if((lastkey!=0) && (currentkey==0)) 
	{
		if(KeyPressTimes<10);
		OnKeyPress(lastkey);				//  �����¼�
		OnKeyUp(lastkey);						//  ��������
		NoKeyPressTimes=0;
	}
	if((lastkey!=0) && (currentkey==lastkey))
	{
		if(KeyPressTimes<250)	KeyPressTimes++;

		if((KeyPressTimes%5)==0)	OnKeyPress(currentkey);	  // 0.5��һ��
		if(KeyPressTimes==30)OnKeyDown3s(currentkey);
		if(KeyPressTimes==50)OnKeyDown5s(currentkey);
		if(KeyPressTimes==200)OnKeyDown10s(currentkey);
	}
	lastkey=currentkey;
	return(currentkey);
}
