/**
 * @file EVE_HalImpl.h
 * @brief Eve_Hal framework APIs
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

#ifndef EVE_HAL_IMPL__H
#define EVE_HAL_IMPL__H
#include "EVE_HalDefs.h"

extern EVE_HalPlatform g_HalPlatform;

/*********
** INIT **
*********/

/* Initialize HAL platform */
void EVE_HalImpl_initialize();

/* Release HAL platform */
void EVE_HalImpl_release();

/* Get the default configuration parameters.
Use `deviceIdx` to choose the connected device. */
bool EVE_HalImpl_defaults(EVE_HalParameters *parameters, size_t deviceIdx);

/* Opens a new HAL context using the specified parameters */
bool EVE_HalImpl_open(EVE_HalContext *phost, const EVE_HalParameters *parameters);

/* Close a HAL context */
void EVE_HalImpl_close(EVE_HalContext *phost);

/* Idle. Call regularly to update frequently changing internal state */
void EVE_HalImpl_idle(EVE_HalContext *phost);

/*************
** TRANSFER **
*************/

EVE_HAL_EXPORT void EVE_Hal_startTransfer(EVE_HalContext *phost, EVE_TRANSFER_T rw, uint32_t addr);
EVE_HAL_EXPORT void EVE_Hal_endTransfer(EVE_HalContext *phost);

EVE_HAL_EXPORT uint8_t EVE_Hal_transfer8(EVE_HalContext *phost, uint8_t value);
EVE_HAL_EXPORT uint16_t EVE_Hal_transfer16(EVE_HalContext *phost, uint16_t value);
EVE_HAL_EXPORT uint32_t EVE_Hal_transfer32(EVE_HalContext *phost, uint32_t value);

EVE_HAL_EXPORT void EVE_Hal_flush(EVE_HalContext *phost);

/*********
** MISC **
*********/

void EVE_Mcu_initialize();
void EVE_Mcu_release();

void EVE_Millis_initialize();
void EVE_Millis_release();
EVE_HAL_EXPORT uint32_t EVE_millis();
EVE_HAL_EXPORT uint64_t EVE_millis64();

bool EVE_UtilImpl_bootupDisplayGpio(EVE_HalContext *phost);

#endif /* #ifndef EVE_HAL_IMPL__H */

/* end of file */
