 #include "conf.h"
#include "main.h"
#include "Datatype.h"

struct CONFIG  config={

	0xAA55,//	uint16_t vaildsign;
	4, //uint8_t baundrate;    /* =0:600    =1:1200 		=2:2400 		=3:4800 		=4:9600 */
	1, // uint8_t addr; 
	0,//uint8_t parity;		// =0 : n,8,1   =1: o,8,1  =2: e,8,1	 数据格式

	450, //uint16_t temp_start; //启动温度 0.1摄氏度
	300, //uint16_t temp_stop;	//停止温度 0.1摄氏度
	6100, //uint16_t freq; // 启动频率 0.01Hz
	120,	// hv_set; // 设置电压 1V
	2000, //uint16_t i_gate; //电流阈值 
	6, //uint16_t config; // 配置  D0:定频工作  D1: 直接跟踪
	40,  //	int32_t agc_level;
	285,285,//uint16_t up_sub,un_sub;   // 电压采样修正  1L<<8 =1.0
	1L<<8,1L<<8, //uint16_t ip_sub,in_sub;		// 电流修正  1L<<8 =1.0 
	1L<<8,1L<<8,  //uint16_t outp_k,outn_k;   // 输出修正 1L<<8 =1.0
	256,   // uint16_t udc_sub;
	3000, // 最大无功 
};

uint16_t CurrentStatus=0;

void InitRS485Port(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);


}

void initSelfTestPort(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	//  init 自检指示引脚
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_1;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void InitKeyInputMode(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1| GPIO_Pin_2;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_1;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_1;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOC, &GPIO_InitStruct);
	
}






