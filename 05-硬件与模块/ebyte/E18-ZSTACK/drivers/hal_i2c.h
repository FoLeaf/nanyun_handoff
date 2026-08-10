#ifndef HAL_I2C_H
#define HAL_I2C_H

#include <ioCC2530.h>

/* Pin Definitions based on Netlist */
/* P0.0 -> SDA, P0.1 -> SCL */

void I2C_Init(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_SendByte(unsigned char dat);
unsigned char I2C_ReadByte(unsigned char ack);
unsigned char I2C_WaitAck(void);

#endif /* HAL_I2C_H */
