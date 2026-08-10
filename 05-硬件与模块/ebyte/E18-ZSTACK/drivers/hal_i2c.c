#include "hal_i2c.h"

#define SDA P0_0
#define SCL P0_1

/* 
   CC2530 GPIO Configuration:
   P0SEL: 0 = General Purpose I/O, 1 = Peripheral Function
   P0DIR: 0 = Input, 1 = Output
   P0INP: 0 = Pull-up/Pull-down, 1 = Tri-state
*/

static void I2C_Delay(void)
{
    unsigned char i = 20; 
    while(i--);
}

void I2C_Init(void)
{
    P0SEL &= ~0x03; // P0.0, P0.1 as GPIO
    P0DIR |= 0x03;  // P0.0, P0.1 as Output
    // P0INP is handled by P2INP for port 0 in some versions, 
    // but here we have external pull-ups (R12, R13), so we just need Output or Input.
    
    SDA = 1;
    SCL = 1;
}

static void SDA_Mode(unsigned char out)
{
    if(out) P0DIR |= 0x01;  // Output
    else    P0DIR &= ~0x01; // Input
}

void I2C_Start(void)
{
    SDA_Mode(1);
    SDA = 1;
    SCL = 1;
    I2C_Delay();
    SDA = 0;
    I2C_Delay();
    SCL = 0;
}

void I2C_Stop(void)
{
    SDA_Mode(1);
    SDA = 0;
    SCL = 1;
    I2C_Delay();
    SDA = 1;
    I2C_Delay();
}

void I2C_SendByte(unsigned char dat)
{
    unsigned char i;
    SDA_Mode(1);
    for(i=0; i<8; i++)
    {
        SCL = 0;
        if(dat & 0x80) SDA = 1;
        else           SDA = 0;
        dat <<= 1;
        I2C_Delay();
        SCL = 1;
        I2C_Delay();
    }
    SCL = 0;
}

unsigned char I2C_WaitAck(void)
{
    unsigned char ack;
    SDA_Mode(0);
    SCL = 1;
    I2C_Delay();
    if(SDA) ack = 1;
    else    ack = 0;
    SCL = 0;
    I2C_Delay();
    return ack;
}

unsigned char I2C_ReadByte(unsigned char ack)
{
    unsigned char i, dat = 0;
    SDA_Mode(0);
    for(i=0; i<8; i++)
    {
        SCL = 0;
        I2C_Delay();
        SCL = 1;
        dat <<= 1;
        if(SDA) dat |= 0x01;
        I2C_Delay();
    }
    SCL = 0;
    SDA_Mode(1);
    if(ack) SDA = 0;
    else    SDA = 1;
    I2C_Delay();
    SCL = 1;
    I2C_Delay();
    SCL = 0;
    return dat;
}
