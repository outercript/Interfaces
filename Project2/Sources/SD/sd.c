/******************************************************************************
*  (c) copyright Freescale Semiconductor China Ltd. 2008
*  ALL RIGHTS RESERVED
*  File Name: SD.C
*  Description: SD Card Drivers
*  Assembler:  Codewarrior for HC(S)08 V6.0
*  Version: 1.0                                                         
*  Author: Jose Ruiz
*  Location: Guadalajuara, Mexico                                              
*                                                  
* UPDATED HISTORY:
* REV   YYYY.MM.DD  AUTHOR        DESCRIPTION OF CHANGE
* ---   ----------  ------        --------------------- 
* 1.0   2008.01.02  Jose Ruiz      Initial version
******************************************************************************/                                                                        
/* Freescale  is  not  obligated  to  provide  any  support, upgrades or new */
/* releases  of  the Software. Freescale may make changes to the Software at */
/* any time, without any obligation to notify or provide updated versions of */
/* the  Software  to you. Freescale expressly disclaims any warranty for the */
/* Software.  The  Software is provided as is, without warranty of any kind, */
/* either  express  or  implied,  including, without limitation, the implied */
/* warranties  of  merchantability,  fitness  for  a  particular purpose, or */
/* non-infringement.  You  assume  the entire risk arising out of the use or */
/* performance of the Software, or any systems you design using the software */
/* (if  any).  Nothing  may  be construed as a warranty or representation by */
/* Freescale  that  the  Software  or  any derivative work developed with or */
/* incorporating  the  Software  will  be  free  from  infringement  of  the */
/* intellectual property rights of third parties. In no event will Freescale */
/* be  liable,  whether in contract, tort, or otherwise, for any incidental, */
/* special,  indirect, consequential or punitive damages, including, but not */
/* limited  to,  damages  for  any loss of use, loss of time, inconvenience, */
/* commercial loss, or lost profits, savings, or revenues to the full extent */
/* such  may be disclaimed by law. The Software is not fault tolerant and is */
/* not  designed,  manufactured  or  intended by Freescale for incorporation */
/* into  products intended for use or resale in on-line control equipment in */
/* hazardous, dangerous to life or potentially life-threatening environments */
/* requiring  fail-safe  performance,  such  as  in the operation of nuclear */
/* facilities,  aircraft  navigation  or  communication systems, air traffic */
/* control,  direct  life  support machines or weapons systems, in which the */
/* failure  of  products  could  lead  directly to death, personal injury or */
/* severe  physical  or  environmental  damage  (High  Risk Activities). You */
/* specifically  represent and warrant that you will not use the Software or */
/* any  derivative  work of the Software for High Risk Activities.           */
/* Freescale  and the Freescale logos are registered trademarks of Freescale */
/* Semiconductor Inc.                                                        */ 
/*****************************************************************************/

/* Includes */
#include "sd.h"
#include "sci.h"

/* Gobal Variables */
T32_8 gu8SD_Argument;
uint8_t gu8SD_CID[16];


