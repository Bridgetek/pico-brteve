/**
 * @file EVE_HalImpl_WIN32.c
 * @brief Eve_Hal framework APIs for WIN32 host platform
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

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 28159) // Use GetTickCount64 instead of GetTickCount
#endif

#include "EVE_HalImpl.h"
#include "EVE_Platform.h"
#if defined(_WIN32)

/************
** UTILITY **
************/

/**
 * @brief Get current system clock of Coprocessor
 * 
 * @param phost Pointer to Hal context
 * @return uint32_t Frequency of Coprocessor
 */
uint32_t EVE_Hal_currentFrequency(EVE_HalContext *phost)
{
	uint32_t t0;
	uint32_t t1;

	t0 = EVE_Hal_rd32(phost, REG_CLOCK); /* t0 read */
	/* may not be precise */
	EVE_sleep(15625 / 1000);

	t1 = EVE_Hal_rd32(phost, REG_CLOCK); /* t1 read */
	return ((t1 - t0) * 64); /* bitshift 6 places is the same as multiplying 64 */
}

/*********
** MISC **
*********/

/**
 * @brief Init host MCU
 * 
 */
void EVE_Mcu_initialize()
{
	/* no-op */
}

/**
 * @brief Release host MCU
 * 
 */
void EVE_Mcu_release()
{
	/* no-op */
}

/*********
** MISC **
*********/

static DWORD s_Millis_Start;
static ULONGLONG s_Millis64_Start;

/**
 * @brief Init timer
 * 
 */
void EVE_Millis_initialize()
{
	s_Millis_Start = GetTickCount();
	s_Millis64_Start = GetTickCount64();
}

/**
 * @brief Release timer
 * 
 */
void EVE_Millis_release()
{
	/* no-op */
}

/**
 * @brief Get clock in miliseond
 * 
 * global counter to loopback after ~49.71 days
 * 
 * @return uint32_t Clock number
 */
uint32_t EVE_millis()
{
	return GetTickCount() - s_Millis_Start;
}

/**
* @brief Get clock in miliseond
* 
* @return uint64_t Clock number
*/
uint64_t EVE_millis64()
{
	return GetTickCount64() - s_Millis64_Start;
}

#if defined(ESD_SIMULATION)
int Ft_Sleep__ESD(int ms);
#endif

/**
 * @brief Sleep in milisecond
 * 
 * @param ms Milisecond
 */
void EVE_sleep(uint32_t ms)
{
#if defined(ESD_SIMULATION)
	Ft_Sleep__ESD(ms);
#else
	Sleep(ms);
#endif
}

#endif

#ifdef _MSC_VER
#pragma warning(pop)
#endif

/* end of file */
