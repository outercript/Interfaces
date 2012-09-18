/////////////////////////////////////////////////////////////////////////////////////////
//
// IR Communication Interface (SCI) MC9S12XEP100
//
// --------------------------------------------------------------------------------------
// Author : Oscar Suro
// Last Modified : Sept 10, 2012
// 
/////////////////////////////////////////////////////////////////////////////////////////

#include <MC9S12XEP100.h>     /* derivative-specific definitions */
#include "ir.h"

void Setup_IR(){

    // Output Compare as the Carrier Generator
    TIM_TIOS_IOS0 = TRUE;   // Enable Output compare Port 0

    // Configure time for Output Compare 0 (38kHz)
    TIM_TC0  = TIM_TCNT + CARRIER_TIME;

    // Keep the timer interrupts until we need to send data
    TIM_TIE_C0I  = FALSE; 
   
}

void rawSend(unsigned char buff[30]){
    if(buff[0] == 'S'){
        sendSony(buff);
    }
}

void sendSony(unsigned char buff[30]){
    for(unsigned char i=1; i<=3; i++){
        SIRCS_Send_Byte(buff[i]);   
    }
    
    //Habilita la interrupcion Serial
    
        PORTA_PA0 = 1;
        TIM_TIE_C0I = FALSE;
    
    else{
        PORTA_PA0 = 0;
        TIM_TIE_C0I = TRUE;
    } 
    
}

//46 ciclos para generar pulsos durante 600 us
void SIRCS_Send_Byte(){
    
}

void SIRCS_Send_High(){
            
}

void SIRCS_Send_Low(){
    
}