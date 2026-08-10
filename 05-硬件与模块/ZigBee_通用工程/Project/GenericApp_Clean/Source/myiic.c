#include "myiic.h"

static void SDA_OUT(void) {
  // Emulated open-drain. Handled directly in IIC_SDA_H/L macros.
}

static void SDA_IN(void) {
  IIC_SDA_H(); // Release SDA to input/Hi-Z
}

void IIC_Init(void) {
  P0SEL &= ~0x03; // P0_0 and P0_1 as GPIO
  P0DIR &= ~0x03; // Set as input (Hi-Z)
  P0INP &= ~0x03; // Enable pull-up/down for P0_0 and P0_1
  P2INP &= ~0x20; // Select pull-up for Port 0
  P0_0 = 0;       // Prepare output latch to 0
  P0_1 = 0;       // Prepare output latch to 0
}

void IIC_Start(void) {
  SDA_OUT();
  IIC_SDA_H();
  IIC_Delay();
  IIC_SCL_H();
  IIC_Delay();
  IIC_SDA_L();
  IIC_Delay();
  IIC_SCL_L();
  IIC_Delay();
}

void IIC_Stop(void) {
  SDA_OUT();
  IIC_SCL_L();
  IIC_SDA_L();
  IIC_Delay();
  IIC_SCL_H();
  IIC_Delay();
  IIC_SDA_H();
  IIC_Delay();
}

uint8 IIC_Wait_Ack(void) {
  uint8 ucErrTime = 0;
  SDA_IN();
  IIC_SDA_H();
  IIC_Delay();
  IIC_SCL_H();
  IIC_Delay();
  while (IIC_SDA_READ()) {
    ucErrTime++;
    if (ucErrTime > 250) {
      IIC_Stop();
      return 1;
    }
  }
  IIC_SCL_L();
  return 0;
}

void IIC_Ack(void) {
  IIC_SCL_L();
  SDA_OUT();
  IIC_SDA_L();
  IIC_Delay();
  IIC_SCL_H();
  IIC_Delay();
  IIC_SCL_L();
}

void IIC_NAck(void) {
  IIC_SCL_L();
  SDA_OUT();
  IIC_SDA_H();
  IIC_Delay();
  IIC_SCL_H();
  IIC_Delay();
  IIC_SCL_L();
}

void IIC_Send_Byte(uint8 txd) {
  uint8 t;
  SDA_OUT();
  IIC_SCL_L();
  for (t = 0; t < 8; t++) {
    if ((txd & 0x80) >> 7)
      IIC_SDA_H();
    else
      IIC_SDA_L();
    txd <<= 1;
    IIC_Delay();
    IIC_SCL_H();
    IIC_Delay();
    IIC_SCL_L();
    IIC_Delay();
  }
}

uint8 IIC_Read_Byte(unsigned char ack) {
  uint8 i, receive = 0;
  SDA_IN();
  for (i = 0; i < 8; i++) {
    IIC_SCL_L();
    IIC_Delay();
    IIC_SCL_H();
    receive <<= 1;
    if (IIC_SDA_READ())
      receive++;
    IIC_Delay();
  }
  if (!ack)
    IIC_NAck();
  else
    IIC_Ack();
  return receive;
}
