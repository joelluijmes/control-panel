#include "display.h"
#include "io.h"
#include "../../lib/max7221/max7221.h"

#define BIT_0 (1 << 6)
#define BIT_1 (1 << 1)
#define BIT_2 (1 << 5)
#define BIT_3 (1 << 2)
#define BIT_4 (1 << 4)
#define BIT_5 (1 << 3)

static void inline print_hex(uint16_t value)
{
    max7221_display(0, MAX7221_DIGIT0, value%0x10);
    max7221_display(0, MAX7221_DIGIT4, (value/0x10)%0x10);
    max7221_display(0, MAX7221_DIGIT6, (value/0x100)%0x10);
}

static void inline print_binary(uint16_t value)
{
    // The binary LEDs are alternated between two digits (2 and 3)
    // decoding it to binary representation takes ugly if statements

    uint8_t display_left = 0, display_right = 0;

    if (value & (1 << 0))
        display_right |= BIT_0;
    if (value & (1 << 1))
        display_left |= BIT_0;
    if (value & (1 << 2))
        display_right |= BIT_1;
    if (value & (1 << 3))
        display_left |= BIT_1;
    if (value & (1 << 4))
        display_right |= BIT_2;
    if (value & (1 << 5))
        display_left |= BIT_2;
    if (value & (1 << 6))
        display_right |= BIT_3;
    if (value & (1 << 7))
        display_left |= BIT_3;
    if (value & (1 << 8))
        display_right |= BIT_4;
    if (value & (1 << 9))
        display_left |= BIT_4;
    if (value & (1 << 10))
        display_right |= BIT_5;
    if (value & (1 << 11))
        display_left |= BIT_5;
            
    max7221_write(0, MAX7221_DIGIT2, display_left);
    max7221_write(0, MAX7221_DIGIT3, display_right);
}

void display_update(uint16_t reg)
{
    print_hex(reg);
    print_binary(reg);
}