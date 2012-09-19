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
	else if(buff[0] == 'R') {
		//Send Other Protocol
	}
}

void sendSony(unsigned char buff[30]){
	//Deshabilita la interrupcion Serial
	sciRxReady = TRUE;
	
	SIRCS_Send_Start();
	
   /* Decode the message to send */
	
	// Convert each ASCII to Hex and check each bit
    for(unsigned char i=1; i<=3; i++){
		for(unsigned char j=1; j<=8; j*2){
			// Decide what kind of bit to send
			if( ConvertAsciiChar(buff[i])&j > 0){  //movable bitmask per char converted to hex
				SIRCS_Send_High();
			}
			else{
				SIRCS_Send_Low();
			}
			// cht wait until it's done
			while(sendingBit);
		}
    }
}

//46 ciclos para generar pulsos durante 600 us
void SIRCS_Send_Start(){
	sendingBit = 1;
	ir_pulseCount = 60; //OscTime
	ir_idleCount  = 15;	  //LowTime in x us per tick
}

void SIRCS_Send_High(){
	sendingBit = 1;
	ir_pulseCount = 30;
	ir_idleCount = 15;
}

void SIRCS_Send_Low(){
	sendingBit = 1;
	ir_pulseCount = 15;
	ir_idleCount = 15;
}