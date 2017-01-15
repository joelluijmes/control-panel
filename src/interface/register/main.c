#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "io.h"
#include "display.h"

#include "../../lib/mux4067/mux4067.h"
#include "../../lib/spi/single/spi_single.h"
#include "../../lib/max7221/max7221.h"

uint16_t read_mux(uint16_t value)
{
    for (uint8_t i = 0; i < 12; ++i)
    {
       volatile uint16_t val = mux4067_read(11 - i);

        if (val > 750)
            value |= (1 << i);
        else if (val < 250)
            value &= ~(1 << i);

        _delay_us(10);
    }

    return value;
}

typedef struct display_register_t
{
    uint16_t value : 12;
    uint8_t updated : 1;
} display_register_t;

static volatile uint8_t updated = 0;

ISR(PCINT2_vect)
{
    if (BUTTON_PIN & BUTTON_MASK)
        updated = 1;
}

int main(void)
{
    BUTTON_DDR |= BUTTON_MASK;

    // PCINT16
    PCICR = 1 << PCIE2;
    PCMSK2 = 1 << PCINT16;

    max7221_init();
    mux4067_init();

    spi_init(SPI_SLAVE, SPI_32X);
    sei();
    
    display_register_t reg = { 0 };
    uint16_t value = 0;

    while (1) 
    {
        // process user interation
        reg.value = read_mux(value);
        display_update(reg.value);


        // if the value did change by any switch, we say we have updated it
        reg.updated = updated;
            
        // exchange registers from master, either we get a new register to display (and possibly alter it)
        // or we still display the old register. In both ways we let know if we pressed the button, and changed the value
        if (spi_tranceive((uint8_t*)&reg, sizeof(reg)) != SPI_OK)
            continue;
        while (!spi_completed()) ;

        // if the master changed register, we need to update our screen
        if (reg.updated)
            value = reg.value;
    }
}
