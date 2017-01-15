#pragma once
#include "config.h"
#include "../../models/instruction.h"

void display_update(const instruction_t* const instruction);
instruction_t* const display_get(void);