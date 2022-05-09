/**
 @file EVE_HAL_Linux.c
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

/* Only compile for linux platforms unless MPSSE is being used. */
#if defined(USE_LINUX_SPI_DEV) && !defined(USE_MPSSE)

#include <string.h>
#include <stdint.h> // for Uint8/16/32 and Int8/16/32 data types
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "../include/EVE.h"
#include "../include/HAL.h"
#include "../include/Platform.h"

#ifdef EVE_HAL_DEBUG
#define dbg_printf(...) printf(__VA_ARGS__)
#else
#define dbg_printf(...)
#endif

#ifdef EVE_HAL_INFO
#define info_printf printf
#else
#define info_printf(...)
#endif

#ifdef EVE_HAL_ERROR
#define err_printf(...) fprintf(stderr, __VA_ARGS__)
#else
#define err_printf(...)
#endif


// Used to navigate command ring buffer
static uint16_t writeCmdPointer = 0x0000;

void HAL_EVE_Init(void)
{
	Platform_Init();
	// Set Chip Select OFF
	HAL_ChipSelect(0);

	// Reset the display
	Platform_Delay_20ms();
	HAL_PowerDown(1);
	Platform_Delay_20ms();
	Platform_Delay_20ms();
	HAL_PowerDown(0);
	Platform_Delay_20ms();
	Platform_Delay_20ms();

#if (defined EVE1_ENABLE)
	// FT80x_selection - FT80x modules from BRT generally use external crystal
	// You can also send the host command to set the PLL here if you want to change it from the default of 48MHz (FT80x) or 60MHz (FT81x)
	// Clock selection and clock rate selection will put EVE to sleep and so must be before the Active command
	// for example:
	HAL_HostCmdWrite(0x44, 0x00); // 0x44 = HostCMD_CLKEXT
	HAL_HostCmdWrite(0x62, 0x00); // 0x64 = HostCMD_CLK48M
#endif

#if defined (EVE3_ENABLE) || defined (EVE4_ENABLE)
	// can optionally set to 72MHz system clock here
	// In this case also adjust REG_FREQUENCY a few lines down from here in this file
	HAL_HostCmdWrite(0x44, 0x00); // 0x44 = HostCMD_CLKEXT
	HAL_HostCmdWrite(0x61, 0x46);
#endif

#if defined (EVE2_ENABLE) || defined (EVE3_ENABLE)|| defined (EVE4_ENABLE)
	HAL_HostCmdWrite(0x68, 0x00); // Reset
#endif

	// Set active
	info_printf("Activating...");
	HAL_HostCmdWrite(0, 0x00);
	info_printf("done\n");

//	Platform_Delay_500ms();		// Optional delay can be commented so long as we check the REG_ID and REG_CPURESET

	// Read REG_ID register (0x302000) until reads 0x7C
	info_printf("Connecting to FT81X EVE display...");
	while (HAL_MemRead8(EVE_REG_ID) != 0x7C)
	{
		Platform_Delay_20ms();
	}
	info_printf("done\n");

	info_printf("Resetting...");
	// Ensure CPUreset register reads 0 and so FT8xx is ready
	while (HAL_MemRead8(EVE_REG_CPURESET) != 0x00)
	{
		Platform_Delay_20ms();
	}
	info_printf("done\n");

#if defined (EVE3_ENABLE) || defined (EVE4_ENABLE)
	HAL_MemWrite32(EVE_REG_FREQUENCY, 72000000);
#endif



	// This function will not return unless an EVE device is present.
	Platform_Setup();
}

// --------------------- Chip Select line ----------------------------------
void HAL_ChipSelect(int8_t enable)
{
	if (enable)
		Platform_CSlow();
	else
		Platform_CShigh();
}

// -------------------------- Power Down line --------------------------------------
void HAL_PowerDown(int8_t enable)
{
	if (enable)
		Platform_PDlow();
	else
		Platform_PDhigh();
}

// ------------------ Send FT81x register address for writing ------------------
void HAL_SetWriteAddress(uint32_t address)
{
	// Send three bytes of a register address which has to be subsequently
	// written. Ignore return values as this is an SPI write only.
	// Send high byte of address with 'write' bits set.
	struct spi_ioc_transfer xfer[1];
	uint32_t addr = Platform_htobe32((address << 8) | (1UL << 31));
	dbg_printf("%s: 0x%x -> 0x%x \n", __FUNCTION__, address, addr);

	memset(xfer, 0, sizeof(xfer));

	xfer[0].tx_buf = (uintptr_t)&addr;
	xfer[0].rx_buf = (uintptr_t)NULL;
	xfer[0].len = 3;
	xfer[0].cs_change = 0;

	if (Platform_SPI_transfer(xfer, 1) < 0)
	{
		err_printf("HAL_SetWriteAddress: 0x%x Transfer Failed 0x%x \n", address, addr);
	}
}

