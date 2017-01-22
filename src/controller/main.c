#include "config.h"
#include "frontend.h"

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    frontend_init();    

    while (1) 
    {
        frontend_update();

        _delay_ms(10);
    }
}
