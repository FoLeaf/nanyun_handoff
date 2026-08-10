//
// Created by 19y on 2026/4/10.
//

#ifndef STARM_SCHEDULER_H
#define STARM_SCHEDULER_H
#include "main.h"
#include "buzzer.h"
#include "led.h"


#define RAIN

typedef struct Task
{
    void (*task_func)(void);
    uint32_t period;
    uint32_t ticks;
    uint8_t is_exec;
}task_t;

void scheduler_init(void);
void scheduler_run(void);
#endif //STARM_SCHEDULER_H
