#include "display.h"

#include "../../lib/max7221/max7221.h"
#include "../../lib/stp16cpc26/stp16cpc26.h"

#define OPERAND_A 0
#define OPERAND_B 1
#define OPERAND_C 2


static uint8_t read_switch()
{
    ADCSRA |= 1 << ADSC;            // start adc
    
    while (ADCSRA & (1 << ADSC)) ;  // = 1 as long conversion is in progress
    uint16_t val = ADC;

    if (val > 750)
        return OPERAND_A;
    else if (val < 250)
        return OPERAND_C;
    else
        return OPERAND_B;
}

//static alu_t alu = { 0 };
//static instruction_t display =
//{
    //.condition = CONDITION_DISABLED,
    //.alu = &alu
//};

static inline void print_mar(uint16_t val)
{
    max7221_display(0, MAX7221_DIGIT7, val%0x10);
    max7221_display(0, MAX7221_DIGIT3, (val/0x10)%0x10);
    max7221_display(0, MAX7221_DIGIT2, (val/0x100)%0x10);
}

static inline void print_pc(uint16_t val)
{
    max7221_display(0, MAX7221_DIGIT6, val%0x10);
    max7221_display(0, MAX7221_DIGIT4, (val/0x10)%0x10);
    max7221_display(0, MAX7221_DIGIT0, (val/0x100)%0x10);
}

static inline void print_mdr(uint16_t val)
{
    max7221_display(1, MAX7221_DIGIT1, val%0x10);
    max7221_display(0, MAX7221_DIGIT1, (val/0x10)%0x10);
    max7221_display(0, MAX7221_DIGIT5, (val/0x100)%0x10);
}

static inline void print_address(uint16_t val)
{
    max7221_display(1, MAX7221_DIGIT5, val%0x10);
    max7221_display(1, MAX7221_DIGIT7, (val/0x10)%0x10);
    max7221_display(1, MAX7221_DIGIT3, (val/0x100)%0x10);
}

static inline void print_alu(const alu_t* const alu)
{
    stp16_shift(*(uint16_t*)alu);
    stp16_latch();
}

static inline void print_condition(condition_t condition)
{
    max7221_display(1, MAX7221_DIGIT2, (uint8_t)condition);
}

static inline void print_operand(const operand_t* const operand)
{
    switch (operand->register_op)
    {
    case REG_R0:
    case REG_R1:
    case REG_R2:
    case REG_R3:
    case REG_R4:
    case REG_R5:
    case REG_R6:
    case REG_R7:
        max7221_display(1, MAX7221_DIGIT0, 0xFF);
        max7221_display(1, MAX7221_DIGIT4, 'R');   // R
        max7221_display(1, MAX7221_DIGIT6, (uint8_t)operand->register_op);
        return;

    case REG_PC:
        max7221_display(1, MAX7221_DIGIT0, 0xFF);
        max7221_display(1, MAX7221_DIGIT4, 'P');
        max7221_display(1, MAX7221_DIGIT6, 'C');
        return;

    case REG_MDR:
        max7221_display(1, MAX7221_DIGIT0, 'M');
        max7221_display(1, MAX7221_DIGIT4, 'D');
        max7221_display(1, MAX7221_DIGIT6, 'R');
        return;

    case REG_MAR:
        max7221_display(1, MAX7221_DIGIT0, 'M');
        max7221_display(1, MAX7221_DIGIT4, 'A');
        max7221_display(1, MAX7221_DIGIT6, 'R');
        return;

    case REG_STATUS_REG:
        max7221_display(1, MAX7221_DIGIT0, 'S');
        max7221_display(1, MAX7221_DIGIT4, 'R');
        max7221_display(1, MAX7221_DIGIT6, 'G');
        return;

    //case X:
        //max7221_display(1, 0, 0xFF);
        //max7221_display(1, 4, 0xFF);
        //max7221_display(1, 6, 'X');
        //return;

    case REG_Y:
        max7221_display(1, MAX7221_DIGIT0, 0xFF);
        max7221_display(1, MAX7221_DIGIT4, 0xFF);
        max7221_display(1, MAX7221_DIGIT6, 'Y');
        break;

    case REG_Z:
        max7221_display(1, MAX7221_DIGIT0, 0xFF);
        max7221_display(1, MAX7221_DIGIT4, 0xFF);
        max7221_display(1, MAX7221_DIGIT6, 'Z');
        return;

    //case STACK_POINTER:
        //max7221_display(1, MAX7221_DIGIT0, 0xFF);
        //max7221_display(1, MAX7221_DIGIT4, 'S');
        //max7221_display(1, MAX7221_DIGIT6, 'P');
        //return;
    }    

    max7221_display(1, MAX7221_DIGIT6, operand->immediate%0x10);
    max7221_display(1, MAX7221_DIGIT4, (operand->immediate/0x10)%0x10);
    max7221_display(1, MAX7221_DIGIT0, (operand->immediate/0x100)%0x10);
}

void display_update(const instruction_t* const instruction)
{
    print_mar(instruction->mar);
    print_mdr(instruction->mdr);
    print_pc(instruction->pc);
    print_address(instruction->address);
    
    print_alu(&instruction->alu);
    print_condition(instruction->condition);

    volatile uint8_t sw = read_switch();
    if (sw == OPERAND_A)
        print_operand(&instruction->operand_a);
    else if (sw == OPERAND_B)
        print_operand(&instruction->operand_b);
    else
        print_operand(&instruction->operand_c);
}
