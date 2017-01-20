#include "mcp23s17.h"


#define IODIRA       0x00
#define IODIRB       0x01
#define IPOLA        0x02
#define IPOLB        0x03
#define GPINTENA     0x04
#define GPINTENB     0x05
#define DEFVALA      0x06
#define DEFVALB      0x07
#define INTCONA      0x08
#define INTCONB      0x09
#define IOCON        0x0A
#define GPPUA        0x0C
#define GPPUB        0x0D
#define INTFA        0x0E
#define INTFB        0x0F
#define INTCAPA      0x10
#define INTCAPB      0x11
#define GPIOA        0x12
#define GPIOB        0x13
#define OLATA        0x14
#define OLATB        0x15

#define OPCODEW      0x40
#define OPCODER      0x41
#define ADDR_ENABLE  0x08

static void write(uint8_t addr, uint8_t reg, uint8_t value);
static void write_pair(uint8_t addr, uint8_t reg, uint16_t value);
static uint8_t read(uint8_t addr, uint8_t reg);
static uint16_t read_pair(uint8_t addr, uint8_t reg);

static tranceive_t _tranceive;

void mcp23_init(tranceive_t tranceive)
{
    // TODO: remove the 12 3 versions -> back to single CS
    MCP23_SS12_DDR |= MCP23_SS12_MASK;
    MCP23_SS3_DDR |= MCP23_SS3_MASK;

    _tranceive = tranceive;
    write(0, IOCON, ADDR_ENABLE);
}

void mcp23_ddr(uint8_t addr, uint16_t mask)
{
    write_pair(addr, IODIRA, ~mask);      // invert it to make 0 input, 1 output (like AVR)
}

void mcp23_pullup(uint8_t addr, uint16_t mask)
{
    write_pair(addr, GPPUA, mask);
}

void mcp23_invert_input(uint8_t addr, uint16_t mask)
{
    write_pair(addr, IPOLA, mask);
}

void mcp23_output(uint8_t addr, uint16_t mask)
{
    write_pair(addr, GPIOA, mask);
}

uint16_t mcp23_input(uint8_t addr)
{
    return read_pair(addr, GPIOA);
}

static void write(uint8_t addr, uint8_t reg, uint8_t value)
{
    MCP23_SS12_PORT &= ~MCP23_SS12_MASK; MCP23_SS3_PORT &= ~MCP23_SS3_MASK;

    uint8_t buf[] = { OPCODEW | ((addr & 0x03) << 1), reg, value };
    _tranceive(buf, sizeof(buf), 0, 0);

    MCP23_SS12_PORT |= MCP23_SS12_MASK; MCP23_SS3_PORT |= MCP23_SS3_MASK;
}

static void write_pair(uint8_t addr, uint8_t reg, uint16_t value)
{
    MCP23_SS12_PORT &= ~MCP23_SS12_MASK; MCP23_SS3_PORT &= ~MCP23_SS3_MASK;
    
    uint8_t buf[] = { OPCODEW | ((addr & 0x03) << 1), reg, (uint8_t)value, (uint8_t)value >> 8 };
    _tranceive(buf, sizeof(buf), 0, 0);

    MCP23_SS12_PORT |= MCP23_SS12_MASK; MCP23_SS3_PORT |= MCP23_SS3_MASK;
}

static uint8_t read(uint8_t addr, uint8_t reg)
{
    MCP23_SS12_PORT &= ~MCP23_SS12_MASK; MCP23_SS3_PORT &= ~MCP23_SS3_MASK;

    uint8_t buf_in[3] = { OPCODER | ((addr & 0x03) << 1), reg, 0 };
    uint8_t buf_out[3] = { 0 };

    _tranceive(buf_in, sizeof(buf_in), buf_out, sizeof(buf_out));

    MCP23_SS12_PORT |= MCP23_SS12_MASK; MCP23_SS3_PORT |= MCP23_SS3_MASK;

    return buf_out[2];
}

static uint16_t read_pair(uint8_t addr, uint8_t reg)
{
    MCP23_SS12_PORT &= ~MCP23_SS12_MASK; MCP23_SS3_PORT &= ~MCP23_SS3_MASK;
    
    uint8_t buf_in[4] = { OPCODER | ((addr & 0x03) << 1), reg, 0, 0 };
    uint8_t buf_out[4] = { 0 };

    _tranceive(buf_in, sizeof(buf_in), buf_out, sizeof(buf_out));

    MCP23_SS12_PORT |= MCP23_SS12_MASK; MCP23_SS3_PORT |= MCP23_SS3_MASK;

    return buf_out[2] | (buf_out[3] << 8);
}