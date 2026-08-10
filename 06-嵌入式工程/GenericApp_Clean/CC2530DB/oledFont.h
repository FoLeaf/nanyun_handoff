#ifndef __OLEDFONT_H__
#define __OLEDFONT_H__
#include "main.h"
typedef struct
{
    const char name[4];
    const uint8_t data[32];
}ChineseFontTypedef;
extern const uint8_t Font8x16[][16];
extern const uint8_t Font6x8[][6];
extern const ChineseFontTypedef ChineseFont[9];
extern const uint8_t images[][32];
#endif

