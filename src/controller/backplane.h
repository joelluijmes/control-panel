#pragma once

#include "../models/hpu.h"

void backplane_init(void);
void backplane_read(void);
hpu_t* backplane_hpu();