#ifndef OLED_H
#define OLED_H

#include "hal_i2c.h"

#define OLED_ADDR 0x78

void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowString(unsigned char x, unsigned char y, char *str, unsigned char size);
void OLED_SetPos(unsigned char x, unsigned char y);

#endif /* OLED_H */
