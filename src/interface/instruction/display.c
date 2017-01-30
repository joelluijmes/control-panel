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
        return OPERAND_C;
    else if (val < 250)
        return OPERAND_A;
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
    uint8_t values[3] = { 0xFF };

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
        values[1] = 'R';
        values[2] = (uint8_t)operand->register_op;
        break;

    case REG_PC:
        values[1] = 'P';
        values[2] = 'C';
        break;

    case REG_MDR:
        values[0] = 'M';
        values[1] = 'D';
        values[2] = 'R';
        break;

    case REG_MAR:
        values[0] = 'M';
        values[1] = 'A';
        values[2] = 'R';
        break;

    case REG_STATUS_REG:
        values[0] = 'S';
        values[1] = 'R';
        values[2] = 'G';
        break;

    //case X:
        //max7221_display(1, 0, 0xFF);
        //max7221_display(1, 4, 0xFF);
        //max7221_display(1, 6, 'X');
        //return;

    case REG_Y:
        values[2] = 'Y';
        break;

    case REG_Z:
        values[2] = 'Z';
        break;

    //case STACK_POINTER:
        //max7221_display(1, MAX7221_DIGIT0, 0xFF);
        //max7221_display(1, MAX7221_DIGIT4, 'S');
        //max7221_display(1, MAX7221_DIGIT6, 'P');
        //return;

        default:
            values[2] = (operand->immediate/0x100)%0x10;
            values[1] = (operand->immediate/0x10)%0x10;
            values[0] = operand->immediate%0x10;
            break;
    }    

    max7221_display(1, MAX7221_DIGIT0, values[0]);
    max7221_display(1, MAX7221_DIGIT4, values[1]);
    max7221_display(1, MAX7221_DIGIT6, values[2]);
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
