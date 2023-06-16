/**
 * @file EVE_HalImpl_FT4222.c
 * @brief Eve_Hal framework APIs for FT4222 host platform
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
#if defined(FT4222_PLATFORM)

#if defined(EVE_MULTI_PLATFORM_TARGET)
#define EVE_HalImpl_initialize EVE_HalImpl_FT4222_initialize
#define EVE_HalImpl_release EVE_HalImpl_FT4222_release
#define EVE_Hal_list EVE_Hal_FT4222_list
#define EVE_Hal_info EVE_Hal_FT4222_info
#define EVE_Hal_isDevice EVE_Hal_FT4222_isDevice
#define EVE_HalImpl_defaults EVE_HalImpl_FT4222_defaults
#define EVE_HalImpl_open EVE_HalImpl_FT4222_open
#define EVE_HalImpl_close EVE_HalImpl_FT4222_close
#define EVE_HalImpl_idle EVE_HalImpl_FT4222_idle
#define EVE_Hal_flush EVE_Hal_FT4222_flush
#define EVE_Hal_startTransfer EVE_Hal_FT4222_startTransfer
#define EVE_Hal_endTransfer EVE_Hal_FT4222_endTransfer
#define EVE_Hal_transfer8 EVE_Hal_FT4222_transfer8
#define EVE_Hal_transfer16 EVE_Hal_FT4222_transfer16
#define EVE_Hal_transfer32 EVE_Hal_FT4222_transfer32
#define EVE_Hal_transferMem EVE_Hal_FT4222_transferMem
#define EVE_Hal_transferProgMem EVE_Hal_FT4222_transferProgMem
#define EVE_Hal_transferString EVE_Hal_FT4222_transferString
#define EVE_Hal_hostCommand EVE_Hal_FT4222_hostCommand
#define EVE_Hal_hostCommandExt3 EVE_Hal_FT4222_hostCommandExt3
#define EVE_Hal_powerCycle EVE_Hal_FT4222_powerCycle
#define EVE_UtilImpl_bootupDisplayGpio EVE_UtilImpl_FT4222_bootupDisplayGpio
#define EVE_Hal_setSPI EVE_Hal_FT4222_setSPI
#define EVE_Hal_restoreSPI EVE_Hal_FT4222_restoreSPI
#endif

#define FT4222_TRANSFER_SIZE_MAX (0xFFFF)
#define FT4222_WRITE_HEADER_SIZE (3)
#define FT4222_WRITE_SIZE_MAX (FT4222_TRANSFER_SIZE_MAX - FT4222_WRITE_HEADER_SIZE)

#define FT4222_READ_TIMEOUT (5000)
#define FT4222_WRITE_TIMEOUT (5000)

#define FT4222_LATENCY_TIME (2)

DWORD s_NumDevsD2XX;

/**
 * @brief Initialize HAL platform
 * 
 */
void EVE_HalImpl_initialize()
{
#if 0
	FT_DEVICE_LIST_INFO_NODE devList;
	FT_STATUS status;
	uint32_t numdevs;

	status = FT_CreateDeviceInfoList(&numdevs);
	if (FT_OK == status)
	{
		eve_printf_debug("Number of D2xx devices connected = %d\n", numdevs);
		// TODO: g_HalPlatform.TotalDevices = numdevs;

		FT_GetDeviceInfoDetail(0, &devList.Flags, &devList.Type, &devList.ID,
		    &devList.LocId,
		    devList.SerialNumber,
		    devList.Description,
		    &devList.ftHandle);
	}
	else
	{
		eve_printf_debug("FT_CreateDeviceInfoList failed");
		return;
	}

	eve_printf_debug("Information on channel number %d:\n", 0);
	/* print the dev info */
	eve_printf_debug(" Flags=0x%x\n", devList.Flags);
	eve_printf_debug(" Type=0x%x\n", devList.Host);
	eve_printf_debug(" ID=0x%x\n", devList.ID);
	eve_printf_debug(" LocId=0x%x\n", devList.LocId);
	eve_printf_debug(" SerialNumber=%s\n", devList.SerialNumber);
	eve_printf_debug(" Description=%s\n", devList.Description);
	eve_printf_debug(" ftHandle=0x%p\n", devList.ftHandle); /*is 0 unless open*/
#endif
}

/**
 * @brief Release HAL platform
 * 
 */
void EVE_HalImpl_release()
{
	/* no-op */
}

/* List the available devices */
size_t EVE_Hal_list()
{
	s_NumDevsD2XX = 0;
	FT_CreateDeviceInfoList(&s_NumDevsD2XX);
	return s_NumDevsD2XX;
}

/* Get info of the specified device */
void EVE_Hal_info(EVE_DeviceInfo *deviceInfo, size_t deviceIdx)
{
	FT_DEVICE_LIST_INFO_NODE devInfo = { 0 };

	memset(deviceInfo, 0, sizeof(EVE_DeviceInfo));
	if (deviceIdx >= s_NumDevsD2XX)
		return;

	if (FT_GetDeviceInfoDetail((DWORD)deviceIdx,
	        &devInfo.Flags, &devInfo.Type, &devInfo.ID, &devInfo.LocId,
	        devInfo.SerialNumber, devInfo.Description, &devInfo.ftHandle)
	    != FT_OK)
		return;

	strcpy_s(deviceInfo->SerialNumber, sizeof(deviceInfo->SerialNumber), devInfo.SerialNumber);
	strcpy_s(deviceInfo->DisplayName, sizeof(deviceInfo->DisplayName), devInfo.Description);
	if (!strcmp(devInfo.Description, "FT4222 A"))
		deviceInfo->Host = EVE_HOST_FT4222;
	deviceInfo->Opened = devInfo.Flags & FT_FLAGS_OPENED;
}

