#ifndef __BSP_H
#define __BSP_H

#include "main.h"

/* LED Pins */
#define LED1_PORT GPIOA
#define LED1_PIN  GPIO_PIN_11
#define LED2_PORT GPIOA
#define LED2_PIN  GPIO_PIN_12

#define LED1_ON()  HAL_GPIO_WritePin(LED1_PORT, LED1_PIN, GPIO_PIN_SET)
#define LED1_OFF() HAL_GPIO_WritePin(LED1_PORT, LED1_PIN, GPIO_PIN_RESET)
#define LED2_ON()  HAL_GPIO_WritePin(LED2_PORT, LED2_PIN, GPIO_PIN_SET)
#define LED2_OFF() HAL_GPIO_WritePin(LED2_PORT, LED2_PIN, GPIO_PIN_RESET)

/* Switch Pins */
#define SW1_PORT GPIOD
#define SW1_PIN  GPIO_PIN_0
#define SW2_PORT GPIOD
#define SW2_PIN  GPIO_PIN_1
#define SW3_PORT GPIOD
#define SW3_PIN  GPIO_PIN_2
#define SW4_PORT GPIOD
#define SW4_PIN  GPIO_PIN_3

#define READ_SW1() HAL_GPIO_ReadPin(SW1_PORT, SW1_PIN)
#define READ_SW2() HAL_GPIO_ReadPin(SW2_PORT, SW2_PIN)
#define READ_SW3() HAL_GPIO_ReadPin(SW3_PORT, SW3_PIN)
#define READ_SW4() HAL_GPIO_ReadPin(SW4_PORT, SW4_PIN)

/* LoRa Control Pins */
#define LORA_RESET_PORT GPIOB
#define LORA_RESET_PIN  GPIO_PIN_0
#define LORA_BUSY_PORT  GPIOB
#define LORA_BUSY_PIN   GPIO_PIN_1
#define LORA_DIO1_PORT  GPIOB
#define LORA_DIO1_PIN   GPIO_PIN_2
#define LORA_TXEN_PORT  GPIOB
#define LORA_TXEN_PIN   GPIO_PIN_13
#define LORA_RXEN_PORT  GPIOB
#define LORA_RXEN_PIN   GPIO_PIN_14

void BSP_Init(void);

#endif /* __BSP_H */
