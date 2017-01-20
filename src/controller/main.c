#include "config.h"

#include "../models/instruction.h"
#include "../models/status.h"
#include "../models/register.h"
#include "../lib/spi/double/spi_double.h"
#include "../lib/protos/protos.h"
#include "../lib/mcp23s17/mcp23s17.h"


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


#define ASSERT_FRONT_1() (PORT_SS_FRONT_1 &= ~MASK_SS_FRONT_1)
#define DISABLE_FRONT_1() (PORT_SS_FRONT_1 |= MASK_SS_FRONT_1)
#define ASSERT_FRONT_2() (PORT_SS_FRONT_2 &= ~MASK_SS_FRONT_2)
#define DISABLE_FRONT_2() (PORT_SS_FRONT_2 |= MASK_SS_FRONT_2)
#define ASSERT_FRONT_3() (PORT_SS_FRONT_3 &= ~MASK_SS_FRONT_3)
#define DISABLE_FRONT_3() (PORT_SS_FRONT_3 |= MASK_SS_FRONT_3)

void display_instruction()
{
    operand_t a =
    {
        .immediate = 12,
        .register_op = REGISTER_DISABLED
    };

    operand_t b =
    {
        .register_op = 1
    };

    operand_t c =
    {
        .immediate = 32,
        .register_op = REGISTER_DISABLED
    };

    alu_t alu =
    {
        .add = 1,
        .and = 1
    };

    instruction_t instruction =
    {
        .address = 1,
        .mar = 2,
        .operand_a = a,
        .operand_b = b,
        .operand_c = c,
        .alu = alu
    };

    //spi_tranceive_1((uint8_t*)&instruction, sizeof(instruction));
//
    //while (!spi_completed_1()) ;
}

typedef struct display_register_t 
{
    uint16_t value : 12;
    uint8_t updated : 1;
} display_register_t;

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

status_state_t state = { .selected = 0 };
status_display_t display = { 0 };

uint8_t handle_status()
{
    ASSERT_FRONT_2();
    ASSERT_FRONT_3();

    proto_packet_t transmit = proto_create(1, (uint8_t*)&display, sizeof(display));
    proto_packet_t receive = proto_create_empty((uint8_t*)&state, sizeof(state));

    proto_tranceive(&transmit, &receive);

    DISABLE_FRONT_2();
    DISABLE_FRONT_3();

    volatile proto_status_t status = proto_status();

    if (status == IDLE)
        display.clock = state.clock;

    return status == IDLE;
}

register_display_t r1 = { .value = 0x100 };
register_display_t r2 = { .value = 0x200 };
register_display_t r3 = { .value = 0x300 };

register_display_t* registers[] = { &r1, &r2, &r3 } ;

uint8_t prev_selected = -1;

uint8_t handle_register()
{
    register_display_t tmp;
    register_display_t* reg = registers[state.selected];
    reg->updated = prev_selected != state.selected;

    ASSERT_FRONT_1();
    ASSERT_FRONT_2();

    proto_packet_t transmit = proto_create(2, (uint8_t*)reg, sizeof(register_display_t));
    proto_packet_t receive = proto_create_empty((uint8_t*)&tmp, sizeof(register_display_t));

    proto_tranceive(&transmit, &receive);
    
    DISABLE_FRONT_1();
    DISABLE_FRONT_2();
    
    proto_status_t status = proto_status();
    if (status == IDLE)
    {
        reg->updated = 0;
        if (tmp.updated)
            reg->value = tmp.value;
    }

    prev_selected = state.selected;

    return status == IDLE;
}

void main_back(void)
{

    spi_init_0(SPI_MASTER, SPI_128X);

    mcp23_init(spi_tranceive_0);

    mcp23_ddr(1, 0xFFFF);

    while (1)
    {   
        mcp23_output(1, 0xFFFF);
        _delay_ms(500);
        mcp23_output(1, 0);
        _delay_ms(500);
    }
}

int main(void)
 {
    main_back();

    init_random();

    DDR_SS_FRONT_1 |= MASK_SS_FRONT_1;
    DDR_SS_FRONT_2 |= MASK_SS_FRONT_2;
    DDR_SS_FRONT_3 |= MASK_SS_FRONT_3;

    DISABLE_FRONT_1();
    DISABLE_FRONT_2();
    DISABLE_FRONT_3();

    spi_init_1(SPI_MASTER, SPI_128X);
    sei();


    proto_init(spi_tranceive_1, &spi_state_1.on_completed);

    

    while (1) 
    {
        __asm("nop");

        _delay_ms(30);

        //if (!handle_status())
            //continue;
        //
        //if (state.selected > 2)
            //continue;      

            
        handle_register();
    }
}

