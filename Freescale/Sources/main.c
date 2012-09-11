#include <hidef.h>      /* common defines and macros */
#include <stdio.h>
#include <MC9S12XEP100.h>     /* derivative-specific definitions */

#include <string.h>

#include "sci.h"

#pragma LINK_INFO DERIVATIVE "MC9S12XEP100"

// Unsecured status flash
const unsigned char flash_security  @0xFF0F = 0xFE;

volatile unsigned int x;
volatile unsigned int count;
volatile unsigned int time;

volatile Bool manuel;

// Serial Port 0
Bool sciRxReady;
Bool sciRxOverflow;
char sciRxBuffer[30];
unsigned int sciRxIndex;

void TimerInit(void);
void PLLInit(void);

/* Start interrupts */  
#pragma CODE_SEG __NEAR_SEG NON_BANKED
interrupt VectorNumber_Vtimch0 void TimerOverflow_ISR(void){
        TIM_TFLG1 = 0x01;
        
        TIM_TC0 = TIM_TCNT + time;
        
        manuel ^= 1;         
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
    //TIM_TFLG2 = 0x80; // Borra el flag de Interrupcion
}


void PeriphInit(void){
    DDRA  = 0xFF; // Configure A[5, 3..0] as outputs 
    PORTA = 0x00; // Output 0

    SCIOpenCommunication(SCI_0); 
    sciRxReady = FALSE;
    sciRxOverflow = FALSE;
    sciRxIndex = 0;
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
    manuel = 0;
    count = 0;
	time = 402;
	
    PLLInit();    
    PeriphInit();
    TimerInit();
    
    //Output Compare
    TIM_TIOS_IOS0 = TRUE;   // Enable Output compare Port 0
    
    // Configure time for Output Compare 0
    TIM_TC0  = TIM_TCNT + 10;
    
    //Enable Timer Interrupt Output Compare 0 
    TIM_TIE_C0I  = TRUE; 
     
    // SCI - 2 = Enable recieve interrupt, C = Enable Recieve/Transmit
    SCI0CR2 = 0x2C;
    PORTA = 0x00;

    EnableInterrupts;
    
    for(;;) {
    
		if(sciRxReady) {
            count = strcmp(sciRxBuffer, "Sudo!!");
		
            if(!count){
                PORTA_PA0 = 1;
                TIM_TIE_C0I  = FALSE;
            }
            
            else{
                PORTA_PA0 = 0;
                TIM_TIE_C0I  = TRUE;
            }
            
            (void) memset(&sciRxBuffer[0], 0, sizeof(sciRxBuffer));
            sciRxIndex = 0;
            sciRxReady = FALSE;
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

        if (manuel){
            PORTA_PA3 = 1;
        }
        
        else{
            PORTA_PA3 = 0;
        }
        
    } /* loop forever */
  
} /* please make sure that you never leave main */
