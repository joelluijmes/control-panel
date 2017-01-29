#pragma once

#include "../models/hpu.h"
#include "../models/instruction.h"
#include "../models/status.h"
#include "../models/register.h"

void frontend_init(void);
void frontend_update(void);

register_display_t front_register_get();
void front_register_set(register_display_t* reg);

instruction_t* front_instruction_get();
void front_instruction_set(instruction_t* instruction);

status_state_t* front_status_get();
void front_status_set(status_display_t* display);