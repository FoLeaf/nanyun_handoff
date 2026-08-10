#ifndef __MYIIC_H
#define __MYIIC_H

#include "main.h"

/* IIC Pins: PB8 SCL, PB9 SDA */
#include "stm32g0xx_ll_gpio.h"

#define IIC_SCL_PORT GPIOB
#define IIC_SCL_PIN  LL_GPIO_PIN_8
#define IIC_SDA_PORT GPIOB
#define IIC_SDA_PIN  LL_GPIO_PIN_9

#define IIC_SCL_H()  LL_GPIO_SetOutputPin(IIC_SCL_PORT, IIC_SCL_PIN)
#define IIC_SCL_L()  LL_GPIO_ResetOutputPin(IIC_SCL_PORT, IIC_SCL_PIN)

#define IIC_SDA_H()  LL_GPIO_SetOutputPin(IIC_SDA_PORT, IIC_SDA_PIN)
#define IIC_SDA_L()  LL_GPIO_ResetOutputPin(IIC_SDA_PORT, IIC_SDA_PIN)

#define IIC_SDA_READ() LL_GPIO_IsInputPinSet(IIC_SDA_PORT, IIC_SDA_PIN)

void IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
void IIC_Send_Byte(uint8_t txd);
uint8_t IIC_Read_Byte(unsigned char ack);
uint8_t IIC_Wait_Ack(void);
void IIC_Ack(void);
void IIC_NAck(void);

extern uint8_t g_iic_nop_count;

#endif /* __MYIIC_H */
