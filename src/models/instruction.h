#pragma once

#include "hpu.h"

typedef struct alu_t
{
    uint16_t empty : 4;
    uint16_t imm1 : 1;
    uint16_t imm2 : 1;
    uint16_t clr : 1;
    uint16_t sub : 1;
    uint16_t add : 1;
    uint16_t xor : 1;
    uint16_t or : 1;
    uint16_t and : 1;
    uint16_t sp : 1;
    uint16_t shr : 1;
    uint16_t sha : 1;
    uint16_t carry : 1;
} alu_t;

typedef enum decoder_t
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
} decoder_t;

typedef struct operand_t
{
    decoder_t register_op : 4;
    uint8_t stack_pointer : 1;
    uint16_t immediate : 12;
} operand_t;

typedef enum condition_t
{
    DISABLED = 0x07,
    SIGN = 'S',
    OVERFLOW = 'V',
    NEGATIVE = 'N',
    ZERO = 'Z',
    CARRY = 'C'
} condition_t;

typedef struct instruction_t
{
    uint16_t mdr;
    uint16_t mar;
    uint16_t pc;
    uint16_t address;
    condition_t condition;

    alu_t alu;
    operand_t operand_a;
    operand_t operand_b;
    operand_t operand_c;

} instruction_t;
