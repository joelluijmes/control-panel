#include "protos.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/crc16.h>

#define PROTO_HEADER sizeof(uint8_t)*2

static uint16_t calculate_crc(const proto_packet_t* packet);

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

proto_packet_t proto_create(uint8_t id, const uint8_t* payload, uint8_t len)
{
    proto_packet_t packet = proto_create_empty(payload, len);
    packet.id = id;
    packet.crc = calculate_crc(&packet);

    return packet;
}

proto_packet_t proto_create_empty(const uint8_t* payload, uint8_t len)
{
    proto_packet_t packet =
    {
        .len = len,
        .payload = (uint8_t*)payload
    };

    return packet;
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
            receive->id = buf[0];
            if (buf[1] > receive->len)
                receive->partial = 1;
            else if (buf[1] < receive->len)
                receive->len = buf[1];

            state.status = PAYLOAD;
            state.tranceive(transmit->payload, transmit->len, receive->payload, receive->len);
            return;
        }

        case PAYLOAD:
        {
            state.status = FOOTER;
            state.tranceive((uint8_t*)&transmit->crc, sizeof(uint16_t), (uint8_t*)&receive->crc, sizeof(uint16_t));
            return;
        }

        case FOOTER:
        {
            *(uint16_t*)state.buf = calculate_crc(receive) ;
            
            state.status = CHECKSUM;
            state.tranceive(state.buf, sizeof(uint16_t), state.buf, sizeof(uint16_t));

            return;
        }

        case CHECKSUM:
        {
            state.status = *(uint16_t*)state.buf == transmit->crc
                ? IDLE
                : FAILED;

            return; 
        }
    }
}

int8_t proto_tranceive(const proto_packet_t* transmit, proto_packet_t* receive)
{
	//if (state.status != IDLE)
	    //return PROTO_BUSY;

    *state.on_completed = completed;
    state.transmit = transmit;
    state.receive = receive;

    state.status = HEADER;
    return state.tranceive((uint8_t*)&transmit->id, PROTO_HEADER, state.buf, PROTO_HEADER);
}