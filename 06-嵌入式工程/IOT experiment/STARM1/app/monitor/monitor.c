//
// Created by 19y on 2026/4/10.
//

#include "monitor.h"

#include "key.h"

void monitor_task(void)
{
    OLED_ShowSTR(0, 2, "SW2:", 8);
    OLED_ShowSTR(40, 2, key_get_state(SW2) == KEY_DOWN ? "DOWN" : "UP  ", 8);
}
