//
// Created by 19y on 2026/4/17.
//

#ifndef STARM_RAINDROP_H
#define STARM_RAINDROP_H
#include "main.h"
#define RAINDROP_TRUE 0
#define RAINDROP_FALSE 1
#define RAINDROP_GPIO_CLK_ENABLE()                                                \
  do {                                                                         \
    __HAL_RCC_GPIOB_CLK_ENABLE();                                           \
  } while (0)

#define RAINDROP_DO_PORT GPIOB
#define RAINDROP_DO_PIN GPIO_PIN_1
#define RAINDROP_READ() ((RAINDROP_DO_PORT->IDR & RAINDROP_DO_PIN) ? 1 : 0)

void raindrop_test(void);
void raindrop_init(void);
uint8_t raindrop_get_state(void);

#endif //STARM_RAINDROP_H
