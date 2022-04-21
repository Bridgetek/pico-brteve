/**
 @file EVE_MCU_ESP32.c
 @details MCU-specific code for controlling FT8XX on ESP32 devices.
 */
/*
 * ============================================================================
 * History
 * =======
 * Nov 2019		Initial version
 *
 *
 *
 *
 *
 *
 *
 * (C) Copyright Bridgetek Pte Ltd
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd
 * ("Bridgetek") subject to the licence terms set out
 * http://www.ftdichip.com/FTSourceCodeLicenceTerms.htm ("the Licence Terms").
 * You must read the Licence Terms before downloading or using the Software.
 * By installing or using the Software you agree to the Licence Terms. If you
 * do not agree to the Licence Terms then do not download or use the Software.
 *
 * Without prejudice to the Licence Terms, here is a summary of some of the key
 * terms of the Licence Terms (and in the event of any conflict between this
 * summary and the Licence Terms then the text of the Licence Terms will
 * prevail).
 *
 * The Software is provided "as is".
 * There are no warranties (or similar) in relation to the quality of the
 * Software. You use it at your own risk.
 * The Software should not be used in, or for, any medical device, system or
 * appliance. There are exclusions of Bridgetek liability for certain types of loss
 * such as: special loss or damage; incidental loss or damage; indirect or
 * consequential loss or damage; loss of income; loss of business; loss of
 * profits; loss of revenue; loss of contracts; business interruption; loss of
 * the use of money or anticipated savings; loss of information; loss of
 * opportunity; loss of goodwill or reputation; and/or loss of, damage to or
 * corruption of data.
 * There is a monetary cap on Bridgetek's liability.
 * The Software may have subsequently been amended by another user and then
 * distributed by that other user ("Adapted Software").  If so that user may
 * have additional licence terms that apply to those amendments. However, Bridgetek
 * has no liability in relation to those amendments.
 * ============================================================================
 */

// Guard against being used for incorrect CPU type. ESP32 is Xtensa LE based.
#if defined(PLATFORM_ESP32)

#pragma message "Compiling " __FILE__ " for Espressif ESP32"

#include <string.h>
#include <stdint.h>
/* Endian tools required as we send 32 bit values. */
#include <machine/endian.h>

/* REQUIRE FreeRTOS to enable accurate timers. */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
/* Local copy of endian.h */
#include "endian.h"

#include "driver/gpio.h"

#include "esp_system.h"
#include "driver/spi_master.h"
#include "esp_log.h"

#include "../include/MCU.h"

/* SPI Master pins */
#define PIN_NUM_MISO GPIO_NUM_19
#define PIN_NUM_MOSI GPIO_NUM_23
#define PIN_NUM_CLK  GPIO_NUM_18
#define PIN_NUM_CS   GPIO_NUM_22
/* Powerdown pin */
#define PIN_NUM_PD   GPIO_NUM_15

// SPI device
static spi_device_handle_t spi;

// This is the MCU specific section and contains the functions which talk to the
// MCU registers. If porting the code to another MCU, these should be modified
// to suit the registers of the selected MCU.

// ------------------- MCU specific initialisation  ----------------------------
void MCU_Init(void)
{
    esp_err_t ret;
    gpio_config_t io_conf;

    spi_bus_config_t buscfg={
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1
    };
    spi_device_interface_config_t devcfg={
        .clock_speed_hz = 1000000, //Clock out at 10 MHz
        .mode=0, //SPI mode 0
        .spics_io_num= -1, // CS pin operated manually
        .queue_size = 7,
		.address_bits = 0, // Address operated manually
		.command_bits = 0, // Command operated manually
    };

    //Initialize the SPI bus
    ret=spi_bus_initialize(HSPI_HOST, &buscfg, 1);
    assert(ret==ESP_OK);
    //Attach the SPI bus
    ret=spi_bus_add_device(HSPI_HOST, &devcfg, &spi);
    assert(ret==ESP_OK);

    //disable interrupt
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the Chip Select and Power Down pins
    io_conf.pin_bit_mask = BIT(PIN_NUM_PD) | BIT(PIN_NUM_CS);
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
}

void MCU_Setup(void)
{
	// No action required.
}

// ########################### GPIO CONTROL ####################################

// --------------------- Chip Select line low ----------------------------------
inline void MCU_CSlow(void)
{
    gpio_set_level(PIN_NUM_CS, 0);
}  

// --------------------- Chip Select line high ---------------------------------
inline void MCU_CShigh(void)
{
    gpio_set_level(PIN_NUM_CS, 1);
}

