//
// Created by 19y on 2026/4/22.
//

#include "motor.h"

void motor_init(void)
{
    GPIO_InitTypeDef gpio_initstruct;
    gpio_initstruct.Pin=GPIO_PIN_8|GPIO_PIN_9;
    gpio_initstruct.Mode=GPIO_MODE_OUTPUT_PP;
    gpio_initstruct.Pull=GPIO_NOPULL;
    gpio_initstruct.Speed=GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB,&gpio_initstruct);
}

void motor_front(void)
{
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9, GPIO_PIN_RESET);
}

void motor_back(void)
{
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_SET);
}

void motor_stop(void)
{
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_SET);
}