/* Check whether the context is the specified device */
bool EVE_Hal_isDevice(EVE_HalContext *phost, size_t deviceIdx)
{
	FT_DEVICE_LIST_INFO_NODE devInfo = { 0 };

	if (!phost)
		return false;
	if (EVE_HOST != EVE_HOST_FT4222)
		return false;
	if (deviceIdx >= s_NumDevsD2XX)
		return false;

	if (!phost->SpiHandle)
		return false;

	if (FT_GetDeviceInfoDetail((DWORD)deviceIdx,
	        &devInfo.Flags, &devInfo.Type, &devInfo.ID, &devInfo.LocId,
	        devInfo.SerialNumber, devInfo.Description, &devInfo.ftHandle)
	    != FT_OK)
		return false;

	return phost->SpiHandle == devInfo.ftHandle;
}

/**
 * @brief Get the default configuration parameters
 * 
 * @param parameters EVE_Hal framework's parameters
 */
bool EVE_HalImpl_defaults(EVE_HalParameters *parameters, size_t deviceIdx)
{
	bool res = deviceIdx >= 0 && deviceIdx < s_NumDevsD2XX;
	if (!res)
	{
		uint32_t i;

		if (!s_NumDevsD2XX)
			EVE_Hal_list();

		// Select first open device
		deviceIdx = 0;
		for (i = 0; i < s_NumDevsD2XX; ++i)
		{
			FT_DEVICE_LIST_INFO_NODE devInfo;
			if (FT_GetDeviceInfoDetail((DWORD)i,
			        &devInfo.Flags, &devInfo.Type, &devInfo.ID, &devInfo.LocId,
			        devInfo.SerialNumber, devInfo.Description, &devInfo.ftHandle)
			    != FT_OK)
				continue;
			if ((!(devInfo.Flags & FT_FLAGS_OPENED)) && (!strcmp(devInfo.Description, "FT4222 A")))
			{
				deviceIdx = i;
				res = true;
				break;
			}
		}
	}

	parameters->DeviceIdx = (uint32_t)deviceIdx;
	parameters->PowerDownPin = GPIO_PORT0;
	parameters->SpiCsPin = 1;
	parameters->SpiClockrateKHz = 12000;
	return res;
}

/***************************************************************************
* Interface Description    : Function to compute FT4222 sys clock and divisor
*                            to obtain user requested SPI communication clock
*                            Available FT4222_ClockRate (FT4222 system clock):
*                               SYS_CLK_60,
*                               SYS_CLK_24,
*                               SYS_CLK_48,
*                               SYS_CLK_80
*                            Divisors available (FT4222_SPIClock):
*                               CLK_NONE,
*                               CLK_DIV_2,
*                               CLK_DIV_4,
*                               CLK_DIV_8,
*                               CLK_DIV_16,
*                               CLK_DIV_32,
*                               CLK_DIV_64,
*                               CLK_DIV_128,
*                               CLK_DIV_256,
*                               CLK_DIV_512
* Implementation           : Good performance is observed with divisors other than CLK_DIV_2
*                            and CLK_DIV_4 from test report by firmware developers.
*                            Hence supporting the following clocks for SPI communication
*                               5000KHz
*                               10000KHz
*                               15000KHz
*                               20000KHz
*                               30000KHz
*                            Global variable phost->SpiClockrateKHz is
*                            updated accodingly
* Return Value             : bool_t
*                               TRUE : Supported by FT4222
*                               FALSE : Not supported by FT4222
*
* Author                   :
****************************************************************************/

/**
 * @brief Compute the system clock and SPI clock
 * 
 * @param phost Pointer to Hal context
 * @param sysclk System clock
 * @param sysdivisor SPI clock divisor
 * @return true True if ok
 * @return false False if error
 */