// ------------------ Send FT81x register address for reading ------------------
void HAL_SetReadAddress(uint32_t address)
{
	// Send three bytes of a register address which has to be subsequently read.
	// Ignore return values as this is an SPI write only.
	// Send high byte of address with 'read' bits set.
	struct spi_ioc_transfer xfer[1];
	uint32_t addr = Platform_htobe32((address << 8) | (0UL << 31));
	dbg_printf("%s: 0x%x -> 0x%x \n", __FUNCTION__, address, addr);

	memset(xfer, 0, sizeof(xfer));

	xfer[0].tx_buf = (uintptr_t)&addr;
	xfer[0].rx_buf = (uintptr_t)NULL;
	xfer[0].len = 4;
	xfer[0].cs_change = 0;

	if (Platform_SPI_transfer(xfer, 1) < 0)
	{
		err_printf("HAL_SetReadAddress: Transfer Failed \n");
	}
}

// ------------------------ Send a block of data --------------------------
void HAL_Write(const uint8_t *buffer, uint32_t length)
{
	// Send multiple bytes of data after previously sending address. Ignore return
	// values as this is an SPI write only. Data must be the correct endianess
	// for the SPI bus.
	struct spi_ioc_transfer xfer[1];
	dbg_printf("%s: %d \n", __FUNCTION__, length);

	memset(xfer, 0, sizeof(xfer));

	xfer[0].tx_buf = (uintptr_t)buffer;
	xfer[0].rx_buf = (uintptr_t)NULL;
	xfer[0].len = length;
	xfer[0].cs_change = 0;

	if (Platform_SPI_transfer(xfer, 1) < 0)
	{
		err_printf("HAL_Write: Transfer Failed \n");
	}
}

// ------------------------ Send a 32-bit data value --------------------------
void HAL_Write32(uint32_t val32)
{
	uint32_t val = Platform_htole32(val32);
	// Send four bytes of data after previously sending address. Ignore return
	// values as this is an SPI write only.
	struct spi_ioc_transfer xfer[1];
	dbg_printf("%s: 0x%x \n", __FUNCTION__, val32);

	memset(xfer, 0, sizeof(xfer));

	xfer[0].tx_buf = (uintptr_t)&val;
	xfer[0].rx_buf = (uintptr_t)NULL;
	xfer[0].len = 4;
	xfer[0].cs_change = 0;

	if (Platform_SPI_transfer(xfer, 1) < 0)
	{
		err_printf("HAL_Write32: Transfer Failed \n");
	}
}

// ------------------------ Send a 16-bit data value --------------------------
void HAL_Write16(uint16_t val16)
{
	uint16_t val = Platform_htole16(val16);
	// Send two bytes of data after previously sending address. Ignore return
	// values as this is an SPI write only.
	struct spi_ioc_transfer xfer[1];
	dbg_printf("%s: 0x%x \n", __FUNCTION__, val16);

	memset(xfer, 0, sizeof(xfer));

	xfer[0].tx_buf = (uintptr_t)&val;
	xfer[0].rx_buf = (uintptr_t)NULL;
	xfer[0].len = 2;
	xfer[0].cs_change = 0;

	if (Platform_SPI_transfer(xfer, 1) < 0)
	{
		err_printf("HAL_Write16: Transfer Failed \n");
	}
}

// ------------------------ Send an 8-bit data value ---------------------------
void HAL_Write8(uint8_t val8)
{
	// Send one byte of data after previously sending address. Ignore return
	// values as this is an SPI write only.
	struct spi_ioc_transfer xfer[1];
	dbg_printf("%s: 0x%x \n", __FUNCTION__, val8);

	memset(xfer, 0, sizeof(xfer));

	xfer[0].tx_buf = (uintptr_t)&val8;
	xfer[0].rx_buf = (uintptr_t)NULL;
	xfer[0].len = 1;
	xfer[0].cs_change = 0;

	if (Platform_SPI_transfer(xfer, 1) < 0)
	{
		err_printf("HAL_Write8: Transfer Failed \n");
	}
}

