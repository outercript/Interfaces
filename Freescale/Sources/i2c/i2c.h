#include <hidef.h>
#define SCL PORTB_PB0
#define SDA PORTB_PB1
#define I2C_FULL_TIME 145
#define I2C_HALF_TIME 80

extern Bool sendingBit;
extern Bool startBit;
extern Bool stopBit;
extern Bool delayFlag;
extern Bool ErrorFlag;
extern unsigned char IIC_DELAY;

void Setup_I2C(void);
void error(void);
void I2C_wait(Bool IS_FULL_Time);
void IIC_start_bit (void);
void IIC_stop_bit (void);
void IIC_send_byte (unsigned char dato);
Bool IIC_ACK (void);
void IIC_NO_ACK (void);
void IIC_SEND_ACK (void);
void IIC_byte_write (unsigned char direccion, unsigned char dato); 
void IIC_page_write (unsigned char direccion, unsigned char datos[8]);
unsigned char read_byte (void);
unsigned char IIC_current_address_read (void);
unsigned char IIC_random_read (unsigned char direccion);
void IIC_sequential_read (unsigned char direccion, unsigned char data[8]);