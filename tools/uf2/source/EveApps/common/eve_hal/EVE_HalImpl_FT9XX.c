/**
 * @file EVE_HalImpl_FT9XX.c
 * @brief Eve_Hal framework APIs for FT9XX host platform
 *
 * @author Bridgetek
 *
 * @date 2018
 * 
 * MIT License
 *
 * Copyright (c) [2019] [Bridgetek Pte Ltd (BRTChip)]
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#include "EVE_HalImpl.h"
#include "EVE_Platform.h"
#if defined(FT9XX_PLATFORM)

void ticker();
#if defined(__FT930__)
static const uint8_t s_SpimGpioSS[5] = { GPIO30_SPIM_SS0, GPIO_SPIM_SS1, GPIO_SPIM_SS2, GPIO_SPIM_SS3, 29 };
static const pad_dir_t s_SpimFuncSS[5] = { pad30_spim_ss0, pad_spim_ss1, pad_spim_ss2, pad_spim_ss3, pad29_spim_ss0 };
#else
static const uint8_t s_SpimGpioSS[4] = { GPIO_SPIM_SS0, GPIO_SPIM_SS1, GPIO_SPIM_SS2, 35 };
static const pad_dir_t s_SpimFuncSS[4] = { pad_spim_ss0, pad_spim_ss1, pad_spim_ss2, pad_spim_ss3 };
#endif
#define GPIO_SS_NB (sizeof(s_SpimGpioSS) / sizeof(s_SpimGpioSS[0]))

/*********
** INIT **
*********/

/**
 * @brief Initialize HAL platform
 * 
 */
void EVE_HalImpl_initialize()
{
}

/**
 * @brief Release HAL platform
 * 
 */
void EVE_HalImpl_release()
{
#ifndef PANL_APPLET
	spi_uninit(SPIM);
#endif
}

/* List the available devices */
EVE_HAL_EXPORT size_t EVE_Hal_list()
{
	return 1;
}

/* Get info of the specified device. Devices of type EVE_HOST_UNKNOWN should be ignored */
EVE_HAL_EXPORT void EVE_Hal_info(EVE_DeviceInfo *deviceInfo, size_t deviceIdx)
{
	memset(deviceInfo, 0, sizeof(EVE_DeviceInfo));
	strcpy_s(deviceInfo->DisplayName, sizeof(deviceInfo->DisplayName), "FT9XX");
	strcpy_s(deviceInfo->SerialNumber, sizeof(deviceInfo->SerialNumber), "FT9XX");
	deviceInfo->Host = EVE_HOST_EMBEDDED;
}

/* Check whether the context is the specified device */
EVE_HAL_EXPORT bool EVE_Hal_isDevice(EVE_HalContext *phost, size_t deviceIdx)
{
	return true;
}

/**
 * @brief Get the default configuration parameters
 * 
 * @param parameters EVE_Hal framework's parameters
 */
bool EVE_HalImpl_defaults(EVE_HalParameters *parameters, size_t deviceIdx)
{
	parameters->PowerDownPin = GPIO_FT800_PWD;
	parameters->SpiCsPin = deviceIdx < GPIO_SS_NB ? deviceIdx : 0; // SS0-3
	return true;
}

/**
 * @brief Set number of SPI channel
 * 
 * @param phost Pointer to Hal context
 * @param numchnls Number of channel
 * @param numdummy Number of dummy bytes
 */
