#include "spi.h"
#include <avr/io.h>
#include <util/delay.h>

#define MIN(a, b) ((a < b) ? a : b)

#if defined(SPCR)
    #define SPI_SINGLE
    #include "single/spi_single.h"
#elif defined(SPCR0) && defined(SPCR1)
    #define SPI_DOUBLE
    #include "double/spi_double.h"
#else
    #error 'Unsupported device'
#endif

int8_t _spi_tranceive(spi_state_t* state, const uint8_t* tran_buf, uint8_t tran_len, uint8_t* recv_buf, uint8_t recv_len)
{
    if (state->running)
        return SPI_ALREADY_STARTED;

    //if (!buffer || !len)
    //{
        //if (state->hook)
            //state->hook(state);
    //}
    //else
    {
        state->tran_buf = tran_buf;
        state->tran_len = tran_len;
        state->recv_buf = recv_buf;
        state->recv_len = recv_len;
        state->len = MIN(tran_len, recv_len);

        state->index = 0;
        state->running = 1;
    
        if (tran_buf && tran_len > 0)
            *state->spdr = tran_buf[0];    // start  by transmitting the first byte
        else if (state->mode == SPI_MASTER)
            *state->spdr = 0xFF;
    }

    return SPI_OK;
}   

void _spi_isr(spi_state_t* state)
{
    PORTA |= 1 << 3;
    if (!state->running)
        return;

    uint8_t recv = *state->spdr;               // read the received byte

    if (++state->index >= state->len)       // completed
    {
        state->running = 0;
            
        if (state->hook)
            state->hook(state);
    }
    else
    {
        //_delay_us(5);
        *state->spdr = state->index < state->tran_len ? state->tran_buf[state->index] : 0xFF;    // transmit next byte
    }
        
    if (state->index < state->recv_len)
        state->recv_buf[state->index - 1] = recv;       // store the received byte

    PORTA &= ~(1 << 3);
}