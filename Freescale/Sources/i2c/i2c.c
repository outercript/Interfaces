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
      // Set error flag to zero to monitor a frame error
      ErrorFlag=0;
      
      // Set SDA as output
      DDRB = 0x03;  //SDA = bit 1 del puerto B
      I2C_wait(1);
      
      // Set both outputs in high level
      SDA=1;
      //wait 2.5 us
      I2C_wait(0);
      
      /* Generate Start Bit Condition */
      SCL=1;
      //wait 5 us
      I2C_wait(1);
      SDA=0;
      //wait 5 us
      I2C_wait(1);
      SCL=0;
      I2C_wait(1);
      
      /* Leave both at zero */
   }

   void IIC_stop_bit (void)
   {
      
      // Set SDA as output
      DDRB = 0x03;  //SDA = bit 1 del puerto B
      I2C_wait(1);
      // Force clock to zero and wait first half bit time
      SCL=0;
      I2C_wait(0);
      
      // Set SDA to zero and wait second half bit time
      SDA=0;
      I2C_wait(1);
      
      /* Generate Stop Condition */
      SCL=1;
      I2C_wait(1);
      SDA=1;
      I2C_wait(1);
      
      /* Leave Both at High level*/
   }


  void I2C_wait(Bool IS_FULL_Time) 
  {
      // Wait 5 us
      if(IS_FULL_Time){
           IIC_DELAY = I2C_FULL_TIME;    
      }
      // Wait 2.5 us 
      else{
           IIC_DELAY = I2C_HALF_TIME;
      }
      
      // Enable the timer interruption
      delayFlag   = 1;
      TIM_TC2     = TIM_TCNT + IIC_DELAY;
      TIM_TIE_C2I = TRUE;
      
      
      // Wait until the interruption sets the flag to zero
      while(delayFlag);
  }
  
  void error(void){
      ErrorFlag=1;
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
      
      // Set SDA as output
      DDRB = 0x03;  //SDA = bit 1 del puerto B

      do{
          // Set clock to zero and wait first half bit time
          SCL=0;
          I2C_wait(0);
          
          // Send the most significant bit
          if(temp&0x80){
              SDA=1;     
          }
          else{
              SDA=0;
          }
          
          // Shift left to send the next bit
          temp<<=1;
          
          // Wait second half bit time
          I2C_wait(0);
          
          // Set clock to high and wait full bit time
          SCL=1;
          I2C_wait(1);

      }while (--contador);
      
      // Set clock to zero
      SCL=0;
   }


/*****************************************************************************/
/*                     Read Acknowledge Bit                     */
/***************************************************************************/

   Bool IIC_ACK (void)
   {
      volatile Bool temp;
      SCL=0;
      
      // Set SDA as input
      DDRB = 0x01;  //SDA = bit 1 del puerto B
      I2C_wait(1);
      
      // Set Clock to high and wait first half bit time 
      SCL=1;
      I2C_wait(0);
      
      // Read data from the slave 
      if(SDA){
          temp=1;
      }else{
          temp=0;
      }
      
      // Wait second half bit time and set clock to zero
      I2C_wait(0);
      SCL=0;
      
      // Set SDA as output
      DDRA = 0x03;
      SDA  = 0;
      
      return temp;
   }


/*****************************************************************************/
/*               Send Negative Acknowledge Bit                     */
/***************************************************************************/

   void IIC_NO_ACK (void)
   {
      // Set SDA as output
      DDRB = 0x03;  //SDA = bit 1 del puerto B
      
      // Force clock to zero and wait first half bit time
      SCL=0;
      I2C_wait(0);
      
      // Set SDA as output and wait second half bit time
      DDRB = 0x03;
      SDA=1;
      I2C_wait(0);
      
      // Raise clock full bit time so the SDA can be read by the slave 
      SCL=1;
      I2C_wait(1);
      
      // Set both outputs to zero
      SCL=0;
      SDA=0;
   }


/*****************************************************************************/
/*                     Send Acknowledge Bit                     */
/***************************************************************************/

   void IIC_SEND_ACK (void)
   {
      // Set SDA as output
      DDRB = 0x03;  //SDA = bit 1 del puerto B
      
      SCL=0;
      
      //wait half bit
      I2C_wait(0); 
      
      //Change SDA data while clock is down
      SDA=0;
      
      //wait second half bit
      I2C_wait(0);
      
      //Raise clock to publish data
      SCL=1;
      
      //wait full bit time for clock
      I2C_wait(1); 
      
      //set clock to zero
      SCL=0;
   }


   
/*##########################      WRITE FUNCTIONS       ##############################*/


/*****************************************************************************/
/*                        Byte Write                           */
/***************************************************************************/

   void IIC_byte_write (unsigned char direccion, unsigned char dato)
   {
      // Initiate frame
      IIC_start_bit();
      
      // Controling only one device A2=A1=A0=0
      // Sending Control bits and default device address to zero
      IIC_send_byte(0xA0);     
      
      // wait for acknowledge from slave 
      while(IIC_ACK()){
          IR_PORT = TRUE;
          I2C_wait(1);
          
          // Initiate frame
          IIC_start_bit();
          
          // Controling only one device A2=A1=A0=0
          // Sending Control bits and default device address to zero
          IIC_send_byte(0xA0);
      }
      
      // Send memory address to write      
      IIC_send_byte(direccion);
      
      // Expect an acknowledge from slave
      if (IIC_ACK()){ error(); }
      
      // Send data byte
      IIC_send_byte(dato);
      
      // Expect an acknowledge
      if (IIC_ACK()){ error(); }
      
      // End frame
      IIC_stop_bit();
   }


