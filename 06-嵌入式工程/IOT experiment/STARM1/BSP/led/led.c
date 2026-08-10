//
// Created by 19y on 2026/4/10.
//

#include "led.h"

void led_init(void)
{
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitTypeDef gpio_initstruct;
    gpio_initstruct.Pin=GPIO_PIN_13;
    gpio_initstruct.Mode=GPIO_MODE_OUTPUT_PP;
    gpio_initstruct.Speed=GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC,&gpio_initstruct);
}

void led_test_toggle(void)
{
    HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
}