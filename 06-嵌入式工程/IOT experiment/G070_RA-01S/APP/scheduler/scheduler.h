//
// Created by 19y on 2026/4/28.
//

#ifndef G070_RA_01S_SCHEDULER_H
#define G070_RA_01S_SCHEDULER_H
#include "main.h"

typedef struct
{
    void (*task)(void);
    uint32_t period;
    uint32_t lastTick;
    uint8_t isExec;
}task_t;

void scheduler_init(void);
void scheduler_run(void);

#endif //G070_RA_01S_SCHEDULER_H
