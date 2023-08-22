/**
 * @file EVE_Platform_RP2040.h
 * @brief Eve_Hal framework APIs for RP2040 host platform
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

#ifndef EVE_PLATFORM_RP2040__H
#define EVE_PLATFORM_RP2040__H

#include "EVE_Config.h"
#if defined(RP2040_PLATFORM)

/* RP2040 platform libraries */
#include "pico/stdlib.h"
#include "hardware/spi.h"
/* #include "hardware/dma.h" */
/* #include "hardware/pio.h" */

/* Standard C libraries */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

/* RP2040 default platform definitions. Configuration can be changed programatically in initialization parameters */
/* Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments */
#define EVE_DEFAULT_SPI0_MISO 4 /* GP0, GP4, GP16 */
#define EVE_DEFAULT_SPI0_CS 5 /* GP1, GP5, GP17 */
#define EVE_DEFAULT_SPI0_SCK 2 /* GP2, GP6, GP18 */
#define EVE_DEFAULT_SPI0_MOSI 3 /* GP3, GP7, GP19 */
#define EVE_DEFAULT_SPI0_INT 6
#define EVE_DEFAULT_SPI0_PWD 7
#define EVE_DEFAULT_SPI0_IO2 14
#define EVE_DEFAULT_SPI0_IO3 15

#define EVE_DEFAULT_SPI1_MISO 12 /* GP8, GP12 */
#define EVE_DEFAULT_SPI1_CS1 13 /* GP9, GP13 */
#define EVE_DEFAULT_SPI1_CS2 22
#define EVE_DEFAULT_SPI1_SCK 10 /* GP10, GP14 */
#define EVE_DEFAULT_SPI1_MOSI 11 /* GP11, GP15 */
#define EVE_DEFAULT_SPI1_INT 16
#define EVE_DEFAULT_SPI1_PWD 17

/* Only use SPI0 by default, otherwise EVE_Hal_open will alternate between SPI0 and SPI1 for multiple devices */
#define EVE_DEFAULT_SPI0_ONLY 1

/*
By default, `stdin` and `stdout` go through UART0, using GP0 and GP1.
Using USB CDC (USB serial) is possible too, specified in the CMake options, see Getting Started guide.
*/

#ifdef CFG_TUSB_DEBUG_PRINTF
#define eve_printf CFG_TUSB_DEBUG_PRINTF
#endif

#endif
#endif /* #ifndef EVE_PLATFORM_RP2040__H */

/* end of file */
