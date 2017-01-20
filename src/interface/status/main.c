#include "config.h"
#include "display.h"

#include "../../lib/spi/single/spi_single.h"
#include "../../lib/max7221/max7221.h"
#include "../../lib/protos/protos.h"
#include "../../models/status.h"

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>

static uint16_t read_adc(uint8_t mux)
{
    ADMUX = (ADMUX & 0xF0) | mux;    // select channel without changing value
    ADCSRA |= 1 << ADSC;    // start adc
    
    while (ADCSRA & (1 << ADSC)) ; // = 1 as long conversion is in progress
    return ADC;
}

#define ROT1_SAMPLE() (((ROT1_PIN & ROT1_MASK) == 0) << 0)
#define ROT2_SAMPLE() (((ROT2_PIN & ROT2_MASK) == 0) << 1)
#define ROT4_SAMPLE() (((ROT4_PIN & ROT4_MASK) == 0) << 2)
#define ROT8_SAMPLE() (((ROT8_PIN & ROT8_MASK) == 0) << 3)


static uint8_t read_rot()
{
    return ROT1_SAMPLE() | ROT2_SAMPLE() | ROT4_SAMPLE() | ROT8_SAMPLE();
}

#define DOWN 0
#define MIDDLE 1
#define UP 2

ISR(PCINT0_vect)
{
    //if (!(PIN_SS & MASK_SS))
        return;

    spi_reset();
    proto_reset();
}

static uint8_t read_switch()
{
    uint16_t val = read_adc(CLOCK_ADC);
    if (val > 750)
        return UP;
    else if (val < 250)
        return DOWN;
    else
        return MIDDLE;
}

void init_random(void)
{
    ADCSRA = 1 << ADEN | 1 << ADPS2 | 1 << ADPS1 | 1 << ADPS0;

    uint8_t val = 0;
    for (uint8_t i = 0; i < 7; ++i)
    {
        ADMUX = 1 << REFS0 | i;			// AVCC as reference | ADC0
        ADCSRA |= 1 << ADSC;

        while (ADCSRA & (1 << ADSC)) ; // = 1 as long conversion is in progress
        
        if (ADC & 0x02)
            val |= (1 << i);
    }

    srand(val);
}

int main(void)
{   
    DDRA |= 1 << 3;

    init_random();

    PCICR = 1 << PCIE0;
    PCMSK0 = 1 << PCINT2;

    //spi_init(SPI_SLAVE | SPI_NONBLOCKING, SPI_64X);
    //sei();

    //Starts AD in Free Running mode with prescaler of 128
    ADMUX = 1 << REFS0;			// AVCC as reference | ADC0
    ADCSRA = 1 << ADEN | 1 << ADPS2;
    
    ROT1_DDR &= ~ROT1_MASK;
    ROT1_PORT |= ROT1_MASK;
    ROT2_DDR &= ~ROT2_MASK;
    ROT2_PORT |= ROT2_MASK;
    ROT4_DDR &= ~ROT4_MASK;
    ROT4_PORT |= ROT4_MASK;
    ROT8_DDR &= ~ROT8_MASK;
    ROT8_PORT |= ROT8_MASK;

    max7221_init();

    status_state_t state = { 0 };
    status_display_t display = { 0 };

    proto_init(spi_tranceive, &spi_state.on_completed);

    DDR_MISO = MASK_MISO;
    while (1)
    {
        //state.clock.clock_speed = read_adc(POT_ADC);
        //state.selected = read_rot();
//
        //volatile proto_status_t status = proto_status();
        //
        //if (status == IDLE || status == FAILED)
        //{
            //proto_packet_t transmit = proto_create(10, (uint8_t*)&state, sizeof(state));
            //proto_packet_t receive = proto_create_empty((uint8_t*)&display, sizeof(display));
//
            //proto_tranceive(&transmit, &receive);
        //}     
//
        //if (status == IDLE)

        PORT_MISO |= MASK_MISO;
            display_update(&display);
            PORT_MISO &= ~MASK_MISO;

        ++display.clock.clock_speed;
        _delay_ms(10);
    }
}

