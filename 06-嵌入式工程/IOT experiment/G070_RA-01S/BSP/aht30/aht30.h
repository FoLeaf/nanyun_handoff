#ifndef __AHT30_H
#define __AHT30_H

#include <stdint.h>

/* AHT30 Commands */
#define AHT30_ADDR_WRITE    0x70
#define AHT30_ADDR_READ     0x71
#define AHT30_CMD_INIT      0xBE
#define AHT30_CMD_MEASURE   0xAC
#define AHT30_CMD_SOFTRESET 0xBA

/* Hardware Interface Abstraction */
typedef struct {
    uint8_t (*I2C_Write_Bytes)(uint8_t addr, uint8_t *buf, uint8_t len);
    uint8_t (*I2C_Read_Bytes)(uint8_t addr, uint8_t *buf, uint8_t len);
    void (*Delay_ms)(uint32_t ms);
} AHT30_Hardware_t;

/* AHT30 Device Context */
typedef struct {
    AHT30_Hardware_t hw;
    float temperature;
    float humidity;
    uint8_t initialized;
} AHT30_HandleTypeDef;

/* API */
uint8_t AHT30_Init(AHT30_HandleTypeDef *dev);
uint8_t AHT30_ReadMeasure(AHT30_HandleTypeDef *dev);

#endif /* __AHT30_H */
