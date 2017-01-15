#include "spi_single.h"

#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#ifdef SPI_SINGLE

spi_state_t spi_state = { .spdr = &SPDR };

void spi_init(uint8_t mode, uint8_t divider)
{
    spi_state.mode = mode;
    SPCR = 1 << SPIE | 1 << SPE | (divider & 0x03);

    if (mode == SPI_MASTER)
    {
        DDR_MISO &= ~MASK_MISO;
        DDR_MOSI |= MASK_MOSI;
        DDR_SCK |= MASK_SCK;
        DDR_SS |= MASK_SS;

        SPCR |= 1 << MSTR;
    }
    else if (mode == SPI_SLAVE)
    {
        DDR_MISO |= MASK_MISO;
        DDR_MOSI &= ~MASK_MOSI;
        DDR_SCK &= ~MASK_SCK;
        DDR_SS &= ~MASK_SS;
    }
        
    if (divider & 0x04)
        SPSR |= 1 << SPI2X;
}

uint8_t spi_completed(void)
{
    return !spi_state.running;
}

int8_t spi_tranceive(const uint8_t* tran_buf, uint8_t tran_len, uint8_t* recv_buf, uint8_t recv_len)
{
    return _spi_tranceive(&spi_state, tran_buf, tran_len, recv_buf, recv_len);
}

int8_t spi_receive(uint8_t* buffer, uint8_t len)
{
    return _spi_tranceive(&spi_state, NULL, 0, buffer, len);
}

int8_t spi_transmit(uint8_t* buffer, uint8_t len)
{
    return _spi_tranceive(&spi_state, buffer, len, NULL, len);
}

void spi_install_hook(spi_isr_hook_t hook)
{
    spi_state.hook = hook;
}

ISR(SPI_STC_vect)
{
    _spi_isr(&spi_state);
}

#endif
