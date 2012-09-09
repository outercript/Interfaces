/////////////////////////////////////////////////////////////////////////////////////////
//
// Serial Communication Interface (SCI) MC9S12XEP100
//
// --------------------------------------------------------------------------------------
// Copyright (c) 2006 SofTec Microsystems
// http://www.softecmicro.com/
// 
/////////////////////////////////////////////////////////////////////////////////////////

#include <hidef.h>

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// Setups the default times for pooling the serial ports.
//
// RX_TIME = BUS_CLK / (16 * Serial_Baud)  - 16x oversample bit time 
// TX_TIME = BUS_CLK / Serial_Baud         - Actual bit time 
/////////////////////////////////////////////////////////////////////////////////////////
#define SCI_RX_TIME          208
#define SCI_TX_TIME         3328

#define RX_PIN         PORTA_PA4
#define TX_PIN         PORTA_PA3

#define BUFFER_SIZE           30

    
// Serial Port 0
extern Bool sciRxReady;
extern Bool sciRxOverflow;
extern char sciRxBuffer[BUFFER_SIZE];
extern unsigned int sciRxIndex;
extern unsigned int sciRxStatus;

extern Bool sciTxAvailable;
extern char sciTxBuffer[BUFFER_SIZE];
extern unsigned int sciTxIndex;
extern unsigned int sciTxStatus;

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void TimerInit(void);
void SCIOpenCommunication();
void SCI_SetupTransmit();
void SCI_SetupRecieve();

//void SCICloseCommunication(unsigned char sci_num);
//Bool SCISendBuffer(unsigned char sci_num, unsigned char buffer);
//Bool SCIGetBuffer(unsigned char sci_num, unsigned char *buffer);
//Bool SCICheckGetBuffer(unsigned char sci_num);

unsigned char ConvertCharAscii(unsigned char value);
void SendString(char buf[30]);
void SendHexValue(unsigned char hex_value);

