/**
 * @file EVE_Platform_WIN32.h
 * @brief EVE platform for WIN32
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

#ifndef EVE_PLATFORM_WIN32__H
#define EVE_PLATFORM_WIN32__H

#include "EVE_Config.h"
#if defined(_WIN32)

/* Disable legacy C functions under TCC */
#ifndef NO_OLDNAMES
#define NO_OLDNAMES
#endif

/* Sane options for Windows header */
#ifndef NOMINMAX
#define EVE_NOMINMAX
#define NOMINMAX
#endif
#if !defined(NTDDI_VERSION) && !defined(_WIN32_WINNT) && !defined(WINVER)
#define NTDDI_VERSION 0x06000000 /* NTDDI_VISTA */
#define _WIN32_WINNT 0x0600 /* _WIN32_WINNT_VISTA */
#define WINVER 0x0600 /* _WIN32_WINNT_VISTA */
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS
#define MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS 0 // FIX C5105
#endif

/* C library inclusions */
#ifdef POINTS
#pragma message(__FILE__ "(" EVE_CONFIG_STR(__LINE__) "): error WINPOINTS: Invalid include order, " \
                                                      "this header must be included before EVE_GpuDefs.h")
#endif
#define POINTS WINPOINTS
#include <Windows.h>
#undef POINTS

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <string.h>

#define _USE_MATH_DEFINES 1
#include <math.h>

#include <direct.h>
#include <time.h>
#include <io.h>

#ifdef EVE_NOMINMAX
#undef EVE_NOMINMAX
#undef NOMINMAX
#endif

#endif
#endif /* #ifndef EVE_PLATFORM_WIN32__H */

/* end of file */