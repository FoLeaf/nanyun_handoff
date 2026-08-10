#include "hal_defs.h"

#ifndef _LIB_RADIO_H_
#define _LIB_RADIO_H_
void Radio_Init(uint16 panId,uint8 channel,uint16 addr);
void Radio_Send(uint16 addr,uint8 *buf,uint8 len);
#endif