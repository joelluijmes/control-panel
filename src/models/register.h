#pragma once

typedef struct register_display_t
{
    uint16_t value : 12;
    uint8_t updated : 1;
} register_display_t;
