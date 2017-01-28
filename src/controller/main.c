#include "config.h"
#include "frontend.h"
#include "backplane.h"

#include "../lib/protos/protos.h"
#include "../lib/spi/spi.h"
#include "../lib/spi/double/spi_double.h"
#include "../lib/mcp23s17/mcp23s17.h"
#include "../models/status.h"

#include <avr/io.h>
#include <util/delay.h>

static status_display_t display;
static instruction_t instruction;

void process(void)
{
    backplane_read();

    status_state_t* state = front_status_get();

    // display.memory.enable = !!(backplane.memory.read || backplane.memory.write);
    display.memory.mmio = backplane.memory.mmio_enable;
    display.memory.read = backplane.memory.read;
    display.memory.write = backplane.memory.write;

    display.clock.clock_speed = state->clock.clock_speed;

    front_status_set(&display);

    instruction.alu.add = !!(backplane.alu == ALU_ADD);
    instruction.alu.sub = !!(backplane.alu == ALU_SUB);
    instruction.alu.and = !!(backplane.alu == ALU_AND);
    instruction.alu.or = !!(backplane.alu == ALU_OR);
    instruction.alu.xor = !!(backplane.alu == ALU_XOR);
    instruction.alu.clr = !!(backplane.alu == ALU_CLEAR);

    frontend_update();
}

int main(void)
{
    backplane_init();
    frontend_init();  

    front_instruction_set(&instruction);

    while (1)
    {
        process();
        _delay_ms(50);
    }
}
