#ifndef __OLEDFONT_H__
#define __OLEDFONT_H__
#include "hal_types.h"
typedef struct
{
    const char name[4];
    const uint8 data[32];
}ChineseFontTypedef;
extern const uint8 Font8x16[][16];
extern const uint8 Font6x8[][6];
extern const ChineseFontTypedef ChineseFont[9];
extern const uint8 images[][32];
#endif

