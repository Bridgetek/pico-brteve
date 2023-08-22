/**
 * @file EVE_Cmd.h
 * @brief EVE's command read/write
 
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

#ifndef EVE_CMD__H
#define EVE_CMD__H
#include "EVE_HalDefs.h"

/********************
** COPROCESSOR CMD **
********************/

#define EVE_CMD_STRING_MAX 511

/* Get the current read pointer.
Safe to use during ongoing command transaction */
EVE_HAL_EXPORT uint16_t EVE_Cmd_rp(EVE_HalContext *phost);

/* Get the current write pointer.
Updates cached write pointer when CMDB is not supported.
Safe to use during ongoing command transaction */
EVE_HAL_EXPORT uint16_t EVE_Cmd_wp(EVE_HalContext *phost);

/* Get the currently available space.
Updates cached available space.
Safe to use during ongoing command transaction */
EVE_HAL_EXPORT uint16_t EVE_Cmd_space(EVE_HalContext *phost);

/* Begin writing a function, keeps the transfer open.
While a command transaction is ongoing,
HAL functions outside of EVE_Cmd_* must not be used. */
EVE_HAL_EXPORT void EVE_Cmd_startFunc(EVE_HalContext *phost);

/* End writing a function, closes the transfer */
EVE_HAL_EXPORT void EVE_Cmd_endFunc(EVE_HalContext *phost);

/* Write a buffer to the command buffer. 
Waits if there is not enough space in the command buffer. 
Returns false in case a coprocessor fault occurred */
EVE_HAL_EXPORT bool EVE_Cmd_wrMem(EVE_HalContext *phost, const uint8_t *buffer, uint32_t size);

/* Write a progmem buffer to the command buffer. 
Waits if there is not enough space in the command buffer. 
Returns false in case a coprocessor fault occurred */
EVE_HAL_EXPORT bool EVE_Cmd_wrProgMem(EVE_HalContext *phost, eve_progmem_const uint8_t *buffer, uint32_t size);

/* Write a string to the command buffer, padded to 4 bytes. 
Waits if there is not enough space in the command buffer. 
Parameter `maxLength` can be set up to `EVE_CMD_STRING_MAX`.
Returns false in case a coprocessor fault occurred */
EVE_HAL_EXPORT uint32_t EVE_Cmd_wrString(EVE_HalContext *phost, const char *str, uint32_t maxLength);

/* Write a 8-bit value to the command buffer. 
Uses a cache to write 4 bytes at once. 
Waits if there is not enough space in the command buffer. 
Returns false in case a coprocessor fault occurred */
EVE_HAL_EXPORT bool EVE_Cmd_wr8(EVE_HalContext *phost, uint8_t value);

/* Write a 16-bit value to the command buffer. 
Uses a cache to write 4 bytes at once. 
Wire endianness is handled by the transfer. 
Waits if there is not enough space in the command buffer. 
Returns false in case a coprocessor fault occurred */
EVE_HAL_EXPORT bool EVE_Cmd_wr16(EVE_HalContext *phost, uint16_t value);

/* Write a value to the command buffer. 
Wire endianness is handled by the transfer. 
Waits if there is not enough space in the command buffer. 
Returns false in case a coprocessor fault occurred */
EVE_HAL_EXPORT bool EVE_Cmd_wr32(EVE_HalContext *phost, uint32_t value);

/* Move the write pointer forward by the specified number of bytes. 
Returns the previous write pointer */
EVE_HAL_EXPORT uint16_t EVE_Cmd_moveWp(EVE_HalContext *phost, uint16_t bytes);

/* Wait for the command buffer to fully empty. 
Returns false in case a coprocessor fault occurred */
EVE_HAL_EXPORT bool EVE_Cmd_waitFlush(EVE_HalContext *phost);

/* Wait for the command buffer to have at least the requested amount of free space.
Returns 0 in case a coprocessor fault occurred */
EVE_HAL_EXPORT uint32_t EVE_Cmd_waitSpace(EVE_HalContext *phost, uint32_t size);

/* Wait for logo to finish displaying. 
(Waits for both the read and write pointer to go to 0) */
EVE_HAL_EXPORT bool EVE_Cmd_waitLogo(EVE_HalContext *phost);

/* Wait for a 32-bit value that was set by `EVE_CoCmd_memWrite32(phost, ptr, value)`.
Returns true when the value is found. Returns false otherwise
when the coprocessor has flushed, or a coprocessor fault occured. */
EVE_HAL_EXPORT bool EVE_Cmd_waitRead32(EVE_HalContext *phost, uint32_t ptr, uint32_t value);

/* Restore the internal state of EVE_Cmd.
Call this after manually writing the the coprocessor buffer */
EVE_HAL_EXPORT void EVE_Cmd_restore(EVE_HalContext *phost);

#endif /* #ifndef EVE_HAL_INCL__H */

/* end of file */