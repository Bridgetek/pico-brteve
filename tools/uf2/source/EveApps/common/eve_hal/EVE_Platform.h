/**
 * @file EVE_Platform.h
 * @brief Platform selector
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

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EVE_PLATFORM__H
#define EVE_PLATFORM__H

/* Include all public headers. */
#include "EVE_Config.h"
#if defined(_WIN32)
#include "EVE_Platform_WIN32.h"
#endif
#if defined(BT8XXEMU_PLATFORM)
#include "EVE_Platform_BT8XXEMU.h"
#endif
#if defined(FT4222_PLATFORM)
#include "EVE_Platform_FT4222.h"
#endif
#if defined(MPSSE_PLATFORM)
#include "EVE_Platform_MPSSE.h"
#endif
#if defined(FT9XX_PLATFORM)
#include "EVE_Platform_FT9XX.h"
#endif
#if defined(RP2040_PLATFORM)
#include "EVE_Platform_RP2040.h"
#endif
#include "EVE_GpuTypes.h"
#include "EVE_HalDefs.h"
#include "EVE_Cmd.h"
#include "EVE_MediaFifo.h"
#include "EVE_CoCmd.h"
#include "EVE_CoDl.h"
#include "EVE_GpuDefs.h"
#if defined(ENABLE_ILI9488_HVGA_PORTRAIT) || defined(ENABLE_KD2401_HVGA_PORTRAIT)
#include "EVE_ILI9488.h"
#endif
#include "EVE_Util.h"
#include "EVE_LoadFile.h"

#define eve_noop() \
	do             \
	{              \
	} while (false)

#ifdef EVE_DEBUG_BREAK_SIGABRT
/* Used by LittleFS tests */
#include <signal.h>
#define eve_debug_break() \
	do                    \
	{                     \
		fflush(NULL);     \
		raise(SIGABRT);   \
	} while (false)
#else
#if defined(_DEBUG)
#if defined(_MSC_VER)
#define eve_debug_break() __debugbreak()
#elif defined(__GCC__)
#define eve_debug_break() __builtin_trap()
#else
#define eve_debug_break() eve_noop()
#endif
#else
#define eve_debug_break() eve_noop()
#endif
#endif

#if defined(__GNUC__) || defined(__clang__)
#define eve_deprecated(msg) __attribute__((deprecated(msg)))
#elif defined(_MSC_VER)
#define eve_deprecated(msg) __declspec(deprecated(msg))
#elif (__cplusplus >= 201402L)
#define eve_deprecated(msg) [[deprecated(msg)]]
#else
#define eve_deprecated(msg)
#endif

#ifndef eve_printf
#define eve_printf(fmt, ...) printf(fmt, ##__VA_ARGS__)
#endif

#ifndef eve_sprintf
#define eve_sprintf(str, fmt, ...) sprintf(str, fmt, ##__VA_ARGS__)
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#define eve_assume(cond) __assume(cond)
#else
#define eve_assume(cond) eve_noop()
#endif

#if defined(_DEBUG)
#define eve_printf_debug_once(fmt, ...)                 \
	do                                                  \
	{                                                   \
		static bool eve_printf_debug_once_flag = false; \
		if (!eve_printf_debug_once_flag)                \
		{                                               \
			eve_printf(fmt, ##__VA_ARGS__);             \
			eve_printf_debug_once_flag = true;          \
		}                                               \
	} while (false)
#define eve_printf_debug(fmt, ...) eve_printf(fmt, ##__VA_ARGS__)
#define eve_assert(cond)                                                                                                           \
	do                                                                                                                             \
	{                                                                                                                              \
		if (!(cond))                                                                                                               \
		{                                                                                                                          \
			const char *str = #cond;                                                                                               \
			const char *sf = __FILE__;                                                                                             \
			eve_printf("EVE Assert Failed: %s (in file '%s' on line '%i')\n", str ? str : "<NULL>", sf ? sf : "<NULL>", __LINE__); \
			eve_debug_break();                                                                                                     \
		}                                                                                                                          \
		eve_assume(cond);                                                                                                          \
	} while (false)
#define eve_assert_ex(cond, ex)                                                                                                                             \
	do                                                                                                                                                      \
	{                                                                                                                                                       \
		if (!(cond))                                                                                                                                        \
		{                                                                                                                                                   \
			const char *str = #cond;                                                                                                                        \
			const char *sf = __FILE__;                                                                                                                      \
			eve_printf("EVE Assert Failed: %s (%s) (in file '%s' on line '%i')\n", ex ? ex : "<NULL>", str ? str : "<NULL>", sf ? sf : "<NULL>", __LINE__); \
			eve_debug_break();                                                                                                                              \
		}                                                                                                                                                   \
		eve_assume(cond);                                                                                                                                   \
	} while (false)
#define eve_assert_do(cond) eve_assert(cond)
#define eve_trace(str)                                                                                                     \
	do                                                                                                                     \
	{                                                                                                                      \
		const char *sc = __FILE__;                                                                                         \
		eve_printf("EVE Trace: %s (in file '%s' on line '%i')\n", (str) ? (str) : "<NULL>", sf ? sf : "<NULL>", __LINE__); \
	} while (false)
#else
#define eve_printf_debug_once(fmt, ...) eve_noop()
#define eve_printf_debug(fmt, ...) eve_noop()
#define eve_assert(cond) eve_assume(cond)
#define eve_assert_ex(cond, ex) eve_assume(cond)
#define eve_assert_do(cond)      \
	do                           \
	{                            \
		bool r__assert = (cond); \
		r__assert = r__assert;   \
		eve_assume(r__assert);   \
	} while (false)
#define eve_trace(cond) eve_noop()
#endif

#ifndef NOMINMAX
#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif
#endif

/* Custom scope keyword (to avoid tripping the auto-formatter on scopes) */
#ifndef eve_scope
#define eve_scope() if (true)
#endif

#endif /* #ifndef EVE_PLATFORM__H */

#ifdef __cplusplus
}
#endif

/* end of file */