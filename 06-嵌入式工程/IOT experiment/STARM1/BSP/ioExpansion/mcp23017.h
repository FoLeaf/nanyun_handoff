//
// Created by 19y on 2026/4/10.
//

#ifndef STARM_MCP23017_H
#define STARM_MCP23017_H
#include "main.h"

// MCP23017 I2C Address (assuming A2, A1, A0 are tied to GND)
// Write: 0x40, Read: 0x41
#define MCP23017_ADDRESS_WRITE 0x40
#define MCP23017_ADDRESS_READ  0x41

// Registers (Bank 0)
#define MCP23017_IODIRA   0x00
#define MCP23017_IODIRB   0x01
#define MCP23017_IPOLA    0x02
#define MCP23017_IPOLB    0x03
#define MCP23017_GPINTENA 0x04
#define MCP23017_GPINTENB 0x05
#define MCP23017_DEFVALA  0x06
#define MCP23017_DEFVALB  0x07
#define MCP23017_INTCONA  0x08
#define MCP23017_INTCONB  0x09
#define MCP23017_IOCONA   0x0A
#define MCP23017_IOCONB   0x0B
#define MCP23017_GPPUA    0x0C
#define MCP23017_GPPUB    0x0D
#define MCP23017_INTFA    0x0E
#define MCP23017_INTFB    0x0F
#define MCP23017_INTCAPA  0x10
#define MCP23017_INTCAPB  0x11
#define MCP23017_GPIOA    0x12
#define MCP23017_GPIOB    0x13
#define MCP23017_OLATA    0x14
#define MCP23017_OLATB    0x15

void mcp23017_init(void);
void mcp23017_write_reg(uint8_t reg, uint8_t value);
uint8_t mcp23017_read_reg(uint8_t reg);

#endif //STARM_MCP23017_H
