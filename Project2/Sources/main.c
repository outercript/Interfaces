#include <hidef.h>      /* common defines and macros */
#include <stdio.h>
#include "derivative.h"      /* derivative-specific definitions */
#include "spi.h"
#include "sci.h"
#include "sd.h"
#include "Fat.h"

// Serial Port 0
Bool sciRxReady;
Bool sciRxOverflow;
unsigned int sciRxIndex;
unsigned char sciRxBuffer[30];


/* Start interrupts */  
#pragma CODE_SEG __NEAR_SEG NON_BANKED
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
    
    // Serial Port 0
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
    PLLCTL |= 0x40;   // enable PLL, set automatic bandwidth control
    
    while(!(CRGFLG & 0x08));
    CLKSEL  = 0x80;   // enable PLL, keep SYSCLK running in wait mode    
}

void main(void) {
    uint8_t retcode;
    uint8_t data[512];

  /* Initialize Peripherials */
    PLLInit();    
    TimerInit();
    PeriphInit();
    EnableInterrupts;
    
    (void)memset(&data[0],0, sizeof(data));

    SendString(SCI_0,"Initializing SD Card\r\n\0");
    retcode = SD_Init();
    
    //Try to read data from SD
    if(retcode == 0){
        SendString(SCI_0,"Reading SD Card\r\n\0");
        retcode = SD_Read_Block(135, &data[0]);
        
        //FAT_Read_Master_Block();
        
        //FAT_LS();
    }
    
    SendString(SCI_0,"Finished!\r\n\0");

  /* loop forever */
    for(;;) {
        PORTA_PA2 = TRUE;
        PORTA_PA3 = TRUE;
    } 
  /* please make sure that you never leave main */
}
