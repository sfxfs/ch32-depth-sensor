#pragma once

#include "debug.h"

#define spi_start_transmit() GPIO_WriteBit(GPIOC, GPIO_Pin_4, Bit_RESET)
#define spi_end_transmit() GPIO_WriteBit(GPIOC, GPIO_Pin_4, Bit_SET)

void spi_init();
int spi_transmit(uint8_t *tx_buf, uint8_t *rx_buf, size_t size);
