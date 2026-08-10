#include "hal_defs.h"

#ifndef _LIB_FLASH_H_
#define _LIB_FLASH_H_
void Flash_Erase(uint8 page);
void Flash_Write(uint8 page, uint16 offset, uint8 *buf, uint16 length);
void Flash_Read(uint8 page, uint16 offset, uint8 *buf, uint16 length);
#endif