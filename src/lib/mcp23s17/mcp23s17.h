#pragma once

#include <inttypes.h>
#include "config.h"

typedef int8_t (*tranceive_t)(const uint8_t* tran_buf, uint8_t tran_len, uint8_t* recv_buf, uint8_t recv_len);

#define MCP23_IODIRA    0x00
#define MCP23_IODIRB    0x01
#define MCP23_IPOLA     0x02
#define MCP23_IPOLB     0x03
#define MCP23_GPINTENA  0x04
#define MCP23_GPINTENB  0x05
#define MCP23_DEFVALA   0x06
#define MCP23_DEFVALB   0x07
#define MCP23_INTCONA   0x08
#define MCP23_INTCONB   0x09
#define MCP23_IOCON     0x0A
#define MCP23_GPPUA     0x0C
#define MCP23_GPPUB     0x0D
#define MCP23_INTFA     0x0E
#define MCP23_INTFB     0x0F
#define MCP23_INTCAPA   0x10
#define MCP23_INTCAPB   0x11
#define MCP23_GPIOA     0x12
#define MCP23_GPIOB     0x13
#define MCP23_OLATA     0x14
#define MCP23_OLATB     0x15

#define MCP23_INTPOL    0x02
#define MCP23_ODR       0x04
#define MCP23_HAEN      0x08
#define MCP23_DISSLW    0x10
#define MCP23_SEQOP     0x20
#define MCP23_MIRROR    0x40
#define MCP23_BANK      0x80s

void mcp23_init(tranceive_t tranceive);
void mcp23_write(uint8_t addr, uint8_t reg, uint8_t value);
void mcp23_write_wide(uint8_t addr, uint8_t reg, uint16_t value);
uint8_t mcp23_read(uint8_t addr, uint8_t reg);
uint16_t mcp23_read_wide(uint8_t addr, uint8_t reg);

inline void mcp23_ddr(uint8_t addr, uint16_t mask)
{
    mcp23_write_wide(addr, MCP23_IODIRA, ~mask);
}

inline void mcp23_pullup(uint8_t addr, uint16_t mask)
{
    mcp23_write_wide(addr, MCP23_GPPUA, mask);
}

inline void mcp23_output(uint8_t addr, uint16_t mask)
{
    mcp23_write_wide(addr, MCP23_GPIOA, mask);
}

inline uint16_t mcp23_input(uint8_t addr)
{
    mcp23_read_wide(addr, MCP23_GPIOA);
}
