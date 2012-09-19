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

void Setup_IR(void){

    IR_PORT = FALSE;

    // Output Compare as the Carrier Generator
    TIM_TIOS_IOS0 = TRUE;   // Enable Output compare Port 0
    TIM_TIOS_IOS1 = TRUE;   // Enable Output compare Port 0


    // Keep the timer interrupts until we need to send data
    TIM_TIE_C0I  = FALSE;
    TIM_TIE_C1I  = FALSE; 
   
}

void rawSend(unsigned char buff[30]){
    if(buff[0] == 'S'){
        sendSony(buff);
    }
	else if(buff[0] == 'R') {
	    sendSony(buff);	
	}
}

void sendSony(unsigned char buff[30]){
    unsigned char i;
    unsigned char j;
    volatile unsigned char tmp;

	SIRCS_Send_Start();
	
   /* Decode the message to send */
	
	// Convert each ASCII to Hex and check each bit
    for(i=1; i<=3; i++){
        tmp = buff[i];
        tmp = ConvertAsciiChar(tmp);
        
		for(j=0; j<4; j++){
			// Decide what kind of bit to send
			if(tmp & 0x08){  //movable bitmask per char converted to hex
				SIRCS_Send_High();
			}
			else{
				SIRCS_Send_Low();
			}
			IR_sendBit();
			tmp <<= 1;		
		}
    }
}

//46 ciclos para generar pulsos durante 600 us
void SIRCS_Send_Start(void){
	ir_pulseCount = SONY_SHIGH; //OscTime
	ir_idleCount  = SONY_SLOW;	  //LowTime in 40 us per tick
	IR_sendBit();
}

void SIRCS_Send_High(void){
	ir_pulseCount = SONY_1HIGH;
	ir_idleCount = SONY_1LOW;
}

void SIRCS_Send_Low(void){
	ir_pulseCount = SONY_0HIGH;
	ir_idleCount = SONY_0LOW;
}

void IR_sendBit(void){
    // Thats true
    sendingBit = TRUE;
    ControlCount = 0;
    ControlStatus = 0;
    // Configure time for Output Compare 0 (38kHz)
    TIM_TC0     = TIM_TCNT + CARRIER_TIME;
    TIM_TC1     = TIM_TCNT + CONTROLER_TIME;
    
    // Enable Output compare interrupts
    TIM_TIE_C0I = TRUE;
	TIM_TIE_C1I = TRUE;
	
	// cht wait until it's done
	while(sendingBit);
	TIM_TIE_C0I = FALSE;
	TIM_TIE_C1I = FALSE;
}