// ------------------------ Read a 32-bit data value --------------------------
uint32_t HAL_Read32(void)
{
	// Read 4 bytes from a register has been previously addressed. Send dummy
	// 00 bytes as only the incoming value is important.
	uint32_t val32;
	struct spi_ioc_transfer xfer[1];

	memset(xfer, 0, sizeof(xfer));

	xfer[0].tx_buf = (uintptr_t)NULL;
	xfer[0].rx_buf = (uintptr_t)&val32;
	xfer[0].len = 4;
	xfer[0].cs_change = 0;

	if (Platform_SPI_transfer(xfer, 1) < 0)
	{
		err_printf("HAL_Read32: Transfer Failed \n");
	}

	dbg_printf("%s: 0x%x \n", __FUNCTION__, val32);
	// Return combined 32-bit value
	return Platform_le32toh(val32);
}

// ------------------------ Read a 16-bit data value ---------------------------
uint16_t HAL_Read16(void)
{
	// Read 2 bytes from a register has been previously addressed. Send dummy
	// 00 bytes as only the incoming value is important.
	uint16_t val16;
	struct spi_ioc_transfer xfer[1];

	memset(xfer, 0, sizeof(xfer));

	xfer[0].tx_buf = (uintptr_t)NULL;
	xfer[0].rx_buf = (uintptr_t)&val16;
	xfer[0].len = 2;
	xfer[0].cs_change = 0;

	if (Platform_SPI_transfer(xfer, 1) < 0)
	{
		err_printf("HAL_Read16: Transfer Failed \n");
	}

	dbg_printf("%s: 0x%x \n", __FUNCTION__, val16);
	// Return combined 16-bit value
	return Platform_le16toh(val16);
}

// ------------------------ Read an 8-bit data value ---------------------------
uint8_t HAL_Read8(void)
{
	// Read 1 byte from a register has been previously addressed. Send dummy
	// 00 byte as only the incoming value is important.
	uint8_t val8;
	struct spi_ioc_transfer xfer[1];

	memset(xfer, 0, sizeof(xfer));

	xfer[0].tx_buf = (uintptr_t)NULL;
	xfer[0].rx_buf = (uintptr_t)&val8;
	xfer[0].len = 1;
	xfer[0].cs_change = 0;

	if (Platform_SPI_transfer(xfer, 1) < 0)
	{
		err_printf("HAL_Read8: Transfer Failed \n");
	}

	dbg_printf("%s: 0x%x \n", __FUNCTION__, val8);
	// Return 8-bit value read
	return val8;
}

// ################# COMBINED ADDRESSING AND DATA FUNCTIONS ####################

// This section has combined calls which carry out a full write or read cycle
// including chip select, address, and data transfer.
// This would often be used for register writes and reads.

// -------------- Write a 32-bit value to specified address --------------------
void HAL_MemWrite32(uint32_t address, uint32_t val32)
{
	struct spi_ioc_transfer xfer[2];
	uint32_t addr = Platform_htobe32((address << 8) | (1UL << 31));
	uint32_t val = Platform_htole32(val32);
	dbg_printf("%s: %0x%x 0x%x \n", __FUNCTION__, address, val32);

	memset(xfer, 0, sizeof(xfer));

	HAL_ChipSelect(1);

	xfer[0].tx_buf = (uintptr_t)&addr;
	xfer[0].rx_buf = (uintptr_t)NULL;
	xfer[0].len = 3;
	xfer[0].cs_change = 0;
	xfer[1].tx_buf = (uintptr_t)&val;
	xfer[1].rx_buf = (uintptr_t)NULL;
	xfer[1].len = 4;
	xfer[1].cs_change = 0;

	if (Platform_SPI_transfer(xfer, 2) < 0)
	{
		err_printf("HAL_MemWrite32: Transfer Failed \n");
	}
	HAL_ChipSelect(0);
}

