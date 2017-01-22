#include "spi_double.h"

#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#ifdef SPI_DOUBLE

spi_state_t spi0_state = { .spdr = &SPDR0, .spsr = &SPSR0, .spcr = &SPCR0, .tranceive_byte = spi0_tranceive_byte };
spi_state_t spi1_state = { .spdr = &SPDR1, .spsr = &SPSR1, .spcr = &SPCR1, .tranceive_byte = spi1_tranceive_byte };

void spi0_init(uint8_t mode, uint8_t divider)
{
    spi0_state.mode = mode;

    if (mode == SPI_MASTER)
    {
        MOSI0_DDR |= MOSI0_MASK;
        SCK0_DDR |= SCK0_MASK;
        SS0_DDR |= SS0_MASK;
        SS0_PORT |= SS0_MASK;
        
        SPCR0 |= 1 << MSTR;
    } 
    else if (mode == SPI_SLAVE)
    {
        
    }

    if (divider & 0x04)
        SPSR0 |= 1 << SPI2X;

    SPCR0 |= 1 << SPE | (divider & 0x03);
}

void spi1_init(uint8_t mode, uint8_t divider)
{
    spi1_state.mode = mode;

    if (mode == SPI_MASTER)
    {
        MOSI1_DDR |= MOSI1_MASK;
        SCK1_DDR |= SCK1_MASK;
        SS1_DDR |= SS1_MASK;
        SS1_PORT |= SS1_MASK;
        
        SPCR1 |= 1 << MSTR1;        
    }
    else if (mode == SPI_SLAVE)
    {
        
    }

    if (divider & 0x04)
        SPSR1 |= 1 << SPI2X;

    SPCR1 |= 1 << SPE1 | (divider & 0x03);
}

uint8_t spi0_completed(void) 
{
    return !spi0_state.running;    
}

uint8_t spi1_completed(void)
{
    return !spi1_state.running;
}

int8_t spi0_tranceive(const uint8_t* tran_buf, uint8_t tran_len, uint8_t* recv_buf, uint8_t recv_len)
{
    spi0_state.tran_buf = tran_buf;
    spi0_state.tran_len = tran_len;
    spi0_state.recv_buf = recv_buf;
    spi0_state.recv_len = recv_len;

    return _spi_tranceive(&spi0_state);
}

int8_t spi0_receive(uint8_t* buffer, uint8_t len)
{
    return spi0_tranceive(NULL, 0, buffer, len);
}

int8_t spi0_transmit(const uint8_t* buffer, uint8_t len)
{
    return spi0_tranceive(buffer, len, NULL, 0);
}

int8_t spi1_tranceive(const uint8_t* tran_buf, uint8_t tran_len, uint8_t* recv_buf, uint8_t recv_len)
{
    spi1_state.tran_buf = tran_buf;
    spi1_state.tran_len = tran_len;
    spi1_state.recv_buf = recv_buf;
    spi1_state.recv_len = recv_len;

    return _spi_tranceive(&spi1_state);
}

int8_t spi1_receive(uint8_t* buffer, uint8_t len)
{
    return spi1_tranceive(NULL, 0, buffer, len);
}

int8_t spi1_transmit(const uint8_t* buffer, uint8_t len)
{
    return spi1_tranceive(buffer, len, NULL, 0);
}

uint8_t spi0_tranceive_byte(uint8_t c)
{
    SPDR0 = c;
    while (!(SPSR0 & (1 << SPIF))) ;
    
    return SPDR0;
}

uint8_t spi1_tranceive_byte(uint8_t c)
{
    SPDR1 = c;
    while (!(SPSR1 & (1 << SPIF))) ;
    
    return SPDR1;
}

//ISR(SPI0_STC_vect)
//{
    //_spi_isr(&spi0_state);
//}
//
//ISR(SPI1_STC_vect)
//{
    //_spi_isr(&spi1_state);
//}

#endif
