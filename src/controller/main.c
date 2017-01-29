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

    // update the clock frequency only if the difference is above the threshold or changed mode
    if (prev_mode == state->clock.clock_mode && abs(state->clock.clock_speed - prev_adc) < ADC_THRES)
        return;

    // first map the ADC value to the range, update the timer registers and calculate the frequency to show
    uint8_t scaler = TC_DISABLED_PRE;
    if (state->clock.clock_mode == CLK_SLOW)
    {
        OCR1A = map(state->clock.clock_speed, ADC_MIN, ADC_MAX, CLK_SLOW_MIN, 65535.0);
        scaler = (1 << CS12 | 1 << CS10);   // 1024
        display.clock.clock_speed = F_CPU/(2*1024*(1+OCR1A));
    }
    else if (state->clock.clock_mode == CLK_FAST)
    {
        OCR1A = map(state->clock.clock_speed, ADC_MIN, ADC_MAX, CLK_FAST_MIN, 65535.0);
        scaler = (1 << CS11);               // 8
        display.clock.clock_speed = F_CPU/(2*8*(1+OCR1A));
    }
        
   // TCNT1 = OCR1A/2;
    TCCR1B = (TCCR1B & 0xF8) | scaler;

    prev_mode = state->clock.clock_mode;
    prev_adc = state->clock.clock_speed;
}

void process(void)
{
    backplane_read();

    display.memory.mmio = backplane.memory.mmio_enable;
    display.memory.read = backplane.memory.read;
    display.memory.write = backplane.memory.write;

    instruction.alu.add = !!(backplane.alu == ALU_ADD);
    instruction.alu.sub = !!(backplane.alu == ALU_SUB);
    instruction.alu.and = !!(backplane.alu == ALU_AND);
    instruction.alu.or = !!(backplane.alu == ALU_OR);
    instruction.alu.xor = !!(backplane.alu == ALU_XOR);
    instruction.alu.clr = !!(backplane.alu == ALU_CLEAR);

    process_clock();

    frontend_update();
}

int main(void)
{
    // enable break line
    DDRD |= 1 << 2;
    PORTD = 1 << 2;

    DDRB |= 1 << 1;             // enable output of OC1A
    TCCR1A = 1 << COM1A0;       // Toggle OC1A
    TCCR1B = 1 << WGM12;     // 64 divider
    
    backplane_init();
    frontend_init();  

    front_instruction_set(&instruction);
    front_status_set(&display);

    while (1)
    {
        process();
        _delay_ms(50);
    }
}
