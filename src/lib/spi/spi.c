#include "spi.h"
#include <avr/io.h>
#include <util/delay.h>

#define MIN(a, b) ((a < b) ? a : b)
#define MAX(a, b) ((a > b) ? a : b)

#if defined(SPCR)
    #define SPI_SINGLE
    #include "single/spi_single.h"
#elif defined(SPCR0) && defined(SPCR1)
    #define SPI_DOUBLE
    #include "double/spi_double.h"
#else
    #error 'Unsupported device'
#endif

int8_t _spi_tranceive(spi_state_t* state)
{
    uint8_t len = MAX(state->tran_len, state->recv_len);
    uint8_t i = 0;
    
    if (state->mode == (SPI_NONBLOCKING | SPI_SLAVE))
    {
        if ((!(*state->spcr & (1 << SPIF))))
        {
            *state->spdr = state->tran_len ? state->tran_buf[0] : 0xFF;
            *state->spcr |= 1 << SPIF;

            return SPI_PENDING;
        }

        *state->spcr &= ~(1 << SPIF);
        ++i;
    }
	
	for (; i < len; ++i)
	{
		uint8_t tran = i < state->tran_len ? state->tran_buf[i] : 0xFF;
	    uint8_t recv = state->tranceive_byte(tran);	
		
		if (i < state->recv_len)
		    state->recv_buf[i] = recv;
	}
	
    if (state->on_completed)
        state->on_completed(state);

    return SPI_OK;
}   

void _spi_isr(spi_state_t* state)
{  
    if (state->recv_len)
        state->recv_buf[0] = *state->spdr;

    _spi_tranceive(state);
}
