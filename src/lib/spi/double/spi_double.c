#include "spi_double.h"

#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#ifdef SPI_DOUBLE

spi_state_t spi_state_0 = { .spdr = &SPDR0 };
spi_state_t spi_state_1 = { .spdr = &SPDR1 };

void spi_init_0(uint8_t mode, uint8_t divider)
{
    spi_state_0.mode = mode;

    if (mode == SPI_MASTER)
    {
        DDR_MOSI_0 |= MASK_MOSI_0;
        DDR_SCK_0 |= MASK_SCK_0;
        DDR_SS_0 |= MASK_SS_0;
        SPCR0 = 1 << SPIE | 1 << SPE | 1 << MSTR | (divider & 0x03);
    } 
    else if (mode == SPI_SLAVE)
    {
        
    }

    if (divider & 0x04)
        SPSR0 |= 1 << SPI2X;
}

void spi_init_1(uint8_t mode, uint8_t divider)
{
    spi_state_1.mode = mode;

    SPCR1 = 1 << SPIE1 | 1 << SPE1 | (divider & 0x03);

    if (mode == SPI_MASTER)
    {
        DDR_MOSI_1 |= MASK_MOSI_1;
        DDR_SCK_1 |= MASK_SCK_1;
        DDR_SS_1 |= MASK_SS_1;
        SPCR1 |= 1 << MSTR1;
    }
    else if (mode == SPI_SLAVE)
    {
        
    }

    if (divider & 0x04)
        SPSR1 |= 1 << SPI2X;
}

uint8_t spi_completed_0(void) 
{
    return !spi_state_0.running;    
}

uint8_t spi_completed_1(void)
{
    return !spi_state_1.running;
}

int8_t spi_tranceive_0(const uint8_t* tran_buf, uint8_t tran_len, uint8_t* recv_buf, uint8_t recv_len)
{
    return _spi_tranceive(&spi_state_0, tran_buf, tran_len, recv_buf, recv_len);
}

int8_t spi_receive_0(uint8_t* buffer, uint8_t len)
{
    return _spi_tranceive(&spi_state_0, NULL, 0, buffer, len);
}

int8_t spi_transmit_0(uint8_t* buffer, uint8_t len)
{
    return _spi_tranceive(&spi_state_0, buffer, len, NULL, 0);
}

int8_t spi_tranceive_1(const uint8_t* tran_buf, uint8_t tran_len, uint8_t* recv_buf, uint8_t recv_len)
{
    return _spi_tranceive(&spi_state_1, tran_buf, tran_len, recv_buf, recv_len);
}

int8_t spi_receive_1(uint8_t* buffer, uint8_t len)
{
    return _spi_tranceive(&spi_state_1, NULL, 0, buffer, len);
}

int8_t spi_transmit_1(uint8_t* buffer, uint8_t len)
{
    return _spi_tranceive(&spi_state_1, buffer, len, NULL, 0);
}

void spi_install_hook_0(spi_isr_hook_t hook)
{
    spi_state_0.hook = hook;
}

void spi_install_hook_1(spi_isr_hook_t hook)
{
    spi_state_1.hook = hook;
}

ISR(SPI0_STC_vect)
{
    _spi_isr(&spi_state_0);
}

ISR(SPI1_STC_vect)
{
    _spi_isr(&spi_state_1);
}

#endif
