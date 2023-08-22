/**
 * @file EVE_MediaFifo.c
 * @brief EVE's mediafifo controller
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

#include "EVE_MediaFifo.h"
#include "EVE_Platform.h"

#ifdef EVE_SUPPORT_MEDIAFIFO

/* Set the media FIFO. 
Returns false in case a coprocessor fault occurred */
bool EVE_MediaFifo_set(EVE_HalContext *phost, uint32_t address, uint32_t size)
{
	bool res;

	if (!EVE_Hal_supportMediaFifo(phost))
	{
		eve_assert_ex(false, "EVE_MediaFifo_set is not available on the current graphics platform\n");
		return false;
	}

	if (phost->MediaFifoAddress != address || phost->MediaFifoSize != size)
	{
		EVE_Cmd_startFunc(phost);
		EVE_Cmd_wr32(phost, CMD_MEDIAFIFO);
		EVE_Cmd_wr32(phost, address);
		EVE_Cmd_wr32(phost, size);
		EVE_Cmd_endFunc(phost);

		/* Must flush for fifo pointers to be ready. */
		res = EVE_Cmd_waitFlush(phost);
	}
	else
	{
		res = true;
	}

	if (res)
	{
		eve_assert(EVE_Hal_rd32(phost, REG_MEDIAFIFO_READ) < size);
		eve_assert(EVE_Hal_rd32(phost, REG_MEDIAFIFO_WRITE) < size);
		phost->MediaFifoAddress = address;
		phost->MediaFifoSize = size;
	}
	else
	{
		phost->MediaFifoAddress = 0;
		phost->MediaFifoSize = 0;
	}

	return res;
}

void EVE_MediaFifo_close(EVE_HalContext *phost)
{
	phost->MediaFifoAddress = 0;
	phost->MediaFifoSize = 0;
}

/* Get the current read pointer. */
uint32_t EVE_MediaFifo_rp(EVE_HalContext *phost)
{
	return EVE_Hal_rd32(phost, REG_MEDIAFIFO_READ);
}

/* Get the current write pointer. */
uint32_t EVE_MediaFifo_wp(EVE_HalContext *phost)
{
	return EVE_Hal_rd32(phost, REG_MEDIAFIFO_WRITE);
}

/* Get the currently available space. */
uint32_t EVE_MediaFifo_space(EVE_HalContext *phost)
{
	if (!phost->MediaFifoSize)
		return 0;

	int32_t rp = EVE_Hal_rd32(phost, REG_MEDIAFIFO_READ);
	int32_t wp = EVE_Hal_rd32(phost, REG_MEDIAFIFO_WRITE);
#if 1
	return rp > wp
	    ? (rp - wp - 4)
	    : (rp + phost->MediaFifoSize - wp - 4);
#else
	int32_t diff = wp - rp;
	if (diff < 0)
		diff += phost->MediaFifoSize;
	eve_assert(diff >= 0 && diff < phost->MediaFifoSize);
	return phost->MediaFifoSize - diff - 4;
#endif
}

