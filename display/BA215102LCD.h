#ifndef BA215102LCDH
#define  BA215102LCDH
#define DISPBUFLEN	48

#include "datatype.h"
#define dispPhaseA() {SetBit((DispBuf[19]),5);}
#define dispPhaseB() {SetBit((DispBuf[19]),6);}
#define dispPhaseC() {SetBit((DispBuf[20]),5);}
#define dispPhase() {dispPhaseA();dispPhaseB();dispPhaseC();}
#define dispLineAB() {dispPhaseA();SetBit((DispBuf[21]),3);}
#define dispLineBC() {dispPhaseB();SetBit((DispBuf[19]),7);}
#define dispLineCA() {dispPhaseC();SetBit((DispBuf[29]),3);}
#define dispLine() {dispLineAB();dispLineBC();dispLineCA();}
#define unDispLine() {ClrBit((DispBuf[19]),5);ClrBit((DispBuf[19]),6);ClrBit((DispBuf[20]),5);ClrBit((DispBuf[21]),3);ClrBit((DispBuf[19]),7);ClrBit((DispBuf[29]),3);}

#define dispU() {DispBuf[16]|=0x07;DispBuf[18]|=0x91;}
#define dispI() {DispBuf[16]|=0x19;DispBuf[18]|=0x0d;}
#define dispP() {DispBuf[16]|=0x4c;DispBuf[18]|=0xb8;}
#define dispH() {DispBuf[16]|=0x46;DispBuf[18]|=0xb0;}
#define dispQ() {DispBuf[16]|=0x2F;DispBuf[18]|=0x99;}
	
#define dispKA() {SetBit((DispBuf[37]),6);}
#define dispKW() {SetBit((DispBuf[37]),5);}
#define dispMW() {SetBit((DispBuf[37]),4);}
#define dispRATE() {SetBit((DispBuf[37]),2);}
#define dispMVAR() {SetBit((DispBuf[37]),1);}
#define dispKVAR() {SetBit((DispBuf[37]),0);}
#define dispA() {SetBit((DispBuf[39]),2);}
#define dispKV() {SetBit((DispBuf[39]),1);}
#define dispV() {SetBit((DispBuf[39]),0);}
#define dispSignLine1() {SetBit((DispBuf[19]),4);}
#define dispSignLine2() {SetBit((DispBuf[22]),3);}
#define dispSignLine3() {SetBit((DispBuf[20]),4);}
#define unDispSignLine1() {ClrBit((DispBuf[19]),4);}
#define unDispSignLine2() {ClrBit((DispBuf[22]),3);}
#define unDispSignLine3() {ClrBit((DispBuf[20]),4);}

#define dispES() {echoChar(L_S,8);}
#define dispEP() {echoChar(L_P,8);}
#define dispEQ() {echoChar(L_Q,8);}
#define dispF4() {echoChar(L_F,8);}
#define dispKWH() {}  /////
#define dispKVARH() {}
#define dispKVAH() {}

#define disp485COM() {SetBit((DispBuf[15]),2)};	// 显示 RS485 通讯标志
#define hide485COM() {ClrBit((DispBuf[15]),2)};	// 停止显示 RS485 通讯标志

#define dispMENU() {SetBit(DispBuf[17],7);}
#define hideMENU() {ClrBit(DispBuf[17],7);}

extern void unFlashByte(uint8 lines,uint8 pos);
extern void flashByte(uint8 lines,uint8 pos);
extern void dispTHDU(uint16);
extern void dispTHDI(uint16);

#endif
