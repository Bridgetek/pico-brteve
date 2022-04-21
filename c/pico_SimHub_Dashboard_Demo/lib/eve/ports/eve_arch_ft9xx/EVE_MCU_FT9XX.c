/**
 @file EVE_MCU_FT9XX.c
 */
/*
 * ============================================================================
 * History
 * =======
 * Nov 2019		Initial beta for FT81x and FT80x
 * Mar 2020		Updated beta - added BT815/6 commands
 * Mar 2021		Beta with BT817/8 support added
 *
 *
 *
 *
 *
 * (C) Copyright,  Bridgetek Pte. Ltd.
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

// Guard against being used for incorrect CPU type.
#if defined(PLATFORM_FT9XX)

#pragma message "Compiling " __FILE__ " for BridgeTek FT9XX"

#include <string.h>
#include <stdint.h> // for Uint8/16/32 and Int8/16/32 data types
#include <machine/endian.h>

#include <ft900.h>
#include <ft900_spi.h>
#include <ft900_gpio.h>

#include "../include/MCU.h"
#include "EVE_config.h"

// SPI Master pins
#if defined(__FT900__)

#define PIN_NUM_MISO 30
#define PIN_NUM_MOSI 29
#define PIN_NUM_IO2  31
#define PIN_NUM_IO3  32
#define PIN_NUM_CLK  27
#define PIN_NUM_CS   28
// Powerdown pin
#define PIN_NUM_PD   43

#elif defined(__FT930__)

#define PIN_NUM_MISO 35
#define PIN_NUM_MOSI 36
#define PIN_NUM_IO2  37
#define PIN_NUM_IO3  38
#define PIN_NUM_CLK  34
#define PIN_NUM_CS   30
// Powerdown pin
#define PIN_NUM_PD   15

#endif //

// This is the MCU specific section and contains the functions which talk to the
// PIC registers. If porting the code to a different PIC or to another MCU, these
// should be modified to suit the registers of the selected MCU.

// ------------------- MCU specific initialisation  ----------------------------
void MCU_Init(void)
{
	// Initialize SPIM HW
	sys_enable(sys_device_spi_master);

	gpio_function(PIN_NUM_CLK, pad_spim_sck); /* GPIO27 to SPIM_CLK */
#if __FT900__
	gpio_function(PIN_NUM_CS, pad_spim_ss0); /* GPIO28 as CS */
	gpio_function(PIN_NUM_PD, pad_gpio43);
#else
	gpio_function(PIN_NUM_CS, pad30_spim_ss0); /* GPIO30 as CS */
	gpio_function(PIN_NUM_PD, pad_gpio15);
#endif

	gpio_function(PIN_NUM_MOSI, pad_spim_mosi); /* GPIO29 to SPIM_MOSI */
	gpio_function(PIN_NUM_MISO, pad_spim_miso); /* GPIO30 to SPIM_MISO */

	gpio_dir(PIN_NUM_CLK, pad_dir_output);
	gpio_dir(PIN_NUM_CS, pad_dir_output);
	gpio_dir(PIN_NUM_MOSI, pad_dir_output);
	gpio_dir(PIN_NUM_MISO, pad_dir_input);
	gpio_dir(PIN_NUM_PD, pad_dir_output);
#if (SPI_ENABLE == ENABLE_SPI_QUAD)
	/* Initialize IO2 and IO3 pad/pin for quad settings */
	gpio_function(PIN_NUM_IO2, pad_spim_io2); /* GPIO31 to IO2 */
	gpio_function(PIN_NUM_IO3, pad_spim_io3); /* GPIO32 to IO3 */
	gpio_dir(PIN_NUM_IO2, pad_dir_output);
	gpio_dir(PIN_NUM_IO3, pad_dir_output);
#endif
	gpio_write(PIN_NUM_CS, 1);
	gpio_write(PIN_NUM_PD, 1);

	spi_init(SPIM, spi_dir_master, spi_mode_0, 8);
}