// -------------- Write a 16-bit value to specified address --------------------
void HAL_MemWrite16(uint32_t address, uint16_t val16)
{
	struct spi_ioc_transfer xfer[2];
	uint32_t addr = Platform_htobe32((address << 8) | (1UL << 31));
	uint16_t val = Platform_htole16(val16);
	dbg_printf("%s: %0x%x 0x%x \n", __FUNCTION__, address, val16);

	memset(xfer, 0, sizeof(xfer));

	HAL_ChipSelect(1);

	xfer[0].tx_buf = (uintptr_t)&addr;
	xfer[0].rx_buf = (uintptr_t)NULL;
	xfer[0].len = 3;
	xfer[0].cs_change = 0;
	xfer[1].tx_buf = (uintptr_t)&val;
	xfer[1].rx_buf = (uintptr_t)NULL;
	xfer[1].len = 2;
	xfer[1].cs_change = 0;

	if (Platform_SPI_transfer(xfer, 2) < 0)
	{
		err_printf("HAL_MemWrite16: Transfer Failed \n");
	}
	HAL_ChipSelect(0);
}

// -------------- Write an 8-bit value to specified address --------------------
void HAL_MemWrite8(uint32_t address, uint8_t val8)
{
	struct spi_ioc_transfer xfer[2];
	uint32_t addr = Platform_htobe32((address << 8) | (1UL << 31));
	dbg_printf("%s: %0x%x 0x%x \n", __FUNCTION__, address, val8);

	HAL_ChipSelect(1);

	memset(xfer, 0, sizeof(xfer));

	xfer[0].tx_buf = (uintptr_t)&addr;
	xfer[0].rx_buf = (uintptr_t)NULL;
	xfer[0].len = 3;
	xfer[0].cs_change = 0;
	xfer[1].tx_buf = (uintptr_t)&val8;
	xfer[1].rx_buf = (uintptr_t)NULL;
	xfer[1].len = 1;
	xfer[1].cs_change = 0;

	if (Platform_SPI_transfer(xfer, 2) < 0)
	{
		err_printf("HAL_MemWrite8: Transfer Failed \n");
	}
	HAL_ChipSelect(0);
}

// -------------- Read a 32-bit value from specified address --------------------
uint32_t HAL_MemRead32(uint32_t address)
{
	uint32_t val32;
	struct spi_ioc_transfer xfer[2];
	uint32_t addr = Platform_htobe32((address << 8) | (0UL << 31));
	dbg_printf("%s: 0x%x -> 0x%x \n", __FUNCTION__, address, addr);

	memset(xfer, 0, sizeof(xfer));

	HAL_ChipSelect(1);

	xfer[0].tx_buf = (uintptr_t)&addr;
	xfer[0].rx_buf = (uintptr_t)NULL;
	xfer[0].len = 4;
	xfer[0].cs_change = 0;
	xfer[1].tx_buf = (uintptr_t)NULL;
	xfer[1].rx_buf = (uintptr_t)&val32;
	xfer[1].len = 4;
	xfer[1].cs_change = 0;

	if (Platform_SPI_transfer(xfer, 2) < 0)
	{
		err_printf("HAL_MemRead32: Transfer Failed \n");
	}

	HAL_ChipSelect(0);
	dbg_printf("%s: 0x%x \n", __FUNCTION__, val32);
	// Return 32-bit value read
	return Platform_le32toh(val32);
}
// -------------- Read a 16-bit value from specified address --------------------
uint16_t HAL_MemRead16(uint32_t address)
{
	uint16_t val16;
	struct spi_ioc_transfer xfer[2];
	uint32_t addr = Platform_htobe32((address << 8) | (0UL << 31));
	dbg_printf("%s: 0x%x -> 0x%x \n", __FUNCTION__, address, addr);

	HAL_ChipSelect(1);

	memset(xfer, 0, sizeof(xfer));

	xfer[0].tx_buf = (uintptr_t)&addr;
	xfer[0].rx_buf = (uintptr_t)NULL;
	xfer[0].len = 4;
	xfer[0].cs_change = 0;
	xfer[1].tx_buf = (uintptr_t)NULL;
	xfer[1].rx_buf = (uintptr_t)&val16;
	xfer[1].len = 2;
	xfer[1].cs_change = 0;

	if (Platform_SPI_transfer(xfer, 2) < 0)
	{
		err_printf("HAL_MemRead16: Transfer Failed \n");
	}

	HAL_ChipSelect(0);
	dbg_printf("%s: 0x%x \n", __FUNCTION__, val16);
	// Return 16-bit value read
	return Platform_le16toh(val16);
}
// -------------- Read an 8-bit value from specified address --------------------
uint8_t HAL_MemRead8(uint32_t address)
{
	uint8_t val8;
	struct spi_ioc_transfer xfer[2];
	uint32_t addr = Platform_htobe32((address << 8) | (0UL << 31));
	dbg_printf("%s: 0x%x -> 0x%x \n", __FUNCTION__, address, addr);

	HAL_ChipSelect(1);

	memset(xfer, 0, sizeof(xfer));

	xfer[0].tx_buf = (uintptr_t)&addr;
	xfer[0].rx_buf = (uintptr_t)NULL;
	xfer[0].len = 4;
	xfer[0].cs_change = 0;
	xfer[1].tx_buf = (uintptr_t)NULL;
	xfer[1].rx_buf = (uintptr_t)&val8;
	xfer[1].len = 1;
	xfer[1].cs_change = 0;

	if (Platform_SPI_transfer(xfer, 2) < 0)
	{
		err_printf("HAL_MemRead8: Transfer Failed \n");
	}

	HAL_ChipSelect(0);
	dbg_printf("%s: 0x%x \n", __FUNCTION__, val8);
	// Return 8-bit value read
	return val8;
}
// ############################# HOST COMMANDS #################################
// -------------------------- Write a host command -----------------------------
void HAL_HostCmdWrite(uint8_t cmd, uint8_t param)
{
	struct spi_ioc_transfer xfer[3];
	uint8_t zero = 0;

	HAL_ChipSelect(1);

	memset(xfer, 0, sizeof(xfer));

	xfer[0].tx_buf = (uintptr_t)&cmd;
	xfer[0].rx_buf = (uintptr_t)NULL;
	xfer[0].len = 1;
	xfer[0].cs_change = 0;
	xfer[1].tx_buf = (uintptr_t)&param;
	xfer[1].rx_buf = (uintptr_t)NULL;
	xfer[1].len = 1;
	xfer[1].cs_change = 0;
	xfer[2].tx_buf = (uintptr_t)&zero;
	xfer[2].rx_buf = (uintptr_t)NULL;
	xfer[2].len = 1;
	xfer[2].cs_change = 0;

	if (Platform_SPI_transfer(xfer, 3) < 0)
	{
		err_printf("HAL_HostCmdWrite: Transfer Failed \n");
	}

	HAL_ChipSelect(0);
}

