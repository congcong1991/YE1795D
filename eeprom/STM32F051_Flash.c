#include "conf.h"
#include "STM32F051_Flash.h"
#include "stm32f030.h"
#include "main.h"

void initEEPROM()
{
	loadConfig();
//	loadAdjustData();
}
void EarsePage(uint32 address)  //  
{
		FLASH_Unlock();
	  FLASH_ErasePage(address);
		FLASH_Lock();
}


void RDBYTES(uint32_t address,uint8_t num, uint8_t *Data)
{
	uint8_t count;
	for(count=0;count<num/2;count++)
	{
		*((uint16_t *)Data+count)=*((uint16_t *)address+count);
  }
}

void WRBYTES(uint32_t address,unsigned char num, unsigned char *Data)
{
	uint8_t count;
	FLASH_Unlock();
//	FLASH_ErasePage(address);
	for(count=0;count<(num/2+1);count++)
	{
		FLASH_ProgramHalfWord((address+2*count), *((uint16*)(Data+2*count)));
  }
	FLASH_Lock();
}		

#define  FlashDataAddress   0x08007f00   //往片内FLASH写数据的地址




void saveConfig()
{
	EarsePage(FlashDataAddress);
	WRBYTES(FlashDataAddress,sizeof(struct CONFIG),(uint8*)&config);
}
void loadConfig()
{
	RDBYTES(FlashDataAddress,sizeof(config.vaildsign),(unsigned char*)&config);
	if(config.vaildsign!=0x54AA)
	{	
		config.vaildsign=0x54AA;
		saveConfig();
		return;
	}
	RDBYTES(FlashDataAddress,sizeof(struct CONFIG),((unsigned char*)&config));
}




