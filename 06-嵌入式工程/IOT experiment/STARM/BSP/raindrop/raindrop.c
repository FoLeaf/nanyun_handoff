//
// Created by 19y on 2026/4/17.
//

#include "raindrop.h"
#include "oled.h"
#include "scheduler.h"

void raindrop_init(void)
{
    RAINDROP_GPIO_CLK_ENABLE();
    GPIO_InitTypeDef gpio_initstruct;
    gpio_initstruct.Mode = GPIO_MODE_INPUT;
    gpio_initstruct.Pin = RAINDROP_DO_PIN;
    gpio_initstruct.Pull = GPIO_NOPULL;
    gpio_initstruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(RAINDROP_DO_PORT,&gpio_initstruct);
}

uint8_t raindrop_get_state(void)
{
    return RAINDROP_READ()?RAINDROP_TRUE:RAINDROP_FALSE;
}

void raindrop_test(void)
{
    OLED_ShowSTR(0,0,"Raindrop Test",8);
    OLED_ShowNUM(0,2,RAINDROP_READ(),1,8);
}
