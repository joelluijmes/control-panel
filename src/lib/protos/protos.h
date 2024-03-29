#pragma once

#include <inttypes.h>

#define PROTO_DISPLAY   0

#define PROTO_OK        0
#define PROTO_BUSY      -1
#define PROTO_PENDING   -2
#define PROTO_PARTIAL   -3
#define PROTO_INV_CRC   -4
#define PROTO_INV_ARG   -5

typedef struct proto_packet_t
{
    uint8_t id;
    uint8_t len;
    
    uint8_t* payload;
    uint16_t crc;
	
	uint8_t partial : 1;
} proto_packet_t;

typedef enum proto_status_t
{
    IDLE,
    FAILED,
    HEADER,
    PAYLOAD,
    FOOTER

} proto_status_t;

typedef int8_t (*tranceive_t)(const uint8_t* tran_buf, uint8_t tran_len, uint8_t* recv_buf, uint8_t recv_len);
typedef void (*completed_t)();

void proto_init(tranceive_t tranceive, completed_t* on_completed);
void proto_reset(void);
int8_t proto_completed();
proto_status_t proto_status();
proto_packet_t proto_create(uint8_t id, uint8_t* payload, uint8_t len);
int8_t proto_tranceive(const proto_packet_t* transmit, proto_packet_t* receive);
void proto_update_crc(proto_packet_t* packet);

static inline int8_t proto_receive(proto_packet_t* receive)
{
    return proto_tranceive(0, receive);
}

static inline int8_t proto_transmit(const proto_packet_t* transmit)
{
    return proto_tranceive(transmit, 0);
}
