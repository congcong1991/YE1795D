#include "conf.h" 
#include "DataType.h"
#include "display.h"

void clearDisplay(){
	uint16 idata i;
	for(i=0;i<DISPBUFLEN;i++)	{DispBuf[i]=0x00;}
		for(i=0;i<DISPBUFLEN;i++)	{DispFlashBuf[i]=0x00;}

}
void fillDisplay(){
	uint16  idata i;
	for(i=0;i<DISPBUFLEN;i++)	{DispBuf[i]=0xff;}
}


