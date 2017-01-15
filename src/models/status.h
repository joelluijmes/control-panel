#pragma once

typedef struct sreg_t
{
    uint8_t enable      : 1;
    uint8_t sign        : 1;
    uint8_t overflow    : 1;
    uint8_t negative    : 1;
    uint8_t zero        : 1;
    uint8_t carry       : 1;
} sreg_t;

typedef struct memory_t
{
    uint8_t enable  : 1;
    uint8_t mmio    : 1;
    uint8_t write   : 1;
    uint8_t read    : 1;
} memory_t;

typedef enum clock_mode_t
{
    SLOW,
    MANUAL,
    FAST
} clock_mode_t;

typedef struct clock_t
{
    clock_mode_t clock_mode : 2;
    uint16_t clock_speed : 14;  
} clock_t;

typedef struct status_display_t
{
    sreg_t status;
    memory_t memory;
    clock_t clock;    
} status_display_t;

typedef struct status_state_t
{
    uint8_t step : 1;
    clock_t clock;
} status_state_t;