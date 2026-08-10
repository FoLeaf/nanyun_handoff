#ifndef AHT30_H
#define AHT30_H

#include "hal_i2c.h"

#define AHT30_ADDR_WRITE 0x70
#define AHT30_ADDR_READ  0x71

typedef struct {
    float humidity;
    float temperature;
} AHT30_Data;

unsigned char AHT30_Init(void);
unsigned char AHT30_ReadData(AHT30_Data *data);

#endif /* AHT30_H */
