#include <hidef.h>      /* common defines and macros */
#include <stdio.h>
#include <MC9S12XEP100.h>     /* derivative-specific definitions */

#include <string.h>

#include "sci.h"
#include "ir.h"
#include "syllabus.h"

#pragma LINK_INFO DERIVATIVE "MC9S12XEP100"

// Unsecured status flash
const unsigned char flash_security  @0xFF0F = 0xFE;

volatile unsigned int x;
volatile unsigned int count;
volatile unsigned int time;

// Serial Port 0
Bool sciRxReady;
Bool sciRxOverflow;
unsigned char sciRxBuffer[30];
unsigned int sciRxIndex;

void TimerInit(void);
void PLLInit(void);

/* Start interrupts */  
#pragma CODE_SEG __NEAR_SEG NON_BANKED

interrupt VectorNumber_Vtimch0 void Carrier_ISR(void){
        
    // Clear Interrupt Flag 
    TIM_TFLG1 |= TIM_TFLG1_C0F_MASK;

    // Setup Output Compare Time        
    TIM_TC0 = TIM_TCNT + CARRIER_TIME;

    // Toggle!!
    toggle ^= 1;

    // Output the result =)
    if(toggle){
        IR_PORT = TRUE;
    }
     
    else{
        IR_PORT = FALSE;
    } 
}

interrupt VectorNumber_Vtimch1 void IR_Controler(void){
    volatile unsigned int temp;
    temp = ControlCount;
        
    // Clear Interrupt Flag 
    TIM_TFLG1 |= TIM_TFLG1_C1F_MASK;

    // Setup Output Compare Time        
    TIM_TC1 = TIM_TCNT + CONTROLER_TIME;

    // Carrier output true
    if(ControlCount == ir_pulseCount && !ControlStatus){
        ControlCount=0;
		ControlStatus=1;
		IR_PORT = FALSE;
        TIM_TIE_C0I = FALSE;
    }
    
    // Carrier output false 
    else if(ControlCount == ir_idleCount && ControlStatus){
		ControlCount = 0;
        ControlStatus = 0;
		sendingBit = 0;
        IR_PORT = FALSE;
        TIM_TIE_C1I = FALSE;
    }
    
    else{
        ControlCount++;
    }
		
}


interrupt VectorNumber_Vsci0 void SciReception_ISR(void){
    
    if(sciRxReady) 
        return;
    
    // Clear Interrupt by reading status and data registers
    sciRxBuffer[sciRxIndex] = SCI0SR1; // Read Status Register
    sciRxBuffer[sciRxIndex] = SCI0DRL; // Read Data Register
    
    // Mark reception complete if char is 13 or \r
    if(sciRxBuffer[sciRxIndex] == 13 || sciRxBuffer[sciRxIndex] == 10){
        sciRxReady = TRUE;
        sciRxBuffer[sciRxIndex] = 0;
    }
    
    // Prevent buffer overflow by keeping within the range.
    if(sciRxIndex < 30){
        sciRxIndex += 1; 
    } 
    
    else{
        sciRxOverflow = TRUE;
    }
}

#pragma CODE_SEG DEFAULT
/* End interrupts */

void TimerInit(void){

    // setup Timer System Control Registers
    TIM_TSCR1  = 0x90; // TSCR1 - Enable normal timer
    TIM_TSCR2  = 0x00; // TSCR2 - 0x80 Timer Interrupt Enable
    TIM_TSCR2 |= 0x00; // TSCR2 - 0x00 1 Prescaler
    
    TIM_PACTL  = 0x00; // Setup Timer Preset  

}


void PeriphInit(void){
    DDRA  = 0xFF; // Configure A[5, 3..0] as outputs 
    PORTA = 0x00; // Output 0

    SCIOpenCommunication(SCI_0); 
    sciRxReady = FALSE;
    sciRxOverflow = FALSE;
    sciRxIndex = 0;
    
    // SCI - 2 = Enable recieve interrupt, C = Enable Recieve/Transmit
    SCI0CR2 = 0x2C;
}

void PLLInit(void){
    
    CLKSEL &= 0x7F;
    SYNR    = 0x07;   // use PLL and 4-MHz crystal to generate 24-MHz system clock
    REFDV   = 0;      //
    
    PLLCTL  |= 0x40;   // enable PLL, set automatic bandwidth control
    
    while(!(CRGFLG & 0x08));
    
    CLKSEL  = 0x80;   // enable PLL, keep SYSCLK running in wait mode
    
}

void main(void) {
    count = 0;
	
    PLLInit();    
    PeriphInit();
    TimerInit();
    Setup_IR();

    EnableInterrupts;
    
    for(;;) {
    
		if(sciRxReady) {
            SCICloseCommunication(SCI_0);
		    rawSend(sciRxBuffer);
            
            (void) memset(&sciRxBuffer[0], 0, sizeof(sciRxBuffer));
            sciRxIndex = 0;
            sciRxReady = FALSE;
            SCIOpenCommunication(SCI_0);
        }
	 	

        if(sciRxOverflow) {
            PORTA_PA1 = 1;
            SendString(SCI_0, "Buffer overflow!\r\n\0");
            SendString(SCI_0, "Erasing buffer!\r\n\0");
            (void) memset(&sciRxBuffer[0], 0, sizeof(sciRxBuffer));
            sciRxIndex = 0;
            sciRxOverflow = FALSE;
        }
        
        else{
            PORTA_PA1 = 0;
        }
        
    } /* loop forever */
  
} /* please make sure that you never leave main */