/* Write a buffer to the media FIFO. 
Waits if there is not enough space in the media FIFO. 
Returns false in case a coprocessor fault occurred */
bool EVE_MediaFifo_wrMem(EVE_HalContext *phost, const uint8_t *buffer, uint32_t size, uint32_t *transfered)
{
	if (!EVE_Hal_supportMediaFifo(phost))
	{
		eve_assert_ex(false, "EVE_MediaFifo_wrMem is not available on the current graphics platform\n");
		return false;
	}

	if (!phost->MediaFifoSize)
	{
		eve_printf_debug("EVE Media FIFO has not been set, cannot write\n");
		return false;
	}

#if 1
	/* Two strategies. 
	- Wait for entire space and write the entire buffer.
	- Wait for half the fifo to be available, and write in parts. */

	/* If the size is within the media fifo size, write the entire buffer at once */
	if (size <= phost->MediaFifoSize - 4)
	{
		uint32_t wp;
		int32_t overflow;

		if (!EVE_MediaFifo_waitSpace(phost, size, transfered))
			return false;

		wp = EVE_Hal_rd32(phost, REG_MEDIAFIFO_WRITE);
		overflow = (int32_t)(wp + size) - (int32_t)(phost->MediaFifoSize);
		if (overflow > 0)
		{
			eve_assert(phost->MediaFifoAddress + wp + size - overflow <= RAM_G_SIZE);
			eve_assert(phost->MediaFifoAddress + overflow <= RAM_G_SIZE);
			EVE_Hal_wrMem(phost, phost->MediaFifoAddress + wp, buffer, size - overflow);
			EVE_Hal_wrMem(phost, phost->MediaFifoAddress, &buffer[size - overflow], overflow);
		}
		else
		{
			eve_assert(phost->MediaFifoAddress + wp + size <= RAM_G_SIZE);
			EVE_Hal_wrMem(phost, phost->MediaFifoAddress + wp, buffer, size);
		}
		wp += size;
		if (wp >= phost->MediaFifoSize)
			wp -= phost->MediaFifoSize;
		eve_assert(wp < phost->MediaFifoSize);
		EVE_Hal_wr32(phost, REG_MEDIAFIFO_WRITE, wp);
		if (transfered)
			*transfered = size;
		return true;
	}
	else
	{
		/* Otherwise, write in parts. */
		uint32_t halfSize = ((phost->MediaFifoSize >> 3) << 2) - 4;
		uint32_t remaining = size;
		uint32_t done = 0;
		uint32_t wp;

		wp = EVE_Hal_rd32(phost, REG_MEDIAFIFO_WRITE);
		while (remaining)
		{
			uint32_t transfer = min(halfSize, remaining);
			int32_t overflow;
			if (!EVE_MediaFifo_waitSpace(phost, transfer, transfered))
				return false;

			overflow = (int32_t)(wp + transfer) - (int32_t)(phost->MediaFifoSize);
			if (overflow > 0)
			{
				eve_assert(phost->MediaFifoAddress + wp + transfer - overflow <= RAM_G_SIZE);
				eve_assert(phost->MediaFifoAddress + overflow <= RAM_G_SIZE);
				EVE_Hal_wrMem(phost, phost->MediaFifoAddress + wp, &buffer[done], transfer - overflow);
				EVE_Hal_wrMem(phost, phost->MediaFifoAddress, &buffer[done + transfer - overflow], overflow);
			}
			else
			{
				eve_assert(phost->MediaFifoAddress + wp + transfer <= RAM_G_SIZE);
				EVE_Hal_wrMem(phost, phost->MediaFifoAddress + wp, &buffer[done], transfer);
			}
			wp += transfer;
			done += transfer;
			remaining -= transfer;
			if (wp >= phost->MediaFifoSize)
				wp -= phost->MediaFifoSize;
			eve_assert(wp < phost->MediaFifoSize);
			EVE_Hal_wr32(phost, REG_MEDIAFIFO_WRITE, wp);

			if (transfered)
			{
				*transfered = done;
				if (EVE_Cmd_rp(phost) == EVE_Cmd_wp(phost))
					return true; /* Early exit, finished processing. */
			}
		}

		return true;
	}
#else
	eve_scope()
	{
		uint32_t halfSize = ((phost->MediaFifoSize >> 3) << 2) - 4;
		uint32_t remaining = size;
		uint32_t done = 0;
		uint32_t wp;

		/* Write to media FIFO as soon as space is available */
		wp = EVE_Hal_rd32(phost, REG_MEDIAFIFO_WRITE);
		while (remaining)
		{
			uint32_t transfer = min(
			    EVE_MediaFifo_waitSpace(phost, 4),
			    min(halfSize, remaining));
			if (!transfer)
				return false; // !phost->CmdFault;
			int32_t overflow = (int32_t)(wp + transfer) - (int32_t)(phost->MediaFifoSize);
			if (overflow > 0)
			{
				EVE_Hal_wrMem(phost, phost->MediaFifoAddress + wp, &buffer[done], transfer - overflow);
				EVE_Hal_wrMem(phost, phost->MediaFifoAddress, &buffer[done + transfer - overflow], overflow);
			}
			else
			{
				EVE_Hal_wrMem(phost, phost->MediaFifoAddress + wp, &buffer[done], transfer);
			}
			wp += transfer;
			done += transfer;
			remaining -= transfer;
			if (wp >= phost->MediaFifoSize)
				wp -= phost->MediaFifoSize;
			eve_assert(wp < phost->MediaFifoSize);
			EVE_Hal_wr32(phost, REG_MEDIAFIFO_WRITE, wp);

			if (transfered)
			{
				*transfered = done;
				if (EVE_Cmd_rp(phost) == EVE_Cmd_wp(phost))
					return true; /* Early exit, finished processing. */
			}
		}

		return true;
	}
#endif
}

