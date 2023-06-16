/**
 * @file EVE_LoadFile_STDIO.c
 * @brief Eve_Hal framework APIs for loading file with POSIX
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

#include "EVE_LoadFile.h"
#include "EVE_Platform.h"
#if !defined(FT9XX_PLATFORM) && !defined(RP2040_PLATFORM)

#include <stdio.h>

/**
 * @brief Mount the SDcard
 * 
 * @param phost  Pointer to Hal context
 * @return true True if ok
 * @return false False if error
 */
EVE_HAL_EXPORT bool EVE_Util_loadSdCard(EVE_HalContext *phost)
{
	/* no-op */
	return true;
}

EVE_HAL_EXPORT bool EVE_Util_sdCardReady(EVE_HalContext *phost)
{
	/* no-op */
	return true;
}

/**
 * @brief Load a raw file into RAM_G
 * 
 * @param phost  Pointer to Hal context
 * @param address Address in RAM_G
 * @param filename File to load
 * @return true True if ok
 * @return false False if error
 */
#ifdef _WIN32
static bool loadRawFile(EVE_HalContext *phost, uint32_t address, const char *filename, const wchar_t *filenameW)
#else
EVE_HAL_EXPORT bool EVE_Util_loadRawFile(EVE_HalContext *phost, uint32_t address, const char *filename)
#endif
{
	FILE *afile;
	uint32_t ftsize = 0;
	uint8_t pbuff[8192];
	uint16_t blocklen;
	uint32_t addr = address;
	errno_t err = 0;

#ifdef _WIN32
	err = filename ? fopen_s(&afile, filename, "rb") : _wfopen_s(&afile, filenameW, L"rb");
#else
	err = fopen_s(&afile, filename, "rb"); // read Binary (rb)
#endif
	if (err || afile == NULL)
	{
#ifdef _WIN32
		if (!filename)
			eve_printf_debug("Unable to open: %ls\n", filenameW);
		else
#endif
			eve_printf_debug("Unable to open: %s\n", filename);
		return false;
	}
	fseek(afile, 0, SEEK_END);
	ftsize = ftell(afile);
	fseek(afile, 0, SEEK_SET);
	while (ftsize > 0)
	{
		blocklen = ftsize > 8192 ? 8192 : (uint16_t)ftsize;
		blocklen = (uint16_t)fread(pbuff, 1, blocklen, afile);
		ftsize -= blocklen;
		EVE_Hal_wrMem(phost, addr, pbuff, blocklen);
		addr += blocklen;
	}
	fclose(afile);

	return true;
}

#ifdef _WIN32

EVE_HAL_EXPORT bool EVE_Util_loadRawFile(EVE_HalContext *phost, uint32_t address, const char *filename)
{
	return loadRawFile(phost, address, filename, NULL);
}

EVE_HAL_EXPORT bool EVE_Util_loadRawFileW(EVE_HalContext *phost, uint32_t address, const wchar_t *filename)
{
	return loadRawFile(phost, address, NULL, filename);
}

#endif

/**
 * @brief Load file into RAM_G by CMD_INFLATE
 * 
 * @param phost  Pointer to Hal context
 * @param address Address to write
 * @param filename File to load
 * @return true True if ok
 * @return false False if error
 */
#ifdef _WIN32
static bool loadInflateFile(EVE_HalContext *phost, uint32_t address, const char *filename, const wchar_t *filenameW)
#else
EVE_HAL_EXPORT bool EVE_Util_loadInflateFile(EVE_HalContext *phost, uint32_t address, const char *filename)
#endif
{
	FILE *afile;
	uint32_t ftsize = 0;
	uint8_t pbuff[8192];
	uint16_t blocklen;
	errno_t err = 0;

	if (!EVE_Cmd_waitSpace(phost, 8))
		return false; // Space for CMD_INFLATE

#ifdef _WIN32
	// afile = filename ? fopen(filename, "rb") : _wfopen(filenameW, L"rb");
	err = filename ? fopen_s(&afile, filename, "rb") : _wfopen_s(&afile, filenameW, L"rb");
#else
	err = fopen_s(&afile, filename, "rb"); // read Binary (rb)
#endif
	if (err || afile == NULL)
	{
#ifdef _WIN32
		if (!filename)
			eve_printf_debug("Unable to open: %ls\n", filenameW);
		else
#endif
			eve_printf_debug("Unable to open: %s\n", filename);
		return false;
	}
	EVE_Cmd_wr32(phost, CMD_INFLATE);
	EVE_Cmd_wr32(phost, address);
	fseek(afile, 0, SEEK_END);
	ftsize = ftell(afile);
	fseek(afile, 0, SEEK_SET);
	while (ftsize > 0)
	{
		blocklen = ftsize > 8192 ? 8192 : (uint16_t)ftsize;
		blocklen = (uint16_t)fread(pbuff, 1, blocklen, afile); /* copy the data into pbuff and then transfter it to command buffer */
		ftsize -= blocklen;
		blocklen += 3;
		blocklen &= ~3U;

		if (!EVE_Cmd_wrMem(phost, pbuff, blocklen)) /* copy data continuously into command memory */
			break;
	}

	fclose(afile); /* close the opened compressed file */

	return EVE_Cmd_waitFlush(phost);
}

