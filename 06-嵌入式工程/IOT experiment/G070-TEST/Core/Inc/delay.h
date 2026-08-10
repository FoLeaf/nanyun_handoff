#ifndef __DELAY_H
#define __DELAY_H

#include "main.h"

/**
  * @brief  Microsecond delay (SysTick-based, ~1us resolution)
  * @param  us: delay in microseconds (max ~1000000 = 1s)
  */
void delay_us(uint32_t us);

/**
  * @brief  Millisecond delay (wrapper around HAL_Delay)
  * @param  ms: delay in milliseconds
  */
void delay_ms(uint32_t ms);

#endif /* __DELAY_H */
