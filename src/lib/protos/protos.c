#include "protos.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/crc16.h>

#define PROTO_HEADER sizeof(uint8_t)*2
#define BUFSIZE 32

static uint16_t calculate_crc(const proto_packet_t* packet);
static void completed();

typedef struct state_t 
{
    proto_status_t status : 3;

    uint8_t buf[PROTO_HEADER];

    const proto_packet_t* transmit;
    proto_packet_t* receive;

    tranceive_t tranceive;
    completed_t* on_completed;
} state_t;

state_t state;

void proto_init(tranceive_t tranceive, completed_t* on_completed)
{
    state.tranceive = tranceive;
    state.on_completed = on_completed;
}

void proto_reset(void)
{
    state.status = IDLE;
    state.transmit = 0;
    state.receive = 0;
}

int8_t proto_completed()
{
    return state.status == IDLE;
}

proto_status_t proto_status()
{
    return state.status;
}

proto_packet_t proto_create(uint8_t id, uint8_t* payload, uint8_t len)
{
    proto_packet_t packet =
    {
        .id = id,
        .payload = payload,
        .len = len
    };

    return packet;
}

void proto_update_crc(proto_packet_t* packet)
{
    packet->crc = calculate_crc(packet);
}

int8_t proto_tranceive(const proto_packet_t* transmit, proto_packet_t* receive)
{
    //if (state.status != IDLE)
    //return PROTO_BUSY;

    *state.on_completed = completed;
    state.transmit = transmit;
    state.receive = receive;

    state.status = HEADER;

    // tranceive
    if (transmit != 0 && receive != 0)
        return state.tranceive((uint8_t*)&transmit->id, PROTO_HEADER, state.buf, PROTO_HEADER);
    // receive only
    if (transmit == 0 && receive != 0)
        return state.tranceive(0, 0, state.buf, PROTO_HEADER);
    // transmit only
    if (transmit != 0 && receive == 0)
        return state.tranceive((uint8_t*)&transmit->id, PROTO_HEADER, 0, 0);

    state.status = FAILED;
    return PROTO_INV_ARG;
}

static uint16_t calculate_crc(const proto_packet_t* packet)
{
    uint16_t crc = 0xFFFF;
    crc = _crc_ccitt_update(crc, packet->id);
    crc = _crc_ccitt_update(crc, packet->len);

    for (uint8_t i = 0; i < packet->len; ++i)
        crc = _crc_ccitt_update(crc, packet->payload[i]);

    return crc;
}

static void completed()
{
    const proto_packet_t* transmit = state.transmit;
    proto_packet_t* receive = state.receive;
    uint8_t* buf = state.buf;

    switch (state.status)
    {
        case HEADER:
        {
            if (receive)
            {
                // not the expected id
                if (receive->id != buf[0])
                {
                    state.status = FAILED;
                    return;
                }
                
                if (buf[1] > receive->len)
                    receive->partial = 1;
                else if (buf[1] < receive->len)
                    receive->len = buf[1];
            }

            state.status = PAYLOAD;

            // tranceive
            if (transmit != 0 && receive != 0)
                state.tranceive(transmit->payload, transmit->len, receive->payload, receive->len);
            // receive only
            else if (transmit == 0 && receive != 0)
                state.tranceive(0, 0, receive->payload, receive->len);
            // transmit only
            else if (transmit != 0 && receive == 0)
                state.tranceive(transmit->payload, transmit->len, 0, 0);

            return;
        }

        case PAYLOAD:
        {
            state.status = FOOTER;

            // tranceive
            if (transmit != 0 && receive != 0)
                state.tranceive((uint8_t*)&transmit->crc, sizeof(uint16_t), (uint8_t*)&receive->crc, sizeof(uint16_t));
            // receive only
            else if (transmit == 0 && receive != 0)
                state.tranceive(0, 0, (uint8_t*)&receive->crc, sizeof(uint16_t));
            // transmit only
            else if (transmit != 0 && receive == 0)
                state.tranceive((uint8_t*)&transmit->crc, sizeof(uint16_t), 0, 0);

            return;
        }

        case FOOTER:
        {
            __asm("nop");

            state.status = IDLE;
            if (receive)
            {
                state.status = receive->crc == calculate_crc(receive)
                    ? IDLE
                    : FAILED;
            }
            
            __asm("nop");
            return;
        }
    }
}