#ifdef _WIN32

EVE_HAL_EXPORT bool EVE_Util_loadInflateFile(EVE_HalContext *phost, uint32_t address, const char *filename)
{
	return loadInflateFile(phost, address, filename, NULL);
}

EVE_HAL_EXPORT bool EVE_Util_loadInflateFileW(EVE_HalContext *phost, uint32_t address, const wchar_t *filename)
{
	return loadInflateFile(phost, address, NULL, filename);
}

#endif

/**
 * @brief Load image into RAM_G
 * 
 * @param phost  Pointer to Hal context
 * @param address Address in RAM_G
 * @param filename File to load
 * @param format Target format of image
 * @return true True if ok
 * @return false False if error
 */
#ifdef _WIN32
static bool loadImageFile(EVE_HalContext *phost, uint32_t address, const char *filename, const wchar_t *filenameW, uint32_t *format)
#else
EVE_HAL_EXPORT bool EVE_Util_loadImageFile(EVE_HalContext *phost, uint32_t address, const char *filename, uint32_t *format)
#endif
{
	FILE *afile;
	uint32_t ftsize = 0;
	uint8_t pbuff[8192];
	uint16_t blocklen;
	errno_t err = 0;

	if (phost->CmdFault)
		return false;

#ifdef _WIN32
	// afile = filename ? fopen(filename, "rb") : _wfopen(filenameW, L"rb");
	err = filename ? fopen_s(&afile, filename, "rb") : _wfopen_s(&afile, filenameW, L"rb");
#else
	err = fopen_s(&afile, filename, "rb"); // read Binary (rb)
#endif
	if (err || afile == NULL)
	{
#ifdef _WIN32
		if (!filename)
			eve_printf_debug("Unable to open: %ls\n", filenameW);
		else
#endif
			eve_printf_debug("Unable to open: %s\n", filename);
		return 0;
	}
	EVE_Cmd_wr32(phost, CMD_LOADIMAGE);
	EVE_Cmd_wr32(phost, address);
	EVE_Cmd_wr32(phost, OPT_NODL);
	// TODO: Let it write into the scratch display list handle,
	//  and read it out and write into the bitmapInfo the proper
	//  values to use. Replace compressed bool with uint8 enum to
	//  specify the loading mechanism
	fseek(afile, 0, SEEK_END);
	ftsize = ftell(afile);
	fseek(afile, 0, SEEK_SET);
	while (ftsize > 0)
	{
		blocklen = ftsize > 8192 ? 8192 : (uint16_t)ftsize;
		blocklen = (uint16_t)fread(pbuff, 1, blocklen, afile); /* copy the data into pbuff and then transfter it to command buffer */
		ftsize -= blocklen;
		blocklen += 3;
		blocklen &= ~3U;

		if (!EVE_Cmd_wrMem(phost, pbuff, blocklen))
			break;
	}

	fclose(afile); /* close the opened jpg file */

	if (!EVE_Cmd_waitFlush(phost))
		return false;

	if (format)
		*format = EVE_Hal_rd32(phost, 0x3097e8);

	return true;
}

#ifdef _WIN32

EVE_HAL_EXPORT bool EVE_Util_loadImageFile(EVE_HalContext *phost, uint32_t address, const char *filename, uint32_t *format)
{
	return loadImageFile(phost, address, filename, NULL, format);
}

