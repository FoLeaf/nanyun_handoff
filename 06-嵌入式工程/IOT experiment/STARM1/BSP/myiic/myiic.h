#ifndef __MYIIC_H
#define __MYIIC_H

#include "delay.h"
#include "main.h"

/* I2C IO Configuration */
#define IIC_SCL_PORT GPIOB
#define IIC_SCL_PIN GPIO_PIN_8
#define IIC_SCL_PORTCLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()

#define IIC_SDA_PORT GPIOB
#define IIC_SDA_PIN GPIO_PIN_9
#define IIC_SDA_PORTCLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
/* I2C IO Operations */
#define IIC_SCL(x)                                                             \
  do {                                                                         \
    x ? HAL_GPIO_WritePin(IIC_SCL_PORT, IIC_SCL_PIN, GPIO_PIN_SET)             \
      : HAL_GPIO_WritePin(IIC_SCL_PORT, IIC_SCL_PIN, GPIO_PIN_RESET);          \
  } while (0)

#define IIC_SDA(x)                                                             \
  do {                                                                         \
    x ? HAL_GPIO_WritePin(IIC_SDA_PORT, IIC_SDA_PIN, GPIO_PIN_SET)             \
      : HAL_GPIO_WritePin(IIC_SDA_PORT, IIC_SDA_PIN, GPIO_PIN_RESET);          \
  } while (0)

#define IIC_READ_SDA HAL_GPIO_ReadPin(IIC_SDA_PORT, IIC_SDA_PIN)

/* I2C Operations */
void IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
void IIC_Send_Byte(uint8_t txd);
uint8_t IIC_Read_Byte(uint8_t ack);
uint8_t IIC_Wait_Ack(void);
void IIC_Ack(void);
void IIC_NAck(void);
uint8_t IIC_Write_Bytes(uint8_t dev_addr, uint8_t *buf, uint8_t len);
uint8_t IIC_Read_Bytes(uint8_t dev_addr, uint8_t *buf, uint8_t len);

#endif /* __MYIIC_H */
