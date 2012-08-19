#include <hidef.h>
#include "mc9s12xep100.h"
#pragma LINK_INFO DERIVATIVE "mc9s12xep100"


// Unsecured status flash
const unsigned char flash_security  @0xFF0F = 0xFE;

// Trololo
volatile unsigned int x;

/////////////////////////////////////////////////////////////////////////////////////////
// PeriphInit
// --------------------------------------------------------------------------------------
// Initializes various registers and peripherals
/////////////////////////////////////////////////////////////////////////////////////////
void PeriphInit(void)
{
  // Configures PA[3..0] port as output  
  PORTA = 0x00;                                                    
  DDRA = 0x0F;                        

  // Configures PortB as input                                   
  DDRB = 0x00;                                    
    
  // Enables pull-ups on portB
  PUCR |= 0x02;                                   
  
  EnableInterrupts;
}


/////////////////////////////////////////////////////////////////////////////////////////
// Main
/////////////////////////////////////////////////////////////////////////////////////////
void main(void){

    PeriphInit(); 
    
    for(;;){                  // Forever
  
        if (PORTA == 0x00){ 
            for(x=0; x<5000;x++)   
            PORTA = 0x0F;
            //TIM_TFLG2 = 0x80; // Borra el flag de Interrupcion
            for(x=0; x<5000;x++)
            PORTA = 0x00;        
        }
 
    }
}