#if defined(_DEBUG)
void debugBackupRamG(EVE_HalContext *phost);
#endif

static bool checkWait(EVE_HalContext *phost, uint32_t rpOrSpace)
{
	if (EVE_CMD_FAULT(rpOrSpace))
	{
		char err[128];
		(void)err;
		/* Coprocessor fault */
		phost->CmdWaiting = false;
		eve_printf_debug("Coprocessor fault in processing media FIFO\n");
#if defined(_DEBUG)
		debugBackupRamG(phost);
		if (EVE_CHIPID >= EVE_BT815)
		{
			EVE_Hal_rdMem(phost, (uint8_t *)err, RAM_ERR_REPORT, 128);
			eve_printf_debug("%s\n", err);
			EVE_Hal_displayMessage(phost, err, 128);
		}
		else
		{
			EVE_Hal_displayMessage(phost, "Coprocessor fault in processing media FIFO ", sizeof("Coprocessor fault in processing media FIFO "));
		}
#endif
		/* eve_debug_break(); */
		return false;
	}

	if (phost->Status == EVE_STATUS_ERROR)
	{
		phost->CmdWaiting = false;
		eve_printf_debug("Host error\n");
		return false;
	}

	return true;
}

static bool handleWait(EVE_HalContext *phost, uint16_t rpOrSpace)
{
	/* Check for coprocessor fault */
	if (!checkWait(phost, rpOrSpace))
		return false;

	/* Process any idling */
	EVE_Hal_idle(phost);

	/* Process user idling */
	if (phost->CbCmdWait
	    && !phost->CbCmdWait(phost))
	{
		/* Wait aborted */
		phost->CmdWaiting = false;
		eve_printf_debug("Wait for media FIFO aborted\n");
		return false;
	}
	return true;
}

/* Wait for the media FIFO to fully empty.
Returns false in case a coprocessor fault occurred */
bool EVE_MediaFifo_waitFlush(EVE_HalContext *phost, bool orCmdFlush)
{
	return EVE_MediaFifo_waitSpace(phost, phost->MediaFifoSize - 4, orCmdFlush);
}

/* Wait for the media FIFO to have at least the requested amount of free space.
Returns 0 in case a coprocessor fault occurred */
uint32_t EVE_MediaFifo_waitSpace(EVE_HalContext *phost, uint32_t size, bool orCmdFlush)
{
	uint32_t space;

	if (!EVE_Hal_supportMediaFifo(phost))
	{
		eve_assert_ex(false, "EVE_MediaFifo_waitSpace is not available on the current graphics platform\n");
		return 0;
	}

	if (!phost->MediaFifoSize)
	{
		eve_printf_debug("EVE Media FIFO has not been set, cannot wait for space\n");
		return 0;
	}

	if (size > (phost->MediaFifoSize - 4))
	{
		eve_printf_debug("Requested free space exceeds media FIFO\n");
		return 0;
	}

	eve_assert(!phost->CmdWaiting);
	phost->CmdWaiting = true;

#if 1
	space = EVE_MediaFifo_space(phost);
	if (!checkWait(phost, space))
		return 0;
#endif

	do
	{
		space = EVE_MediaFifo_space(phost);
		if (!handleWait(phost, (uint16_t)space))
			return 0;
		phost->CmdWaiting = false;
		uint32_t cmdSpace = EVE_Cmd_waitSpace(phost, 0);
		if (!cmdSpace)
			return 0; /* Check for coprocessor error */
		if (orCmdFlush && cmdSpace == (EVE_CMD_FIFO_SIZE - 4))
			return 0; /* Processed */
		phost->CmdWaiting = true;
	} while (space < size);

	phost->CmdWaiting = false;
	return space;
}

#endif

/* end of file */
