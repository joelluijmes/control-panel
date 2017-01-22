#pragma once
#include "../spi.h"

#ifdef SPI_DOUBLE

extern spi_state_t spi0_state;
extern spi_state_t spi1_state;

void spi0_init(uint8_t mode, uint8_t divider);
void spi1_init(uint8_t mode, uint8_t divider);

uint8_t spi0_completed(void);
uint8_t spi1_completed(void);

int8_t spi0_tranceive(const uint8_t* tran_buf, uint8_t tran_len, uint8_t* recv_buf, uint8_t recv_len);
int8_t spi0_receive(uint8_t* buffer, uint8_t len);
int8_t spi0_transmit(const uint8_t* buffer, uint8_t len);

int8_t spi1_tranceive(const uint8_t* tran_buf, uint8_t tran_len, uint8_t* recv_buf, uint8_t recv_len);
int8_t spi1_receive(uint8_t* buffer, uint8_t len);
int8_t spi1_transmit(const uint8_t* buffer, uint8_t len);

uint8_t spi0_tranceive_byte(uint8_t c);
uint8_t spi1_tranceive_byte(uint8_t c);

#endif