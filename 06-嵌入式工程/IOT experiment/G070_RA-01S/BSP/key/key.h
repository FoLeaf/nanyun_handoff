#ifndef _KEY_H
#define _KEY_H

#include "main.h"

#define KEY1 1
#define KEY2 2
#define KEY3 3
#define KEY4 4

extern uint8_t g_key_val;

void key_init(void);
void key_task(void);

#endif /* _KEY_H */
