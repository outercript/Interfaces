#include "SPI.h"

/************************************************/
void SPI_Init(void){

  // Configure Port H [4..7] in SPI Mode 
  MODRR   = MODRR_MODRR6_MASK;
   
  // SPI Baud Rate Register Config
  // Baud Rate Divisor  = (2+1)*2^(4+1) = 96
  // BaudRate           = 32MHz / 96 = 333kHz
  SPI2BR  = 0x24; // SPI Speed = 333 Khz 
  
  // Configure SPI Control Register 1
  SPI2CR1 = 0x00;     // Manual SS Pin Mode
  SPI2CR1 = SPI2CR1_SPE_MASK | SPI2CR1_MSTR_MASK;
  
  // Configure SPI Control Register 2		
  SPI2CR2 = 0x00;
  
  // Disable SS Port 
  SPI_SS = DISABLE; 

}

/************************************************/
void SPI_Send_byte(uint8_t data){
    uint8_t tmp;
    
    // Wait until the Transmit is ready
	while(!SPI2SR_SPTEF);
	
	// Load the data to transmission buffer
	SPI2DR = data;
	
	// Wait until the Transmit is complete
	while(!SPI2SR_SPIF);
	tmp = SPI2DR;
}

/************************************************/
uint8_t SPI_Receive_byte(void){
    uint8_t tmp;
    
	// Wait until the Transmit is ready
	while(!SPI2SR_SPTEF);
	
	// Trigger 8 clocks to receive data
	SPI2DR=0x00;
	
	while(!SPI2SR_SPIF);
	tmp = SPI2DR;
		
	return(SPI2DR);
}

/************************************************/
void SPI_High_rate(void){

  SPI2BR  = 0x11; // 4MHz 
  SPI2CR1 = 0x00;    		
  SPI2CR1 = SPI2CR1_SPE_MASK | SPI2CR1_MSTR_MASK;
  
}