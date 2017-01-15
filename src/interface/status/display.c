#include "display.h"
#include "../../lib/max7221/max7221.h"

static inline void print_clock(clock_t clock)
{
    max7221_display(0, MAX7221_DIGIT3, clock.clock_speed%10);
    max7221_display(0, MAX7221_DIGIT2, (clock.clock_speed/10)%10);
    max7221_display(0, MAX7221_DIGIT4, (clock.clock_speed/100)%10);
    max7221_display(0, MAX7221_DIGIT0, (clock.clock_speed/1000)%10);
}

static inline void print_status(sreg_t sreg)
{
    uint8_t val = *(uint8_t*)&sreg;
    max7221_write(0, MAX7221_DIGIT5, val << 1);
}

static inline void print_memory(memory_t mem)
{
    uint8_t val = *(uint8_t*)&mem;
    max7221_write(0, MAX7221_DIGIT1, val << 3);
}

void display_update(const status_display_t* const display)
{
    print_clock(display->clock);
    print_status(display->status);
    print_memory(display->memory);
}