void setSPI(EVE_HalContext *phost, EVE_SPI_CHANNELS_T numchnls, uint8_t numdummy)
{
	uint8_t spimGpio = s_SpimGpioSS[phost->SpiCsPin];
	pad_dir_t spimFunc = s_SpimFuncSS[phost->SpiCsPin];

	/* Reconfigure the SPI */
	eve_assert_do(!sys_enable(sys_device_spi_master));
	gpio_function(GPIO_SPIM_CLK, pad_spim_sck); /* GPIO27 to SPIM_CLK */
	gpio_function(spimGpio, spimFunc); /* GPIO as SS0-SS4 */
	gpio_function(GPIO_SPIM_MOSI, pad_spim_mosi); /* GPIO29 to SPIM_MOSI */
	gpio_function(GPIO_SPIM_MISO, pad_spim_miso); /* GPIO30 to SPIM_MISO */

	gpio_dir(GPIO_SPIM_CLK, pad_dir_output);
	gpio_dir(spimGpio, pad_dir_output);
	gpio_dir(GPIO_SPIM_MOSI, pad_dir_output);
	gpio_dir(GPIO_SPIM_MISO, pad_dir_input);

	if (numchnls > EVE_SPI_SINGLE_CHANNEL)
	{
		/* Initialize IO2 and IO3 pad/pin for dual and quad settings */
		gpio_function(GPIO_SPIM_IO2, pad_spim_io2);
		gpio_function(GPIO_SPIM_IO3, pad_spim_io3);
		gpio_dir(GPIO_SPIM_IO2, pad_dir_output);
		gpio_dir(GPIO_SPIM_IO3, pad_dir_output);
	}

	gpio_write(spimGpio, 1);

	/* Change clock frequency to 25 MHz (100 MHz / 4) */
	eve_assert_do(!spi_init(SPIM, spi_dir_master, spi_mode_0, 4));

	/* Enable FIFO of QSPI */
	spi_option(SPIM, spi_option_fifo_size, 64);
	spi_option(SPIM, spi_option_fifo, 1);
	spi_option(SPIM, spi_option_fifo_receive_trigger, 1);

	switch (numchnls)
	{
	case EVE_SPI_QUAD_CHANNEL:
		spi_option(SPIM, spi_option_bus_width, 4);
		break;
	case EVE_SPI_DUAL_CHANNEL:
		spi_option(SPIM, spi_option_bus_width, 2);
		break;
	case EVE_SPI_SINGLE_CHANNEL:
		spi_option(SPIM, spi_option_bus_width, 1);
		break;
	}

	phost->SpiChannels = numchnls;
	phost->SpiDummyBytes = numdummy;
}

/**
 * @brief Opens a new HAL context using the specified parameters
 * 
 * @param phost Pointer to Hal context
 * @param parameters EVE_Hal framework's parameters
 * @return true True if ok
 * @return false False if error
 */
bool EVE_HalImpl_open(EVE_HalContext *phost, const EVE_HalParameters *parameters)
{
	phost->SpiCsPin = parameters->SpiCsPin < GPIO_SS_NB ? parameters->SpiCsPin : 0;
	phost->PowerDownPin = parameters->PowerDownPin;
	eve_printf_debug("EVE open PWD: %d, SS: %d\n",
	    (unsigned int)phost->PowerDownPin, (unsigned int)s_SpimGpioSS[phost->SpiCsPin]);

#ifdef EVE_MULTI_GRAPHICS_TARGET
	phost->GpuDefs = &EVE_GpuDefs_FT80X;
#endif

	gpio_function(phost->PowerDownPin, pad_func_0);
	gpio_dir(phost->PowerDownPin, pad_dir_output);
	gpio_write(phost->PowerDownPin, 0);

	/* Initialize single channel */
	setSPI(phost, EVE_SPI_SINGLE_CHANNEL, 1);

	gpio_write(phost->PowerDownPin, 1);

	/* Initialize the context variables */
	phost->Status = EVE_STATUS_OPENED;
	++g_HalPlatform.OpenedDevices;

	return true;
}

/**
 * @brief Close a HAL context
 * 
 * @param phost Pointer to Hal context
 */
void EVE_HalImpl_close(EVE_HalContext *phost)
{
	phost->Status = EVE_STATUS_CLOSED;
	--g_HalPlatform.OpenedDevices;
#ifndef PANL_APPLET
	spi_close(SPIM, phost->SpiCsPin);
#endif
}

/**
 * @brief Idle. Call regularly to update frequently changing internal state
 * 
 * @param phost Pointer to Hal context
 */
void EVE_HalImpl_idle(EVE_HalContext *phost)
{
	/* no-op */
}

/*************
** TRANSFER **
*************/

/**
 * @brief Start data transfer to Coprocessor
 * 
 * @param phost Pointer to Hal context
 * @param rw Read or Write
 * @param addr Address to read/write
 */
