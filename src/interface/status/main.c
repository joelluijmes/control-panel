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

static uint8_t read_rot()
{
    return ROT1_SAMPLE() | ROT2_SAMPLE() | ROT4_SAMPLE() | ROT8_SAMPLE();
}

#define DOWN 0
#define MIDDLE 1
#define UP 2

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

static completed_t completed;
static void completed_spi(spi_state_t* state)
{
    completed((uint8_t*)state->tran_buf, state->tran_len, (uint8_t*)state->recv_buf, state->recv_len);
}

int main(void)
{   
    DDRA |= 1 << 3;

    spi_init(SPI_SLAVE, SPI_64X);
    sei();

    //Starts AD in Free Running mode with prescaler of 128
    ADMUX = 1 << REFS0;			// AVCC as reference | ADC0
    ADCSRA = 1 << ADEN | 1 << ADPS2 | 1 << ADPS1 | 1 << ADPS0;
    
    MAX7221_DATA_DDR |= MAX7221_DATA_MASK;
    MAX7221_CS_DDR |= MAX7221_CS_MASK;
    MAX7221_SCK_DDR |= MAX7221_SCK_MASK;

    ROT1_DDR &= ~ROT1_MASK;
    ROT2_DDR &= ~ROT2_MASK;
    ROT4_DDR &= ~ROT4_MASK;
    ROT8_DDR &= ~ROT8_MASK;

    max7221_init();

    uint8_t tran_buf[32] = { 2, 3, 4, 5, 6, 7}, recv_buf[32];
    proto_packet_t transmit = proto_create(2, tran_buf, 2);
    proto_packet_t receive = { .payload = recv_buf, .len = 32 };

    spi_install_hook(completed_spi);
    proto_init(&completed, spi_tranceive);

    while (1)
    {
        //spi_tranceive(tran_buf, 6, recv_buf, 6);
        //while (!spi_completed());

        proto_tranceive(&transmit, &receive);
        proto_wait();        
    }
    //status_t* display = display_get();
    //display->clock = read_adc(POT_ADC);
//
    //display_update(display);
    //while (1)
    //{
        //display->clock = read_adc(POT_ADC);
//
        //display_update(display);
        ////_delay_ms(25);
    //}
    
}

