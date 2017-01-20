#pragma once

#include <inttypes.h>
#include "config.h"

typedef int8_t (*tranceive_t)(const uint8_t* tran_buf, uint8_t tran_len, uint8_t* recv_buf, uint8_t recv_len);

void mcp23_init(tranceive_t tranceive);
void mcp23_ddr(uint8_t addr, uint16_t mask);
void mcp23_pullup(uint8_t addr, uint16_t mask);
void mcp23_invert_input(uint8_t addr, uint16_t mask);
void mcp23_output(uint8_t addr, uint16_t mask);
uint16_t mcp23_input(uint8_t addr);