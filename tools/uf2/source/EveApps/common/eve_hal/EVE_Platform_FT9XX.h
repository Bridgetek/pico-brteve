/**
 * @file EVE_Platform_FT9XX.h
 * @brief EVE platform for FT9XX
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

#ifndef EVE_PLATFORM_FT900__H
#define EVE_PLATFORM_FT900__H

#include "EVE_Config.h"
#if defined(FT9XX_PLATFORM)

#if defined(FT900_PLATFORM)

#define GPIO_UART0_TX (48)
#define GPIO_UART0_RX (49)

#define GPIO_SPIM_CLK (27)
#define GPIO_SPIM_SS0 (28)
#define GPIO_SPIM_MOSI (29)
#define GPIO_SPIM_MISO (30)

#define GPIO_SPIM_IO2 (31)
#define GPIO_SPIM_IO3 (32)

#define GPIO_SPIM_SS1 (33)
#define GPIO_SPIM_SS2 (34)

#define GPIO_ILI9488_CS1 (33)
#define GPIO_ILI9488_DCX (34)

#define GPIO_FT800_INT (42)
#define GPIO_FT800_PWD (43)

#define pad_ili9488_dcx (pad_func_0)
#define pad_ili9488_cs1 (pad_func_0)

#define GPIO_SD_CLK (19)
#define GPIO_SD_CMD (20)
#define GPIO_SD_DAT3 (21)
#define GPIO_SD_DAT2 (22)
#define GPIO_SD_DAT1 (23)
#define GPIO_SD_DAT0 (24)
#define GPIO_SD_CD (25)
#define GPIO_SD_WP (26)

#elif defined(FT93X_PLATFORM)

#define GPIO_UART0_TX (23)
#define GPIO_UART0_RX (22)

#define GPIO_SPIM_CLK (34)
#define GPIO30_SPIM_SS0 (30)
#define GPIO_SPIM_MOSI (36)
#define GPIO_SPIM_MISO (35)

#define GPIO_SPIM_IO2 (37)
#define GPIO_SPIM_IO3 (38)

#define GPIO_SPIM_SS1 (31)
#define GPIO_SPIM_SS2 (32)
#define GPIO_SPIM_SS3 (33)

#define GPIO_ILI9488_CS1 (31)
#define GPIO_ILI9488_DCX (8)

#define GPIO_FT800_INT (14)
#define GPIO_FT800_PWD (15)

#define pad_ili9488_dcx (pad_func_0)
#define pad_ili9488_cs1 (pad_func_0)

#define GPIO_SD_CLK (0)
#define GPIO_SD_CMD (1)
#define GPIO_SD_DAT3 (6)
#define GPIO_SD_DAT2 (5)
#define GPIO_SD_DAT1 (4)
#define GPIO_SD_DAT0 (3)
#define GPIO_SD_CD (2)
#define GPIO_SD_WP (7)

#endif

/* Timer 1 is been utilized in case of FT900 platform */
#define FT900_FT_MILLIS_TIMER (timer_select_b)
#define FT900_TIMER_MAX_VALUE (65536L)
#define FT900_TIMER_PRESCALE_VALUE (100)
#define FT900_TIMER_OVERFLOW_VALUE (1000)

/* Hardware or Module specific macros for gpio line numbers  */
#if defined(CLEO35)

#undef GPIO_FT800_PWD
#define GPIO_FT800_PWD (66) //for cleo35 module

#elif defined(CLEO50)

#undef GPIO_FT800_PWD
#define GPIO_FT800_PWD (0) //for cleo50 module

#elif defined(FT900DEMOBOARD)

#undef GPIO_FT800_PWD
#define GPIO_FT800_PWD (32)

#elif defined(PANL35)

#undef GPIO_FT800_INT
#undef GPIO_FT800_PWD
#define GPIO_FT800_INT (0)
#define GPIO_FT800_PWD (1)

#elif defined(PANL70)

#undef GPIO_FT800_INT
#undef GPIO_FT800_PWD
#define GPIO_FT800_INT (0)
#define GPIO_FT800_PWD (1)
#define GPIO_GOODIX (33)

#elif defined(PANL70PLUS)

#undef GPIO_FT800_INT
#undef GPIO_FT800_PWD
#define GPIO_FT800_INT (0)
#define GPIO_FT800_PWD (1)
#define GPIO_GOODIX (33)
#define pad_gpio_goodix (pad_func_0)

#endif

#ifndef RTC_PRESENT
#ifdef FT900_PLATFORM_RTC_I2C
#define RTC_PRESENT (1)
#endif
#endif /* RTC_PRESENT */

/* Standard C libraries */
#define ESD_USING_NEW_LIB // Using NewLib C Library for runtime

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

/* FT900 libraries */
#include "ft900_uart_simple.h"
#include "ft900_sdhost.h"
#include "ft900_spi.h"
#include "ft900_gpio.h"
#include "ft900_rtc.h"
#include "ft900_interrupt.h"
#include "ft900_i2cm.h"
#include "ft900.h"
#include "ft900_delay.h"

#define eve_printf(fmt, ...) iprintf(fmt, ##__VA_ARGS__)
#define eve_sprintf(str, fmt, ...) siprintf(str, fmt, ##__VA_ARGS__)

#endif
#endif /* #ifndef EVE_PLATFORM_FT900__H */

/* end of file */
