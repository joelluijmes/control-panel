#pragma once

#include <inttypes.h>

#define PROTO_DISPLAY   0

#define PROTO_OK        0
#define PROTO_INV_CRC   -1
#define PROTO_PARTIAL   -2

typedef struct proto_packet_t
{
    uint8_t id;
    uint8_t len;
    
    const uint8_t* payload;
    uint16_t crc;
} proto_packet_t;

typedef int8_t (*tranceive_t)(const uint8_t* tran_buf, uint8_t tran_len, uint8_t* recv_buf, uint8_t recv_len);
typedef void (*completed_t)(const uint8_t* tran_buf, uint8_t tran_len, uint8_t* recv_buf, uint8_t recv_len);

void proto_init(completed_t* pcompleted, tranceive_t tranceive);
void proto_wait(void);
proto_packet_t proto_create(uint8_t id, const uint8_t* payload, uint8_t len);
void proto_tranceive(const proto_packet_t* transmit, proto_packet_t* receive);
