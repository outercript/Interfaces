/////////////////////////////////////////////////////////////////////////////////////////
//
// Software Serial Communication Interface (SCI) MC9S12XEP100
//
// --------------------------------------------------------------------------------------
// Author : Oscar Suro
// Last Modified: Sept 8, 2012
// 
/////////////////////////////////////////////////////////////////////////////////////////

#include <mc9s12xep100.h>
#include "sci.h"


/* Start interrupts */  
#pragma CODE_SEG __NEAR_SEG NON_BANKED
interrupt VectorNumber_Vtimch0 void SerialTx_ISR(void){
    volatile int shift;
    unsigned int temp;

    // Clear Output Compare 0 Interrupt Flag
    TIM_TFLG1 = TIM_TFLG1_C0F_MASK;
         
    // Setup default TX Time
    TIM_TC0 = TIM_TCNT + SCI_BIT_TIME;
     
    // Debug!!!
    shift   = sciTxStatus;
    temp    = sciTxBuffer[sciTxIndex];
    
    // Check for transmission end
    if(temp == '\0'){
        sciTxAvailable  = TRUE; // Make Tx Available
        sciTxStatus     = 0;    // Reset Tx Status
        sciTxIndex      = 0;    // Reset Tx Buffer Index
        TIM_TIE_C0I     = FALSE;   // Disable Output compare Port 0   
        return;  
    }
    
    // Start bit
    if(sciTxStatus == 0){        
        TX_PIN = 0;
        sciTxStatus++;
    }
    
    // Data bits    
    else if(sciTxStatus > 0 && sciTxStatus < 9){
        shift = sciTxStatus - 1;
        temp >>= shift;
        
        if(temp & 0x01){
            TX_PIN = TRUE;
        }
        else{
            TX_PIN = FALSE;
        }
        
        sciTxStatus++;
        
    }
    
    // Stop bit  
    else if(sciTxStatus == 9){
        TX_PIN = 1;         // Raise Stop bit
        sciTxStatus = 0;    // Reset Tx Status
        sciTxIndex += 1;    // Increase Tx Buffer Index  
    }

}


#pragma CODE_SEG DEFAULT
/* End interrupts */


/////////////////////////////////////////////////////////////////////////////////////////
// TimerInit
// --------------------------------------------------------------------------------------
// Configures System timers for this thing to work =)
/////////////////////////////////////////////////////////////////////////////////////////
void TimerInit(void){

    // Setup Timer System Control Registers
    TIM_TSCR1_TEN  = TRUE;      // Enable Timer
    TIM_TSCR1_TFFCA = TRUE;     // Enable Fast Timer Flag Clear  

    TIM_TSCR2_PR = 0;  // Select 1 Prescaler    
    TIM_PACTL  = 0; // Setup Timer Preset  

}


/////////////////////////////////////////////////////////////////////////////////////////
// SCI_SetupTransmit
// --------------------------------------------------------------------------------------
// Configures & Initialize the Transmit Module
/////////////////////////////////////////////////////////////////////////////////////////
void SCI_SetupTransmit(){

    // Initialize the Transmit Flags
    sciTxAvailable  = TRUE;
    sciTxIndex      = 0;
    sciTxStatus     = 0;
    TX_PIN          = TRUE;
      
    //Enable Output compare port 0 for Serial Transmit
    TIM_TIOS_IOS0 = TRUE;   // Enable Output compare Port 0  

}


/////////////////////////////////////////////////////////////////////////////////////////
// SCI_SetupReceive
// --------------------------------------------------------------------------------------
// Configures & Initialize the Receive Module
/////////////////////////////////////////////////////////////////////////////////////////
void SCI_SetupRecieve(){

    // Initialize the Receive Flags
    sciRxReady      = FALSE;
    sciRxOverflow   = FALSE;
    sciRxIndex      = 0;
    sciRxStatus     = 0;
    
    //Enable Output compare port 1 for Serial Receive
    TIM_TIOS_IOS1 = TRUE;   // Enable Output compare Port 0
    
    // Configure time for Output Compare 1
    TIM_TC1  = TIM_TCNT + SCI_POLL_TIME;
    
    //Enable Timer Interrupt Output Compare 1 
    TIM_TIE_C1I  = TRUE;    

}


/////////////////////////////////////////////////////////////////////////////////////////
// SCIOpenCommunication
// --------------------------------------------------------------------------------------
// Configures SCI registers for Enable Transmit and Receive data
/////////////////////////////////////////////////////////////////////////////////////////
void SCIOpenCommunication(){
    // Initialize the Timer modules
    TimerInit();
    
    // Initialize the Transmit Module
    SCI_SetupTransmit();
    
    // Initialize the Receive Module
    //SCI_SetupRecieve();
    
    EnableInterrupts;
 
}


/////////////////////////////////////////////////////////////////////////////////////////
// ConvertCharAscii
// --------------------------------------------------------------------------------------
// This function converts a single-digit hexadecimal number in its ASCII code
/////////////////////////////////////////////////////////////////////////////////////////
unsigned char ConvertCharAscii(unsigned char value)
{
  if (value<10)
    return value+48;
  else
    return value+55;
}

/////////////////////////////////////////////////////////////////////////////////////////
// SendString
// --------------------------------------------------------------------------------------
// Sends a char string on the SCI
/////////////////////////////////////////////////////////////////////////////////////////
void SendString(char buffer[BUFFER_SIZE]){
    unsigned int index;
    
    if(!sciTxAvailable)
        return;
    
    // Set the transmit line busy
    sciTxAvailable = FALSE;  

    // Copy string to the buffer
    for(index=0; buffer[index] !='\0'; index++){
        sciTxBuffer[index] = buffer[index]; 
    }
    
    // Configure time for Output Compare 0
    TIM_TC0  = TIM_TCNT + SCI_BIT_TIME;

    // Enable Timer Interrupt Output Compare 0 
    TIM_TIE_C0I  = TRUE; 
      
}

/////////////////////////////////////////////////////////////////////////////////////////
// SendHexValue
// --------------------------------------------------------------------------------------
// This function sends a two-digit hex value on the SCI
/////////////////////////////////////////////////////////////////////////////////////////
void SendHexValue(unsigned char hex_value)
{
  char hex_L, hex_H;

  hex_L = hex_value & 0x0F;
  hex_H = hex_value>>4;
  //(void)SCISendBuffer(ConvertCharAscii(hex_H));
  //(void)SCISendBuffer(ConvertCharAscii(hex_L));
  SendString("\r\n\0");
}

