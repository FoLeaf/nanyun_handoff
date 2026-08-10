#include "cs100a.h"
#include "delay.h"
#include <string.h>

static TIM_HandleTypeDef htim_cs100a;
static CS100A_DebugInfo cs100a_debug;

/* Adjust error coefficient as needed */
static const float err = 1.0f;

#define CS100A_TIMER_FREQ_HZ      1000000U
#define CS100A_WAIT_IDLE_MS       30U
#define CS100A_WAIT_RISING_MS     60U
#define CS100A_ECHO_TIMEOUT_US    40000U
#define CS100A_SOUND_CM_PER_US    0.0343f

static void CS100A_ConfigEchoPull(uint32_t pull)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = CS100A_ECHO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = pull;
    HAL_GPIO_Init(CS100A_ECHO_PORT, &GPIO_InitStruct);
}

static GPIO_PinState CS100A_ReadEchoWithPull(uint32_t pull)
{
    CS100A_ConfigEchoPull(pull);
    delay_us(20);
    return HAL_GPIO_ReadPin(CS100A_ECHO_PORT, CS100A_ECHO_PIN);
}

static void CS100A_CaptureEchoPullTest(void)
{
    cs100a_debug.echo_pullup_test = CS100A_ReadEchoWithPull(GPIO_PULLUP);
    cs100a_debug.echo_pulldown_test = CS100A_ReadEchoWithPull(GPIO_PULLDOWN);
    CS100A_ConfigEchoPull(GPIO_NOPULL);
}

void CS100A_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* TRIG pin: PA8, push-pull output, default low */
    GPIO_InitStruct.Pin   = CS100A_TRIG_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(CS100A_TRIG_PORT, &GPIO_InitStruct);
    HAL_GPIO_WritePin(CS100A_TRIG_PORT, CS100A_TRIG_PIN, GPIO_PIN_RESET);

    /* ECHO pin: PB0, idle low */
    GPIO_InitStruct.Pin  = CS100A_ECHO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(CS100A_ECHO_PORT, &GPIO_InitStruct);
}

/**
  * @brief  TIM3 init for CS100A echo measurement at 1us resolution.
  */
void CS100A_TIM_Init(void)
{
    __HAL_RCC_TIM3_CLK_ENABLE();

    uint32_t pclk1 = HAL_RCC_GetPCLK1Freq();
    uint32_t prescaler = (pclk1 / CS100A_TIMER_FREQ_HZ);
    if (prescaler == 0U)
    {
        prescaler = 1U;
    }

    htim_cs100a.Instance               = TIM3;
    htim_cs100a.Init.Prescaler         = prescaler - 1U;
    htim_cs100a.Init.Period            = 0xFFFFU;
    htim_cs100a.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    htim_cs100a.Init.CounterMode       = TIM_COUNTERMODE_UP;
    htim_cs100a.Init.RepetitionCounter = 0;
    htim_cs100a.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_Base_Init(&htim_cs100a) != HAL_OK)
    {
        while (1);
    }
}

