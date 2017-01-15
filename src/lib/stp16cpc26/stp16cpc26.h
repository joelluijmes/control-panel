#pragma once
#include "config.h"

void stp16_init(void);
void stp16_output(uint8_t enable);
void stp16_latch(void);
void stp16_shift(uint16_t val);