EVE_HAL_EXPORT bool EVE_Util_loadImageFileW(EVE_HalContext *phost, uint32_t address, const wchar_t *filename, uint32_t *format)
{
	return loadImageFile(phost, address, NULL, filename, format);
}

#endif

#ifdef _WIN32
static bool loadCmdFile(EVE_HalContext *phost, const char *filename, const wchar_t *filenameW, uint32_t *transfered)
#else
EVE_HAL_EXPORT bool EVE_Util_loadCmdFile(EVE_HalContext *phost, const char *filename, uint32_t *transfered)
#endif
{
	FILE *afile;
	uint32_t ftsize = 0;
	uint8_t pbuff[8192];
	uint16_t blocklen;
	errno_t err = 0;

#ifdef _WIN32
	// afile = filename ? fopen(filename, "rb") : _wfopen(filenameW, L"rb");
	err = filename ? fopen_s(&afile, filename, "rb") : _wfopen_s(&afile, filenameW, L"rb");
#else
	err = fopen_s(&afile, filename, "rb"); // read Binary (rb)
#endif
	if (err || afile == NULL)
	{
#ifdef _WIN32
		if (!filename)
			eve_printf_debug("Unable to open: %ls\n", filenameW);
		else
#endif
			eve_printf_debug("Unable to open: %s\n", filename);
		return false;
	}
	fseek(afile, 0, SEEK_END);
	ftsize = ftell(afile);
	fseek(afile, 0, SEEK_SET);
	while (ftsize > 0)
	{
		blocklen = ftsize > 8192 ? 8192 : (uint16_t)ftsize;
		blocklen = (uint16_t)fread(pbuff, 1, blocklen, afile); /* copy the data into pbuff and then transfter it to command buffer */
		ftsize -= blocklen;
		blocklen += 3;
		blocklen &= ~3U;
		if (!EVE_Cmd_wrMem(phost, pbuff, blocklen)) /* copy data continuously into command memory */
			break;
		if (transfered)
			*transfered += blocklen;
	}

	fclose(afile); /* close the opened file */

	return EVE_Cmd_waitFlush(phost);
}

#ifdef _WIN32

EVE_HAL_EXPORT bool EVE_Util_loadCmdFile(EVE_HalContext *phost, const char *filename, uint32_t *transfered)
{
	return loadCmdFile(phost, filename, NULL, transfered);
}

EVE_HAL_EXPORT bool EVE_Util_loadCmdFileW(EVE_HalContext *phost, const wchar_t *filename, uint32_t *transfered)
{
	return loadCmdFile(phost, NULL, filename, transfered);
}

#endif

#ifdef _WIN32
static size_t readFile(EVE_HalContext *phost, uint8_t *buffer, size_t size, const char *filename, const wchar_t *filenameW)
#else
EVE_HAL_EXPORT size_t EVE_Util_readFile(EVE_HalContext *phost, uint8_t *buffer, size_t size, const char *filename)
#endif
{
	// Read up to `size` number of bytes from the file into `buffer`, then return the number of read bytes
	FILE *afile;
	size_t read;
	errno_t err = 0;

#ifdef _WIN32
	// afile = filename ? fopen(filename, "rb") : _wfopen(filenameW, L"rb");
	err = filename ? fopen_s(&afile, filename, "rb") : _wfopen_s(&afile, filenameW, L"rb");
#else
	err = fopen_s(&afile, filename, "rb"); // read Binary (rb)
#endif
	if (err || afile == NULL)
	{
#ifdef _WIN32
		if (!filename)
			eve_printf_debug("Unable to open: %ls\n", filenameW);
		else
#endif
			eve_printf_debug("Unable to open: %s\n", filename);
		return 0;
	}
	read = fread(buffer, 1, size, afile);
	fclose(afile);
	return read;
}

#ifdef _WIN32

EVE_HAL_EXPORT size_t EVE_Util_readFile(EVE_HalContext *phost, uint8_t *buffer, size_t size, const char *filename)
{
	return readFile(phost, buffer, size, filename, NULL);
}

EVE_HAL_EXPORT size_t EVE_Util_readFileW(EVE_HalContext *phost, uint8_t *buffer, size_t size, const wchar_t *filename)
{
	return readFile(phost, buffer, size, NULL, filename);
}

