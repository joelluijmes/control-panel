#include "spi_single.h"

#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#ifdef SPI_SINGLE

spi_state_t spi_state = { .spdr = &SPDR, .spsr = &SPSR, .spcr = &SPCR };

void spi_init(uint8_t mode, uint8_t divider)
{
    spi_state.mode = mode;
    SPCR = 1 << SPE | (divider & 0x03);

    if (mode == SPI_MASTER)
    {
        DDR_MISO &= ~MASK_MISO;
        DDR_MOSI |= MASK_MOSI;
        DDR_SCK |= MASK_SCK;
        DDR_SS |= MASK_SS;

        SPCR |= 1 << MSTR;
    }
    else if (mode & SPI_SLAVE)
    {
        DDR_MISO |= MASK_MISO;
        DDR_MOSI &= ~MASK_MOSI;
        DDR_SCK &= ~MASK_SCK;
        DDR_SS &= ~MASK_SS;
    }
        
    if (divider & 0x04)
        SPSR |= 1 << SPI2X;
}

void spi_reset()
{
    if (spi_state.mode == SPI_MASTER)
        SPCR = 1 << SPE | 1 << MSTR | (SPCR & 0x03);
    else
        SPCR = 1 << SPE | (SPCR & 0x03);

    uint8_t tmp = SPDR; // to clear the SPIF if need be
    SPDR = 0xFF;
}

uint8_t spi_completed(void)
{
    return !spi_state.running;
}

int8_t spi_tranceive(const uint8_t* tran_buf, uint8_t tran_len, uint8_t* recv_buf, uint8_t recv_len)
{
    spi_state.tran_buf = tran_buf;
    spi_state.tran_len = tran_len;
    spi_state.recv_buf = recv_buf;
    spi_state.recv_len = recv_len;

    return _spi_tranceive(&spi_state);
}

int8_t spi_receive(uint8_t* buffer, uint8_t len)
{
    return spi_tranceive(NULL, 0, buffer, len);
}

int8_t spi_transmit(uint8_t* buffer, uint8_t len)
{
    return spi_tranceive(buffer, len, NULL, len);
}

int8_t spi_tranceive_byte(uint8_t c)
{
    SPDR = c;
    while (!(SPSR & (1 << SPIF))) ;
    
    return SPDR;
}

ISR(SPI_STC_vect)
{
    _spi_isr(&spi_state);
}

#endif