void EVE_Hal_startTransfer(EVE_HalContext *phost, EVE_TRANSFER_T rw, uint32_t addr)
{
	eve_assert(phost->Status == EVE_STATUS_OPENED);

	if (rw == EVE_TRANSFER_READ)
	{
		eve_assert(3 + phost->SpiDummyBytes <= 5);
		uint8_t spidata[5]; // FIXME: phost->SpiDummyBytes // ?
		spidata[0] = (addr >> 16);
		spidata[1] = (addr >> 8);
		spidata[2] = addr & 0xff;
		spi_open(SPIM, phost->SpiCsPin);
		spi_writen(SPIM, spidata, 3 + phost->SpiDummyBytes);
		phost->Status = EVE_STATUS_READING;
	}
	else
	{
		uint8_t spidata[4];
		spidata[0] = (0x80 | (addr >> 16));
		spidata[1] = (addr >> 8);
		spidata[2] = addr;

		spi_open(SPIM, phost->SpiCsPin);
		spi_writen(SPIM, spidata, 3);

		phost->Status = EVE_STATUS_WRITING;
	}
}

/**
 * @brief End data transfer
 * 
 * @param phost Pointer to Hal context
 */
void EVE_Hal_endTransfer(EVE_HalContext *phost)
{
	eve_assert(phost->Status == EVE_STATUS_READING || phost->Status == EVE_STATUS_WRITING);

	spi_close(SPIM, phost->SpiCsPin);
	phost->Status = EVE_STATUS_OPENED;
}

/**
 * @brief Flush data to Coprocessor
 * 
 * @param phost Pointer to Hal context
 */
void EVE_Hal_flush(EVE_HalContext *phost)
{
	eve_assert(phost->Status == EVE_STATUS_OPENED);
	/* no-op */
}

/**
 * @brief Read a block data from Coprocessor
 * 
 * @param phost Pointer to Hal context
 * @param buffer Buffer to get result
 * @param size Number of bytes to read
 */
static inline void rdBuffer(EVE_HalContext *phost, uint8_t *buffer, uint32_t size)
{
	spi_readn(SPIM, buffer, size);
}

/**
 * @brief Write a block data to Coprocessor
 * 
 * @param phost Pointer to Hal context
 * @param buffer Data buffer to write
 * @param size Size of buffer
 */
static inline void wrBuffer(EVE_HalContext *phost, const uint8_t *buffer, uint32_t size)
{
	spi_writen(SPIM, buffer, size);
}

/**
 * @brief Write 8 bit to Coprocessor
 * 
 * @param phost Pointer to Hal context
 * @param value Value to write
 * @return uint8_t Number of bytes transfered
 */
static inline uint8_t transfer8(EVE_HalContext *phost, uint8_t value)
{
	if (phost->Status == EVE_STATUS_READING)
	{
		spi_read(SPIM, value);
		return value;
	}
	else
	{
		spi_write(SPIM, value);
		return 0;
	}
}

/**
 * @brief Write 8 bits to Coprocessor
 * 
 * @param phost Pointer to Hal context
 * @param value Value to write
 * @return uint8_t Number of bytes transfered
 */
uint8_t EVE_Hal_transfer8(EVE_HalContext *phost, uint8_t value)
{
	return transfer8(phost, value);
}

/**
 * @brief Write 2 bytes to Coprocessor
 * 
 * @param phost Pointer to Hal context
 * @param value Value to write
 * @return uint16_t Number of bytes transfered
 */
uint16_t EVE_Hal_transfer16(EVE_HalContext *phost, uint16_t value)
{
	uint8_t buffer[2];
	if (phost->Status == EVE_STATUS_READING)
	{
		rdBuffer(phost, buffer, 2);
		return (uint16_t)buffer[0]
		    | (uint16_t)buffer[1] << 8;
	}
	else
	{
		buffer[0] = value & 0xFF;
		buffer[1] = value >> 8;
		wrBuffer(phost, buffer, 2);
		return 0;
	}
}

/**
 * @brief Write 4 bytes to Coprocessor
 * 
 * @param phost Pointer to Hal context
 * @param value Value to write
 * @return uint32_t Number of bytes transfered
 */