#endif

#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
#ifdef _WIN32
static bool loadMediaFile(EVE_HalContext *phost, const char *filename, const wchar_t *filenameW, uint32_t *transfered)
#else
EVE_HAL_EXPORT bool EVE_Util_loadMediaFile(EVE_HalContext *phost, const char *filename, uint32_t *transfered)
#endif
{
	FILE *afile;
	uint32_t remaining = 0;
	uint32_t blockSize = ((phost->MediaFifoSize >> 3) << 2) - 4;
#pragma warning(push)
#pragma warning(disable : 6255)
	uint8_t *pbuff = (uint8_t *)_alloca(blockSize);
#pragma warning(pop)
	uint16_t blocklen;
	errno_t err = 0;
	if (!transfered)
		EVE_Util_closeFile(phost);
	if (phost->CmdFault)
		return false;
	if (phost->LoadFileHandle)
	{
		afile = phost->LoadFileHandle;
	}
#ifdef _WIN32
	else if (!filename)
	{
		err = _wfopen_s(&afile, filenameW, L"rb");
	}
#endif
	else
	{
		err = fopen_s(&afile, filename, "rb");
	}
	if (err || afile == NULL)
	{
#ifdef _WIN32
		if (!filename)
			eve_printf_debug("Unable to open: %ls\n", filenameW);
		else
#endif
			eve_printf_debug("Unable to open: %s\n", filename);
		return false;
	}
	if (!phost->LoadFileHandle)
	{
		fseek(afile, 0, SEEK_END);
		remaining = ftell(afile);
		fseek(afile, transfered ? *transfered : 0, SEEK_SET);
		if (transfered)
			phost->LoadFileHandle = afile;
	}
	else
	{
		remaining = (uint32_t)phost->LoadFileRemaining;
	}
	while (remaining > 0)
	{
		blocklen = (uint16_t)min(blockSize, remaining);
		blocklen = (uint16_t)fread((void *)pbuff, 1, blocklen, afile); /* Copy the data into pbuff and then transfter it to command buffer */

		if (blocklen == 0)
		{
			eve_printf_debug("Read 0 bytes, unexpected end of file, %i bytes remaining", (int)remaining);
			break;
		}

		remaining -= blocklen;
		blocklen += 3;
		blocklen &= ~3U;

		if (transfered)
		{
			uint32_t transferedPart = 0;
			bool wrRes = EVE_MediaFifo_wrMem(phost, pbuff, blocklen, &transferedPart); /* Copy data continuously into media fifo memory */
			*transfered += transferedPart;
			if (transferedPart < blocklen)
			{
				long offset = (long)transferedPart - (long)blocklen; /* Negative */
				fseek(afile, offset, SEEK_CUR); /* Seek back */
				remaining -= offset; /* Increments remaining (double negative) */
				break; /* Early exit, processing done */
			}
			if (!wrRes)
				break;
		}
		else
		{
			if (!EVE_MediaFifo_wrMem(phost, pbuff, blocklen, NULL)) /* Copy data continuously into media fifo memory */
				break; /* Coprocessor fault */
		}
	}
	if (!transfered)
	{
		fclose(afile); /* Close the opened file */
	}
	else if (remaining)
	{
		phost->LoadFileRemaining = remaining; /* Save remaining */
	}
	else
	{
		EVE_Util_closeFile(phost);
	}
	return transfered ? EVE_Cmd_waitFlush(phost) : EVE_MediaFifo_waitFlush(phost, false);
}

void EVE_Util_closeFile(EVE_HalContext *phost)
{
	if (phost->LoadFileHandle)
	{
		fclose(phost->LoadFileHandle);
		phost->LoadFileHandle = NULL;
		phost->LoadFileRemaining = 0;
	}
}

#ifdef _WIN32

EVE_HAL_EXPORT bool EVE_Util_loadMediaFile(EVE_HalContext *phost, const char *filename, uint32_t *transfered)
{
	return loadMediaFile(phost, filename, NULL, transfered);
}

EVE_HAL_EXPORT bool EVE_Util_loadMediaFileW(EVE_HalContext *phost, const wchar_t *filename, uint32_t *transfered)
{
	return loadMediaFile(phost, NULL, filename, transfered);
}

#endif
#endif

#endif

/* end of file */
