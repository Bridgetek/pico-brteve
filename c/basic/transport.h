#ifndef TRANSPORT__H
#define TRANSPORT__H

#include "pico/stdlib.h"
#include "hardware/spi.h"

#define PIN_PDN  7
#define PIN_CS   5
#define PIN_MISO 4
#define PIN_MOSI 3
#define PIN_SCK  2

#define SPI_CLK_RATE  (1 * 1000 * 1000) /*15Mhz*/
#define SPI_DUMMY 1
#define SPI_PORT spi0

void spi_setup();
void cs_select();
void cs_deselect();
void pdn_down();
void pdn_up();

uint8_t spi_read();
void spi_write(uint8_t value);

#endif
