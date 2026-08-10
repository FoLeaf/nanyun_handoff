#ifndef __OLED_H
#define __OLED_H

#include "hal_types.h"

void OLED_Init(void);
void OLED_Clear(void);
void OLED_DisplayOn(void);
void OLED_DisplayOff(void);
void OLED_SetPos(uint8 x, uint8 y);
void OLED_ShowChar(uint8 x, uint8 y, uint8 chr, uint8 size);
void OLED_ShowString(uint8 x, uint8 y, char *str);
void OLED_ShowChineseChar(uint8 x, uint8 y, uint8 index);

#endif /* __OLED_H */