bool computeCLK(EVE_HalContext *phost, FT4222_ClockRate *sysclk, FT4222_SPIClock *sysdivisor, const EVE_HalParameters *parameters)
{
	/* phost->Parameters.SpiClockrateKHz is the user requested SPI communication clock */

	if (parameters->SpiClockrateKHz <= 5000)
	{
		/* set to 5000 KHz */
		*sysclk = SYS_CLK_80;
		*sysdivisor = CLK_DIV_16;
		phost->SpiClockrateKHz = 5000;
	}
	else if (parameters->SpiClockrateKHz <= 10000)
	{
		/* set to 10000 KHz */
		*sysclk = SYS_CLK_80;
		*sysdivisor = CLK_DIV_8;
		phost->SpiClockrateKHz = 10000;
	}
	else if (parameters->SpiClockrateKHz <= 15000)
	{
		/* This is the default */
		/* set to 15000 KHz */
		*sysclk = SYS_CLK_60;
		*sysdivisor = CLK_DIV_4;
		phost->SpiClockrateKHz = 15000;
	}
	else if (parameters->SpiClockrateKHz <= 20000)
	{
		/* set to 20000 KHz */
		*sysclk = SYS_CLK_80;
		*sysdivisor = CLK_DIV_4;
		phost->SpiClockrateKHz = 20000;
	}
	else
	{
		/* 30MHz with Quad SPI seems to work reliably on BT816 */
		/* set to 30000 KHz */
		*sysclk = SYS_CLK_60;
		*sysdivisor = CLK_DIV_2;
		phost->SpiClockrateKHz = 30000;
	}
	eve_printf_debug("User Selected SPI clk : %d KHz\n", (int)parameters->SpiClockrateKHz);
	eve_printf_debug("Configured clk : %d KHz, Ft4222 sys clk enum = %d , divisor enum = %d\n", (int)phost->SpiClockrateKHz, *sysclk, *sysdivisor);
	return true;
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
	FT_STATUS status;
	FT4222_Version pversion;
	FT4222_ClockRate ftclk = 0;
	FT4222_ClockRate selclk = 0;
	FT4222_SPIClock seldiv = 0;
	/* GPIO0         , GPIO1      , GPIO2       , GPIO3         } */
	GPIO_Dir gpio_dir[4] = { GPIO_OUTPUT, GPIO_INPUT, GPIO_INPUT, GPIO_INPUT };

	DWORD deviceIdxA = parameters->DeviceIdx;
	FT_DEVICE_LIST_INFO_NODE devInfoA;
	DWORD deviceIdxB;
	FT_DEVICE_LIST_INFO_NODE devInfoB;

	bool ret = true;

	phost->SpiHandle = phost->GpioHandle = NULL;

#ifdef EVE_MULTI_GRAPHICS_TARGET
	phost->GpuDefs = &EVE_GpuDefs_FT80X;
#endif

	memset(&devInfoA, 0, sizeof(devInfoA));
	status = FT_GetDeviceInfoDetail(deviceIdxA,
	    &devInfoA.Flags, &devInfoA.Type, &devInfoA.ID, &devInfoA.LocId,
	    devInfoA.SerialNumber, devInfoA.Description, &devInfoA.ftHandle);
	if (status != FT_OK)
	{
		eve_printf_debug("FT_GetDeviceInfoDetail failed\n");
		ret = false;
	}
	if (devInfoA.Flags & FT_FLAGS_OPENED)
	{
		eve_printf_debug("Device FT4222 A already opened\n");
		ret = false;
	}
	eve_printf_debug("FT4222 open '%s'\n", devInfoA.Description);

	if (ret)
	{
		for (deviceIdxB = deviceIdxA + 1; deviceIdxB < s_NumDevsD2XX; ++deviceIdxB)
		{
			memset(&devInfoB, 0, sizeof(devInfoB));
			status = FT_GetDeviceInfoDetail(deviceIdxB,
			    &devInfoB.Flags, &devInfoB.Type, &devInfoB.ID, &devInfoB.LocId,
			    devInfoB.SerialNumber, devInfoB.Description, &devInfoB.ftHandle);
			if (status != FT_OK)
				continue;
			if (!strcmp(devInfoB.Description, "FT4222 B"))
				break;
		}
		if (deviceIdxB >= s_NumDevsD2XX)
		{
			eve_printf_debug("FT4222 B not found\n");
			ret = false;
		}
		else if (devInfoB.Flags & FT_FLAGS_OPENED)
		{
			eve_printf_debug("Device FT4222 B already opened\n");
			ret = false;
		}
	}

	if (ret)
	{
		status = FT_Open(deviceIdxA, &phost->SpiHandle);
		if (status != FT_OK)
		{
			eve_printf_debug("FT_Open FT4222 A failed %d\n", (int)status);
			phost->SpiHandle = NULL;
			ret = false;
		}
	}

	if (ret)
	{
		status = FT_Open(deviceIdxB, &phost->GpioHandle);
		if (status != FT_OK)
		{
			eve_printf_debug("FT_Open FT4222 B failed %d\n", (int)status);
			FT_Close(phost->SpiHandle);
			phost->GpioHandle = NULL;
			phost->SpiHandle = NULL;
			ret = false;
		}
	}

	if (ret)
	{
		status = FT4222_GetVersion(phost->SpiHandle, &pversion);
		if (status != FT4222_OK)
			eve_printf_debug("FT4222_GetVersion failed\n");
		else
			eve_printf_debug("SPI:chipversion = 0x%x\t dllversion = 0x%x\n", (unsigned int)pversion.chipVersion, (unsigned int)pversion.dllVersion);
	}

	if (ret)
	{
		//Set default Read timeout 5s and Write timeout 5sec
		status = FT_SetTimeouts(phost->SpiHandle, FT4222_READ_TIMEOUT, FT4222_WRITE_TIMEOUT);
		if (status != FT_OK)
		{
			eve_printf_debug("FT_SetTimeouts failed!\n");
			ret = false;
		}
	}

	if (ret)
	{
		// no latency to usb
		status = FT_SetLatencyTimer(phost->SpiHandle, FT4222_LATENCY_TIME);
		if (status != FT_OK)
		{
			eve_printf_debug("FT_SetLatencyTimerfailed!\n");
			ret = false;
		}
	}

	if (ret)
	{
		if (!computeCLK(phost, &selclk, &seldiv, parameters))
		{
			eve_printf_debug("Requested clock %d KHz is not supported in FT4222\n", parameters->SpiClockrateKHz);
			ret = false;
		}
	}

	if (ret)
	{
		status = FT4222_SetClock(phost->SpiHandle, selclk);
		if (status != FT4222_OK)
		{
			eve_printf_debug("FT4222_SetClock failed!\n");
			ret = false;
		}

		status = FT4222_GetClock(phost->SpiHandle, &ftclk);

		if (status != FT4222_OK)
			eve_printf_debug("FT4222_SetClock failed\n");
		else
			eve_printf_debug("FT4222 clk = %d\n", ftclk);
	}

	if (ret)
	{
		/* Interface 1 is SPI master */
		status = FT4222_SPIMaster_Init(
		    phost->SpiHandle,
		    SPI_IO_SINGLE,
		    seldiv,
		    CLK_IDLE_LOW, //,CLK_IDLE_HIGH
		    CLK_LEADING, // CLK_LEADING CLK_TRAILING
		    parameters->SpiCsPin); /* slave selection output pins */
		if (status != FT4222_OK)
		{
			eve_printf_debug("Init FT4222 as SPI master device failed!\n");
			ret = false;
		}
		phost->SpiCsPin = parameters->SpiCsPin;
		phost->PowerDownPin = parameters->PowerDownPin;

		status = FT4222_SPI_SetDrivingStrength(phost->SpiHandle, DS_4MA, DS_4MA, DS_4MA);
		if (status != FT4222_OK)
		{
			eve_printf_debug("FT4222_SPI_SetDrivingStrength failed!\n");
		}

		EVE_sleep(20);

		status = FT4222_SetSuspendOut(phost->GpioHandle, false);
		if (status != FT4222_OK)
		{
			eve_printf_debug("Disable suspend out function on GPIO2 failed!\n");
			ret = false;
		}

		status = FT4222_SetWakeUpInterrupt(phost->GpioHandle, false);
		if (status != FT4222_OK)
		{
			eve_printf_debug("Disable wakeup/interrupt feature on GPIO3 failed!\n");
			ret = false;
		}
		/* Interface 2 is GPIO */
		status = FT4222_GPIO_Init(phost->GpioHandle, gpio_dir);
		if (status != FT4222_OK)
		{
			eve_printf_debug("Init FT4222 as GPIO interface failed!\n");
			ret = false;
		}
	}

	if (ret)
	{
		phost->SpiChannels = EVE_SPI_SINGLE_CHANNEL;
		phost->SpiDummyBytes = 1;
		phost->Status = EVE_STATUS_OPENED;
		++g_HalPlatform.OpenedDevices;
	}

	return ret;
}

