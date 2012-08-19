#include <hidef.h>      /* common defines and macros */
#include "mc9s12xep100.h"      /* derivative-specific definitions */

#pragma LINK_INFO DERIVATIVE "mc9s12xep100"

// Unsecured status flash
const unsigned char flash_security  @0xFF0F = 0xFE;

volatile unsigned int x;
void TimerDelay(unsigned int time);
#define  hi_time  1000
#define  lo_time  2000

/* Codigo para las interrupciones *  
#pragma CODE_SEG __NEAR_SEG NON_BANKED


interrupt void TimerOverflow_ISR(void){
    PORTA = 0x0F;
    TIM_TFLG2 = 0x80; // Borra el flag de Interrupcion
}


#pragma CODE_SEG DEFAULT
* Terminan las interrupciones *

void Timer_Init(void){
    TIM_TSCR2 = 0x87; // TCNT prescaler + Interrupciones Habilitadas
    TIM_TSCR1 = 0x80; // Timer del Sistema
    TIM_TFLG2 = 0x80; // Borra el flag de Interrupcion
}
*/

void TimerDelay (unsigned int time){ // input time in us. 
    unsigned int ix;
    unsigned int k; 
    ix = 0;

    for (ix = 0; ix < 512; ix++){
        k = 0;
        for (k = 0; k < 32; k++);
    }
        
}


void PeriphInit(void){
    DDRA  = 0x0F; // Configure A[3..0] as outputs 
    PORTA = 0x00; // Output 0
    
    // EnableInterrupts;
}

void main(void) {
    Bool manuel;
    manuel = 0;
        
    PeriphInit();
    //Timer_Init();
    
    
    for(;;) {

        if (manuel){   
            PORTA = 0x0F;
            manuel = 0;
        }
        
        else{
            PORTA = 0x00;
            manuel = 1;
        }
        
        TimerDelay(10);
      
           
    } /* loop forever */
  /* please make sure that you never leave main */
}
