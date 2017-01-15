#pragma once

#include "../spi.h"

#ifdef SPI_SINGLE

extern spi_state_t spi_state;

void spi_init(uint8_t mode, uint8_t divider);

uint8_t spi_completed(void);
int8_t spi_tranceive(const uint8_t* tran_buf, uint8_t tran_len, uint8_t* recv_buf, uint8_t recv_len);
int8_t spi_receive(uint8_t* buffer, uint8_t len);
int8_t spi_transmit(uint8_t* buffer, uint8_t len);
void spi_install_hook(spi_isr_hook_t hook);

#endif