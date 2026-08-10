#include "myiic.h"
#include "delay.h"

uint8_t g_iic_nop_count = 1; // ~188kHz I2C at 64MHz, within SSD1306 400kHz max

static inline void IIC_Delay(void) {
  for (uint8_t i = 0; i < g_iic_nop_count; i++) {
      __NOP();
  }
}

static void SDA_OUT(void) {
  /*
   * STM32在开漏输出(OD)模式下天然支持双向操作。
   * 输出1时，引脚呈现高阻态，此时可以直接读取外部电平。
   * 因此不需要频繁调用极度耗时的 HAL_GPIO_Init 来回切换模式。
   */
}

static void SDA_IN(void) {
  /* OD模式下，拉高SDA释放总线后即可读取外部电平，无需切换GPIO模式 */
  IIC_SDA_H();
}

void IIC_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOB_CLK_ENABLE();

  // init both SCL and SDA as OD with Pull-up
  GPIO_InitStruct.Pin = IIC_SCL_PIN | IIC_SDA_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GPIOB, IIC_SCL_PIN | IIC_SDA_PIN, GPIO_PIN_SET);
}

void IIC_Start(void) {
  SDA_OUT();
  IIC_SDA_H();
  IIC_SCL_H();
  IIC_Delay();
  IIC_SDA_L();
  IIC_Delay();
  IIC_SCL_L();
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

uint8_t IIC_Wait_Ack(void) {
  uint8_t ucErrTime = 0;
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

void IIC_Send_Byte(uint8_t txd) {
  uint8_t t;
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

uint8_t IIC_Read_Byte(unsigned char ack) {
  uint8_t i, receive = 0;
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
