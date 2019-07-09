#include "conf.h"
#ifdef HT1623
#include "HT1623.h"
#include "display.h"
//#include <intrins.h>
	    // displa
extern void Delay(uint16 count);



void SendBit_1623(uint8 tmp_data,uint8 tmp_cnt)		//data�ĸ�cntλд��HT1623����λ��ǰ
{
	uint8 i;
	for(i =0; i <tmp_cnt; i ++)
	{
 		if((tmp_data&0x80)==0){ lcd_data_clr();}
		else { lcd_data_set();}
		lcd_wr_clr();
		lcd_wr_set();//Delay(2);
		tmp_data<<=1;
	}
}

void SendDataBit_1623(uint8 tmp_data,uint8 tmp_cnt)	//data�ĵ�cntλд��HT1623����λ��ǰ
{
	uint8 i;
	for(i =0; i <tmp_cnt; i ++)
	{
		if((tmp_data&0x01)==0){ lcd_data_clr();}
		else {lcd_data_set(); }
		lcd_wr_clr();//Delay(2);
		lcd_wr_set();//Delay(2);
		tmp_data>>=1;
	}
}

void SendCmd(uint8 lcd_command)
{

	lcd_cs_clr();//Delay(2);
	SendBit_1623(0x80,4);			//д���־��"100"��9λcommand�������
	SendBit_1623(lcd_command,8); 		//û��ʹ�е�����ʱ����������Ϊ�˱�̷���
	lcd_cs_set();						//ֱ�ӽ�command�����λд"0"
}

void WriteAll_1623(uint8 tmp_addr,uint8 *tmp_p,uint8 tmp_cnt)
{
	uint8  i;
	lcd_cs_clr();//Delay(2);
//	_nop_();
//	_nop_();
	SendBit_1623(0xa0,3);			//д���־��"101"
	SendBit_1623(tmp_addr,6);			//д��addr�ĸ�6λ
	for(i =0; i <tmp_cnt; i++,tmp_p++)		//����д������
	{
		SendDataBit_1623(*tmp_p,4);
		
	}
	lcd_cs_set();
}



void	initDisplay()
{
	uint8 i;	
	INITDISPPORT();
	lcd_cs_set();			   
	lcd_wr_set();
	lcd_data_set();

	SendCmd(LCDOFF);
	SendCmd(SYSEN);

	SendCmd(RC256);
	SendCmd(BIAS);
	SendCmd(LCDON);
	for(i=0;i<DISPBUFLEN;i++)
	{
		DispBuf[i]=0x00;
		DispFlashBuf[i]=0x00;
	}
}



#endif

