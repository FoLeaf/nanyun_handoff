//
// Created by 19y on 2026/4/28.
//

#ifndef G070_RA_01S_VOLTMEASURE_H
#define G070_RA_01S_VOLTMEASURE_H
#include "main.h"
#include "adc.h"
void voltmeasure_init(void);
uint16_t voltmeasure_get_advalue(void);
float voltmeasure_get_voltage(void);
#endif //G070_RA_01S_VOLTMEASURE_H