uint32_t EVE_Hal_transfer32(EVE_HalContext *phost, uint32_t value)
{
	uint8_t buffer[4];
	if (phost->Status == EVE_STATUS_READING)
	{
		rdBuffer(phost, buffer, 4);
		return (uint32_t)buffer[0]
		    | (uint32_t)buffer[1] << 8
		    | (uint32_t)buffer[2] << 16
		    | (uint32_t)buffer[3] << 24;
	}
	else
	{
		buffer[0] = value & 0xFF;
		buffer[1] = (value >> 8) & 0xFF;
		buffer[2] = (value >> 16) & 0xFF;
		buffer[3] = value >> 24;
		wrBuffer(phost, buffer, 4);
		return 0;
	}
}

/**
 * @brief Transfer (read/write) a block data to Coprocessor
 * 
 * @param phost Pointer to Hal context
 * @param result Buffer to get data transfered, NULL when write
 * @param buffer Buffer where data is transfered, NULL when read
 * @param size Size of buffer
 */
void EVE_Hal_transferMem(EVE_HalContext *phost, uint8_t *result, const uint8_t *buffer, uint32_t size)
{
	if (!size)
		return;

	if (result && buffer)
	{
		/* not implemented */
		eve_debug_break();
	}
	else if (result)
	{
		rdBuffer(phost, result, size);
	}
	else if (buffer)
	{
		wrBuffer(phost, buffer, size);
	}
}

/**
 * @brief Transfer a block data from program memory
 * 
 * @param phost Pointer to Hal context
 * @param result Buffer to get data transfered, NULL when write
 * @param buffer Buffer where data is transfered, NULL when read
 * @param size Size of buffer
 */
void EVE_Hal_transferProgMem(EVE_HalContext *phost, uint8_t *result, eve_progmem_const uint8_t *buffer, uint32_t size)
{
	if (!size)
		return;

	if (result && buffer)
	{
		/* not implemented */
		eve_debug_break();
	}
	else if (result)
	{
		/* not implemented */
		eve_debug_break();
	}
	else if (buffer)
	{
		eve_assert(!((uintptr_t)buffer & 0x3)); // must be 32-bit aligned
		eve_assert(!(size & 0x3)); // must be 32-bit aligned
		eve_progmem_const uint32_t *buf32 = (eve_progmem_const uint32_t *)(void eve_progmem_const *)buffer;
		size >>= 2;
		while (size--)
		{
			uint32_t value = *(buf32++);
			wrBuffer(phost, (uint8_t *)(&value), 4);
		}
	}
}

/**
 * @brief Transfer a string to EVE platform
 * 
 * @param phost Pointer to Hal context
 * @param str String to transfer
 * @param index Start position in the string
 * @param size Size of string
 * @param padMask Padding mask
 * @return uint32_t Numer of bytes transfered
 */
uint32_t EVE_Hal_transferString(EVE_HalContext *phost, const char *str, uint32_t index, uint32_t size, uint32_t padMask)
{
	if (!size)
	{
		/* TODO: Support different padding options */
		eve_assert(padMask == 0x3);
		EVE_Hal_transfer32(phost, 0);
		return 4;
	}

	eve_assert(size <= EVE_CMD_STRING_MAX);
	uint32_t transferred = 0;
	if (phost->Status == EVE_STATUS_WRITING)
	{
		transferred += (uint32_t)strnlen(str, size) + 1;
		eve_assert(str[transferred - 1] == '\0');
		wrBuffer(phost, (uint8_t *)str, transferred);
		if (transferred & padMask)
		{
			uint32_t pad = 4 - (transferred & padMask);
			uint8_t padding[4] = { 0 };
			wrBuffer(phost, padding, pad);
			transferred += pad;
			eve_assert(!(transferred & 0x3));
		}
	}
	else
	{
		/* not implemented */
		eve_debug_break();
	}
	return transferred;
}

/************
** UTILITY **
************/

/**
 * @brief Send a host command to Coprocessor
 * 
 * @param phost Pointer to Hal context
 * @param cmd Command to send
 */
void EVE_Hal_hostCommand(EVE_HalContext *phost, uint8_t cmd)
{
	eve_assert(phost->Status == EVE_STATUS_OPENED);

	uint8_t hcmd[4] = { 0 };
	hcmd[0] = cmd;
	hcmd[1] = 0;
	hcmd[2] = 0;
	hcmd[3] = 0;

	spi_open(SPIM, phost->SpiCsPin);
	spi_writen(SPIM, hcmd, 3);
	spi_close(SPIM, phost->SpiCsPin);
}

