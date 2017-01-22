#pragma once

#include <inttypes.h>

typedef enum hpu_decoder_t
{
    R0,
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    PC,
    MDR,
    MAR,
    STATUS_REG,
    Y,  
    Z,
    X,
    DISABLED
} hpu_decoder_t;

typedef struct hpu_t
{
    uint16_t a_bus : 12;
    uint16_t b_bus : 12;
    uint16_t c_bus : 12;

    hpu_decoder_t target_decoder : 4;
} hpu_t;