#include "stp16cpc26.h"
#include <util/delay.h>

#define CLK_ASSERT() (STP16_CLK_PORT |= STP16_CLK_MASK)
#define CLK_DISABLE() (STP16_CLK_PORT &= ~STP16_CLK_MASK)
#define DATA_ASSERT() (STP16_DATA_PORT |= STP16_DATA_MASK)
#define DATA_DISABLE() (STP16_DATA_PORT &= ~STP16_DATA_MASK)
#define LE_ASSERT() (STP16_LE_PORT |= STP16_LE_MASK)
#define LE_DISABLE() (STP16_LE_PORT &= ~STP16_LE_MASK)
#define OE_ASSERT() (STP16_OE_PORT &= ~STP16_OE_MASK)
#define OE_DISABLE() (STP16_OE_PORT |= STP16_OE_MASK)

void stp16_init(void)
{
    STP16_CLK_DDR |= STP16_CLK_MASK;
    STP16_DATA_DDR |= STP16_DATA_MASK;
    STP16_LE_DDR |= STP16_LE_MASK;
    STP16_OE_DDR |= STP16_OE_MASK;
}

void stp16_output(uint8_t enable)
{
    if (enable)
        OE_ASSERT();
    else
        OE_DISABLE();
}

void stp16_latch(void)
{
    LE_ASSERT();
    _delay_us(100);
    LE_DISABLE();
}

void stp16_shift(uint16_t val)
{
    for (uint8_t i = 0; i < 16; ++i)
    {
        if (val & (1 << i))
            DATA_ASSERT();
        else
            DATA_DISABLE();

        _delay_us(100);
        CLK_ASSERT();
        _delay_us(100);
        CLK_DISABLE();
        _delay_us(100);
    }
}