/**
 * @brief This API sends a 3byte command to the phost
 * 
 * @param phost Pointer to Hal context
 * @param cmd Command to send
 */
void EVE_Hal_hostCommandExt3(EVE_HalContext *phost, uint32_t cmd)
{
	eve_assert(phost->Status == EVE_STATUS_OPENED);

	uint8_t hcmd[4] = { 0 };
	hcmd[0] = cmd & 0xff;
	hcmd[1] = (cmd >> 8) & 0xff;
	hcmd[2] = (cmd >> 16) & 0xff;
	hcmd[3] = 0;

	spi_open(SPIM, phost->SpiCsPin);
	spi_writen(SPIM, hcmd, 3);
	spi_close(SPIM, phost->SpiCsPin);
}

/**
 * @brief Toggle PD_N pin of FT800 board for a power cycle
 * 
 * @param phost Pointer to Hal context
 * @param up Up or Down
 */
bool EVE_Hal_powerCycle(EVE_HalContext *phost, bool up)
{
	if (up)
	{
		gpio_write(phost->PowerDownPin, 0);
		EVE_sleep(20);
		setSPI(phost, EVE_SPI_SINGLE_CHANNEL, 1);
		gpio_write(phost->PowerDownPin, 1);
		EVE_sleep(20);
	}
	else
	{
		gpio_write(phost->PowerDownPin, 1);
		EVE_sleep(20);
		gpio_write(phost->PowerDownPin, 0);
		EVE_sleep(20);
	}
	return true;
}

/**
 * @brief Set number of SPI channel
 * 
 * @param phost Pointer to Hal context
 * @param numchnls Number of channel
 * @param numdummy Number of dummy bytes
 */
void EVE_Hal_setSPI(EVE_HalContext *phost, EVE_SPI_CHANNELS_T numchnls, uint8_t numdummy)
{
	if (EVE_CHIPID < EVE_FT810)
		return;

	uint8_t writebyte = 0;

	if ((numchnls > EVE_SPI_QUAD_CHANNEL) || (numdummy > 2) || (numdummy < 1))
		return; // error

	// Switch EVE to multi channel SPI mode
	writebyte = numchnls;
	if (numdummy == 2)
		writebyte |= EVE_SPI_TWO_DUMMY_BYTES;
	EVE_Hal_wr8(phost, REG_SPI_WIDTH, writebyte);
	EVE_Hal_flush(phost);

	// Switch FT9XX to multi channel SPI mode
	setSPI(phost, numchnls, numdummy);
}

void EVE_Hal_restoreSPI(EVE_HalContext *phost)
{
	if (EVE_CHIPID < EVE_FT810)
		return;

	setSPI(phost, phost->SpiChannels, phost->SpiDummyBytes);
}

/**
 * @brief Get current system clock of Coprocessor
 * 
 * @param phost Pointer to Hal context
 * @return uint32_t Frequency of Coprocessor
 */
uint32_t EVE_Hal_currentFrequency(EVE_HalContext *phost)
{
	uint32_t t0;
	uint32_t t1;
	int32_t r = 15625;

	t0 = EVE_Hal_rd32(phost, REG_CLOCK); /* t0 read */

	__asm__(
	    "   move.l  $r0,%0               \n\t"
	    "   mul.l   $r0,$r0,100          \n\t"
	    "1:                              \n\t"
	    "   sub.l   $r0,$r0,3            \n\t" /* Subtract the loop time = 4 cycles */
	    "   cmp.l   $r0,0                \n\t" /* Check that the counter is equal to 0 */
	    "   jmpc    gt, 1b               \n\t"
	    : /* Outputs */
	    : "r"(r) /* Inputs */
	    : "$r0"); /* Using */

	t1 = EVE_Hal_rd32(phost, REG_CLOCK); /* t1 read */
	return ((t1 - t0) << 6); /* bitshift 6 places is the same as multiplying 64 */
}

/*********
** MISC **
*********/

/**
 * @brief Sdcard initialization
 * 
 */