/**
 * @brief Close a HAL context
 * 
 * @param phost Pointer to Hal context
 */
void EVE_HalImpl_close(EVE_HalContext *phost)
{
	FT4222_STATUS status;
	phost->Status = EVE_STATUS_CLOSED;
	--g_HalPlatform.OpenedDevices;

	if ((status = FT4222_UnInitialize(phost->SpiHandle)) != FT4222_OK)
		eve_printf_debug("FT4222_UnInitialize failed %d\n", status);

	if ((status = FT4222_UnInitialize(phost->GpioHandle)) != FT4222_OK)
		eve_printf_debug("FT4222_UnInitialize failed %d\n", status);

	if ((status = FT_Close(phost->SpiHandle)) != FT_OK)
		eve_printf_debug("CLOSE failed %d\n", status);

	if ((status = FT_Close(phost->GpioHandle)) != FT_OK)
		eve_printf_debug("CLOSE failed %d\n", status);
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

static bool flush(EVE_HalContext *phost);

/**
 * @brief Increase RAM_G adress
 * 
 * @param addr Address offset
 * @param inc Number of bytes to increase
 * @return uint32_t New address in RAM_G
 */
static inline uint32_t incrementRamGAddr(EVE_HalContext *phost, uint32_t addr, uint32_t inc)
{
	if (!EVE_Hal_supportCmdB(phost) || (addr != REG_CMDB_WRITE))
	{
		bool wrapCmdAddr = (addr >= RAM_CMD) && (addr < (RAM_CMD + EVE_CMD_FIFO_SIZE));
		addr += inc;
		if (wrapCmdAddr)
			addr = RAM_CMD + (addr & EVE_CMD_FIFO_MASK);
	}
	return addr;
}

/**
 * @brief Read a block data from Coprocessor
 * 
 * @param phost Pointer to Hal context
 * @param buffer Buffer to get result
 * @param size Number of bytes to read
 */
static inline bool rdBuffer(EVE_HalContext *phost, uint8_t *buffer, uint32_t size)
{
	FT4222_STATUS status;

	uint8_t hrdpkt[8] = { 0 }; // 3 byte addr + 2 or 1 byte dummy

	while (size)
	{
		uint32_t sizeTransferred;
		uint32_t bytesPerRead;
		uint32_t addr = phost->SpiRamGAddr;

		/* Compose the HOST MEMORY READ packet */
		hrdpkt[0] = (uint8_t)(addr >> 16) & 0xFF;
		hrdpkt[1] = (uint8_t)(addr >> 8) & 0xFF;
		hrdpkt[2] = (uint8_t)(addr & 0xFF);

		if (size <= FT4222_TRANSFER_SIZE_MAX)
			bytesPerRead = size;
		else
			bytesPerRead = FT4222_TRANSFER_SIZE_MAX;

		if (phost->SpiChannels == EVE_SPI_SINGLE_CHANNEL)
		{
			uint16_t sizeWritten;
			uint16_t sizeRead;

			status = FT4222_SPIMaster_SingleWrite(
			    phost->SpiHandle,
			    hrdpkt,
			    3 + phost->SpiDummyBytes, /* 3 address and chosen dummy bytes */
			    &sizeWritten,
			    FALSE);

			if ((status != FT4222_OK) || (sizeWritten != (3 + phost->SpiDummyBytes)))
			{
				eve_printf_debug("FT4222_SPIMaster_SingleWrite failed, sizeTransferred is %d with status %d\n", sizeWritten, status);
				if (sizeWritten != (3 + phost->SpiDummyBytes))
					phost->Status = EVE_STATUS_ERROR;
				return false;
			}

			status = FT4222_SPIMaster_SingleRead(
			    phost->SpiHandle,
			    buffer,
			    bytesPerRead,
			    &sizeRead,
			    TRUE);

			if ((status != FT4222_OK) || (sizeRead != bytesPerRead))
			{
				eve_printf_debug("FT4222_SPIMaster_SingleRead failed,sizeTransferred is %d with status %d\n", sizeRead, status);
				if (sizeRead != bytesPerRead)
					phost->Status = EVE_STATUS_ERROR;
				return false;
			}

			sizeTransferred = sizeRead;
		}
		else
		{
			status = FT4222_SPIMaster_MultiReadWrite(
			    phost->SpiHandle,
			    (uint8 *)buffer,
			    hrdpkt,
			    0,
			    3 + phost->SpiDummyBytes, /* 3 addr + dummy bytes */
			    bytesPerRead,
			    &sizeTransferred);

			if ((status != FT4222_OK) || (sizeTransferred != bytesPerRead))
			{
				eve_printf_debug("FT4222_SPIMaster_MultiReadWrite failed, sizeTransferred is %d with status %d\n", sizeTransferred, status);
				if (sizeTransferred != bytesPerRead)
					phost->Status = EVE_STATUS_ERROR;
				return false;
			}
		}

		buffer += sizeTransferred;
		if (size)
		{
			eve_assert_ex(sizeTransferred <= size, "Cannot have transferred more than size\n");
			size -= sizeTransferred;
		}

		if (!EVE_Hal_supportCmdB(phost) || (addr != REG_CMDB_WRITE))
		{
			bool wrapCmdAddr = (addr >= RAM_CMD) && (addr < (RAM_CMD + EVE_CMD_FIFO_SIZE));
			addr += sizeTransferred;
			if (wrapCmdAddr)
				addr = RAM_CMD + (addr & EVE_CMD_FIFO_MASK);
			phost->SpiRamGAddr = addr;
		}
	}

	return true;
}

/**
 * @brief Write a block data to Coprocessor
 * 
 * @param phost Pointer to Hal context
 * @param buffer Data buffer to write
 * @param size Size of buffer
 */
static inline bool wrBuffer(EVE_HalContext *phost, const uint8_t *buffer, uint32_t size)
{
	FT4222_STATUS status;

	if (buffer && (size < (sizeof(phost->SpiWrBuf) - phost->SpiWrBufIndex - FT4222_WRITE_HEADER_SIZE)))
	{
		/* Write to buffer */
		memcpy(&phost->SpiWrBuf[phost->SpiWrBufIndex + FT4222_WRITE_HEADER_SIZE], buffer, size);
		phost->SpiWrBufIndex += size;
		return true;
	}
	else
	{
		if (buffer && phost->SpiWrBufIndex)
		{
			/* Buffer is over size, flush now */
			if (!flush(phost))
				return false;

			/* Write to buffer */
			if (size < (sizeof(phost->SpiWrBuf) - FT4222_WRITE_HEADER_SIZE))
				return wrBuffer(phost, buffer, size);
		}

		if (buffer || phost->SpiWrBufIndex)
		{
			/* Flush now, or write oversize buffer */
			uint32_t addr = phost->SpiRamGAddr;

			if (!buffer)
			{
				/* Flushing */
				size = phost->SpiWrBufIndex;
				phost->SpiWrBufIndex = 0;
			}

			while (size)
			{
				uint32_t sizeTransferred;
				uint32_t bytesPerWrite;

				if (size <= FT4222_WRITE_SIZE_MAX)
					bytesPerWrite = size + FT4222_WRITE_HEADER_SIZE;
				else
					bytesPerWrite = FT4222_TRANSFER_SIZE_MAX;

				if (buffer)
				{
					/* Writing large buffer */
					memcpy(phost->SpiWrBuf + FT4222_WRITE_HEADER_SIZE, buffer, (bytesPerWrite - FT4222_WRITE_HEADER_SIZE));
				}

				/* Compose the HOST MEMORY WRITE packet */
				phost->SpiWrBuf[0] = (addr >> 16) | 0x80; /* MSB bits 10 for WRITE */
				phost->SpiWrBuf[1] = (addr >> 8) & 0xFF;
				phost->SpiWrBuf[2] = addr & 0xFF;

				if (phost->SpiChannels == EVE_SPI_SINGLE_CHANNEL)
				{
					uint16_t sizeWritten;

					status = FT4222_SPIMaster_SingleWrite(
					    phost->SpiHandle,
					    phost->SpiWrBuf,
					    bytesPerWrite,
					    &sizeWritten,
					    TRUE);

					if ((status != FT4222_OK) || (sizeWritten != bytesPerWrite))
					{
						eve_printf_debug("%d FT4222_SPIMaster_SingleWrite failed, sizeTransferred is %d with status %d\n", __LINE__, sizeWritten, status);
						if (sizeWritten != bytesPerWrite)
							phost->Status = EVE_STATUS_ERROR;
						return false;
					}

					sizeTransferred = sizeWritten;
				}
				else
				{
					uint8_t dummyRead[8];

					status = FT4222_SPIMaster_MultiReadWrite(
					    phost->SpiHandle,
					    dummyRead,
					    phost->SpiWrBuf,
					    0,
					    bytesPerWrite,
					    0,
					    &sizeTransferred);

					if (status != FT4222_OK)
					{
						eve_printf_debug("FT4222_SPIMaster_MultiReadWrite failed, status %d\n", status);
						return false;
					}

					sizeTransferred = bytesPerWrite;
				}

				if (buffer)
				{
					buffer += sizeTransferred - FT4222_WRITE_HEADER_SIZE;
				}
				if (size)
				{
					eve_assert_ex((sizeTransferred - FT4222_WRITE_HEADER_SIZE) <= (int32_t)size, "Cannot have transferred more than size\n");
					size -= sizeTransferred - FT4222_WRITE_HEADER_SIZE;
					// eve_assert_ex(!(buffer && size), "Cannot have space left after flushing buffer\n");
				}

				addr = incrementRamGAddr(phost, addr, (sizeTransferred - FT4222_WRITE_HEADER_SIZE));
				phost->SpiRamGAddr = addr;
			}
		}

		return true;
	}
}

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

	if (!EVE_Hal_supportCmdB(phost) && addr == REG_CMD_WRITE && rw == EVE_TRANSFER_WRITE)
	{
		/* Bypass fifo write pointer write */
#if !defined(EVE_SUPPORT_CMDB) || defined(EVE_MULTI_GRAPHICS_TARGET)
		phost->SpiWpWriting = true;
#else
		eve_assert(false);
#endif
	}
	else if (addr != incrementRamGAddr(phost, phost->SpiRamGAddr, phost->SpiWrBufIndex) || rw == EVE_TRANSFER_READ)
	{
		/* Close any write transfer that was left open, if the address changed */
		flush(phost);
		phost->SpiRamGAddr = addr;
	}

	if (phost->Status != EVE_STATUS_ERROR)
	{
		if (rw == EVE_TRANSFER_READ)
			phost->Status = EVE_STATUS_READING;
		else
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
	uint32_t addr;

	eve_assert(phost->Status == EVE_STATUS_READING || phost->Status == EVE_STATUS_WRITING);

	/* Transfers to FIFO and DL are kept open */
	addr = phost->SpiRamGAddr;
	if (addr != (EVE_Hal_supportCmdB(phost) ? REG_CMDB_WRITE : REG_CMD_WRITE)
	    && !((addr >= RAM_CMD) && (addr < (RAM_CMD + EVE_CMD_FIFO_SIZE)))
	    && !((addr >= RAM_DL) && (addr < (RAM_CMD + EVE_DL_SIZE))))
	{
		flush(phost);
	}

#if !defined(EVE_SUPPORT_CMDB) || defined(EVE_MULTI_GRAPHICS_TARGET)
	if (!EVE_Hal_supportCmdB(phost))
	{
		phost->SpiWpWriting = false;
	}
#endif

	if (phost->Status != EVE_STATUS_ERROR)
		phost->Status = EVE_STATUS_OPENED;
}

/**
 * @brief Flush data to Coprocessor
 * 
 * @param phost Pointer to Hal context
 */
static bool flush(EVE_HalContext *phost)
{
	bool res = true;
	if (phost->SpiWrBufIndex)
	{
		res = wrBuffer(phost, NULL, 0);
	}
	eve_assert(!phost->SpiWrBufIndex);
#if !defined(EVE_SUPPORT_CMDB) || defined(EVE_MULTI_GRAPHICS_TARGET)
	if (!EVE_Hal_supportCmdB(phost))
	{
		if (phost->SpiWpWritten)
		{
			phost->SpiWpWritten = false;
			phost->SpiRamGAddr = REG_CMD_WRITE;
			phost->SpiWrBufIndex = 2;
			phost->SpiWrBuf[FT4222_WRITE_HEADER_SIZE + 0] = phost->SpiWpWrite & 0xFF;
			phost->SpiWrBuf[FT4222_WRITE_HEADER_SIZE + 1] = phost->SpiWpWrite >> 8;
			res = wrBuffer(phost, NULL, 0);
		}
		eve_assert(!phost->SpiWrBufIndex);
	}
#endif
	return res;
}

/**
 * @brief Flush data to Coprocessor
 * 
 * @param phost Pointer to Hal context
 */
void EVE_Hal_flush(EVE_HalContext *phost)
{
	eve_assert(phost->Status == EVE_STATUS_OPENED);
	flush(phost);
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
#if defined(EVE_BUFFER_WRITES)
#if !defined(EVE_SUPPORT_CMDB) || defined(EVE_MULTI_GRAPHICS_TARGET)
	if (!EVE_Hal_supportCmdB(phost))
	{
		eve_assert(!phost->SpiWpWriting);
	}
#endif
#endif
	if (phost->Status == EVE_STATUS_READING)
	{
		rdBuffer(phost, &value, 1);
		return value;
	}
	else
	{
		wrBuffer(phost, &value, 1);
		return 0;
	}
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
#if defined(EVE_BUFFER_WRITES)
#if !defined(EVE_SUPPORT_CMDB) || defined(EVE_MULTI_GRAPHICS_TARGET)
	if (!EVE_Hal_supportCmdB(phost))
	{
		if (phost->SpiWpWriting)
		{
			phost->SpiWpWrite = value;
			phost->SpiWpWritten = true;
			return 0;
		}
	}
#endif
#endif
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
#if defined(EVE_BUFFER_WRITES)
#if !defined(EVE_SUPPORT_CMDB) || defined(EVE_MULTI_GRAPHICS_TARGET)
	if (!EVE_Hal_supportCmdB(phost))
	{
		eve_assert(!phost->SpiWpWriting);
	}
#endif
#endif
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

#if defined(EVE_BUFFER_WRITES)
#if !defined(EVE_SUPPORT_CMDB) || defined(EVE_MULTI_GRAPHICS_TARGET)
	if (!EVE_Hal_supportCmdB(phost))
	{
		eve_assert(!phost->SpiWpWriting);
	}
#endif
#endif

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

#if defined(EVE_BUFFER_WRITES)
#if !defined(EVE_SUPPORT_CMDB) || defined(EVE_MULTI_GRAPHICS_TARGET)
	if (!EVE_Hal_supportCmdB(phost))
	{
		eve_assert(!phost->SpiWpWriting);
	}
#endif
#endif

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
	uint32_t transferred;

	if (!size)
	{
		/* TODO: Support different padding options */
		eve_assert(padMask == 0x3);
		EVE_Hal_transfer32(phost, 0);
		return 4;
	}

#if defined(EVE_BUFFER_WRITES)
#if !defined(EVE_SUPPORT_CMDB) || defined(EVE_MULTI_GRAPHICS_TARGET)
	if (!EVE_Hal_supportCmdB(phost))
	{
		eve_assert(!phost->SpiWpWriting);
	}
#endif
#endif
	eve_assert(size <= EVE_CMD_STRING_MAX);
	transferred = 0;
	if (phost->Status == EVE_STATUS_WRITING)
	{
		uint8_t buffer[EVE_CMD_STRING_MAX + 1];

		for (;;)
		{
			char c = str[index + (transferred)];
			buffer[transferred++] = c;
			// putchar(c ? c : '_');
			if (!c)
			{
				break;
			}
			if (transferred >= size)
			{
				buffer[transferred++] = 0;
				// putchar('_');
				break;
			}
		}
		while (transferred & padMask)
		{
			buffer[transferred++] = 0;
			// putchar('_');
		}

		eve_assert(transferred);

		wrBuffer(phost, buffer, transferred);
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
	FT4222_STATUS status;
	uint8_t dummyRead;

	uint8_t transferArray[3];

	uint16_t sizeTransferred;
	uint32_t sizeOfRead;

	flush(phost);

	transferArray[0] = cmd;
	transferArray[1] = 0;
	transferArray[2] = 0;

	switch (phost->SpiChannels)
	{
	case EVE_SPI_SINGLE_CHANNEL:
		/* FYI : All HOST CMDs should only be executed in single channel mode*/
		status = FT4222_SPIMaster_SingleWrite(
		    phost->SpiHandle,
		    transferArray,
		    sizeof(transferArray),
		    &sizeTransferred,
		    true);
		if (FT4222_OK != status)
		{
			eve_printf_debug("SPI write failed = %d\n", status);
			phost->Status = EVE_STATUS_ERROR;
		}
		break;
	case EVE_SPI_DUAL_CHANNEL:
	case EVE_SPI_QUAD_CHANNEL:
		/* only reset command among phost commands can be executed in multi channel mode*/
		status = FT4222_SPIMaster_MultiReadWrite(
		    phost->SpiHandle,
		    &dummyRead,
		    transferArray,
		    0,
		    sizeof(transferArray),
		    0,
		    &sizeOfRead);
		if (FT4222_OK != status)
		{
			eve_printf_debug("SPI write failed = %d\n", status);
			phost->Status = EVE_STATUS_ERROR;
		}
		break;
	default:
		eve_printf_debug("No transfer\n");
	}
}

/**
 * @brief This API sends a 3byte command to the phost
 * 
 * @param phost Pointer to Hal context
 * @param cmd Command to send
 */
void EVE_Hal_hostCommandExt3(EVE_HalContext *phost, uint32_t cmd)
{
	FT4222_STATUS status;
	uint8_t dummyRead;
	uint8_t transferArray[3];

	uint16_t sizeTransferred;
	uint32_t sizeOfRead;

	flush(phost);

	transferArray[0] = cmd;
	transferArray[1] = (cmd >> 8) & 0xff;
	transferArray[2] = (cmd >> 16) & 0xff;

	switch (phost->SpiChannels)
	{
	case EVE_SPI_SINGLE_CHANNEL:
		status = FT4222_SPIMaster_SingleWrite(
		    phost->SpiHandle,
		    transferArray,
		    sizeof(transferArray),
		    &sizeTransferred,
		    true);
		if (FT4222_OK != status)
		{
			eve_printf_debug("SPI write failed = %d\n", status);
			phost->Status = EVE_STATUS_ERROR;
		}
		break;
	case EVE_SPI_DUAL_CHANNEL:
	case EVE_SPI_QUAD_CHANNEL:
		/* FYI : Mostly all HOST CMDs can be executed in single channel mode
		* except system reset cmd */
		status = FT4222_SPIMaster_MultiReadWrite(
		    phost->SpiHandle,
		    &dummyRead,
		    transferArray,
		    0,
		    sizeof(transferArray),
		    0,
		    &sizeOfRead);
		if (FT4222_OK != status)
		{
			eve_printf_debug("SPI write failed = %d\n", status);
			phost->Status = EVE_STATUS_ERROR;
		}
		break;
	default:
		eve_printf_debug("No transfer\n");
	}
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
	FT4222_STATUS ftstatus;
	FT4222_SPIMode spimode;

	flush(phost);

	/* switch FT4222 to relevant multi channel SPI communication mode */
	if (numchnls == EVE_SPI_DUAL_CHANNEL)
		spimode = SPI_IO_DUAL;
	else if (numchnls == EVE_SPI_QUAD_CHANNEL)
		spimode = SPI_IO_QUAD;
	else
		spimode = SPI_IO_SINGLE;

	ftstatus = FT4222_SPIMaster_SetLines(phost->SpiHandle, spimode);
	if (FT4222_OK != ftstatus)
		eve_printf_debug("FT4222_SPIMaster_SetLines failed with status %d\n", ftstatus);

	/* FT81x swicthed to dual/quad mode, now update global HAL context */
	phost->SpiChannels = numchnls;
	phost->SpiDummyBytes = numdummy;
}

/**
 * @brief Toggle PD_N pin of FT800 board for a power cycle
 * 
 * @param phost Pointer to Hal context
 * @param up Up or Down
 */
bool EVE_Hal_powerCycle(EVE_HalContext *phost, bool up)
{
	bool res = true;
	flush(phost);

	if (up)
	{
		FT4222_STATUS status = FT4222_OTHER_ERROR;

		if (FT4222_OK != (status = FT4222_GPIO_Write(phost->GpioHandle, phost->PowerDownPin, 0)))
		{
			eve_printf_debug("FT4222_GPIO_Write error = %d\n", status);
			res = false;
		}
		EVE_sleep(20);

		if (FT4222_OK != (status = FT4222_GPIO_Write(phost->GpioHandle, phost->PowerDownPin, 1)))
		{
			eve_printf_debug("FT4222_GPIO_Write error = %d\n", status);
			res = false;
		}
		EVE_sleep(20);
	}
	else
	{
		FT4222_STATUS status = FT4222_OTHER_ERROR;

		if (FT4222_OK != (status = FT4222_GPIO_Write(phost->GpioHandle, phost->PowerDownPin, 1)))
		{
			eve_printf_debug("FT4222_GPIO_Write error = %d\n", status);
			res = false;
		}
		EVE_sleep(20);

		if (FT4222_OK != (status = FT4222_GPIO_Write(phost->GpioHandle, phost->PowerDownPin, 0)))
		{
			eve_printf_debug("FT4222_GPIO_Write error = %d\n", status);
			res = false;
		}
		EVE_sleep(20);
	}

	/* Reset to single channel SPI mode */
	setSPI(phost, EVE_SPI_SINGLE_CHANNEL, 1);

	return res;
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
	uint8_t writebyte;

	flush(phost);
	if (EVE_CHIPID < EVE_FT810)
		return;

	writebyte = 0;

	if ((numchnls > EVE_SPI_QUAD_CHANNEL) || (numdummy > 2) || (numdummy < 1))
		return; // error

	/* Switch EVE to multi channel SPI mode */
	writebyte = numchnls;
	if (numdummy == 2)
		writebyte |= EVE_SPI_TWO_DUMMY_BYTES;
	EVE_Hal_wr8(phost, REG_SPI_WIDTH, writebyte);

	/* Switch FT4222 to multi channel SPI mode */
	setSPI(phost, numchnls, numdummy);
}

void EVE_Hal_restoreSPI(EVE_HalContext *phost)
{
	flush(phost);
	if (EVE_CHIPID < EVE_FT810)
		return;

	setSPI(phost, phost->SpiChannels, phost->SpiDummyBytes);
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
	/* no-op */
	return true;
}

#endif /* #if defined(FT4222_PLATFORM) */

/* end of file */