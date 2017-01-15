#include "protos.h"

#include <util/crc16.h>

#define PROTO_HEADER sizeof(uint8_t)*2

static uint16_t calculate_crc(const proto_packet_t* packet);
static void completed(const uint8_t* buf, uint8_t len);

typedef enum status_t 
{
    HEADER,
    PAYLOAD,
    FOOTER
} status_t;

typedef struct state_t 
{
    volatile status_t status : 2;
    volatile uint8_t running : 1;

    volatile proto_packet_t* transmit;
    volatile proto_packet_t* receive;


    tranceive_t tranceive;
} state_t;

state_t state;

void proto_init(completed_t* pcompleted, tranceive_t tranceive)
{
    *pcompleted = completed;
    state.tranceive = tranceive;
}

void proto_wait(void)
{
    while (state.running) ;
}

proto_packet_t proto_create(uint8_t id, const uint8_t* payload, uint8_t len)
{
    proto_packet_t packet =
    {
        .id = id,
        .len = len,
        .payload = payload
    };

    packet.crc = calculate_crc(&packet);
    return packet;
}

void proto_tranceive(const proto_packet_t* transmit, proto_packet_t* receive)
{
    if (state.running)
        return;

    // start by transmitting the header
    state.status = HEADER;
    state.running = 1;
    state.transmit = transmit;
    state.receive = receive;

    state.tranceive((uint8_t*)&state.transmit->id, PROTO_HEADER, (uint8_t*)state.receive, PROTO_HEADER);    
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

static void handle_header(const uint8_t* buf, uint8_t len)
{
    volatile uint8_t packet_id = buf[0];
    volatile uint8_t packet_len = buf[1];
    
    state.tranceive(state.transmit->payload, state.transmit->len, state.receive->payload, state.receive->len);
    
    state.status = PAYLOAD;
}

static void handle_payload(const uint8_t* buf, uint8_t len)
{
    state.tranceive((uint8_t*)&state.transmit->crc, sizeof(uint16_t), (uint8_t*)&state.receive->crc, sizeof(uint16_t));
    state.status = FOOTER;
}

static void handle_footer(const uint8_t* buf, uint8_t len)
{
    state.running = 0;
}

static void completed(const uint8_t* buf, uint8_t len)
{
    switch (state.status)
    {
        case HEADER:
            handle_header(buf, len);
            return;

        case PAYLOAD:
            handle_payload(buf, len);
            return;

        case FOOTER:
            handle_footer(buf, len);
            return;

        default:
            return;
    }
}