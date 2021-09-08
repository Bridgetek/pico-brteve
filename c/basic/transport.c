#include <stdio.h>

#include "transport.h"

void spi_setup(){
    // Enable SPI 0 at 15 MHz and connect to GPIOs
    spi_init(SPI_PORT, SPI_CLK_RATE);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1); // deselect

    printf("SPI initialised, let's goooooo\n");
}

uint8_t spi_read(){
    uint8_t buf;
    spi_read_blocking(SPI_PORT, 0, &buf, 1);

    return buf;
}

void spi_write(uint8_t value){
    spi_write_blocking(SPI_PORT, &value, 1);
}

void cs_select() {
    asm volatile("nop \n nop \n nop");
    gpio_put(PIN_CS, 0);  // Active low
    asm volatile("nop \n nop \n nop");
}

void cs_deselect() {
    asm volatile("nop \n nop \n nop");
    gpio_put(PIN_CS, 1);
    asm volatile("nop \n nop \n nop");
}

void pdn_down() {
    gpio_put(PIN_PDN, 0);
}
void pdn_up() {
    gpio_put(PIN_PDN, 1);
}

/* end of file */
