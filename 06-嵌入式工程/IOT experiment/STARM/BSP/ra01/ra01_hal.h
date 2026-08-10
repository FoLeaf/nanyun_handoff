#ifndef __RA01_HAL_H__
#define __RA01_HAL_H__

#include "main.h"
#include <stdint.h>

// Initialize the HAL (GPIO, SPI, etc.)
void SX1276HALInit(void);

// SPI Read/Write byte
uint8_t SpiInOut(uint8_t outData);

// NSS Control: status=0 -> Enable (Low), status=1 -> Disable (High)
void SpiNSSEnable(uint8_t status);

// Reset Control: status=0 -> Reset (Low), status=1 -> Release (High)
void SX127X_ResetPinControl(uint8_t status);

// Read DIO0: returns 1 if high, 0 if low
uint8_t SX1276ReadDio0(void);
uint8_t SX1276ReadDio1(void);
uint8_t SX1276ReadDio3(void);
uint8_t SX1276ReadDio4(void);

#endif /* __RA01_HAL_H__ */
