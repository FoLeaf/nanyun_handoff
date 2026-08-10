//
// Created by 19y on 2026/4/10.
//

#include "scheduler.h"
#include "key.h"
#include "monitor.h"
#include "raindrop.h"
#include "rc522_test.h"
#include "ra01_test.h"
uint8_t taskNum=0;
task_t tasks[]=
        {
                // {AHT30_Test_Run,100},
                // {RC522_Test_Run,200},
                // {key_scan, 10},
                // {led_test_toggle, 1000},
                // {buzzer_beep,1000},
                // {monitor_task,100},
                // {raindrop_test,100},
                {RA01_Test_Periodic, 10}
        };
void scheduler_init(void)
{
    RA01_Test_Init();
    taskNum=sizeof(tasks)/sizeof(tasks[0]);
}

void scheduler_run(void)
{
    static uint32_t scheduler_ticks=0;
    for (int i = 0; i < taskNum; ++i)
    {
        scheduler_ticks=HAL_GetTick();
        if(scheduler_ticks>=tasks[i].ticks+tasks[i].period||tasks[i].is_exec==0)
        {
            tasks[i].task_func();
            tasks[i].ticks=scheduler_ticks;
            tasks[i].is_exec=1;
        }
    }
}
