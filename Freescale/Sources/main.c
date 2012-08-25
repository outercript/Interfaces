#include <hidef.h>      /* common defines and macros */
#include <MC9S12XEP100.h>     /* derivative-specific definitions */
#pragma LINK_INFO DERIVATIVE "MC9S12XEP100"

// Unsecured status flash
const unsigned char flash_security  @0xFF0F = 0xFE;

volatile unsigned int x;
volatile Bool manuel;

void TimerInit(void);

/* Start interrupts */  
#pragma CODE_SEG __NEAR_SEG NON_BANKED
interrupt VectorNumber_Vtimch0 void TimerOverflow_ISR(void){
            TIM_TFLG1 = 0x01;
            PORTA = 0x0F;
            manuel ^= 1; 
            TIM_TC0 = TIM_TCNT + 0x0FFF;   
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
}


void main(void) {
    manuel = 0;
        
    PeriphInit();
    TimerInit();
    
    TIM_TIOS = 0x01;
    TIM_TIE  = 0x01;
    TIM_TC0  = TIM_TCNT + 50; 
    
    EnableInterrupts;
    
    for(;;) {
        
        if (manuel){   
            PORTA = 0x0F;
        }
        
        else{
            PORTA = 0x00;
        }
        
    } /* loop forever */
  
} /* please make sure that you never leave main */
