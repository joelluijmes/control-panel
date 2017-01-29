#include "config.h"
#include "display.h"

#include "../../lib/stp16cpc26/stp16cpc26.h"
#include "../../lib/spi/single/spi_single.h"
#include "../../lib/max7221/max7221.h"
#include "../../lib/protos/protos.h"

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

int main(void)
{
    // make sure WDT is disabled after a reset
    MCUSR &= ~(1 << WDRF);
    WDTCSR |= (1 << WDCE) | (1 << WDE);
    WDTCSR = 0x00;

    //wdt_enable(WDTO_30MS);

    // initialize the ADC with prescaler of 16x (ADC clock must < 1MHz)
    ADMUX = 1 << REFS0;
    ADCSRA = 1 << ADEN | 1 << ADPS2;

    // initialize single LED driver (just a fancy shift register)
    stp16_init();
    stp16_output(1);
    // initialize the displays driver
    max7221_init();
    
    // initialize the SPI, note that the slave is nonblocking for the first byte, meaning
    // that continues until the interrupt is called. The rest of the SPI transmission is
    // blocking.
    spi_init(SPI_SLAVE | SPI_NONBLOCKING, SPI_64X);
    proto_init(spi_tranceive, &spi_state.on_completed);
    sei();

    // placeholder
    static instruction_t tmp_instruction, instruction;
    static proto_status_t status;
    
    while (1) 
    {
        // reset the watchdog
        wdt_reset();

        status = proto_status();

        // while the transmission hasn't completed, process user interaction
        while (status != IDLE && status != FAILED)
        {
            // always update the display, it also handles the switch position (locally)
            display_update(&instruction);
            status = proto_status();            
        }

        // debug
        if (status == FAILED)
            __asm("nop");

        // update the display on successful transmission :)
        if (status == IDLE)
            instruction = tmp_instruction;

        // create the packet
        proto_packet_t receive = proto_create(20, (uint8_t*)&tmp_instruction, sizeof(instruction_t));
        // start the receivement of the packet
        proto_receive(&receive);
    }
}
