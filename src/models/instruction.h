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

//typedef enum registers_t
//{
    //R0,
    //R1,
    //R2,
    //R3,
    //R4,
    //R5,
    //R6,
    //R7,
    //PC,
    //MDR,
    //MAR,
    //STATUS_REG,
    //Y,
    //Z,
    //STACK_POINTER,
    //REGISTER_DISABLED = 0x0F,
//} registers_t;

typedef struct operand_t
{
    hpu_decoder_t register_op : 4;
    uint8_t stack_pointer : 1;
    uint16_t immediate : 12;
} operand_t;

typedef enum condition_t
{
    CONDITION_DISABLED = 0,
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
