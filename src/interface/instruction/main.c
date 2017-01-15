#include "config.h"
#include "display.h"

#include "../../lib/spi/single/spi_single.h"
#include "../../lib/max7221/max7221.h"
#include "../../lib/stp16cpc26/stp16cpc26.h"

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

int main(void)
{
    ADMUX = 1 << REFS0 | 3;			// AVCC as reference | ADC3
    ADCSRA = 1 << ADEN | 1 << ADPS2 | 1 << ADPS1 | 1 << ADPS0;

    stp16_init();
    stp16_output(1);
    
    max7221_init();
    //max7221_clear();
    
    spi_init(SPI_SLAVE, SPI_32X);
    sei();

    while (1) 
    {
        instruction_t instruction = { 0 };
        if (spi_tranceive((uint8_t*)&instruction, sizeof(instruction)) != SPI_OK)
            continue;

        while (!spi_completed()) ;

        display_update(&instruction);

    }
}
