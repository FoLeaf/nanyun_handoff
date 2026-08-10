#ifndef __MYIIC_H
#define __MYIIC_H

#include <ioCC2530.h>
#include "hal_types.h"

// I2C bit-bang delay count. CPU @ 32MHz, each NOP = 31.25ns.
// IIC_Delay() = 10*N NOPs per transition, inlined (no function call overhead).
// With CNT=0: raw GPIO speed (~2.6-4MHz SCL) — may exceed AHT30/SSD1306 spec.
// Recommended: CNT=1 (~1MHz SCL) for AHT30, CNT=2 (~645kHz) for safety margin.
#ifndef IIC_DELAY_CNT
#define IIC_DELAY_CNT 1
#endif

#define IIC_Delay() do {                     \
  volatile uint8 _i = 10 * IIC_DELAY_CNT;    \
  while (_i--) { asm("NOP"); }               \
} while(0)

// IIC Pins: P0_1 SCL, P0_0 SDA
// Emulate open-drain by toggling P0DIR (input for HIGH/Hi-Z, output for LOW)
#define IIC_SCL_H()  do { P0DIR &= ~0x02; } while(0)
#define IIC_SCL_L()  do { P0_1 = 0; P0DIR |= 0x02; } while(0)

#define IIC_SDA_H()  do { P0DIR &= ~0x01; } while(0)
#define IIC_SDA_L()  do { P0_0 = 0; P0DIR |= 0x01; } while(0)

#define IIC_SDA_READ() P0_0

void IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
void IIC_Send_Byte(uint8 txd);
uint8 IIC_Read_Byte(unsigned char ack);
uint8 IIC_Wait_Ack(void);
void IIC_Ack(void);
void IIC_NAck(void);

#endif /* __MYIIC_H */
