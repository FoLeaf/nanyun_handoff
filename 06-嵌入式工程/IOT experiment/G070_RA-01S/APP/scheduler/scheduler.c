//
// Created by 19y on 2026/4/28.
//

#include "scheduler.h"

#include "input_capture.h"
#include "key.h"
#include "led.h"
#include "oled.h"
#include "voltmeasure.h"
#include "monitor.h"
#include "ra01s_test.h"

void led_task(void)
{
    if (g_key_val != 0)
    {
        if (g_key_val == KEY1) led_control(1, 1);
        else if (g_key_val == KEY2) led_control(1, 0);
        else if (g_key_val == KEY3) led_control(2, 1);
        else if (g_key_val == KEY4) led_control(2, 0);

        g_key_val = 0;
    }
}

task_t tasks[] =
{
    {key_task, 5},
    {led_task, 20},
#if LORA_TEST_IS_TX_NODE
    {monitor_task, 100},
#endif
    {RA01S_Test_Loop, 100},
};

uint8_t taskNum = 0;

void scheduler_init(void)
{
    taskNum = sizeof(tasks) / sizeof(task_t);
#if LORA_TEST_IS_TX_NODE
    OLED_Init();
#endif
    key_init();
    led_init();
    voltmeasure_init();
    input_capture_init();
}

void scheduler_run(void)
{
    static uint32_t currentTick;
    for (uint8_t i = 0; i < taskNum; i++)
    {
        currentTick = uwTick;
        if (currentTick - tasks[i].lastTick >= tasks[i].period || tasks[i].isExec == 0)
        {
            tasks[i].task();
            tasks[i].lastTick = currentTick;
            tasks[i].isExec = 1;
        }
    }
}
