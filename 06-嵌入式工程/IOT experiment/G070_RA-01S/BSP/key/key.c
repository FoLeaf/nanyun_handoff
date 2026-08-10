#include "key.h"
#include "multi_button.h"
#include "gpio.h"

uint8_t g_key_val = 0;

static Button btn1;
static Button btn2;
static Button btn3;
static Button btn4;

static uint8_t read_button_pin(uint8_t button_id)
{
    switch(button_id)
    {
        case KEY1: return HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_0);
        case KEY2: return HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_1);
        case KEY3: return HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_2);
        case KEY4: return HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_3);
        default: return 0;
    }
}

static void btn_press_down_cb(Button *btn, void *user_data)
{
    g_key_val = btn->button_id;
}

void key_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Enable GPIOD Clock */
    __HAL_RCC_GPIOD_CLK_ENABLE();

    /* Configure GPIO pins: PD0, PD1, PD2, PD3 */
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* Initialize buttons with active low level (0) */
    button_init(&btn1, read_button_pin, 0, KEY1);
    button_init(&btn2, read_button_pin, 0, KEY2);
    button_init(&btn3, read_button_pin, 0, KEY3);
    button_init(&btn4, read_button_pin, 0, KEY4);

    /* Attach events */
    button_attach(&btn1, BTN_PRESS_DOWN, btn_press_down_cb, NULL);
    button_attach(&btn2, BTN_PRESS_DOWN, btn_press_down_cb, NULL);
    button_attach(&btn3, BTN_PRESS_DOWN, btn_press_down_cb, NULL);
    button_attach(&btn4, BTN_PRESS_DOWN, btn_press_down_cb, NULL);

    /* Start buttons */
    button_start(&btn1);
    button_start(&btn2);
    button_start(&btn3);
    button_start(&btn4);
}

void key_task(void)
{
    button_ticks();
}
