//
// Created by 19y on 2026/4/28.
//

#include "led.h"

void led_init(void)
{
    GPIO_InitTypeDef gpio_initstruct;
    gpio_initstruct.Pin=GPIO_PIN_11;
    gpio_initstruct.Mode=GPIO_MODE_OUTPUT_PP;
    gpio_initstruct.Pull=GPIO_PULLUP;
    gpio_initstruct.Speed=GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA,&gpio_initstruct);
    gpio_initstruct.Pin=GPIO_PIN_12;
    HAL_GPIO_Init(GPIOA,&gpio_initstruct);
}

void led_control(uint8_t snum,uint8_t  state)
{
    HAL_GPIO_WritePin(GPIOA,snum==1?GPIO_PIN_11:GPIO_PIN_12,!state);
}