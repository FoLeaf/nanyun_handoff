#ifndef __RA01_H__
#define __RA01_H__

#include "main.h"
#include <stdint.h>

// Forward declaring the SDK config struct to avoid exposing all SDK internals
typedef struct {
    uint32_t frequency; // e.g. 433000000
    uint8_t power;      // e.g. 14 for 14dBm
    uint32_t bandwidth; // 0=7.8k, 1=10.4k ... 7=125k ... etc.
    uint32_t datarate;  // SpreadingFactor, e.g. 7
    uint8_t coderate;   // 1=4/5 ...
} RA01_Config_t;

// Initialize RA-01 module
void RA01_Init(void);

// Send data (blocking wrapper with timeout)
// Returns 1 on success, 0 on timeout/failure
uint8_t RA01_Send(uint8_t *data, uint16_t len, uint32_t timeoutMs);

// Start continuous reception mode
void RA01_StartReceive(void);

// Check if data is received, returns length of received data and copies to buffer
// If no data, returns 0
uint16_t RA01_PopReceivedData(uint8_t *buffer, uint16_t buffer_max_len);

// Must be called continuously in the main loop to handle state transitions
void RA01_Process(void);

#endif /* __RA01_H__ */
