/**
 @file EVE_libmpsse.c
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

// Guard against being used for incorrect platform or architecture.
// USE_MPSSE holds the MPSSE channel to open.
#if defined(USE_MPSSE)

#pragma message ("Compiling " __FILE__ " for libMPSSE")

#include <string.h>
#include <stdio.h>
#include <stdint.h> // for Uint8/16/32 and Int8/16/32 data types
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifndef _WIN32
#include <endian.h>
#include <unistd.h>
#endif // _WIN32

#include "ftd2xx.h"
#include "libmpsse_spi.h"


// This is the Windows Platform specific section and contains the functions which
// enable the GPIO and SPI interfaces.

// ------------------ Platform specific initialisation  ------------------------

FT_HANDLE ftHandle;

void MCU_Init(void)
{
	FT_DEVICE_LIST_INFO_NODE devList;
	ChannelConfig channelConf;
	DWORD channel;
	DWORD channels;
	FT_STATUS status;

	channelConf.ClockRate = 100000;
	channelConf.LatencyTimer = 10;
	channelConf.configOptions = SPI_CONFIG_OPTION_MODE0 | SPI_CONFIG_OPTION_CS_DBUS3 | SPI_CONFIG_OPTION_CS_ACTIVELOW;

	Init_libMPSSE();

	status = SPI_GetNumChannels(&channels);
	for (channel = 0; channel < channels; channel++)
	{
		status = SPI_GetChannelInfo(channel, &devList);
		printf("SPI_GetNumChannels returned %d for channel %d\n", status, channel);
		/*print the dev info*/
		printf("		VID/PID: 0x%04x/0x%04x\n", devList.ID >> 16, devList.ID & 0xffff);
		printf("		SerialNumber: %s\n", devList.SerialNumber);
		printf("		Description: %s\n", devList.Description);
	}

	if (channels > USE_MPSSE)
	{
		// Open the the channel specified by the USE_MPSSE macro.
		// This must be defined to get this far.
		channel = USE_MPSSE;

		status = SPI_OpenChannel(channel, &ftHandle);
		if (status != FT_OK)
		{
			fprintf(stderr, "Channel %d failed to open status %d\n", channel, status);
			exit (-2);
		}
		status = SPI_InitChannel(ftHandle, &channelConf);
		if (status != FT_OK)
		{
			fprintf(stderr, "Channel %d failed to initialise SPI status %d\n", channel, status);
			exit (-3);
		}
	}
	else
	{
		fprintf(stderr, "No SPI channels found\n");
		exit (-1);
	}
}

void MCU_Setup(void)
{
}

// ########################### GPIO CONTROL ####################################

// --------------------- Chip Select line low ----------------------------------
void MCU_CSlow(void)
{
	SPI_ToggleCS(ftHandle, TRUE);
}

// --------------------- Chip Select line high ---------------------------------
void MCU_CShigh(void)
{
	SPI_ToggleCS(ftHandle, FALSE);
}

// -------------------------- PD line low --------------------------------------
void MCU_PDlow(void)
{
	// PD# set to 0, connect BLUE wire of MPSSE to PD# of FT8xx board
	SPI_ChangeCS(ftHandle, SPI_CONFIG_OPTION_MODE0 | SPI_CONFIG_OPTION_CS_DBUS7 | SPI_CONFIG_OPTION_CS_ACTIVELOW);
	SPI_ToggleCS(ftHandle, TRUE);
}

// ------------------------- PD line high --------------------------------------
void MCU_PDhigh(void)
{
	// PD# set to 1, connect BLUE wire of MPSSE to PD# of FT8xx board
	SPI_ToggleCS(ftHandle, FALSE);
	SPI_ChangeCS(ftHandle, SPI_CONFIG_OPTION_MODE0 | SPI_CONFIG_OPTION_CS_DBUS3 | SPI_CONFIG_OPTION_CS_ACTIVELOW);
}

// ------------------------- Delay functions -----------------------------------

