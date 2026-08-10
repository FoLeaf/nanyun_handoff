#ifndef __MYSPI_H
#define __MYSPI_H

#include "main.h"
#include "stm32f1xx_ll_gpio.h"
#include <stdint.h>

/* ========================================== */
/*           SPI 硬件引脚配置宏               */
/* ========================================== */

// 时钟使能定义
#define MYSPI_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()

// SCK (SPI Clock) - 输出
#define MYSPI_SCK_PORT GPIOB
#define MYSPI_SCK_PIN GPIO_PIN_14
#define MYSPI_SCK_PIN_LL LL_GPIO_PIN_14
#define MYSPI_SCK(x)                                                           \
  do {                                                                         \
    x ? LL_GPIO_SetOutputPin(MYSPI_SCK_PORT, MYSPI_SCK_PIN_LL)                 \
      : LL_GPIO_ResetOutputPin(MYSPI_SCK_PORT, MYSPI_SCK_PIN_LL);              \
  } while (0)

// MOSI (Master Out Slave In) - 输出
#define MYSPI_MOSI_PORT GPIOB
#define MYSPI_MOSI_PIN GPIO_PIN_13
#define MYSPI_MOSI_PIN_LL LL_GPIO_PIN_13
#define MYSPI_MOSI(x)                                                          \
  do {                                                                         \
    x ? LL_GPIO_SetOutputPin(MYSPI_MOSI_PORT, MYSPI_MOSI_PIN_LL)               \
      : LL_GPIO_ResetOutputPin(MYSPI_MOSI_PORT, MYSPI_MOSI_PIN_LL);            \
  } while (0)

// MISO (Master In Slave Out) - 输入
#define MYSPI_MISO_PORT GPIOB
#define MYSPI_MISO_PIN GPIO_PIN_15
#define MYSPI_MISO_PIN_LL LL_GPIO_PIN_15
#define MYSPI_MISO_READ()                                                      \
  (LL_GPIO_IsInputPinSet(MYSPI_MISO_PORT, MYSPI_MISO_PIN_LL) ? 1 : 0)

/* ========================================== */
/*                API 接口函数                */
/* ========================================== */

void MYSPI_Init(void);
void MYSPI_Delay(void);
void MYSPI_SendByte(uint8_t byte);
uint8_t MYSPI_ReadByte(void);
uint8_t MYSPI_ReadWriteByte(uint8_t tx_data);

#endif /* __MYSPI_H */
