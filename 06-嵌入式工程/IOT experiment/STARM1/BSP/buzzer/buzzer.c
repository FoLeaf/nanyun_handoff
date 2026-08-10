//
// Created by 19y on 2026/4/10.
//

#include "buzzer.h"
void buzzer_init(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitTypeDef gpio_initstruct;
    gpio_initstruct.Speed=GPIO_SPEED_FREQ_LOW;
    gpio_initstruct.Pull=GPIO_NOPULL;
    gpio_initstruct.Mode=GPIO_MODE_OUTPUT_PP;
    gpio_initstruct.Pin=GPIO_PIN_13;
    HAL_GPIO_Init(GPIOB,&gpio_initstruct);
}

void buzzer_on(void)
{
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_SET);
}

void buzzer_off(void)
{
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_RESET);
}

void buzzer_beep(void)
{
    HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_13);
}