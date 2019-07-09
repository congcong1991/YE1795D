#ifndef _MAIN_H_
#define _MAIN_H_

#include "conf.h"
#include "datatype.h"

extern void  Delay(uint16 count);


#define BEGINLOOPS 2000
#define ENDLOOPS	10000

//#define DEBUG

struct dds_s{
	int32_t phase; // ????  /* ?? 1L<<16??1.0 */
	int32_t u0; // ????  mV
	int32_t u1; // ????  mV
	int32_t udc; //?????? mV
	int32_t cc;  // ???? pF
	int64_t e0; // ???? 10-18 J
	int64_t e1; // ?????? 10-18J 
	int64_t er;  // ?????? 10-18J
	int64_t ei;  // ??????? 10-18J
	int64_t de; // ????
	int32_t tns1; // ?????? nS
	int32_t tns2; // ?????? ns
};
extern struct dds_s dds_p,dds_n;

#define PI2			411775	/* (2L<<16)*3.1415926 */
#define PI1			205887	/* (1L<<16)*3.1415926 */

#ifndef __packed 
#define __packed
#endif
typedef __packed struct CONFIG	{			 // 配置信息
	uint16_t vaildsign;
	uint8_t baundrate;    /* =0:600    =1:1200 		=2:2400 		=3:4800 		=4:9600 */
	uint8_t addr; 
	uint8_t parity;		// =0 : n,8,1   =1: o,8,1  =2: e,8,1	 数据格式

	uint16_t temp_start; //启动温度 0.1摄氏度
	uint16_t temp_stop;	//停止温度 0.1摄氏度
	uint16_t freq; // 启动频率 0.01Hz
	uint16_t max_hv; // 设置电压 1V
	uint16_t i_gate; //电流阈值 
	uint16_t config; // 配置  D0:定频工作  D1: 直接跟踪
	uint16_t agc_level;  // 自动幅度电平
	uint16_t up_sub,un_sub;   // 电压采样修正  1L<<8 =1.0
	uint16_t ip_sub,in_sub;		// 电流修正  1L<<8 =1.0 
	uint16_t outp_k,outn_k;   // 输出修正 1L<<8 =1.0
	uint16_t udc_sub;  // 电源电压修正 1L<<8 1.0
	uint16_t max_q;  // 最大无功

}CONFIG;

extern  struct CONFIG config;
/* config */



//自动频率跟踪
#define isAFC() ((config.config&0x0004)!=0)
// 跟踪方式 ，阻抗最低, 最小值 
#define isMin() ((config.config&0x0010)!=0)
// 自动搜索 ，
#define isSearch() ((config.config&0x0001)!=0)
// 自动幅度控制
#define isAGC() ((config.config&0x0002)!=0)

// 自动温度控制模式
#define isConfigAutoTemp() ((config.config&0x0100)!=0)
// 18B20 控制频率
#define is18B20AFC() ((config.config&0x008)!=0)


/* 电源控制采用 比较形式 */
#define isCompRelay() ((config.config&0x0200)!=0)
#define setCompRelay() { config.config |= 0x0200; }
#define clrCompRelay() { config.config &= ~0x0200; }

typedef struct PARAMETER				 // 所有参数
{
	int32_t up,un;   // 两端电压  mV
	int32_t ip,in;		// 两端电流　uA
	uint32_t ubase;   // 偏置电压  mV; 
	float i;   // 电流
	float fi;  // 相角
	float f;  // 频率
	uint8_t locked;
	uint16_t currDC;   //直流电压
	uint16_t ext_temp; // 外部温度 0.1 摄氏度  0-150
	uint16_t curr_freq;  // 当前频率
	uint32_t kk; // 系统校正系数
	uint32_t t0; // 设置频率对应计数值 200倍频率
	uint32_t v_t; // 电压设置脉冲宽度
	
	uint16_t currI;			// 当前电流采样值
//	uint16_t currExtTemp;	// 当前外部温度采样值
	int16_t currInterTemp; // 当前内部温度采样值
	uint8_t sampled; // 采样值有效标志
	uint16_t lastI;  // 上一次电流采样值
	uint16_t maxValue;  //跟踪极限值 
	uint16_t currV;  // 当前电源电压 
	uint16_t currZ; // 阻抗 0.01K
	int32_t 	currP;  // 
	int32_t currQ;  // 无功功率
	int32_t 	currPF;
	int32_t currS; // 总功率 
	
	uint8_t model;
	uint32_t disc_l_k;  // 放电电感修正系数 
	int32_t cc; 			// 输出电容
	uint32_t hv_set;  // 设置电压  mV ;
	
	uint32_t LLL;
	
}PARAMETER;

extern uint16_t CurrentStatus;
#define isAutoTempMode() ((CurrentStatus&0x02)!=0)
#define setAutoTempMode() {CurrentStatus |= 0x02;}
#define clrAutoTempMode() {CurrentStatus &= ~0x02;}

#define isEnable() ((CurrentStatus&0x01)!=0)
#define setEnable() {CurrentStatus |= 0x01;}
#define clrEnable() {CurrentStatus &= ~0x01;}

#define isTempOver() ((CurrentStatus&0x04)!=0)
#define setTempOver() {CurrentStatus |= 0x04;}
#define clrTempOver() {CurrentStatus &= ~0x04;}

#define isHaltMode() ((CurrentStatus&0x08)!=0)
#define setHaltMode() {CurrentStatus |= 0x08;}
#define clrHaltMode() {CurrentStatus &= ~0x08;}

#define isDS18B20fine() ((CurrentStatus&0x10)!=0)
#define DS18B20fine() {CurrentStatus |= 0x10;}
#define DS18B20error() {CurrentStatus &= ~0x10;}

extern  struct PARAMETER  parameter;
extern void parameterLoop(void);
#define isYE1795D3_2() (parameter.model==0x03)
#define isYE1795D3() (parameter.model==0x02)
#define isYE1795D1() (parameter.model==0x00)
#define isYE1795D6() (parameter.model==0x01)
#endif

