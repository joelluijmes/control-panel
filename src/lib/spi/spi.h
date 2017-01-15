#pragma once

#include "config.h"
#include <inttypes.h>
#include <avr/io.h>

#if defined(SPCR)
    #define SPI_SINGLE
#elif defined(SPCR0) && defined(SPCR1)
    #define SPI_DOUBLE
#else
    #error 'Unsupported device'
#endif

#define SPI_ALREADY_STARTED 0
#define SPI_OK -1

#define SPI_MASTER 0
#define SPI_SLAVE 1

#define SPI_2X      4
#define SPI_4X      0
#define SPI_8X      5
#define SPI_16X     1
#define SPI_32X     6
#define SPI_64X     2
#define SPI_128X    3

typedef struct spi_state_t spi_state_t;
typedef void (*spi_isr_hook_t)(spi_state_t* state);

struct spi_state_t {
    volatile const uint8_t* tran_buf;
    volatile uint8_t* recv_buf;
    volatile uint8_t tran_len;
    volatile uint8_t recv_len;
    volatile uint8_t len;

    volatile uint8_t index;
    
    volatile uint8_t mode : 1;
    volatile uint8_t running : 1;

    volatile uint8_t* spdr;
    spi_isr_hook_t hook;
};


int8_t _spi_tranceive(spi_state_t* state, const uint8_t* tran_buf, uint8_t tran_len, uint8_t* recv_buf, uint8_t recv_len);
void _spi_isr(spi_state_t* state);