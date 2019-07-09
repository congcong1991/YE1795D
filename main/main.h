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
typedef __packed struct CONFIG	{			 // ������Ϣ
	uint16_t vaildsign;
	uint8_t baundrate;    /* =0:600    =1:1200 		=2:2400 		=3:4800 		=4:9600 */
	uint8_t addr; 
	uint8_t parity;		// =0 : n,8,1   =1: o,8,1  =2: e,8,1	 ���ݸ�ʽ

	uint16_t temp_start; //�����¶� 0.1���϶�
	uint16_t temp_stop;	//ֹͣ�¶� 0.1���϶�
	uint16_t freq; // ����Ƶ�� 0.01Hz
	uint16_t max_hv; // ���õ�ѹ 1V
	uint16_t i_gate; //������ֵ 
	uint16_t config; // ����  D0:��Ƶ����  D1: ֱ�Ӹ���
	uint16_t agc_level;  // �Զ����ȵ�ƽ
	uint16_t up_sub,un_sub;   // ��ѹ��������  1L<<8 =1.0
	uint16_t ip_sub,in_sub;		// ��������  1L<<8 =1.0 
	uint16_t outp_k,outn_k;   // ������� 1L<<8 =1.0
	uint16_t udc_sub;  // ��Դ��ѹ���� 1L<<8 1.0
	uint16_t max_q;  // ����޹�

}CONFIG;

extern  struct CONFIG config;
/* config */



//�Զ�Ƶ�ʸ���
#define isAFC() ((config.config&0x0004)!=0)
// ���ٷ�ʽ ���迹���, ��Сֵ 
#define isMin() ((config.config&0x0010)!=0)
// �Զ����� ��
#define isSearch() ((config.config&0x0001)!=0)
// �Զ����ȿ���
#define isAGC() ((config.config&0x0002)!=0)

// �Զ��¶ȿ���ģʽ
#define isConfigAutoTemp() ((config.config&0x0100)!=0)
// 18B20 ����Ƶ��
#define is18B20AFC() ((config.config&0x008)!=0)


/* ��Դ���Ʋ��� �Ƚ���ʽ */
#define isCompRelay() ((config.config&0x0200)!=0)
#define setCompRelay() { config.config |= 0x0200; }
#define clrCompRelay() { config.config &= ~0x0200; }

typedef struct PARAMETER				 // ���в���
{
	int32_t up,un;   // ���˵�ѹ  mV
	int32_t ip,in;		// ���˵�����uA
	uint32_t ubase;   // ƫ�õ�ѹ  mV; 
	float i;   // ����
	float fi;  // ���
	float f;  // Ƶ��
	uint8_t locked;
	uint16_t currDC;   //ֱ����ѹ
	uint16_t ext_temp; // �ⲿ�¶� 0.1 ���϶�  0-150
	uint16_t curr_freq;  // ��ǰƵ��
	uint32_t kk; // ϵͳУ��ϵ��
	uint32_t t0; // ����Ƶ�ʶ�Ӧ����ֵ 200��Ƶ��
	uint32_t v_t; // ��ѹ����������
	
	uint16_t currI;			// ��ǰ��������ֵ
//	uint16_t currExtTemp;	// ��ǰ�ⲿ�¶Ȳ���ֵ
	int16_t currInterTemp; // ��ǰ�ڲ��¶Ȳ���ֵ
	uint8_t sampled; // ����ֵ��Ч��־
	uint16_t lastI;  // ��һ�ε�������ֵ
	uint16_t maxValue;  //���ټ���ֵ 
	uint16_t currV;  // ��ǰ��Դ��ѹ 
	uint16_t currZ; // �迹 0.01K
	int32_t 	currP;  // 
	int32_t currQ;  // �޹�����
	int32_t 	currPF;
	int32_t currS; // �ܹ��� 
	
	uint8_t model;
	uint32_t disc_l_k;  // �ŵ�������ϵ�� 
	int32_t cc; 			// �������
	uint32_t hv_set;  // ���õ�ѹ  mV ;
	
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

