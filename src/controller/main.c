#include "config.h"

#include "../models/instruction.h"
#include "../models/status.h"
#include "../lib/spi/double/spi_double.h"
#include "../lib/protos/protos.h"


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

//void handle_register(display_register_t* reg)
//{
    //spi_tranceive_1((uint8_t*)reg, sizeof(display_register_t));
    //while (!spi_completed_1()) ;
//}

static completed_t completed;
static void completed_spi(spi_state_t* state)
{
    completed((uint8_t*)state->tran_buf, state->tran_len, (uint8_t*)state->recv_buf, state->recv_len);
}

int main(void)
{
    DDR_SS_FRONT_1 |= MASK_SS_FRONT_1;
    DDR_SS_FRONT_2 |= MASK_SS_FRONT_2;
    DDR_SS_FRONT_3 |= MASK_SS_FRONT_3;

    DISABLE_FRONT_1();
    DISABLE_FRONT_2();
    DISABLE_FRONT_3();

    spi_init_1(SPI_MASTER, SPI_128X);
    sei();

    //display_register_t reg = { 0 };

    status_display_t status = { 0 };
    status.clock.clock_speed = 123;

    status_state_t state = { 0 };

    proto_packet_t transmit = proto_create(PROTO_DISPLAY, (uint8_t*)&status, sizeof(status));
    proto_packet_t receive = { .payload = (uint8_t*)&state, .len = sizeof(state) };

    spi_install_hook_1(completed_spi);
    proto_init(&completed, spi_tranceive_1);

    while (1) 
    {
        ASSERT_FRONT_1();
        ASSERT_FRONT_2();
        ASSERT_FRONT_3();

        proto_tranceive(&transmit, &receive);
        proto_wait();
        
        DISABLE_FRONT_1();
        DISABLE_FRONT_2();
        DISABLE_FRONT_3();


        _delay_ms(100);
    }
}

