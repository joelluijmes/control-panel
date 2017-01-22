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

static uint16_t read_mux(uint16_t value)
{
    // the hardware features a mux that feeds the switch into an ADC to read the position of the tumble switch
    // we iterate through all 12 switches
    for (uint8_t i = 0; i < 12; ++i)
    {
        // read the value of that switch (note that the MSB is on the left)
        uint16_t val = mux4067_read(11 - i);

        // ADC returns 0 - 1023: > 750 it is in UP; < 250 is DOWN; 250 - 750 is MIDDLE
        // MIDDLE means the original value, if it is forced to be up or down, force that bit:
        if (val > 750)
            value |= (1 << i);
        else if (val < 250)
            value &= ~(1 << i);
    }

    return value;
}

static volatile uint8_t updated = 0;
ISR(PCINT2_vect)
{
    if (BUTTON_PIN & BUTTON_MASK)
        updated = 1;
}

int main(void)
{
    // enable pin change interrupt for the button 
    PCICR = 1 << PCIE2;
    PCMSK2 = 1 << PCINT16;

    // initialize the display driver
    max7221_init();
    // initialize the mux for the switches
    mux4067_init();

    // initialize the SPI, note that the slave is nonblocking for the first byte, meaning
    // that continues until the interrupt is called. The rest of the SPI transmission is 
    // blocking. 
    spi_init(SPI_SLAVE | SPI_NONBLOCKING, SPI_64X);
    proto_init(spi_tranceive, &spi_state.on_completed);
    sei();
    
    // placeholders
    register_display_t tmp = { 0 };
    register_display_t reg = { 0 };
    uint16_t value = 0;

    while (1) 
    {
        // update the value by reading what the user did with the switches
        reg.value = read_mux(value);
        // show that value on the LEDs
        display_update(reg.value);
        
        // make sure to store it in the packet
        reg.updated = updated;
        
        // if we are not tranceiving we are updating the packet to be tranceived.
        // if we are tranceiving, but the SS is high the transmission has been corrupt and has to be reset
        volatile proto_status_t status = proto_status();
        if (status == IDLE || status == FAILED || (SS_PIN & SS_MASK))
        {
            // only on successful transfer we process the switches
            if (status == IDLE)
            {
                // if the button is pressed we remember it as the new value
                if (updated)
                {
                    value = reg.value;
                    updated = 0;
                }

                // if the backend gave us a new value to display
                if (tmp.updated)
                    value = tmp.value;
                
                __asm("nop");
            }
            
            // debug
            if (status == FAILED)
                __asm("nop");
            
            // create the packets
            proto_packet_t transmit = proto_create(20, (uint8_t*)&reg, sizeof(register_display_t));
            proto_packet_t receive = proto_create_empty((uint8_t*)&tmp, sizeof(register_display_t));

            // make sure the SPI hardware is in a valid state
            spi_reset();
            // set the packets to be tranceived
            proto_tranceive(&transmit, &receive);
        }
    }
}
