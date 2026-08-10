#ifndef __CS100A_H
#define __CS100A_H

#include "main.h"

/* CS100A Pin Definitions (adapt to your wiring) */
#define CS100A_TRIG_PORT    GPIOA
#define CS100A_TRIG_PIN     GPIO_PIN_8

#define CS100A_ECHO_PORT    GPIOB
#define CS100A_ECHO_PIN     GPIO_PIN_0

/* Negative return values from CS100A_Get() */
#define CS100A_ERR_NO_RISING_EDGE   (-1.0f)
#define CS100A_ERR_ECHO_TIMEOUT     (-2.0f)
#define CS100A_ERR_ECHO_STUCK_HIGH  (-3.0f)

typedef struct
{
    GPIO_PinState echo_before;
    GPIO_PinState trig_high_readback;
    GPIO_PinState echo_during_trig;
    GPIO_PinState trig_low_readback;
    GPIO_PinState echo_after_wait;
    GPIO_PinState echo_pullup_test;
    GPIO_PinState echo_pulldown_test;
} CS100A_DebugInfo;

typedef struct
{
    GPIO_PinState trig_high_readback;
    GPIO_PinState echo_when_trig_high;
    GPIO_PinState trig_low_readback;
    GPIO_PinState echo_when_trig_low;
} CS100A_PinTestInfo;

/**
  * @brief  Initialize CS100A GPIO pins (TRIG output, ECHO input).
  */
void CS100A_Init(void);

/**
  * @brief  Initialize TIM for CS100A echo pulse measurement.
  */
void CS100A_TIM_Init(void);

/**
  * @brief  Read distance from CS100A.
  * @retval Distance in cm, or a CS100A_ERR_* value on failure.
  */
float CS100A_Get(void);

/**
  * @brief  Get pin states captured during the last CS100A_Get() call.
  */
void CS100A_GetLastDebug(CS100A_DebugInfo *debug);

/**
  * @brief  Drive TRIG high/low and read ECHO. Used to verify a PA8->PB0 jumper.
  */
void CS100A_RunPinTest(CS100A_PinTestInfo *test);

#endif /* __CS100A_H */
