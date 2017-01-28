#include "config.h"
#include "display.h"

#include "../../lib/spi/single/spi_single.h"
#include "../../lib/max7221/max7221.h"
#include "../../lib/protos/protos.h"
#include "../../models/status.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdlib.h>


static uint16_t read_adc(uint8_t mux)
{
    // the board has a potmeter and tumble switch, therefor use this nice function to read the ADC  
    ADMUX = (ADMUX & 0xF0) | mux;
    // start the conversion
    ADCSRA |= 1 << ADSC;            
    
    // wait for completion
    while (ADCSRA & (1 << ADSC)) ;
    return ADC;
}

static uint8_t read_rot()
{
    // the rotary encoder gives out the binary value of the position
    #define ROT1_SAMPLE() (((ROT1_PIN & ROT1_MASK) == 0) << 0)
    #define ROT2_SAMPLE() (((ROT2_PIN & ROT2_MASK) == 0) << 1)
    #define ROT4_SAMPLE() (((ROT4_PIN & ROT4_MASK) == 0) << 2)
    #define ROT8_SAMPLE() (((ROT8_PIN & ROT8_MASK) == 0) << 3)

    return ROT1_SAMPLE() | ROT2_SAMPLE() | ROT4_SAMPLE() | ROT8_SAMPLE();
}

#define DOWN 0
#define MIDDLE 1
#define UP 2

static uint8_t read_switch()
{
    // ADC returns 0 - 1023: > 750 it is in UP; < 250 is DOWN; 250 - 750 is MIDDLE
    uint16_t val = read_adc(CLOCK_ADC);
    if (val > 750)
        return UP;
    else if (val < 250)
        return DOWN;
    else
        return MIDDLE;
}

int main(void)
{   
    // make sure WDT is disabled after a reset
    MCUSR &= ~(1 << WDRF);
    WDTCSR |= (1 << WDCE) | (1 << WDE);
    WDTCSR = 0x00;
    
    // wdt_enable(WDTO_30MS);

    // initialize the ADC with prescaler of 16x (ADC clock must < 1MHz)
    ADMUX = 1 << REFS0;
    ADCSRA = 1 << ADEN | 1 << ADPS2;
    
    // enable the internal pull-ups for the rotary encoder (default DDR is input)
    ROT1_PORT |= ROT1_MASK;
    ROT2_PORT |= ROT2_MASK;
    ROT4_PORT |= ROT4_MASK;
    ROT8_PORT |= ROT8_MASK;

    // initialize display driver
    max7221_init();

    // initialize the SPI, note that the slave is nonblocking for the first byte, meaning
    // that continues until the interrupt is called. The rest of the SPI transmission is
    // blocking.
    spi_init(SPI_SLAVE | SPI_NONBLOCKING, SPI_64X);
    proto_init(spi_tranceive, &spi_state.on_completed);
    sei();

    // placeholders
    static status_state_t state, tmp_state;
    static status_display_t display, tmp_display;
    static proto_status_t status;

    uint8_t updated = 0;
    while (1)
    {
        status = proto_status();

        // while the transmission hasn't completed, process user interaction
        while (status != IDLE && status != FAILED)
        {
            wdt_reset();

            // received valid packet to display
            if (updated)
                display_update(&display);

            // do the user processing of the interface
            state.clock.clock_speed = read_adc(POT_ADC);
            state.clock.clock_mode = read_switch();
            state.selected = read_rot();

            status = proto_status();
        }
        
        // debug
        if (status == FAILED)
            __asm("nop");

        // update the display on successful transmission :)
        if (status == IDLE)
        {
            display = tmp_display;
            updated = 1;
        }

        tmp_state = state;

        // create the packets
        proto_packet_t transmit = proto_create(3, (uint8_t*)&tmp_state, sizeof(status_state_t));
        proto_update_crc(&transmit);

        proto_packet_t receive = proto_create(30, (uint8_t*)&tmp_display, sizeof(status_display_t));
        // set the packets to be tranceived
        proto_tranceive(&transmit, &receive);
    }
}
