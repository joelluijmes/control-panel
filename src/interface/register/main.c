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
#include <avr/wdt.h>

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
    // make sure WDT is disabled after a reset 
    MCUSR &= ~(1 << WDRF);
    WDTCSR |= (1 << WDCE) | (1 << WDE);
    WDTCSR = 0x00;

    //wdt_enable(WDTO_30MS);

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
    static register_display_t display, tmp_display;
    static uint16_t value;
    static proto_status_t status;

    while (1) 
    {
        wdt_reset();
        status = proto_status();

        // while the transmission hasn't completed, process user interaction
        while (status != IDLE && status != FAILED)
            status = proto_status();

        // debug
        if (status == FAILED)
            __asm("nop");

        // update the display on successful transmission :)
        if (status == IDLE)
        {
            __asm("nop");
            if (tmp_display.updated)
                value = tmp_display.value;

            // do the user inteface processing
            display.value = read_mux(value);
            display.updated = updated;
            display_update(display.value);

            updated = 0;
        }

        tmp_display = display;

        // create the packets
        proto_packet_t transmit = proto_create(1, (uint8_t*)&display, sizeof(register_display_t));
        proto_update_crc(&transmit);

        proto_packet_t receive = proto_create(10, (uint8_t*)&tmp_display, sizeof(register_display_t));
        // set the packets to be tranceived
        proto_tranceive(&transmit, &receive);
    }
}
