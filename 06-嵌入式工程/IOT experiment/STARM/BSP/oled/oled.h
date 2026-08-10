#ifndef __OLED_H__
#define __OLED_H__
#include "main.h"
#include "math.h"
#include "oledFont.h"
#include "delay.h"
#include "string.h"

#include "myiic.h"
void OLED_Init(void);
void OLED_Clear(void);
void OLED_WriteData(uint8_t data);
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t ch,uint8_t FontSize);
void OLED_ShowSTR(uint8_t x,uint8_t y,char *str,uint8_t FontSize);
void OLED_ShowNUM(uint8_t x,uint8_t y,uint32_t num,uint8_t length,uint8_t FontSize);
void OLED_ShowIMG(uint8_t x,uint8_t y,uint8_t width,uint8_t height,const uint8_t *img);
void OLED_ShowSignedNum(uint8_t x,uint8_t y,int32_t num,uint8_t length,uint8_t FontSize);
void OLED_ShowCNStr(uint8_t x,uint8_t y,char *str);
#endif

