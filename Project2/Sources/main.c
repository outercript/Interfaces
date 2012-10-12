#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "spi.h"
#include "sd.h"


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
}


void PLLInit(void){
    CLKSEL &= 0x7F;
    SYNR    = 0x07;   // use PLL and 4-MHz crystal to generate 24-MHz system clock
    REFDV   = 0;      //    
    PLLCTL |= 0x40;   // enable PLL, set automatic bandwidth control
    
    while(!(CRGFLG & 0x08));
    CLKSEL  = 0x80;   // enable PLL, keep SYSCLK running in wait mode    
}

void main(void) {
    uint8_t retcode;
    uint8_t data;
    volatile uint16_t delay;

  /* Initialize Peripherials */
    PLLInit();    
    TimerInit();
    PeriphInit();
    EnableInterrupts;
    
    for(delay=0; delay < 65530; delay++);
    for(delay=0; delay < 65530; delay++);
    for(delay=0; delay < 65530; delay++);
    for(delay=0; delay < 65530; delay++);
    for(delay=0; delay < 65530; delay++);
    for(delay=0; delay < 65530; delay++);
    for(delay=0; delay < 65530; delay++);
    for(delay=0; delay < 65530; delay++);
    for(delay=0; delay < 65530; delay++);
    for(delay=0; delay < 65530; delay++);
    for(delay=0; delay < 65530; delay++);
    for(delay=0; delay < 65530; delay++);
    for(delay=0; delay < 65530; delay++);
    for(delay=0; delay < 65530; delay++);
    for(delay=0; delay < 65530; delay++);
    for(delay=0; delay < 65530; delay++);

    retcode = SD_Init();
    
    //Try to read data from SD
    data = 0x44;
    retcode = SD_Read_Block(0x0003EE00, &data);

  for(;;) {
    PORTA_PA2 = TRUE;
    PORTA_PA3 = TRUE;
  
  } /* loop forever */
  /* please make sure that you never leave main */
}