static void initSdHost()
{
#ifndef PANL_APPLET
#if 1
	/* All SD Host pins except CLK need a pull-up to work. The MM900EV*A module does not have external pull-up, so enable internal one */
	gpio_function(GPIO_SD_CLK, pad_sd_clk);
	gpio_pull(GPIO_SD_CLK, pad_pull_none);
	gpio_function(GPIO_SD_CMD, pad_sd_cmd);
	gpio_pull(GPIO_SD_CMD, pad_pull_pullup);
	gpio_function(GPIO_SD_DAT3, pad_sd_data3);
	gpio_pull(GPIO_SD_DAT3, pad_pull_pullup);
	gpio_function(GPIO_SD_DAT2, pad_sd_data2);
	gpio_pull(GPIO_SD_DAT2, pad_pull_pullup);
	gpio_function(GPIO_SD_DAT1, pad_sd_data1);
	gpio_pull(GPIO_SD_DAT1, pad_pull_pullup);
	gpio_function(GPIO_SD_DAT0, pad_sd_data0);
	gpio_pull(GPIO_SD_DAT0, pad_pull_pullup);
	gpio_function(GPIO_SD_CD, pad_sd_cd);
	gpio_pull(GPIO_SD_CD, pad_pull_pullup);
	gpio_function(GPIO_SD_WP, pad_sd_wp);
	gpio_pull(GPIO_SD_WP, pad_pull_pullup);

	/* Start up the SD Card */
	sys_enable(sys_device_sd_card);
#else
	sdhost_sys_init();
#endif
	sdhost_init();
#endif
}

/**
 * @brief Init FT9x host MCU
 * 
 */
void EVE_Mcu_initialize()
{
#ifndef PANL_APPLET
	sys_reset_all();

	interrupt_enable_globally();

	sys_enable(sys_device_uart0);
	gpio_function(GPIO_UART0_TX, pad_uart0_txd); /* UART0 TXD */
	gpio_function(GPIO_UART0_RX, pad_uart0_rxd); /* UART0 RXD */
	uart_open(UART0, /* Device */
	    1, /* Prescaler = 1 */
	    UART_DIVIDER_115200_BAUD, /* Divider = 1302 */
	    uart_data_bits_8, /* No. Data Bits */
	    uart_parity_none, /* Parity */
	    uart_stop_bits_1); /* No. Stop Bits */
#endif

#if (defined(ENABLE_ILI9488_HVGA_PORTRAIT) || defined(ENABLE_KD2401_HVGA_PORTRAIT))
	/* assign all the respective pins to GPIO and set them to default values */
	gpio_function(GPIO_ILI9488_DCX, pad_ili9488_dcx);
	gpio_dir(GPIO_ILI9488_DCX, pad_dir_output);
	gpio_write(GPIO_ILI9488_DCX, 1);

	gpio_function(GPIO_SPIM_CLK, pad_spim_sck);
	gpio_dir(GPIO_SPIM_CLK, pad_dir_output);
	gpio_write(GPIO_SPIM_CLK, 1);

	gpio_function(GPIO_SPIM_MOSI, pad_spim_mosi);
	gpio_dir(GPIO_SPIM_MOSI, pad_dir_output);
	gpio_write(GPIO_SPIM_MOSI, 1);

	gpio_function(GPIO_ILI9488_CS1, pad_ili9488_cs1);
	gpio_dir(GPIO_ILI9488_CS1, pad_dir_output);
	gpio_write(GPIO_ILI9488_CS1, 1);

	gpio_function(GPIO_SPIM_MISO, pad_spim_miso);
	gpio_dir(GPIO_SPIM_MISO, pad_dir_output);
	gpio_write(GPIO_SPIM_MISO, 1);

	gpio_function(GPIO_SPIM_SS0, pad_spim_ss0);
	gpio_dir(GPIO_SPIM_SS0, pad_dir_output);
	gpio_write(GPIO_SPIM_SS0, 1);

	gpio_function(GPIO_FT800_PWD, pad_func_0); /* FIXME: This needs to be done at open, not init */
	gpio_dir(GPIO_FT800_PWD, pad_dir_output);
	gpio_write(GPIO_FT800_PWD, 1);

	gpio_write(GPIO_ILI9488_DCX, 1);
	gpio_write(GPIO_SPIM_SS0, 1);
	gpio_write(GPIO_FT800_PWD, 1);
	gpio_write(GPIO_ILI9488_CS1, 1);
#endif

	initSdHost();
}

