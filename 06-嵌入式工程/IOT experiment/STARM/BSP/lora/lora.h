#ifndef __LORA_H__
#define __LORA_H__

#include "main.h"

typedef struct {
    uint8_t addh;  // Address High
    uint8_t addl;  // Address Low
    uint8_t reg0;  // UART Baudrate, Parity, Air Data Rate
    uint8_t reg1;  // Sub-packet size, Ambient noise, Transmit power
    uint8_t reg2;  // Channel (frequency)
    uint8_t reg3;  // Advanced config, RSSI, LBT
} lora_config_t;

// === LORA REG0 Constants ===
#define LORA_REG0_BAUD_9600_8N1    (0x60)
// Air Data Rate options
#define LORA_REG0_AIR_RATE_0K3     (0x00) // 0.3k
#define LORA_REG0_AIR_RATE_1K2     (0x01) // 1.2k
#define LORA_REG0_AIR_RATE_2K4     (0x02) // 2.4k (default)
#define LORA_REG0_AIR_RATE_4K8     (0x03) // 4.8k
#define LORA_REG0_AIR_RATE_9K6     (0x04) // 9.6k
#define LORA_REG0_AIR_RATE_19K2    (0x05) // 19.2k
#define LORA_REG0_AIR_RATE_62K5    (0x07) // 62.5k

// === LORA REG1 Constants ===
#define LORA_REG1_SUBPACKET_240    (0x00)
// Transmit Power options
#define LORA_REG1_POWER_22DBM      (0x00) // 22dBm (default)
#define LORA_REG1_POWER_17DBM      (0x01)
#define LORA_REG1_POWER_13DBM      (0x02)
#define LORA_REG1_POWER_10DBM      (0x03)

// === LORA REG2 Constants ===
#define LORA_REG2_CHANNEL_433MHZ   (23)   // Base freq (e.g. 410) + 23 = 433 MHz

// === LORA REG3 Constants ===
#define LORA_REG3_DEFAULT          (0x00) // Transparent, no RSSI, no LBT

// Initialize LoRa module
void lora_init(void);

// Set LoRa mode (0: transparent, 1: WOR TX, 2: WOR RX, 3: Sleep/Config)
void lora_set_mode(uint8_t mode);

// Get default configuration parameters
void lora_get_default_config(lora_config_t *cfg);

// Set and apply configuration
void lora_set_config(lora_config_t *cfg);

// Send data via LoRa
void lora_send(uint8_t *data, uint16_t len);

// Process received data (Call this inside the main loop)
void lora_process(void);

#endif /* __LORA_H__ */
