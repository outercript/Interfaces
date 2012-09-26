#include <MC9S12XEP100.h>     /* derivative-specific definitions */
#include "i2c.h"

void Setup_I2C(void){
    // Output Compare Enable
    TIM_TIOS_IOS2 = TRUE;   // Enable Output compare 2

    // Keep the timer interrupts until we need to send data
    TIM_TIE_C2I  = FALSE;   
}



/*######################   BASIC COMMUNICATION FUNCTIONS   #########################*/

/*****************************************************************************/
/*                     Start/End Communication                     */
/***************************************************************************/

   void IIC_start_bit (void)
   {
      SCL=0;
      SDA=1;
      SCL=1;
      //esperar 5000 ns
      I2C_wait(1);
      SDA=0;
      //esperar 5000 ns
      I2C_wait(1);
      SCL=0;
   }

   void IIC_stop_bit (void)
   {
      SCL=0;
      SDA=0;
      I2C_wait(0);
      SCL=1;
      I2C_wait(1);
      SDA=1;
      I2C_wait(1);
      SCL=0;
      SDA=0;
   }


void I2C_wait(Bool IS_FULL_Time) 
{
    if(IS_FULL_Time) 
    {
         TIM_TC2     = TIM_TCNT + I2C_FULL_TIME;    
    } 
    else{
         TIM_TC2     = TIM_TCNT + I2C_HALF_TIME;
    }
    
    TIM_TIE_C2I = TRUE;
    delayFlag=1;
    while(delayFlag);
    
}

/*****************************************************************************/
/*                           Send Byte                        */
/***************************************************************************/

   void IIC_send_byte (unsigned char dato)
   {
      volatile unsigned char contador;
      volatile unsigned char temp;
      temp=dato;
      contador=8;

      do{
         SCL=0;
         I2C_wait(0);
         if (temp&0x80) SDA=1;      // Send the most significant bit
         else SDA=0;               // and shift left for the next bit
         I2C_wait(0);
         SCL=1;
         temp<<=1;
         I2C_wait(0);

      }while (--contador);
      SCL=0;
   }


/*****************************************************************************/
/*                     Read Acknowledge Bit                     */
/***************************************************************************/

   Bool IIC_ACK (void)
   {
      volatile Bool temp;

      SCL=0;
      //SDA direccion como entrada
      DDRA = 0xBF;  //SDA = bit 7
      I2C_wait(1); 
      SCL=1;
      I2C_wait(0);
      if (SDA) temp=1;
      else temp=0;
      I2C_wait(0);
      SCL=0;
      //SDA direccion salida
      DDRA = 0xFF;
      SDA = 0;
      return temp;
   }


/*****************************************************************************/
/*               Send Negative Acknowledge Bit                     */
/***************************************************************************/

   void IIC_NO_ACK (void)
   {
      SCL=0;
      I2C_wait(0);
      //SDA direccion salida
      DDRA = 0xFF;
      SDA=1;
      I2C_wait(0); // Wait TR time from manual
      SCL=1;
      I2C_wait(1); // Wait clock high time
      SCL=0;
      SDA=0;
   }


/*****************************************************************************/
/*                     Send Acknowledge Bit                     */
/***************************************************************************/

   void IIC_SEND_ACK (void)
   {
      SCL=0;
      //SDA direccion salida
      SDA=0;
      // Wait TR time from manual
      SCL=1;
      // Wait clock high time
      SCL=0;
      // Wait TR time
   }


   
/*##########################      WRITE FUNCTIONS       ##############################*/


/*****************************************************************************/
/*                        Byte Write                           */
/***************************************************************************/

   void IIC_byte_write (unsigned int direccion, unsigned char dato)
   {
      IIC_start_bit();
      //IIC_send_byte(0xA0);                  // suponiendo una sola memoria A2=A1=A0=0
      IIC_send_byte(0xA0 | ((direccion>>13)<<1)); // selección de A2,A1,A0 y WB=0
      if (IIC_ACK()) error();
      IIC_send_byte(direccion>>8);             // 5 bits más significativos
      if (IIC_ACK()) error();                  // de dirección (13 en total)
      IIC_send_byte(direccion);
      if (IIC_ACK()) error();
      IIC_send_byte(dato);
      if (IIC_ACK()) error();
      IIC_stop_bit();
   }


