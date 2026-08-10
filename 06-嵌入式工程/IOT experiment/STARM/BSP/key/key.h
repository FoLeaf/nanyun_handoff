//
// Created by 19y on 2026/4/10.
//

#ifndef STARM_KEY_H
#define STARM_KEY_H

#include "main.h"

typedef enum {
    KEY_NONE = 0,
    SW1, SW2, SW3, SW4, SW5, SW6, SW7, SW8, SW9,
    KEY_MAX
} key_id_t;

typedef enum {
    KEY_UP = 0,
    KEY_DOWN,
} key_state_t;

void key_init(void);
void key_scan(void);
key_state_t key_get_state(key_id_t key_id);

#endif //STARM_KEY_H