/**
 * @brief Release FT9x host MCU
 * 
 */
void EVE_Mcu_release()
{
	/* no-op */
}

/*********
** MISC **
*********/

/* Globals for interrupt implementation */
static uint32_t s_TotalMilliseconds = 0;
static uint64_t s_TotalMilliseconds64 = 0;

/**
 * @brief Init FT9x timer
 * 
 */
void EVE_Millis_initialize()
{
	s_TotalMilliseconds = 0;
#if !defined(PANL_APPLET)
	sys_enable(sys_device_timer_wdt);
#if defined(FT900_PLATFORM)
	timer_prescaler(FT900_TIMER_PRESCALE_VALUE);
#else
	timer_prescaler(FT900_FT_MILLIS_TIMER, FT900_TIMER_PRESCALE_VALUE);
#endif
	timer_init(FT900_FT_MILLIS_TIMER, FT900_TIMER_OVERFLOW_VALUE, timer_direction_up, timer_prescaler_select_on, timer_mode_continuous);

	interrupt_attach(interrupt_timers, 17, ticker);
	/* enabling the interrupts for timer */
	timer_enable_interrupt(FT900_FT_MILLIS_TIMER);

	timer_start(FT900_FT_MILLIS_TIMER);
#endif
}

/**
 * @brief Release FT9x timer
 * 
 */
void EVE_Millis_release()
{
#if !defined(PANL_APPLET)
	timer_stop(FT900_FT_MILLIS_TIMER);
	timer_disable_interrupt(FT900_FT_MILLIS_TIMER);
#endif
}

/**
 * @brief Get clock in miliseond
 * 
 * Need to ensure that below api is called at least once in 6.5 seconds duration for FT900 platform as this module doesnt use timer for context update 
 * global counter to loopback after ~49.71 days
 * 
 * @return uint32_t Clock number
 */
uint32_t EVE_millis()
{
#if defined(PANL_APPLET)
	uint32_t ms = panl_timer_get_time();
	if ((uint64_t)ms < s_TotalMilliseconds)
		s_TotalMilliseconds64 += (1 << 32);
	s_TotalMilliseconds = ms;
	s_TotalMilliseconds64 = (s_TotalMilliseconds64 & ~0xFFFFFFFFULL) | (uint64_t)ms;
#endif
	return s_TotalMilliseconds;
}

/**
* @brief Get clock in miliseond
* 
* Need to ensure that below api is called at least once in 6.5 seconds duration for FT900 platform as this module doesnt use timer for context update 
* global counter to loopback after ~49.71 days
* 
* @return uint32_t Clock number
*/
uint64_t EVE_millis64()
{
#if defined(PANL_APPLET)
	uint32_t ms = panl_timer_get_time();
	if ((uint64_t)ms < s_TotalMilliseconds)
		s_TotalMilliseconds64 += (1 << 32);
	s_TotalMilliseconds = ms;
	s_TotalMilliseconds64 = (s_TotalMilliseconds64 & ~0xFFFFFFFFULL) | (uint64_t)ms;
#endif
	return s_TotalMilliseconds64;
}

/**
 * @brief Clear the interrupt and increment the counter
 * 
 */
void ticker()
{
#if !defined(PANL_APPLET)
	s_TotalMilliseconds += 1;

	timer_disable_interrupt(FT900_FT_MILLIS_TIMER);
	/* Clear the interrupt and increment the counter */
	timer_is_interrupted(FT900_FT_MILLIS_TIMER);
	timer_enable_interrupt(FT900_FT_MILLIS_TIMER);
#endif
}

/**
 * @brief Sleep in milisecond
 * 
 * @param ms Milisecond
 */
void EVE_sleep(uint32_t ms)
{
	delayms(ms);
}

/*********
** MISC **
*********/

/**
 * @brief Display GPIO pins
 * 
 * @param phost Pointer to Hal context
 * @return true True if Ok
 * @return false False if error
 */
bool EVE_UtilImpl_bootupDisplayGpio(EVE_HalContext *phost)
{
	return true;
}

#endif /* #if defined(FT9XX_PLATFORM) */

/* end of file */