// ######################## SUPPORTING FUNCTIONS ###############################

// --------- Increment co-processor address offset counter --------------------
void HAL_IncCmdPointer(uint16_t commandSize)
{
	// Calculate new offset
	writeCmdPointer = (writeCmdPointer + commandSize) & (EVE_RAM_CMD_SIZE   - 1);
	dbg_printf("%s is now 0x%x\n", __FUNCTION__, writeCmdPointer);
}

// --------- Increment co-processor address offset counter --------------------
uint16_t HAL_GetCmdPointer(void)
{
	dbg_printf("%s 0x%x\n", __FUNCTION__, writeCmdPointer);
	// Return new offset
	return writeCmdPointer;
}

void HAL_WriteCmdPointer(void)
{
	dbg_printf("%s 0x%x\n", __FUNCTION__, writeCmdPointer);
	// and move write pointer to here
	HAL_MemWrite32(EVE_REG_CMD_WRITE, writeCmdPointer);
}

// ------ Wait for co-processor read and write pointers to be equal ------------
uint8_t HAL_WaitCmdFifoEmpty(void)
{
	uint32_t readCmdPointer;

	// Wait until the two registers match
	do
	{
		// Read the graphics processor read pointer
		readCmdPointer = HAL_MemRead32(EVE_REG_CMD_READ);
	} while ((writeCmdPointer != readCmdPointer) && (readCmdPointer != 0xFFF));

	if (readCmdPointer == 0xFFF)
	{
		// Return 0xFF if an error occurred
		return 0xFF;
	}
	else
	{
		// Return 0 if pointers became equal successfully
		return 0;
	}
}
// ------------ Check how much free space is available in CMD FIFO -------------
uint16_t HAL_CheckCmdFreeSpace(void)
{
	uint32_t readCmdPointer = 0;
	uint16_t Fullness, Freespace;

	// Check the graphics processor read pointer
	readCmdPointer = HAL_MemRead32(EVE_REG_CMD_READ);

	// Fullness is difference between MCUs current write pointer value and the FT81x's REG_CMD_READ
	Fullness = ((writeCmdPointer - (uint16_t)readCmdPointer) & (EVE_RAM_CMD_SIZE  - 1));
	// Free Space is 4K - 4 - Fullness (-4 avoids buffer wrapping round)
	Freespace = (EVE_RAM_CMD_SIZE   - 4) - Fullness;

	return Freespace;
}

#endif // __linux__