void MCU_Delay_20ms(void)
{
#ifdef _WIN32
    Sleep(20);
#else
	usleep(20 * 1000);
#endif
}

void MCU_Delay_500ms(void)
{
#ifdef _WIN32
    Sleep(500);
#else
	usleep(500 * 1000);
#endif
}

// --------------------- SPI Send and Receive ----------------------------------

uint8_t MCU_SPIRead8(void)
{
	uint8_t DataRead = 0;
	DWORD transferred;

	SPI_Read(ftHandle, &DataRead, 1, &transferred, 0);

	return DataRead;
}

void MCU_SPIWrite8(uint8_t DataToWrite)
{
	DWORD transferred;

	SPI_Write(ftHandle, &DataToWrite, 1, &transferred, 0);
}

uint16_t MCU_SPIRead16(void)
{
	uint16_t DataRead = 0;
	DWORD transferred;

	SPI_Read(ftHandle, (UCHAR *)&DataRead, 2, &transferred, 0);

	return DataRead;
}

void MCU_SPIWrite16(uint16_t DataToWrite)
{
	DWORD transferred;

	SPI_Write(ftHandle, (UCHAR *)&DataToWrite, 2, &transferred, 0);
}

uint32_t MCU_SPIRead24(void)
{
	uint32_t DataRead = 0;
	DWORD transferred;

	SPI_Read(ftHandle, (UCHAR *)&DataRead, 3, &transferred, 0);

	return DataRead;
}

void MCU_SPIWrite24(uint32_t DataToWrite)
{
	DWORD transferred;

	SPI_Write(ftHandle, (UCHAR *)&DataToWrite, 3, &transferred, 0);
}

uint32_t MCU_SPIRead32(void)
{
	uint32_t DataRead = 0;
	DWORD transferred;

	SPI_Read(ftHandle, (UCHAR *)&DataRead, 4, &transferred, 0);

	return DataRead;
}

void MCU_SPIWrite32(uint32_t DataToWrite)
{
	DWORD transferred;

	SPI_Write(ftHandle, (UCHAR *)&DataToWrite, 4, &transferred, 0);
}

void MCU_SPIWrite(const uint8_t *DataToWrite, uint32_t length)
{
	DWORD transferred;

	SPI_Write(ftHandle, (UCHAR *)DataToWrite, length, &transferred, 0);
}

uint16_t MCU_htobe16(uint16_t h)
{
#ifdef _WIN32
	return _byteswap_ushort(h);
#else // _WIN32
	return htobe16(h);
#endif // _WIN32
	}

uint32_t MCU_htobe32(uint32_t h)
{
#ifdef _WIN32
	return _byteswap_ulong(h);
#else // _WIN32
	return htobe32(h);
#endif // _WIN32
}

uint16_t MCU_htole16(uint16_t h)
{
#ifdef _WIN32
    return (h);
#else // _WIN32
	return htole16(h);
#endif // _WIN32
}

uint32_t MCU_htole32(uint32_t h)
{
#ifdef _WIN32
    return (h);
#else // _WIN32
	return htole32(h);
#endif // _WIN32
}

uint16_t MCU_be16toh(uint16_t h)
{
#ifdef _WIN32
	return _byteswap_ushort(h);
#else // _WIN32
	return be16toh(h);
#endif // _WIN32
}

uint32_t MCU_be32toh(uint32_t h)
{
#ifdef _WIN32
	return _byteswap_ulong(h);
#else // _WIN32
	return be32toh(h);
#endif // _WIN32
}

uint16_t MCU_le16toh(uint16_t h)
{
#ifdef _WIN32
    return (h);
#else // _WIN32
	return le16toh(h);
#endif // _WIN32
}

uint32_t MCU_le32toh(uint32_t h)
{
#ifdef _WIN32
    return (h);
#else // _WIN32
	return le32toh(h);
#endif // _WIN32
}

#endif /* defined(USE_MPSSE) */
