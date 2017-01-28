#include "mcp23s17.h"

#define OPCODEW      0x40
#define OPCODER      0x41

static tranceive_t _tranceive;

void mcp23_init(tranceive_t tranceive)
{
    // TODO: remove the 12 3 versions -> back to single CS
    MCP23_SS12_DDR |= MCP23_SS12_MASK;
    MCP23_SS3_DDR |= MCP23_SS3_MASK;

    MCP23_SS12_PORT |= MCP23_SS12_MASK;
    MCP23_SS3_PORT |= MCP23_SS3_MASK;

    _tranceive = tranceive;
}

void mcp23_write(uint8_t addr, uint8_t reg, uint8_t value)
{
    MCP23_SS12_PORT &= ~MCP23_SS12_MASK; MCP23_SS3_PORT &= ~MCP23_SS3_MASK;

    uint8_t buf[] = { OPCODEW | ((addr & 0x07) << 1), reg, value };
    _tranceive(buf, sizeof(buf), 0, 0);

    MCP23_SS12_PORT |= MCP23_SS12_MASK; MCP23_SS3_PORT |= MCP23_SS3_MASK;
}

void mcp23_write_wide(uint8_t addr, uint8_t reg, uint16_t value)
{
    MCP23_SS12_PORT &= ~MCP23_SS12_MASK; MCP23_SS3_PORT &= ~MCP23_SS3_MASK;
    
    uint8_t buf[] = { OPCODEW | ((addr & 0x07) << 1), reg, (uint8_t)value, (uint8_t)(value >> 8) };
    _tranceive(buf, sizeof(buf), 0, 0);

    MCP23_SS12_PORT |= MCP23_SS12_MASK; MCP23_SS3_PORT |= MCP23_SS3_MASK;
}

uint8_t mcp23_read(uint8_t addr, uint8_t reg)
{
    MCP23_SS12_PORT &= ~MCP23_SS12_MASK; MCP23_SS3_PORT &= ~MCP23_SS3_MASK;

    uint8_t buf_in[3] = { OPCODER | ((addr & 0x07) << 1), reg, 0 };
    uint8_t buf_out[3] = { 0 };

    _tranceive(buf_in, sizeof(buf_in), buf_out, sizeof(buf_out));

    MCP23_SS12_PORT |= MCP23_SS12_MASK; MCP23_SS3_PORT |= MCP23_SS3_MASK;

    return buf_out[2];
}

uint16_t mcp23_read_wide(uint8_t addr, uint8_t reg)
{
    MCP23_SS12_PORT &= ~MCP23_SS12_MASK; MCP23_SS3_PORT &= ~MCP23_SS3_MASK;
    
    uint8_t buf_in[4] = { OPCODER | ((addr & 0x07) << 1), reg, 0, 0 };
    uint8_t buf_out[4] = { 0 };

    _tranceive(buf_in, sizeof(buf_in), buf_out, sizeof(buf_out));

    MCP23_SS12_PORT |= MCP23_SS12_MASK; MCP23_SS3_PORT |= MCP23_SS3_MASK;

    return (buf_out[2] << 8) | buf_out[3];
}
