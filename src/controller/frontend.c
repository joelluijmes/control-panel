#include "frontend.h"
#include "backplane.h"

#include "../lib/spi/double/spi_double.h"
#include "../lib/protos/protos.h"

#include <avr/io.h>
#include <util/delay.h>

#define ASSERT_FRONT1() (FRONT_SS1_PORT &= ~FRONT_SS1_MASK)
#define DISABLE_FRONT1() (FRONT_SS1_PORT |= FRONT_SS1_MASK)
#define ASSERT_FRONT2() (FRONT_SS2_PORT &= ~FRONT_SS2_MASK)
#define DISABLE_FRONT2() (FRONT_SS2_PORT |= FRONT_SS2_MASK)
#define ASSERT_FRONT3() (FRONT_SS3_PORT &= ~FRONT_SS3_MASK)
#define DISABLE_FRONT3() (FRONT_SS3_PORT |= FRONT_SS3_MASK)


// ------ REGISTER BOARD ------
static register_display_t* reg_displaying;
static register_display_t reg_receiving;

register_display_t front_register_get()
{
    return reg_receiving;
}

void front_register_set(register_display_t* reg)
{
    reg_displaying = reg;
}

static uint8_t update_register()
{
    if (!reg_displaying)
        return FAILED;

    static register_display_t tmp_reg;

    // Assert the CS to speak to the right board
    ASSERT_FRONT1();

    // create the packets to tranceive
    proto_packet_t transmit = proto_create(10, (uint8_t*)reg_displaying, sizeof(register_display_t));
    proto_update_crc(&transmit);

    proto_packet_t receive = proto_create(1, (uint8_t*)&tmp_reg, sizeof(register_display_t));

    // do the tranceive (blocking)
    proto_tranceive(&transmit, &receive);
    
    // done communicating
    DISABLE_FRONT1();
    
    // get the status of the tranceivement (IDLE -> success; FAILED -> incorrect CRC)
    proto_status_t status = proto_status();
    if (status == IDLE)
        reg_receiving = tmp_reg;

    // debug
    if (status == FAILED)
        __asm("nop");

    // return if tranceivement went OK
    return status == IDLE;
}

// ------ END REGISTER BOARD ------

// ------ INSTRUCTION BOARD ------
static instruction_t* instruction;

instruction_t* front_instruction_get()
{
    return instruction;
}

void front_instruction_set(instruction_t* instr)
{
    instruction = instr;
}

static uint8_t update_instruction(void)
{
    // Assert the CS to speak to the right board
    ASSERT_FRONT2();

    // create the packets to tranceive
    proto_packet_t transmit = proto_create(20, (uint8_t*)instruction, sizeof(instruction_t));
    proto_update_crc(&transmit);

    // do the tranceive (blocking)
    proto_transmit(&transmit);
        
    // done communicating
    DISABLE_FRONT2();
        
    // get the status of the tranceivement (IDLE -> success; FAILED -> incorrect CRC)
    proto_status_t status = proto_status();

    // return if tranceivement went OK
    return status == IDLE;
}
// ------ END INSTRUCTION BOARD ------

// ------ STATUS BOARD ------
static status_state_t status_state;
static status_display_t* status_display;

status_state_t* front_status_get()
{
    return &status_state;
}

void front_status_set(status_display_t* display)
{
    status_display = display;
}

static uint8_t update_status(void)
{
    // placeholder
    status_state_t tmp_state;

    // Assert the CS to speak to the right board
    ASSERT_FRONT3();

    // create the packets to tranceive
    proto_packet_t transmit = proto_create(30, (uint8_t*)&status_display, sizeof(status_display_t));
    proto_update_crc(&transmit);

    proto_packet_t receive = proto_create(3, (uint8_t*)&tmp_state, sizeof(status_state_t));

    // do the tranceive (blocking)
    proto_tranceive(&transmit, &receive);

    // done communicating
    DISABLE_FRONT3();

    // get the status of the tranceivement (IDLE -> success; FAILED -> incorrect CRC)
    proto_status_t status = proto_status();

    // update with the received state
    if (status == IDLE)
        status_state = tmp_state;

    // return if tranceivement went OK
    return status == IDLE;
}
// ------ END STATUS BOARD ------

static uint8_t delayed_tranceive_byte(uint8_t tran)
{
    // we have to delay quite a bit, otherwise the ATtinies can't keep up with us..
    _delay_us(500);

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
    update_register();
    //update_instruction();
    update_status();
}
// ------ END PUBLIC ------