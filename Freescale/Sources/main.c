#include <hidef.h>      /* common defines and macros */
#include <stdio.h>
#include <MC9S12XEP100.h>     /* derivative-specific definitions */
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

/* Start interrupts */  
#pragma CODE_SEG __NEAR_SEG NON_BANKED
interrupt VectorNumber_Vtimch0 void TimerOverflow_ISR(void){
        TIM_TFLG1 = 0x01;
        TIM_TC0 = TIM_TCNT + time;
        
        manuel ^= 1;
        count += 1;         
}

interrupt VectorNumber_Vsci0 void SciReception_ISR(void){
    
    if(sciRxReady) 
        return;
    
    // Clear Interrupt by reading status and data registers
    sciRxBuffer[sciRxIndex] = SCI0SR1; // Read Status Register
    sciRxBuffer[sciRxIndex] = SCI0DRL; // Read Data Register
    
    // Mark reception complete if char is 13 or \r
    if(sciRxBuffer[sciRxIndex] == 13){
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
    TIM_TSCR1  = 0x80; // TSCR1 - Enable normal timer
    TIM_TSCR2  = 0x00; // TSCR2 - 0x80 Timer Interrupt Enable
    TIM_TSCR2 |= 0x07; // TSCR2 - 0x07 128 Prescaler
    
    TIM_PACTL  = 0x00; // Setup Timer Preset  
    //TIM_TFLG2 = 0x80; // Borra el flag de Interrupcion
}


void PeriphInit(void){
    DDRA  = 0x0F; // Configure A[3..0] as outputs 
    PORTA = 0x00; // Output 0
	
    // Configures the ATD peripheral
    // 8 bit data resolution
    ATD0CTL1 = 0x10; 
    // Left justified data, 2 conversion sequence and non-FIFO mode
    ATD0CTL3 = 0x13;
    // fBUS=2MHz, fATDCLK = 1 MHz (PRESCLAER = 0) Select 24 Sample Time
    ATD0CTL4 = 0xE0;

    SCIOpenCommunication(SCI_0); 
    sciRxReady = FALSE;
    sciRxOverflow = FALSE;
    sciRxIndex = 0;
}


void main(void) {
    manuel = 0;
    count = 0;
	time = 0x0FFF;
        
    PeriphInit();
    TimerInit();
    
    //Output Compare
    TIM_TIOS = 0x01; //Enable Output compare Port 0
    TIM_TIE  = 0x01; //Enable Timer Interrupt Output Compare 0
    TIM_TC0  = TIM_TCNT + 50; // Set a dummy time to generate interrupt
    
    // SCI - 2 = Enable recieve interrupt, C = Enable Recieve/Transmit
    SCI0CR2 = 0x2C;
    PORTA = 0x00;
    
    EnableInterrupts;
    
    for(;;) {
		// Select AN00 as input channel and enable multi channel conversions
		ATD0CTL5 = 0x10;  
		while(!(ATD0STAT0 & 0x80));
		if (ATD0DR0H > 0){
		    time = 64 * ATD0DR0H;    
		}
		
        if(sciRxReady) {
            PORTA_PA0 = 1;
            SendString(SCI_0, sciRxBuffer);
            SendString(SCI_0, "\r\n\0");
            (void) memset(&sciRxBuffer[0], 0, sizeof(sciRxBuffer));
            sciRxIndex = 0;
            sciRxReady = FALSE;
        }
        
        else{
            PORTA_PA0 = 0;
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
        
        if(count > 255){      
           SendString(SCI_0, "Togle 255 veces =D \r\n\0");
           count = 0;
        }
        
    } /* loop forever */
  
} /* please make sure that you never leave main */
