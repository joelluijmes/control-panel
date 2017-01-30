#include "config.h"
#include "frontend.h"
#include "backplane.h"

#include "../lib/protos/protos.h"
#include "../lib/spi/spi.h"
#include "../lib/spi/double/spi_double.h"
#include "../lib/mcp23s17/mcp23s17.h"
#include "../models/status.h"

#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#define ADC_THRES 10
#define ADC_MIN 0
#define ADC_MAX 1023
#define CLK_SLOW_MIN 155
#define CLK_SLOW_MAX (uint16_t)(66535UL)
#define CLK_FAST_MIN 9999
#define CLK_FAST_MAX (uint16_t)(66535UL)
#define TC_FAST_PRE (1 << CS11)
#define TC_DISABLED_PRE 0

static status_display_t display;
static instruction_t instruction;

static uint16_t map(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (uint16_t)((x - in_min)/(in_max - in_min) * (out_max - out_min) + out_min);
}

static void process_clock(void)
{
    volatile status_state_t* state = front_status_get();

    static clock_mode_t prev_mode = CLK_HALT;
    static int16_t prev_adc = 0;
    static uint8_t prev_step = 0;

    // update the clock frequency only if the difference is above the threshold or changed mode
    if (prev_mode == state->clock.clock_mode && abs(state->clock.clock_speed - prev_adc) < ADC_THRES)
        return;

    // for auto, first map the ADC value to the range, update the timer registers and calculate the frequency to show
    // manual, disable the output register (it blocks us from writing it as normal IO) then set it high
    uint8_t scaler = TC_DISABLED_PRE;
    if (state->clock.clock_mode == CLK_SLOW)
    {
        TCCR1A |= 1 << COM1A0;
        OCR1A = map(state->clock.clock_speed, ADC_MIN, ADC_MAX, CLK_SLOW_MIN, 65535.0);
        scaler = (1 << CS12 | 1 << CS10);   // 1024
        display.clock.clock_speed = F_CPU/(2*1024*(1+OCR1A));
    }
    else if (state->clock.clock_mode == CLK_FAST)
    {
        TCCR1A |= 1 << COM1A0;
        OCR1A = map(state->clock.clock_speed, ADC_MIN, ADC_MAX, CLK_FAST_MIN, 65535.0);
        scaler = (1 << CS11);               // 8
        display.clock.clock_speed = F_CPU/(2*8*(1+OCR1A));
    }
    else if (state->clock.clock_mode == CLK_MANUAL)
    {
        TCCR1A &= ~(1 << COM1A0);
        if (state->step && prev_step == 0)
            CLK_PORT |= CLK_MASK;
    }
    
    // TCNT1 = OCR1A/2;
    TCCR1B = (TCCR1B & 0xF8) | scaler;

    prev_mode = state->clock.clock_mode;
    prev_adc = state->clock.clock_speed;
    prev_step = state->step;
}

static register_display_t registers[] =
{
    { .value = 0x000 },
    { .value = 0x100 },
    { .value = 0x200 },
    { .value = 0x300 },
    { .value = 0x400 },
    { .value = 0x500 },
    { .value = 0x600 },
    { .value = 0x700 },
    { .value = 0x011 },
    { .value = 0x111 },
    { .value = 0x211 },
    { .value = 0x311 },
    { .value = 0x411 },
    { .value = 0x511 },
    { .value = 0x611 },
    { .value = 0x711 }
};

static void process_register(void)
{
    // TODO: handle on updated registers
    
    registers[backplane.target_decoder].value = backplane.c_bus;

    // if on the status PCB, we selected a different register to display
    static uint8_t prev_selected = -1;
    status_state_t* state = front_status_get();
    if (prev_selected == state->selected)
        return;

    // set it to display on the register board
    register_display_t* reg = &registers[state->selected];
    reg->updated = 1;
    front_register_set(reg);

    prev_selected = state->selected;
}

static void process_instruction(void)
{
    // TODO incorperate the stackpointer
    
    instruction_t* instruction = front_instruction_get();

    instruction->operand_a.register_op = (decoder_t)backplane.left_decoder;
    if (backplane.right_decoder <= REGB_R7)
        instruction->operand_b.register_op = (decoder_t)backplane.right_decoder;
    else if (backplane.right_decoder == REGB_EXT_IMM || backplane.right_decoder == REGB_INT_IMM)
        instruction->operand_b.register_op = REG_DISABLED;

    instruction->operand_c.register_op = (decoder_t)backplane.target_decoder;
    __asm("nop");
}

void process(void)
{
    backplane_read();

    display.memory.mmio = backplane.memory.mmio_enable;
    display.memory.read = backplane.memory.read;
    display.memory.write = backplane.memory.write;

    instruction.alu.add = !!(backplane.alu == ALU_ADD);
    instruction.alu.sub = !!(backplane.alu == ALU_SUB || backplane.alu == ALU_REV_SUB);
    instruction.alu.and = !!(backplane.alu == ALU_AND);
    instruction.alu.or = !!(backplane.alu == ALU_OR);
    instruction.alu.xor = !!(backplane.alu == ALU_XOR);
    instruction.alu.clr = !!(backplane.alu == ALU_CLEAR);

    process_clock();
    process_register();
    process_instruction();

    frontend_update();
}

int main(void)
{
    // enable break line
    BREAK_DDR |= BREAK_MASK;
    BREAK_PORT |= BREAK_MASK;

    // set the DDR of OC1A as output, toggle the output pin, and use CTC mode
    CLK_DDR |= CLK_MASK;
    TCCR1B = 1 << WGM12;

    backplane_init();
    frontend_init();

    front_instruction_set(&instruction);
    front_status_set(&display);

    while (1)
    {
        process();
        _delay_ms(10);

        // be sure to set the clock low if needed
        if (front_status_get()->clock.clock_mode == CLK_MANUAL)
            CLK_PORT &= ~CLK_MASK;

        _delay_ms(50);
    }
}
