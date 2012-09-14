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

    //Output Compare
    TIM_TIOS_IOS0 = TRUE;   // Enable Output compare Port 0

    // Configure time for Output Compare 0
    TIM_TC0  = TIM_TCNT + CARRIER_TIME;

    //Enable Timer Interrupt Output Compare 0 
    TIM_TIE_C0I  = TRUE; 
   
}