/************************************************/
uint8_t SD_Init(void){
    volatile uint8_t retcode;
    volatile uint8_t card_type;
    uint16_t timeout; 

    SPI_Init();                     // SPI Initialization
    card_type = CARD_NONE;
    
    /* SD Software Reboot */
    SendString(SCI_0,"- Soft Reset\r\n\0");
    SPI_SS=DISABLE;
    SD_CLKDelay(20);               // Send 80 clocks

    
    /* Clear the cards buffer */
    SD_CLKDelay(8); 
    
    
    /* Put the card in SPI IDLE State */
    gu8SD_Argument.lword=0;
    SendString(SCI_0,"- Put in Idle State\r\n\0"); 
    for(timeout=0; timeout < 250; timeout++){
    
        // Send IDLE State CMD
        SPI_SS=ENABLE;
        retcode = SD_SendCommand(SD_CMD0, SD_IDLE);
        SPI_SS=DISABLE;
        
        if(retcode == 0){       
            break;
        }
    }
    
    /* Check the card state is IDLE */
    if(retcode != 0){
        SendString(SCI_0,"-- Fail: Not Idle!\r\n\0"); 
        SendString(SCI_0,"-- Error code: ");
        SendHexValue(SCI_0, retcode); 
        
        SendString(SCI_0,"-- Is the card present!?\r\n\0");      
        return(10 + retcode);
    }
   
    
    // Mandatory dummy SPI cycle
    (void)SPI_Receive_byte();
    retcode = 0xFF;
    
        
    /* Check SD Version */
    SendString(SCI_0,"\r\n\0");
    SendString(SCI_0,"- Sending CMD8\r\n\0");
    gu8SD_Argument.lword=0x000001AA;
      
    SPI_SS=ENABLE;
    retcode = SD_SendCommand(SD_CMD8, SD_IDLE);
    SPI_SS=DISABLE;

    // Check CMD8 Response
    if(retcode == 0){
        SendString(SCI_0,"-- CMD8 Accepted\r\n\0");
        for(timeout=0; timeout < 4; timeout++){
            retcode = SPI_Receive_byte();
        }
        
        card_type = CARD_SD2;
        SendString(SCI_0,"-- Detected SD2 or SDHC\r\n\0"); 
    } 
    
    else{
        SendString(SCI_0,"-- CMD8 failed\r\n\0");
        SendString(SCI_0,"-- Error code: ");
        SendHexValue(SCI_0, retcode);
        
        card_type = CARD_SD1;
        SendString(SCI_0,"-- Detected SD1 or MMC\r\n\0"); 
    }
   
   
    // Mandatory dummy SPI cycle
    (void)SPI_Receive_byte();
    retcode = 0xFF;
    
    
    /*  Initialize SD Command */
    SendString(SCI_0,"\r\n\0");
    SendString(SCI_0,"- Starting memory card\r\n\0");
    if(card_type > CARD_MMC){
      
        SendString(SCI_0,"-- Init as SD card\r\n\0");
        for(timeout=0; timeout < 65530; timeout++){
            
            // Send the ACMD command enabler
            gu8SD_Argument.lword=0;
            SPI_SS=ENABLE;
            (void)SD_SendCommand(SD_CMD55, SD_OK);
            SPI_SS=DISABLE;
            
            // Mandatory dummy SPI cycle
            (void)SPI_Receive_byte();
        
            // Send ACMD41 for SDC Cards
            gu8SD_Argument.lword=0x40000000;
            SPI_SS=ENABLE;
            retcode = SD_SendCommand(SD_ACMD41, SD_OK);
            SPI_SS=DISABLE;
            
            // Mandatory dummy SPI cycle
            (void)SPI_Receive_byte();
            
            
            // Detect MMC Cards
            if((retcode & 0x04) && (retcode != 0xFF)){
                SendString(SCI_0,"--- Fail: Not working\r\n\0");
                SendString(SCI_0,"--- Error code: ");
                SendHexValue(SCI_0, retcode);
                
                SendString(SCI_0,"--- Is this a MMC Card?\r\n\0");
                SendString(SCI_0,"\r\n\0");
                card_type = CARD_MMC;
                break;  
            }
            
            // Card exited Idle status
            if(retcode == 0){
                SendString(SCI_0,"--- Success\r\n\0");       
                break;
            }
        }
    }
    
    // MMC Card Initialization
    if(card_type == CARD_MMC){
        SendString(SCI_0,"-- Init as MMC card\r\n\0");
        for(timeout=0; timeout < 65530; timeout++){

            gu8SD_Argument.lword=0;
            SPI_SS=ENABLE;
            retcode = SD_SendCommand(SD_CMD1, SD_OK);
            SPI_SS=DISABLE;
            
            // Mandatory dummy SPI cycle
            (void)SPI_Receive_byte();
            
            if(retcode == 0){
                SendString(SCI_0,"--- Success\r\n\0");       
                break;
            }
        }
    
    }
    
    if(retcode != 0){
        SendString(SCI_0,"- Fail: Memory timed out!\r\n\0");
        SendString(SCI_0,"Error code: ");
        SendHexValue(SCI_0, retcode); 
        return(20 + retcode);  
    }
    
    
    // Mandatory dummy SPI cycle
    (void)SPI_Receive_byte();
    retcode = 0xFF;
        
    
    /* Set SD Block Length */ 
    SendString(SCI_0,"\r\n\0"); 
    SendString(SCI_0,"- Setting Block Size\r\n\0");
    gu8SD_Argument.lword=SD_BLOCK_SIZE;
    
    SPI_SS=ENABLE;
    retcode = SD_SendCommand(SD_CMD16, SD_OK);
    SPI_SS=DISABLE;                    
    
    if(retcode != 0){ 
        SendString(SCI_0,"-- Block Size Fail\r\n\0");
        SendString(SCI_0,"-- Error code: ");
        SendHexValue(SCI_0, retcode);
        return(30 + retcode);      // Command IDLE fail
    }

   
    // Mandatory dummy SPI cycle
    (void)SPI_Receive_byte();
    
    
    SendString(SCI_0,"\r\n\0");
    SendString(SCI_0,"- SD High Speed\r\n\0");
    /* Switch to HIGH Speed Mode */
    SPI_High_rate();
    
    SendString(SCI_0,"\r\n\0");
    SendString(SCI_0,"- Memory Initialized\r\n\0");
    return(OK);
}

