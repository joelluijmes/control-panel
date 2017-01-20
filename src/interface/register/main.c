#include "config.h"
#include "display.h"

#include "../../lib/mux4067/mux4067.h"
#include "../../lib/spi/single/spi_single.h"
#include "../../lib/max7221/max7221.h"
#include "../../lib/protos/protos.h"
#include "../../models/register.h"

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

uint16_t read_mux(uint16_t value)
{
    for (uint8_t i = 0; i < 12; ++i)
    {
       volatile uint16_t val = mux4067_read(11 - i);

        if (val > 750)
            value |= (1 << i);
        else if (val < 250)
            value &= ~(1 << i);
    }

    return value;
}

ISR(PCINT0_vect)
{
    //if (!(PIN_SS & MASK_SS))
        //return;
//
    //volatile proto_status_t status = proto_status();
//
        //spi_reset();
}

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
    PCICR = 1 << PCIE0 | 1 << PCIE2;
    //PCMSK0 = 1 << PCINT2;
    PCMSK2 = 1 << PCINT16;

    max7221_init();
    //mux4067_init();

    //spi_init(SPI_SLAVE | SPI_NONBLOCKING, SPI_64X);
    //sei();
//
    //proto_init(spi_tranceive, &spi_state.on_completed);
    
    register_display_t tmp = { 0 };
    register_display_t reg = { 0 };
    uint16_t value = 0;

    while (1) 
    {
        PORT_MISO |= MASK_MISO;

        //reg.value = read_mux(value);
        
        
        display_update(value);
        

        //if (updated)
            //value = reg.value;
//
        //reg.updated = updated;
            //
        //volatile proto_status_t status = proto_status();
        //if (status == IDLE)
        //{
            //if (tmp.updated)
                //value = tmp.value;
        //}

        PORT_MISO &= ~MASK_MISO;

        ++value;
        _delay_ms(10);

        //continue;
//
        //if (status == IDLE || status == FAILED || (PIN_SS & MASK_SS))
        //{
            //if (status == IDLE) 
                //__asm("nop");
            //if (status == FAILED)
                //__asm("nop");
                //
            //proto_packet_t transmit = proto_create(20, (uint8_t*)&reg, sizeof(register_display_t));
            //proto_packet_t receive = proto_create_empty((uint8_t*)&tmp, sizeof(register_display_t));
//
            //spi_reset();
//
            //proto_tranceive(&transmit, &receive);
        //} 

        updated = 0;        
    }
}
