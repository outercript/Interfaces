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
    TIM_TFLG1 |= TIM_TFLG1_C0F_MASK;
         
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

interrupt VectorNumber_Vtimch1 void SerialRx_ISR(void){
    volatile unsigned int temp;
    volatile unsigned int status;
    volatile char dummy;
    volatile unsigned int chato;
    
    // Clear Output Compare 0 Interrupt Flag
    TIM_TFLG1 |= TIM_TFLG1_C1F_MASK;
    
    if(sciRxReady == TRUE){
        
        return;    
    }
    
    // Read the Receive Pin
    temp = RX_PIN;
    status = sciRxStatus;
    
    if(sciRxStatus == 0){
                
        // Setup default polling time
        TIM_TC1 = TIM_TCNT + SCI_POLL_TIME;
        
        // Start bit received ?
        if(temp == FALSE)
            sciRxStatus++;
        
    }
    
    // Start bit init condition
    else if(sciRxStatus == 1){
    
        //Setup half bit pooling time
        TIM_TC1 = TIM_TCNT + SCI_HALF_TIME;
        sciRxStatus++;           
    }
    
    // Verify start bit
    else if(sciRxStatus == 2){
    
        // Valid Start bit
        if(temp == FALSE){
        
            // Setup full bit pooling time
            TIM_TC1 = TIM_TCNT + SCI_BIT_TIME;
            sciRxStatus++;         
        }
        
        else{
            // Setup fast pooling time
            TIM_TC1 = TIM_TCNT + SCI_POLL_TIME;
            
            // Raise bad start flag and reset status
            sciRxStatus     = 0;
            sciRxBadStart   = TRUE;
            
            // Disable the Receive Interrupt
            TIM_TIE_C1I  = FALSE;
        }
           
    }
    
    // Data bit 0 to 7
    else if(sciRxStatus > 2 && sciRxStatus < 11){
        
        // Setup full bit pooling time
        TIM_TC1 = TIM_TCNT + SCI_BIT_TIME;
        
        // Shift the buffer one position
        sciRxBuffer[sciRxIndex] >>= 1;
        
        // Save the port data to the buffer
        if(temp == TRUE)
            sciRxBuffer[sciRxIndex] |= 0x80;
        
        else{
            sciRxBuffer[sciRxIndex] &= 0x7F;
        }
        
        // Increase Rx Status
        sciRxStatus++;
    }
    
    else if(sciRxStatus == 11){
        
        if(temp == TRUE){
            
            // Setup half bit pooling time
            TIM_TC1 = TIM_TCNT + SCI_HALF_TIME;
            sciRxStatus++;   
        }
        
        else{
            // Setup fast pooling time
            TIM_TC1 = TIM_TCNT + SCI_POLL_TIME;
            
            // Raise bad stop flag
            sciRxStatus     = 0;
            sciRxBadStop    = TRUE;
            
            // Disable the Receive Interrupt
            TIM_TIE_C1I  = FALSE;            
        }
    }
    
    else if(sciRxStatus == 12){
        dummy = sciRxBuffer[sciRxIndex];
        chato = sciRxIndex;
        
        // End of transmission ?
        if(sciRxBuffer[sciRxIndex] == '\n' || sciRxBuffer[sciRxIndex] == '\r'){
            // Raise the Receive Ready Flag
            sciRxReady = TRUE;
            
            // Disable the Receive Interrupt
            TIM_TIE_C1I  = FALSE;           
        }
        
        else{
             // Setup fast pooling time
            TIM_TC1 = TIM_TCNT + SCI_POLL_TIME;

            // Increase the index for next char
            sciRxIndex++;
            
        }
        
        // Reset the count
        sciRxStatus     = 0;
       
    }
    
    // Overflow check
    if(sciRxIndex == BUFFER_SIZE){
        
        // Raise buffer overflow flag
        sciRxOverflow = TRUE;
        
        // Disable the Receive Interrupt
        TIM_TIE_C1I  = FALSE;
        
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
    sciRxBadStart   = FALSE;
    sciRxBadStop    = FALSE;
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
    SCI_SetupRecieve();
    
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
    
    // Erase the buffer
    (void) memset(&sciTxBuffer[0], 0, sizeof(sciTxBuffer));  

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
// GetString
// --------------------------------------------------------------------------------------
// Sends a char string on the SCI
/////////////////////////////////////////////////////////////////////////////////////////
void GetString(char *buffer){
    unsigned int index;
    volatile unsigned int juan;
    
    juan = sciRxIndex;
    index = 0;
    
    if(!sciRxReady)
        return;
    
    // Erase the buffer
    (void) memset(&buffer[0], 0, BUFFER_SIZE);
    
    // Copy string to the buffer
    for(index=0; index <= sciRxIndex; index++){
        buffer[index] = sciRxBuffer[index]; 
    }
    
    // Some formmatting =)
    buffer[index] = '\n';
    buffer[index+1] = '\r';    
    buffer[index+2] = '\0';
       
    // Clear Receive Buffer
    (void) memset(&sciRxBuffer[0], 0, sizeof(sciRxBuffer));
    sciRxIndex = 0;
    
        // Set the transmit line busy
    sciRxReady = FALSE; 
    
    // Configure time for Output Compare 1
    TIM_TC1  = TIM_TCNT + SCI_POLL_TIME;

    // Enable Timer Interrupt Output Compare 1 
    TIM_TIE_C1I  = TRUE; 
      
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

