#include "delay.h"

/**
  * @brief  Microsecond delay using inline assembly NOP loop.
  *         At 64MHz, 1us = 64 cycles.
  *         Each loop iteration: decrement(1) + branch(1) + NOP(1) = 3 cycles.
  *         64/3 ≈ 21 iterations per us.
  * @param  us: delay in microseconds
  */
void delay_us(uint32_t us)
{
    /* 64MHz: 21 iterations per us */
    uint32_t count = us * 21;
    do {
        __nop();
    } while (--count);
}

void delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
}
