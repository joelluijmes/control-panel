#include "backplane.h"

#include "../models/backplane.h"
#include "../lib/spi/double/spi_double.h"
#include "../lib/mcp23s17/mcp23s17.h"

backplane_t backplane;

#define MCP_ADDR0 0
#define MCP_ADDR1 1
#define MCP_ADDR2 2

void backplane_init(void)
{
    // The MCP23S17 IO expanders are connected on the first spi bus
    spi0_init(SPI_MASTER, SPI_128X);
    mcp23_init(spi0_tranceive);

    // Enable addressing mode of the expanders (the same CS is used for all devices)
    mcp23_write(0, MCP23_IOCON, MCP23_HAEN);

    // Set all IO to input
    mcp23_ddr(MCP_ADDR0, 0);
    mcp23_ddr(MCP_ADDR1, 0);
    mcp23_ddr(MCP_ADDR2, 0);

    //mcp23_pullup(MCP_ADDR0, 0);
    //mcp23_pullup(MCP_ADDR1, 0);
    //mcp23_pullup(MCP_ADDR2, 0);
//
    //mcp23_write_wide(MCP_ADDR0, MCP23_IPOLA, 0);
    //mcp23_write_wide(MCP_ADDR1, MCP23_IPOLA, 0);
    //mcp23_write_wide(MCP_ADDR2, MCP23_IPOLA, 0);
}

void backplane_read(void)
{
    // read the first chip, contains the C bus and target
    volatile uint16_t content = mcp23_read_wide(MCP_ADDR0, MCP23_GPIOA);
    // mask out the 12 bits of the C bus
    backplane.c_bus = content & 0x0FFF;
    // get the upper 4 bits for target decoder
    backplane.target_decoder = content >> 12;  

    // read the second expander
    content = mcp23_read_wide(MCP_ADDR1, MCP23_GPIOA);
    backplane.alu = (content & 0x07);
    *(uint8_t*)&backplane.memory = (content >> 8) & 0x1F;    
    backplane.condition = (content >> 5) & 0x07;
    backplane.condition_inverted = !!(content & 0x10);
    backplane.carry = !!(content & 0x8000);
    backplane.sra = !!(content & 0x4000);
    backplane.srl = !!(content & 0x2000);

    // read the third expander
    content = mcp23_read_wide(MCP_ADDR2, MCP23_GPIOA);
    backplane.left_decoder = content & 0x0F;
    backplane.right_decoder = (content >> 4) & 0x0F;
    //backplane.status_enable = 
}

backplane_t* backplane_hpu()
{
    return &backplane;
}