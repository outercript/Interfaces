#include <hidef.h>
#define SCL PORTA_PA6
#define SDA PORTA_PA7
#define I2C_FULL_TIME 160
#define I2C_HALF_TIME 80

extern Bool sendingBit;
extern Bool startBit;
extern Bool stopBit;
extern Bool delayFlag;

void Setup_I2C(void);
void I2C_wait(Bool IS_FULL_Time);
void IIC_start_bit (void);
void IIC_stop_bit (void);
void IIC_send_byte (unsigned char dato);
Bool IIC_ACK (void);
void IIC_NO_ACK (void);
void IIC_SEND_ACK (void);
void IIC_byte_write (unsigned int direccion, unsigned char dato); 
void IIC_page_write (unsigned int direccion, unsigned char datos[32]);
unsigned char read_byte (void);
unsigned char IIC_current_address_read (unsigned int direccion);
unsigned char IIC_random_read (unsigned int direccion);
void IIC_sequential_read (unsigned int direccion, unsigned char *data);