float CS100A_Get(void)
{
    uint32_t tick_start;
    uint32_t pulse_us;

    memset(&cs100a_debug, 0, sizeof(cs100a_debug));
    HAL_TIM_Base_Stop(&htim_cs100a);
    __HAL_TIM_SET_COUNTER(&htim_cs100a, 0);

    cs100a_debug.echo_before = HAL_GPIO_ReadPin(CS100A_ECHO_PORT, CS100A_ECHO_PIN);
    tick_start = HAL_GetTick();
    while (HAL_GPIO_ReadPin(CS100A_ECHO_PORT, CS100A_ECHO_PIN) == GPIO_PIN_SET)
    {
        if ((HAL_GetTick() - tick_start) >= CS100A_WAIT_IDLE_MS)
        {
            return CS100A_ERR_ECHO_STUCK_HIGH;
        }
    }

    /* 1. Send TRIG pulse (>10us) */
    HAL_GPIO_WritePin(CS100A_TRIG_PORT, CS100A_TRIG_PIN, GPIO_PIN_SET);
    delay_us(50);
    cs100a_debug.trig_high_readback = HAL_GPIO_ReadPin(CS100A_TRIG_PORT, CS100A_TRIG_PIN);
    cs100a_debug.echo_during_trig = HAL_GPIO_ReadPin(CS100A_ECHO_PORT, CS100A_ECHO_PIN);
    HAL_GPIO_WritePin(CS100A_TRIG_PORT, CS100A_TRIG_PIN, GPIO_PIN_RESET);
    cs100a_debug.trig_low_readback = HAL_GPIO_ReadPin(CS100A_TRIG_PORT, CS100A_TRIG_PIN);

    /* 2. Wait for ECHO to go HIGH */
    tick_start = HAL_GetTick();
    while (HAL_GPIO_ReadPin(CS100A_ECHO_PORT, CS100A_ECHO_PIN) == GPIO_PIN_RESET)
    {
        if ((HAL_GetTick() - tick_start) >= CS100A_WAIT_RISING_MS)
        {
            cs100a_debug.echo_after_wait = HAL_GPIO_ReadPin(CS100A_ECHO_PORT, CS100A_ECHO_PIN);
            CS100A_CaptureEchoPullTest();
            return CS100A_ERR_NO_RISING_EDGE;
        }
    }
    cs100a_debug.echo_after_wait = GPIO_PIN_SET;

    /* 3. Measure the ECHO high pulse width in microseconds */
    __HAL_TIM_SET_COUNTER(&htim_cs100a, 0);
    if (HAL_TIM_Base_Start(&htim_cs100a) != HAL_OK)
    {
        return CS100A_ERR_ECHO_TIMEOUT;
    }

    /* 4. Wait for ECHO to go LOW */
    while (HAL_GPIO_ReadPin(CS100A_ECHO_PORT, CS100A_ECHO_PIN) == GPIO_PIN_SET)
    {
        if (__HAL_TIM_GET_COUNTER(&htim_cs100a) >= CS100A_ECHO_TIMEOUT_US)
        {
            HAL_TIM_Base_Stop(&htim_cs100a);
            return CS100A_ERR_ECHO_TIMEOUT;
        }
    }

    /* 5. Stop TIM */
    pulse_us = __HAL_TIM_GET_COUNTER(&htim_cs100a);
    HAL_TIM_Base_Stop(&htim_cs100a);

    /* distance = pulse width * sound speed / round trip */
    float distance = pulse_us * CS100A_SOUND_CM_PER_US / 2.0f * err;

    return distance;
}

void CS100A_GetLastDebug(CS100A_DebugInfo *debug)
{
    if (debug == NULL)
    {
        return;
    }

    *debug = cs100a_debug;
}

void CS100A_RunPinTest(CS100A_PinTestInfo *test)
{
    if (test == NULL)
    {
        return;
    }

    HAL_GPIO_WritePin(CS100A_TRIG_PORT, CS100A_TRIG_PIN, GPIO_PIN_SET);
    delay_us(2000);
    test->trig_high_readback = HAL_GPIO_ReadPin(CS100A_TRIG_PORT, CS100A_TRIG_PIN);
    test->echo_when_trig_high = HAL_GPIO_ReadPin(CS100A_ECHO_PORT, CS100A_ECHO_PIN);

    HAL_GPIO_WritePin(CS100A_TRIG_PORT, CS100A_TRIG_PIN, GPIO_PIN_RESET);
    delay_us(2000);
    test->trig_low_readback = HAL_GPIO_ReadPin(CS100A_TRIG_PORT, CS100A_TRIG_PIN);
    test->echo_when_trig_low = HAL_GPIO_ReadPin(CS100A_ECHO_PORT, CS100A_ECHO_PIN);
}
