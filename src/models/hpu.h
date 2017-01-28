#pragma once

#include <inttypes.h>

typedef enum hpu_decoder_t
{
    REG_R0,
    REG_R1,
    REG_R2,
    REG_R3,
    REG_R4,
    REG_R5,
    REG_R6,
    REG_R7,
    REG_PC,
    REG_MDR,
    REG_MAR,
    REG_STATUS_REG,
    REG_Y,
    REG_Z,
    REG_X,
    REG_DISABLED
} hpu_decoder_t;

typedef enum hpu_b_decoder_t
{
    REGB_R0,
    REGB_R1,
    REGB_R2,
    REGB_R3,
    REGB_R4,
    REGB_R5,
    REGB_R6,
    REGB_R7,
    REGB_EXT_IMM = 0x0C,
    REGB_INT_IMM = 0x0E,
    REGB_DISABLED = 0x0F
} hpu_b_decoder_t;

typedef enum hpu_condition_t
{
    COND_CARRY,
    COND_SIGN,
    COND_OVERFLOW,
    COND_NEGATIVE,
    COND_ZERO,
    
    COND_DISABLED
} hpu_condition_t;

typedef enum hpu_alu_t
{
    ALU_CLEAR,
    ALU_REV_SUB,
    ALU_SUB,
    ALU_ADD,
    ALU_XOR,
    ALU_OR,
    ALU_AND,
    ALU_PRESET,
} hpu_alu_t;

typedef struct hpu_memory_t
{
    uint8_t mmio_write : 1;
    uint8_t mmio_enable : 1;
    uint8_t mmio_ready : 1;
    uint8_t read : 1;
    uint8_t write : 1;   
} hpu_memory_t;

typedef struct hpu_registers_t
{
    uint8_t R0;
    uint8_t R1;
    uint8_t R2;
    uint8_t R3;
    uint8_t R4;
    uint8_t R5;
    uint8_t R6;
    uint8_t R7;
    uint16_t MDR : 12;
    uint16_t MAR : 12;
    uint16_t Y : 12;
    uint16_t Z : 12;
    uint16_t X : 12;
} hpu_registers_t;

typedef struct hpu_state_t
{
    hpu_registers_t registers;

} hpu_state_t;