/*****************************************************************************/
/*                        Page Write                           */
/***************************************************************************/

   void IIC_page_write (unsigned int direccion, unsigned char datos[32]){
      volatile unsigned char i;
      
      IIC_start_bit();
      //IIC_send_byte(0xA0);                  // suponiendo una sola memoria A2=A1=A0=0
      IIC_send_byte(0xA0 | ((direccion>>13)<<1)); // selección de A2,A1,A0 y WB=0
      if (IIC_ACK()) error();
      IIC_send_byte(direccion>>8);             // 5 bits más significativos
      if (IIC_ACK()) error();                  // de dirección (13 en total)
      IIC_send_byte(direccion);
      if (IIC_ACK()) error();

      for (i=0; i<32 ; i++)               // Send page of 32 bytes
      {
         IIC_send_byte(datos[i]);
         if (IIC_ACK()) error();
      }

      IIC_stop_bit();
   }



/*##########################      READ FUNCTIONS       ##############################*/


/*****************************************************************************/
/*                           Byte Read                        */
/***************************************************************************/

   unsigned char read_byte (void)
   {
      unsigned char contador;
      unsigned char temp;
      
      contador=8;
      SCL=0;
      
      // SDA direccion como entrada
      
      do
      {
         SCL=1;
         if (SDA) temp=(temp<<1)|0x01;   // Store bit and shift left
         else temp<<=1;
         SCL=0;
      }while(contador--);
      
      // SDA direccion salida
      return temp;
   }



/*****************************************************************************/
/*                     Current Address Read                     */
/***************************************************************************/

   unsigned char IIC_current_address_read (unsigned int direccion)
   {
      unsigned char buffer;
      
      IIC_start_bit();
      
   /* Control Byte */
      IIC_send_byte(0xA1 | ((direccion>>13)<<1)); // selección de A2,A1,A0 y WB=1
      if (IIC_ACK()) error();
      
   /* Read Byte */
      buffer=read_byte();
      
   /* End Communication */
      IIC_NO_ACK();                        // There should not be an ACK 
      IIC_stop_bit();
   
      return buffer;
   }



/*****************************************************************************/
/*                     Random Address Read                        */
/***************************************************************************/

   unsigned char IIC_random_read (unsigned int direccion)
   {   
   
   /* Set random address by a write attempt */
      IIC_start_bit();
   
      IIC_send_byte(0xA0 | ((direccion>>13)<<1)); // selección de A2,A1,A0 y WB=0
      if (IIC_ACK()) error();
      IIC_send_byte(direccion>>8);             // 5 bits más significativos
      if (IIC_ACK()) error();                  // de dirección (13 en total)
      IIC_send_byte(direccion); 
      if (IIC_ACK()) error();
   
   /* Read current random address */
      return IIC_current_address_read(direccion);
   }



/*****************************************************************************/
/*                     Sequential Read                           */
/***************************************************************************/

   void IIC_sequential_read (unsigned int direccion, unsigned char *data)
   {
      volatile unsigned char i;
      IIC_start_bit();
      
   /* Control Byte */
      IIC_send_byte(0xA1 | ((direccion>>13)<<1)); // selección de A2,A1,A0 y WB=1
      if (IIC_ACK()) error();
      
   /* Read Bytes */
      for (i=0; i<sizeof(data); i++)
      {
         data[i]=read_byte();
         if ( i<(sizeof(data)-1) ) IIC_SEND_ACK();   // Send ACK
      }
      
   /* End Communication */
      IIC_NO_ACK();
      IIC_stop_bit();
   }