void MCU_Setup(void)
{
#if defined QUADSPI_ENABLE

#if (defined EVE2_ENABLE || defined EVE3_ENABLE || defined EVE4_ENABLE)
	// Turn on EVE quad-SPI for FT81x devices.
	MCU_CSlow();
	MCU_SPIWrite24(MCU_htobe32((EVE_REG_SPI_WIDTH << 8) | (1 << 31)));
	MCU_SPIWrite8(2);
	MCU_CShigh();

	// Turn on FT9xx quad-SPI.
	spi_option(SPIM, spi_option_bus_width, 4);
#endif//(defined EVE2_ENABLE || defined EVE3_ENABLE || defined EVE4_ENABLE)
#endif// QUADSPI_ENABLE

	// Turn off SPI buffering. Timing of chip select is critical.
	spi_option(SPIM, spi_option_fifo, 0);
}

// ########################### GPIO CONTROL ####################################

// --------------------- Chip Select line low ----------------------------------
inline void MCU_CSlow(void)
{
	//gpio_write(PIN_NUM_CS, 0);
	spi_open(SPIM, 0);
}  

// --------------------- Chip Select line high ---------------------------------
inline void MCU_CShigh(void)
{
	//gpio_write(PIN_NUM_CS, 1);
	spi_close(SPIM, 0);
}

// -------------------------- PD line low --------------------------------------
inline void MCU_PDlow(void)
{
	gpio_write(PIN_NUM_PD, 0);
}

// ------------------------- PD line high --------------------------------------
inline void MCU_PDhigh(void)
{
	gpio_write(PIN_NUM_PD, 1);
}

// --------------------- SPI Send and Receive ----------------------------------

uint8_t MCU_SPIRead8(void)
{
	uint8_t DataRead = 0;

	spi_readn(SPIM, &DataRead, 1);

	return DataRead;
}

void MCU_SPIWrite8(uint8_t DataToWrite)
{
	spi_writen(SPIM, &DataToWrite, 1);
}

uint16_t MCU_SPIRead16(void)
{
	uint16_t DataRead = 0;

	spi_readn(SPIM, (uint8_t *)&DataRead, 2);

	return DataRead;
}

void MCU_SPIWrite16(uint16_t DataToWrite)
{
	spi_writen(SPIM, (uint8_t *)&DataToWrite, 2);
}

uint32_t MCU_SPIRead24(void)
{
	uint32_t DataRead = 0;

	spi_readn(SPIM, (uint8_t *)&DataRead, 3);

	return DataRead;
}

void MCU_SPIWrite24(uint32_t DataToWrite)
{
	spi_writen(SPIM, (uint8_t *)&DataToWrite, 3);
}

uint32_t MCU_SPIRead32(void)
{
	uint32_t DataRead = 0;

	spi_readn(SPIM, (uint8_t *)&DataRead, 4);

	return DataRead;
}

void MCU_SPIWrite32(uint32_t DataToWrite)
{
	spi_writen(SPIM, (uint8_t *)&DataToWrite, 4);
}

void MCU_SPIWrite(const uint8_t *DataToWrite, uint32_t length)
{
	spi_writen(SPIM, DataToWrite, length);
}

void MCU_Delay_20ms(void)
{
	delayms(20);
}

void MCU_Delay_500ms(void)
{
	delayms(500);
}

// FT9XX is Little Endian.
// Use toolchain defined functions.
uint16_t MCU_htobe16(uint16_t h)
{
	return __bswap16(h);
}

uint32_t MCU_htobe32(uint32_t h)
{
	return __bswap32(h);
}

uint16_t MCU_htole16(uint16_t h)
{
	return h;
}

uint32_t MCU_htole32(uint32_t h)
{
	return h;
}

uint16_t MCU_be16toh(uint16_t h)
{
	return __bswap16(h);
}

uint32_t MCU_be32toh(uint32_t h)
{
	return __bswap32(h);
}

uint16_t MCU_le16toh(uint16_t h)
{
	return h;
}

uint32_t MCU_le32toh(uint32_t h)
{
	return h;
}

#endif /* defined(PLATFORM_FT9XX) */