/************************************************/
uint8_t SD_Write_Block(uint32_t u16SD_Block, uint8_t *pu8DataPointer)
{
    uint16_t u16Counter;
    
    SPI_SS=ENABLE;

    gu8SD_Argument.lword=u16SD_Block;
    gu8SD_Argument.lword=gu8SD_Argument.lword<< SD_BLOCK_SHIFT;
    
    if(SD_SendCommand(SD_CMD24,SD_OK)){
        SPI_SS=DISABLE;
        return(WRITE_COMMAND_FAILS);      // Command IDLE fail
    }
    
    SPI_Send_byte(0xFE);
    
    for(u16Counter=0;u16Counter<BLOCK_SIZE;u16Counter++)
        SPI_Send_byte(*pu8DataPointer++);

    SPI_Send_byte(0xFF);    // checksum Bytes not needed
    SPI_Send_byte(0xFF);
 
 
    if((SPI_Receive_byte() & 0x0F) != 0x05){
        SPI_SS=DISABLE;
        return(WRITE_DATA_FAILS);      // Command fail
    }

    SPI_Send_byte(0x00);
    while(SPI_Receive_byte()==0x00);  // Dummy SPI cycle
    
    SPI_SS=DISABLE;
    return(OK);
}


/************************************************/
uint8_t SD_Read_Block(uint32_t u16SD_Block, uint8_t *buffer)
{
    volatile uint8_t u8Temp=0;
    uint16_t u16Counter;
    
    SPI_SS=ENABLE;

    gu8SD_Argument.lword = u16SD_Block;
    gu8SD_Argument.lword = (gu8SD_Argument.lword << SD_BLOCK_SHIFT);

    if(SD_SendCommand(SD_CMD17,SD_OK)){
        SPI_SS=DISABLE;
        return(READ_COMMAND_FAILS);      // Command IDLE fail
    }
    
    while(u8Temp!=0xFE)
        u8Temp = SPI_Receive_byte();
    
    // Fetch the data block, one byte at a time
    for(u16Counter=0; u16Counter<BLOCK_SIZE; u16Counter++){
        u8Temp = SPI_Receive_byte();
        buffer[u16Counter] = u8Temp;
    }

    (void)SPI_Receive_byte();  // Skip CRC SPI cycle
    (void)SPI_Receive_byte();  // Dummy SPI cycle
    
    SPI_SS=DISABLE;
    
    (void)SPI_Receive_byte();  // Dummy SPI cycle 
      
    return(OK);
}



/************************************************/
uint8_t SD_SendCommand(uint8_t u8SDCommand, uint8_t u8SDResponse){

    uint8_t u8SD_crc;
    volatile uint8_t u8Counter;
    volatile uint8_t u8Temp=0xFF;

    /* Send Command byte */
    SPI_Send_byte(u8SDCommand | 0x40);


    /* Send Argument */
    for(u8Counter=0; u8Counter<4; u8Counter++){
        SPI_Send_byte(gu8SD_Argument.bytes[u8Counter]);
    }
    
        
    /* Send CRC */
    u8SD_crc = 0xFF;
    if (u8SDCommand == SD_CMD0) u8SD_crc = 0x95;  // correct crc for CMD0 with arg 0
    if (u8SDCommand == SD_CMD8) u8SD_crc = 0x87;  // correct crc for CMD8 with arg 0X1AA
    SPI_Send_byte(u8SD_crc);
  
  
    /* Response RHandler */
    u8Counter=SD_WAIT_CYCLES;
    do
    {
        u8Temp = SPI_Receive_byte();
        u8Counter--;
    }
    while((u8Temp & 0x80) && (u8Counter > 0));
    
    
    /* Returns the response */
    if(u8Temp == u8SDResponse)   
        return(OK);
    
    else            
        return(u8Temp);
}


/************************************************/
void SD_CLKDelay(uint8_t u8Frames){
    while(u8Frames--) SPI_Send_byte(0xFF);
}