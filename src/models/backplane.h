#pragma once

#include "hpu.h"
#include <inttypes.h>


typedef struct backplane_t
{
    uint16_t c_bus : 12;
    hpu_decoder_t target_decoder : 4;
    hpu_condition_t condition : 3;
    uint8_t condition_inverted : 1;
    uint8_t status_writable : 1;
    hpu_alu_t alu : 3;
    uint8_t carry : 1;
    uint8_t sra : 1;
    uint8_t srl : 1;
    hpu_memory_t memory;
    hpu_decoder_t left_decoder : 4;
    hpu_b_decoder_t right_decoder : 4;
    uint8_t status_enable : 1;
    uint8_t sp_write : 1;
    uint8_t reserved : 4;
} backplane_t;