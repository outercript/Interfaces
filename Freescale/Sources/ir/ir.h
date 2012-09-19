/////////////////////////////////////////////////////////////////////////////////////////
//
// IR Communication Interface (SCI) MC9S12XEP100
//
// --------------------------------------------------------------------------------------
// Author : Oscar Suro
// Last Modified : Sept 10, 2012
// 
/////////////////////////////////////////////////////////////////////////////////////////
#include <hidef.h>

#define IR_PORT PORTA_PA3
#define CARRIER_TIME 402
#define CONTROLER_TIME 1280
#define SONY_1LOW   13
#define SONY_1HIGH  29
#define SONY_SLOW   13
#define SONY_SHIGH  58
#define SONY_0LOW   13
#define SONY_0HIGH  15

extern Bool sendingBit;
extern Bool toggle;
extern Bool ControlStatus;
extern unsigned int ir_pulseCount;
extern unsigned int ir_idleCount;
extern unsigned int ControlCount;




void Setup_IR(void);
void sendSony(unsigned char buff[30]);
void rawSend(unsigned char buff[30]);
void SIRCS_Send_High(void);
void SIRCS_Send_Low(void);
void SIRCS_Send_Start(void);
void IR_sendBit(void);
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////