/*****************************************************************************/
/*                        Page Write                           */
/***************************************************************************/

   void IIC_page_write (unsigned char direccion, unsigned char datos[8]){
      volatile unsigned char i;
      
      // Initiate frame
      IIC_start_bit();
      
      // Controling only one device ( A2=A1=A0=0, WB=0 )
      IIC_send_byte(0xA0);
      
      // Expect an acknowledge from slave
      //if (IIC_ACK()) { error(); }
      while(IIC_ACK()){
          IR_PORT = TRUE;
          I2C_wait(1);
          
          // Initiate frame
          IIC_start_bit();
          
          // Controling only one device A2=A1=A0=0
          // Sending Control bits and default device address to zero
          IIC_send_byte(0xA0);
      }

      // Send memory address to start writing
      IIC_send_byte(direccion);
      
      // Expect an acknowledge from slave
      if (IIC_ACK()){ error(); }
      
      // Send page of 8 bytes
      for (i=0; i<8; i++){      
         // Send data byte
         IIC_send_byte(datos[i]);
           
         // Send page of 8 bytes
         if (IIC_ACK()){ error(); }
      }

      // End frame
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
      
      contador=7;
      
      // Set SDA pin as Input and leave SCL as output
      DDRB = 0x01;  //SDA = second bit of port B  ( 0 = input, 1 = output)
      
      // Set clock to zero to allow SDA changes
      SCL=0;
      
      // Wait half bit time to start changes on SDA
      I2C_wait(0);
      
      do
      {
          // Wait the second half bit time before raising the clock
          I2C_wait(0);
          
          // Raise clock so the SDA input can be read
          SCL=1;
          
          // Wait first half bit time to store the input value
          I2C_wait(0);
          
          // Store bit and shift left
          if (SDA){
              temp=(temp<<1)|0x01;   
          }else{
              temp<<=1;
          }
          
          // Wait second half bit time
          I2C_wait(0);
          
          // Set clock to zero
          SCL=0;
          
          // Wait first half bit time
          I2C_wait(0);
          
      }while(contador--);
      
      // Set SDA and SCL as Output  ( 0 = input, 1 = output)
      // SDA and SCL = Second and First bits respectively of Port B
      DDRB = 0x03;
      
      // Set SDA to zero by default
      SDA=0;
      
      // Wait second half bit time
      I2C_wait(0);
      
      return temp;
   }



/*****************************************************************************/
/*                     Current Address Read                     */
/***************************************************************************/

   unsigned char IIC_current_address_read (void)
   {
      unsigned char buffer;
      
      // Initiate frame
      IIC_start_bit();
      
      // Controling only one device ( A2=A1=A0=0, WB=1 )
      IIC_send_byte(0xA1);
      
      // Expect an acknowledge from slave
      if (IIC_ACK()){ error(); }
      
      // Read Byte From Slave Device
      buffer = read_byte();
      
      // Send a negative acknowledge to end communication
      IIC_NO_ACK();
      
      // End communication
      IIC_stop_bit();
   
      return buffer;
   }



/*****************************************************************************/
/*                     Random Address Read                        */
/***************************************************************************/

   unsigned char IIC_random_read (unsigned char direccion)
   {   
      /* Set random address by a write attempt */
      
      // Start Frame
      IIC_start_bit();
      
      // Controling only one device ( A2=A1=A0=0, WB=0 )
      IIC_send_byte(0xA0);
      
      // Expect an acknowledge from slave
      //if (IIC_ACK()) { error(); }
      while(IIC_ACK()){
          IR_PORT = TRUE;
          I2C_wait(1);
          
          // Initiate frame
          IIC_start_bit();
          
          // Controling only one device A2=A1=A0=0
          // Sending Control bits and default device address to zero
          IIC_send_byte(0xA0);
      }
      
      /* Set Memory address pointer to the desired address*/
      IIC_send_byte(direccion);
      
      // Expect an acknowledge from slave
      if (IIC_ACK()) { error(); }
   
      // Read current random address and return the data
      return IIC_current_address_read();
   }
   

/*****************************************************************************/
/*                     Sequential Read                           */
/***************************************************************************/

   void IIC_sequential_read (unsigned char direccion, unsigned char data[8])
   {
      volatile unsigned char i;
      
      // Start Frame
      IIC_start_bit();
      
      // Controling only one device ( A2=A1=A0=0, WB=0 )
      IIC_send_byte(0xA0);
      
      // Expect an acknowledge from slave
      //if (IIC_ACK()) { error(); }
      while(IIC_ACK()){
          IR_PORT = TRUE;
          I2C_wait(1);
          
          // Initiate frame
          IIC_start_bit();
          
          // Controling only one device A2=A1=A0=0
          // Sending Control bits and default device address to zero
          IIC_send_byte(0xA0);
      }      
      /* Set Memory address pointer to the desired address*/
      IIC_send_byte(direccion);
      
      // Expect an acknowledge from slave
      if (IIC_ACK()) { error(); }
   
      // Initiate frame requesting a read attempt
      IIC_start_bit();
      
      // Controling only one device ( A2=A1=A0=0, WB=1 )
      IIC_send_byte(0xA1);
      
      // Expect an acknowledge from slave
      if (IIC_ACK()){ error(); }
      
      /* Read Bytes */
      for (i=0; i<sizeof(data); i++)
      {
          // Read Byte From Slave
          data[i]=read_byte();
          
          // Keep sending acknowledges after all bytes but the last one
          if ( i<(sizeof(data)-1) ){
              IIC_SEND_ACK();
          }
      }
      
      /* End Communication */
      IIC_NO_ACK();
      IIC_stop_bit();
   }

