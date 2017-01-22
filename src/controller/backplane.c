#include "backplane.h"

#include "../models/hpu.h"
#include "../lib/spi/double/spi_double.h"
#include "../lib/mcp23s17/mcp23s17.h"



static hpu_t hpu;

#define MCP_ADDR0 0
#define MCP_ADDR1 1
#define MCP_ADDR_2 2

void backplane_init(void)
{
    // The MCP23S17 IO expanders are connected on the first spi bus
    spi0_init(SPI_MASTER, SPI_128X);
    mcp23_init(spi0_tranceive);

    // Enable addressing mode of the expanders (the same CS is used for all devices)
    mcp23_write(0, MCP23_IOCON, MCP23_HAEN);

    // Set all IO to input and enable pullup
    // TODO: Pullup needed??
    mcp23_ddr(MCP_ADDR0, 0);
    mcp23_ddr(MCP_ADDR1, 0);
    mcp23_ddr(MCP_ADDR_2, 0);

    mcp23_pullup(MCP_ADDR0, 0xFFFF);
    mcp23_pullup(MCP_ADDR1, 0xFFFF);
    mcp23_pullup(MCP_ADDR_2, 0xFFFF);
}

void backplane_read(void)
{
    // read the first chip, contains the C bus and target
    volatile uint16_t content = mcp23_input(MCP_ADDR0);

    // mask out the 12 bits of the C bus
    hpu.c_bus = ~(content & 0x0FFF); 
    // get the upper 4 bits for target decoder
    hpu.target_decoder = ~(content >> 12);  
}

hpu_t* backplane_hpu()
{
    return &hpu;
}