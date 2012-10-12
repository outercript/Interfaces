#ifndef __SPI__
#define __SPI__

/* Includes */
#include "derivative.h"


/* definitions */
#define SPI_SS    PORTA_PA0      /* Slave Select */
#define _SPI_SS   DDRH_DDRH7 
  
   
#define ENABLE    0
#define DISABLE   1


/* Global Variables */


/* Prototypes */
void SPI_Init(void);
void SPI_Send_byte(uint8_t u8Data);
uint8_t SPI_Receive_byte(void);
void SPI_High_rate(void);


#endif /* __SPI__ */