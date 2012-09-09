#include <hidef.h>      /* common defines and macros */
#include <stdio.h>
#include <MC9S12XEP100.h>     /* derivative-specific definitions */

#include "sci.h"

#pragma LINK_INFO DERIVATIVE "MC9S12XEP100"

// Unsecured status flash
const unsigned char flash_security  @0xFF0F = 0xFE;

volatile unsigned int x;
volatile unsigned int count;


void PLLInit(void);
void PeriphInit(void);


void PeriphInit(void){
    DDRA  = 0xFF; // Configure Port A as output 
    PORTA = 0x00; // Output 0
}

void PLLInit(void){    
    CLKSEL &= 0x7F;   // Verify that Osc/Xtal is used as Clock Sourse
    SYNR    = 0x07;   // Use PLL and 4-MHz crystal to generate 32-MHz system clock
    REFDV   = 0;      // ""
    
    // Enable PLL, set automatic bandwidth control
    PLLCTL  |= 0x40;   
    
    // Wait until PLL is stable
    while(!(CRGFLG & 0x08)); 
    
    // Choose PLL as Clock Source
    CLKSEL  = 0x80;    
}

void main(void) {
	
    PLLInit();    
    PeriphInit();
    SCIOpenCommunication();
    
    SendString("Hola mundo!!\r\n\0");
    
    for(;;) {
        count = 0;      
    } /* loop forever */
  
} /* please make sure that you never leave main */