// -------------------------- PD line low --------------------------------------
inline void MCU_PDlow(void)
{
    gpio_set_level(PIN_NUM_PD, 0);
}

// ------------------------- PD line high --------------------------------------
inline void MCU_PDhigh(void)
{
    gpio_set_level(PIN_NUM_PD, 1);
}

// --------------------- SPI Send and Receive ----------------------------------

void MCU_SPIWrite(const uint8_t *DataToWrite, uint32_t length)
{
    static spi_transaction_t trans;

    trans.length = length * 8;
	trans.rxlength = 0;
    trans.flags = 0;
    trans.tx_buffer = DataToWrite;
    trans.rx_buffer = (uint8_t *)DataToWrite;
    spi_device_transmit(spi, &trans);
}

uint8_t MCU_SPIRead8(void)
{
    uint8_t DataRead = 0;
    static spi_transaction_t trans;

    trans.length = 8;
	trans.rxlength = 0;
    trans.flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA;
    trans.tx_data[0] = 0;
    spi_device_transmit(spi, &trans);
    DataRead = trans.rx_data[0];

    return DataRead;
}

void MCU_SPIWrite8(uint8_t DataToWrite)
{
    static spi_transaction_t trans;

    trans.length = 8;
	trans.rxlength = 0;
    trans.flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA;
    trans.tx_data[0] = DataToWrite;
    spi_device_transmit(spi, &trans);
}

uint16_t MCU_SPIRead16(void)
{
    uint16_t DataRead = 0;
    static spi_transaction_t trans;

    trans.length = 16;
	trans.rxlength = 0;
    trans.flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA;
    *(uint16_t *)trans.tx_data = 0;
    spi_device_transmit(spi, &trans);
    DataRead = *((uint16_t *)trans.rx_data);

    return DataRead;
}

void MCU_SPIWrite16(uint16_t DataToWrite)
{
    static spi_transaction_t trans;

    trans.length = 16;
	trans.rxlength = 0;
    trans.flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA;
    *(uint16_t *)trans.tx_data = DataToWrite;
    spi_device_transmit(spi, &trans);
}

uint32_t MCU_SPIRead24(void)
{
    uint32_t DataRead = 0;
    static spi_transaction_t trans;

    trans.length = 24;
	trans.rxlength = 0;
    trans.flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA;

    *(uint32_t *)trans.tx_data = 0;
    spi_device_transmit(spi, &trans);
    DataRead = *((uint32_t *)trans.rx_data);

    return DataRead;
}

void MCU_SPIWrite24(uint32_t DataToWrite)
{
    static spi_transaction_t trans;

    trans.length = 24;
	trans.rxlength = 0;
    trans.flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA;

    *(uint32_t *)trans.tx_data = DataToWrite;
    spi_device_transmit(spi, &trans);
}

uint32_t MCU_SPIRead32(void)
{
    uint32_t DataRead = 0;
    static spi_transaction_t trans;

    trans.length = 32;
	trans.rxlength = 0;
    trans.flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA;

    *(uint32_t *)trans.tx_data = 0;
    spi_device_transmit(spi, &trans);
    DataRead = *((uint32_t *)trans.rx_data);

    return DataRead;
}

void MCU_SPIWrite32(uint32_t DataToWrite)
{
    static spi_transaction_t trans;

    trans.length = 32;
	trans.rxlength = 0;
    trans.flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA;

    *(uint32_t *)trans.tx_data = DataToWrite;
    spi_device_transmit(spi, &trans);
}


void MCU_Delay_20ms(void)
{
    vTaskDelay(20 / portTICK_RATE_MS);
}

void MCU_Delay_500ms(void)
{
    vTaskDelay(500 / portTICK_RATE_MS);
}

// ESP32 is Little Endian.
// Use toolchain defined functions.
uint16_t MCU_htobe16(uint16_t h)
{
	return htobe16(h);
}

uint32_t MCU_htobe32(uint32_t h)
{
	return htobe32(h);
}

uint16_t MCU_htole16(uint16_t h)
{
	return htole16(h);
}

uint32_t MCU_htole32(uint32_t h)
{
	return htole32(h);
}

uint16_t MCU_be16toh(uint16_t h)
{
	return be16toh(h);
}

uint32_t MCU_be32toh(uint32_t h)
{
	return be32toh(h);
}

uint16_t MCU_le16toh(uint16_t h)
{
	return le16toh(h);
}

uint32_t MCU_le32toh(uint32_t h)
{
	return le32toh(h);
}

#endif /* defined(PLATFORM_ESP32) */
