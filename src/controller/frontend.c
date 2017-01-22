#include "frontend.h"
#include "config.h"

#include "../lib/spi/double/spi_double.h"
#include "../lib/protos/protos.h"
#include "../models/hpu.h"
#include "../models/instruction.h"
#include "../models/status.h"
#include "../models/register.h"

#include <avr/io.h>
#include <util/delay.h>

#define ASSERT_FRONT1() (FRONT_SS1_PORT &= ~FRONT_SS1_MASK)
#define DISABLE_FRONT1() (FRONT_SS1_PORT |= FRONT_SS1_MASK)
#define ASSERT_FRONT2() (FRONT_SS2_PORT &= ~FRONT_SS2_MASK)
#define DISABLE_FRONT2() (FRONT_SS2_PORT |= FRONT_SS2_MASK)
#define ASSERT_FRONT3() (FRONT_SS3_PORT &= ~FRONT_SS3_MASK)
#define DISABLE_FRONT3() (FRONT_SS3_PORT |= FRONT_SS3_MASK)

// ------ STATUS BOARD ------
static status_state_t status_state = { .selected = 0 };
static status_display_t status_display;

static uint8_t update_status(void)
{
    // placeholder
    status_state_t recv_state;

    // Assert the CS to speak to the right board
    ASSERT_FRONT2();
    ASSERT_FRONT3();

    // create the packets to tranceive
    proto_packet_t transmit = proto_create(1, (uint8_t*)&status_display, sizeof(status_display_t));
    proto_packet_t receive = proto_create_empty((uint8_t*)&recv_state, sizeof(status_state_t));

    // do the tranceive (blocking)
    proto_tranceive(&transmit, &receive);

    // done communicating
    DISABLE_FRONT2();
    DISABLE_FRONT3();

    // get the status of the tranceivement (IDLE -> success; FAILED -> incorrect CRC)
    proto_status_t status = proto_status();

    // update with the received state
    if (status == IDLE && recv_state.selected < 4)
    {
        status_display.clock = recv_state.clock;
        status_state = recv_state;
    }

    // return if tranceivement went OK
    return status == IDLE;
}
// ------ END STATUS BOARD ------


// ------ REGISTER BOARD ------
static register_display_t r1 = { .value = 0x100 };
static register_display_t r2 = { .value = 0x200 };
static register_display_t r3 = { .value = 0x300 };
static register_display_t r4 = { .value = 0x400 };


static uint8_t update_register(uint8_t selected)
{
    static uint8_t prev_selected = -1;
    static register_display_t* registers[] = { &r1, &r2, &r3, &r4 } ;

    register_display_t tmp;
    register_display_t* reg = registers[selected];
    reg->updated = prev_selected != selected;

    // Assert the CS to speak to the right board
    ASSERT_FRONT1();
    ASSERT_FRONT2();

    // create the packets to tranceive
    proto_packet_t transmit = proto_create(2, (uint8_t*)reg, sizeof(register_display_t));
    proto_packet_t receive = proto_create_empty((uint8_t*)&tmp, sizeof(register_display_t));

    // do the tranceive (blocking)
    proto_tranceive(&transmit, &receive);
    
    // done communicating
    DISABLE_FRONT1();
    DISABLE_FRONT2();
    
    // get the status of the tranceivement (IDLE -> success; FAILED -> incorrect CRC)
    proto_status_t status = proto_status();
    if (status == IDLE)
    {
        // if the register board pressed the button, update the register
        reg->updated = 0;
        if (tmp.updated)
            reg->value = tmp.value;

        prev_selected = selected;
    }    

    // return if tranceivement went OK
    return status == IDLE;
}
// ------ END REGISTER BOARD ------

static uint8_t delayed_tranceive_byte(uint8_t tran)
{
    // we have to delay quite a bit, otherwise the ATtinies can't keep up with us..
    _delay_us(300);

    return spi1_tranceive_byte(tran);
}

// ------ PUBLIC ------
void frontend_init(void)
{
    // init the Slave Selects pins
    FRONT_SS1_DDR |= FRONT_SS1_MASK;
    FRONT_SS2_DDR |= FRONT_SS2_MASK;
    FRONT_SS3_DDR |= FRONT_SS3_MASK;

    DISABLE_FRONT1();
    DISABLE_FRONT2();
    DISABLE_FRONT3();

    // boards are connected on the SPI1 bus
    spi1_init(SPI_MASTER, SPI_128X);
    // hook into the SPI to delay the master
    spi1_state.tranceive_byte = delayed_tranceive_byte;
    proto_init(spi1_tranceive, &spi1_state.on_completed);
}

void frontend_update(void)
{
    update_status();
    _delay_ms(1);
    update_register(status_state.selected);
    _delay_ms(1);
}
// ------ END PUBLIC ------