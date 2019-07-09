#ifndef _MODBUS_H
#define _MODBUS_H
#include "conf.h"

#define  RX_BUFFER_SIZE 0x30
#define  TX_BUFFER_SIZE 0x40
extern uint8 	xdata	 TxdBuf[];
extern uint8   xdata    RxdBuf[];
extern uint8 	xdata    RxdTelBuf[];
#define getRxdTelCommand() (RxdTelBuf[1])
extern void init485(void);

extern void RxdByte(uint8 c);
extern bit hasByteToTxd(void);		 // =1 有字节待发
extern uint8 getTxdByte(void);
extern bit CheckEven(uint8 c);
extern void startTxd(void);   // 定义在CPU中

extern void modbusLoop(void);

extern uint16 CRC16ISR0(uint8 * puchMsg, uint8 usDataLen);
extern uint16 CRC16ISR1(uint8 * puchMsg, uint8 usDataLen);


extern const code	uint8      auchCRCHi[];
extern const code	 uint8   	  auchCRCLo[];

void MakeComUpdata(void);
void MakeComTxdFrame(void);

#endif										
