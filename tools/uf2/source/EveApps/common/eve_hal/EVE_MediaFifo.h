/**
 * @file EVE_MediaFifo.h
 * @brief EVE's mediafifo controller
 *
 * This file defines the generic APIs of phost access layer for the FT800 or EVE compatible silicon.
 * Application shall access FT800 or EVE resources over these APIs,regardless of I2C or SPI protocol.
 * In addition, there are some helper functions defined for FT800 coprocessor engine as well as phost commands.
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

#ifndef EVE_MEDIAFIFO__H
#define EVE_MEDIAFIFO__H
#include "EVE_HalDefs.h"

/**************************
** COPROCESSOR MEDIAFIFO **
***************************/

#ifdef EVE_SUPPORT_MEDIAFIFO

/* Set the media FIFO. 
Returns false in case a coprocessor fault occurred */
EVE_HAL_EXPORT bool EVE_MediaFifo_set(EVE_HalContext *phost, uint32_t address, uint32_t size);

/* Closes the current media FIFO.
Indication for HAL only */
EVE_HAL_EXPORT void EVE_MediaFifo_close(EVE_HalContext *phost);

/* Get the current read pointer. */
EVE_HAL_EXPORT uint32_t EVE_MediaFifo_rp(EVE_HalContext *phost);

/* Get the current write pointer. */
EVE_HAL_EXPORT uint32_t EVE_MediaFifo_wp(EVE_HalContext *phost);

/* Get the currently available space. */
EVE_HAL_EXPORT uint32_t EVE_MediaFifo_space(EVE_HalContext *phost);

/* Write a buffer to the media FIFO. 
Waits if there is not enough space in the media FIFO. 
Returns false in case a coprocessor fault occurred.
If the transfered pointer is set, the write may exit early 
if the coprocessor function has finished, and the
transfered amount will be set. */
EVE_HAL_EXPORT bool EVE_MediaFifo_wrMem(EVE_HalContext *phost, const uint8_t *buffer, uint32_t size, uint32_t *transfered);

/* Wait for the media FIFO to fully empty. 
When checking if a file is fully processed, EVE_Cmd_waitFlush must be called.
Returns false in case a coprocessor fault occurred, or in case the coprocessor is done processing */
EVE_HAL_EXPORT bool EVE_MediaFifo_waitFlush(EVE_HalContext *phost, bool orCmdFlush);

/* Wait for the media FIFO to have at least the requested amount of free space.
Returns 0 in case a coprocessor fault occurred, or in case the coprocessor is done processing */
EVE_HAL_EXPORT uint32_t EVE_MediaFifo_waitSpace(EVE_HalContext *phost, uint32_t size, bool orCmdFlush);

#else

#define EVE_MediaFifo_set(phost, address, size) (false)
#define EVE_MediaFifo_rp(phost) (0)
#define EVE_MediaFifo_wp(phost) (0)
#define EVE_MediaFifo_space(phost) (~0)
#define EVE_MediaFifo_wrMem(phost, buffer, size, transfered) (false)
#define EVE_MediaFifo_waitFlush(phost) (false)
#define EVE_MediaFifo_waitSpace(phost, size) (false)

#endif

#endif /* #ifndef EVE_HAL_INCL__H */

/* end of file */