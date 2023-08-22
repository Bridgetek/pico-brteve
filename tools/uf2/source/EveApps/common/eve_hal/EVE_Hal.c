/**
 * @file EVE_Hal.c
 * @brief Eve_Hal framework APIs
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

#include "EVE_HalDefs.h"
#include "EVE_Platform.h"

#include "EVE_HalImpl.h"

#include <string.h>

/*********
** INIT **
*********/

EVE_HalPlatform g_HalPlatform;

/**
 * @brief Eve_Hal framework initialization
 * 
 * @return EVE_HalPlatform* Poniter to EVE_HalPlatform struct
 */
EVE_HAL_EXPORT EVE_HalPlatform *EVE_Hal_initialize()
{
	EVE_Mcu_initialize();
	EVE_Millis_initialize();
	EVE_HalImpl_initialize();
	return &g_HalPlatform;
}

/**
 * @brief Close Eve_Hal framework
 * 
 */
EVE_HAL_EXPORT void EVE_Hal_release()
{
	eve_assert_ex(g_HalPlatform.OpenedDevices == 0, "HAL context still open\n");
	EVE_HalImpl_release();
	EVE_Millis_release();
	EVE_Mcu_release();
	memset(&g_HalPlatform, 0, sizeof(EVE_HalPlatform));
}

/**
 * @brief Setup default parameters for Eve_Hal framework
 * 
 * @param parameters Pointer to EVE_HalParameters
 */
EVE_HAL_EXPORT void EVE_Hal_defaults(EVE_HalParameters *parameters)
{
	EVE_Hal_defaultsEx(parameters, -1);
}

EVE_HAL_EXPORT void EVE_Hal_defaultsEx(EVE_HalParameters *parameters, size_t deviceIdx)
{
	memset(parameters, 0, sizeof(EVE_HalParameters));
	eve_assert_do(EVE_HalImpl_defaults(parameters, deviceIdx));
}

/**
 * @brief Open the Eve_Hal framework
 * 
 * @param phost Pointer to Hal context
 * @param parameters Pointer to EVE_HalParameters
 * @return true True if ok
 * @return false False if error
 */
EVE_HAL_EXPORT bool EVE_Hal_open(EVE_HalContext *phost, const EVE_HalParameters *parameters)
{
	memset(phost, 0, sizeof(EVE_HalContext));
	phost->UserContext = parameters->UserContext;
	phost->CbCmdWait = parameters->CbCmdWait;
	return EVE_HalImpl_open(phost, parameters);
}

/**
 * @brief Close the Eve_Hal framework
 * 
 * @param phost Pointer to Hal context
 */
EVE_HAL_EXPORT void EVE_Hal_close(EVE_HalContext *phost)
{
	if (phost->Status == EVE_STATUS_CLOSED)
	{
		eve_printf_debug("Attempt to close HAL context that is already closed\n");
		return;
	}

#ifdef EVE_SUPPORT_MEDIAFIFO
	EVE_Util_closeFile(phost);
#endif
	EVE_HalImpl_close(phost);
	memset(phost, 0, sizeof(EVE_HalContext));
}

/**
 * @brief Idle handler for Eve_Hal framework
 * 
 * @param phost Pointer to Hal context
 */
EVE_HAL_EXPORT void EVE_Hal_idle(EVE_HalContext *phost)
{
	EVE_HalImpl_idle(phost);
}

/*********************
** TRANSFER HELPERS **
*********************/

/**
 * @brief Read 8 bits from Coprocessor's memory
 * 
 * @param phost Pointer to Hal context
 * @param addr Address to be read
 * @return uint8_t Data from Coprocessor
 */
EVE_HAL_EXPORT uint8_t EVE_Hal_rd8(EVE_HalContext *phost, uint32_t addr)
{
	uint8_t value;
	EVE_Hal_startTransfer(phost, EVE_TRANSFER_READ, addr);
	value = EVE_Hal_transfer8(phost, 0);
	EVE_Hal_endTransfer(phost);
	return value;
}

/**
 * @brief Read 2 bytes from Coprocessor's memory
 * 
 * @param phost Pointer to Hal context
 * @param addr Address to be read
 * @return uint16_t Data from Coprocessor
 */
EVE_HAL_EXPORT uint16_t EVE_Hal_rd16(EVE_HalContext *phost, uint32_t addr)
{
	uint16_t value;
	EVE_Hal_startTransfer(phost, EVE_TRANSFER_READ, addr);
	value = EVE_Hal_transfer16(phost, 0);
	EVE_Hal_endTransfer(phost);
	return value;
}

/**
 * @brief Read 4 bytes from Coprocessor's memory
 * 
 * @param phost Pointer to Hal context
 * @param addr Address to be read
 * @return uint16_t Data from Coprocessor
 */
EVE_HAL_EXPORT uint32_t EVE_Hal_rd32(EVE_HalContext *phost, uint32_t addr)
{
	uint32_t value;
	EVE_Hal_startTransfer(phost, EVE_TRANSFER_READ, addr);
	value = EVE_Hal_transfer32(phost, 0);
	EVE_Hal_endTransfer(phost);
	return value;
}

/**
 * @brief Read a block data from Coprocessor's memory
 * 
 * @param phost Pointer to Hal context
 * @param result Buffer where data write to
 * @param addr Address to bbe read
 * @param size Size to be read
 */
EVE_HAL_EXPORT void EVE_Hal_rdMem(EVE_HalContext *phost, uint8_t *result, uint32_t addr, uint32_t size)
{
	EVE_Hal_startTransfer(phost, EVE_TRANSFER_READ, addr);
	EVE_Hal_transferMem(phost, result, NULL, size);
	EVE_Hal_endTransfer(phost);
}

/**
 * @brief Write 8 bits to Coprocessor's memory
 * 
 * @param phost Pointer to Hal context
 * @param addr Address to be write
 * @param v Value to write
 */
EVE_HAL_EXPORT void EVE_Hal_wr8(EVE_HalContext *phost, uint32_t addr, uint8_t v)
{
	EVE_Hal_startTransfer(phost, EVE_TRANSFER_WRITE, addr);
	EVE_Hal_transfer8(phost, v);
	EVE_Hal_endTransfer(phost);
}

/**
 * @brief Write 2 bytes to Coprocessor's memory
 * 
 * @param phost Pointer to Hal context
 * @param addr Address to be write
 * @param v Value to write
 */
EVE_HAL_EXPORT void EVE_Hal_wr16(EVE_HalContext *phost, uint32_t addr, uint16_t v)
{
	EVE_Hal_startTransfer(phost, EVE_TRANSFER_WRITE, addr);
	EVE_Hal_transfer16(phost, v);
	EVE_Hal_endTransfer(phost);
}

/**
 * @brief Write 4 bytes to Coprocessor's memory
 * 
 * @param phost Pointer to Hal context
 * @param addr Address to be write
 * @param v Value to write
 */
EVE_HAL_EXPORT void EVE_Hal_wr32(EVE_HalContext *phost, uint32_t addr, uint32_t v)
{
	EVE_Hal_startTransfer(phost, EVE_TRANSFER_WRITE, addr);
	EVE_Hal_transfer32(phost, v);
	EVE_Hal_endTransfer(phost);
}

/**
 * @brief Write a buffer to Coprocessor's memory
 * 
 * @param phost Pointer to Hal context
 * @param addr Address to be write
 * @param buffer Data to be write
 * @param size Size of buffer
 */
EVE_HAL_EXPORT void EVE_Hal_wrMem(EVE_HalContext *phost, uint32_t addr, const uint8_t *buffer, uint32_t size)
{
	EVE_Hal_startTransfer(phost, EVE_TRANSFER_WRITE, addr);
	EVE_Hal_transferMem(phost, NULL, buffer, size);
	EVE_Hal_endTransfer(phost);
}

/**
 * @brief Write a buffer in ProgMem to Coprocessor's memory
 * 
 * @param phost Pointer to Hal context
 * @param addr Address to be write
 * @param uint8_t Data to be write
 * @param size Size of buffer
 */
EVE_HAL_EXPORT void EVE_Hal_wrProgMem(EVE_HalContext *phost, uint32_t addr, eve_progmem_const uint8_t *buffer, uint32_t size)
{
	EVE_Hal_startTransfer(phost, EVE_TRANSFER_WRITE, addr);
	EVE_Hal_transferProgMem(phost, NULL, buffer, size);
	EVE_Hal_endTransfer(phost);
}

/**
 * @brief Write a string to Coprocessor's memory
 * 
 * @param phost Pointer to Hal context
 * @param addr Address to be write
 * @param str String to be write
 * @param index Start postion in the string
 * @param size Size of the string
 * @param padMask Padding mask
 */
EVE_HAL_EXPORT void EVE_Hal_wrString(EVE_HalContext *phost, uint32_t addr, const char *str, uint32_t index, uint32_t size, uint32_t padMask)
{
	EVE_Hal_startTransfer(phost, EVE_TRANSFER_WRITE, addr);
	EVE_Hal_transferString(phost, str, index, size, padMask);
	EVE_Hal_endTransfer(phost);
}

/************
** UTILITY **
************/

/**
 * @brief Trim the internal clock till the measured frequency is within the acceptable range
 * 
 * @param phost Pointer to Hal context
 * @param lowFreq Low frequency target to trim
 * @return int32_t frequency after trim
 */
EVE_HAL_EXPORT int32_t EVE_Hal_clockTrimming(EVE_HalContext *phost, uint32_t lowFreq)
{
	uint32_t f;

	/* Trim the internal clock by increase the REG_TRIM register till the measured frequency is within the acceptable range.*/
	f = EVE_Hal_currentFrequency(phost);
	for (uint8_t i = 0; i < 31 && f < lowFreq; i++)
	{
		EVE_Hal_wr8(phost, REG_TRIM, i); /* increase the REG_TRIM register value automatically increases the internal clock */
		f = EVE_Hal_currentFrequency(phost);
	}
	EVE_Hal_wr32(phost, REG_FREQUENCY, f); /* Set the final frequency to be used for internal operations */

	return f;
}

/*********
** HOST **
*********/

/**
 * @brief Select clock source for Coprocessor
 * 
 * @param phost Pointer to Hal context
 * @param pllsource Clock source
 */
EVE_HAL_EXPORT void EVE_Host_clockSelect(EVE_HalContext *phost, EVE_PLL_SOURCE_T pllsource)
{
	EVE_Hal_hostCommand(phost, (uint8_t)pllsource);
}

/**
 * @brief Select system clock for Coprocessor
 * 
 * @param phost Pointer to Hal context
 * @param freq Frequency to set
 */
EVE_HAL_EXPORT void EVE_Host_pllFreqSelect(EVE_HalContext *phost, EVE_PLL_FREQ_T freq)
{
	EVE_Hal_hostCommand(phost, (uint8_t)freq);
}

/**
 * @brief Switch power mode for Coprocessor
 * 
 * @param phost Pointer to Hal context
 * @param pwrmode Power mode
 */
EVE_HAL_EXPORT void EVE_Host_powerModeSwitch(EVE_HalContext *phost, EVE_POWER_MODE_T pwrmode)
{
	EVE_Hal_hostCommand(phost, (uint8_t)pwrmode);
}

/**
 * @brief Send reset signal to Coprocessor
 * 
 * @param phost Pointer to Hal context
 */
EVE_HAL_EXPORT void EVE_Host_coreReset(EVE_HalContext *phost)
{
	EVE_Hal_hostCommand(phost, EVE_CORE_RESET);
}

/**
 * @brief Set system clock for Coprocessor
 * 
 * @param phost Pointer to Hal context
 * @param freq Frequency to set
 */
EVE_HAL_EXPORT void EVE_Host_selectSysClk(EVE_HalContext *phost, EVE_81X_PLL_FREQ_T freq)
{
#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
	if (EVE_CHIPID >= EVE_FT810)
	{
#if (EVE_SUPPORT_CHIPID >= EVE_BT815)
		if (EVE_SYSCLK_84M == freq)
			EVE_Hal_hostCommandExt3(phost, (uint32_t)0x61 | (0x80 << 8) | (0x07 << 8));
		else
#endif
		    if (EVE_SYSCLK_72M == freq)
			EVE_Hal_hostCommandExt3(phost, (uint32_t)0x61 | (0x40 << 8) | (0x06 << 8));
		else if (EVE_SYSCLK_60M == freq)
			EVE_Hal_hostCommandExt3(phost, (uint32_t)0x61 | (0x40 << 8) | (0x05 << 8));
		else if (EVE_SYSCLK_48M == freq)
			EVE_Hal_hostCommandExt3(phost, (uint32_t)0x61 | (0x40 << 8) | (0x04 << 8));
		else if (EVE_SYSCLK_36M == freq)
			EVE_Hal_hostCommandExt3(phost, (uint32_t)0x61 | (0x03 << 8));
		else if (EVE_SYSCLK_24M == freq)
			EVE_Hal_hostCommandExt3(phost, (uint32_t)0x61 | (0x02 << 8));
		else if (EVE_SYSCLK_DEFAULT == freq) //default clock
			EVE_Hal_hostCommandExt3(phost, 0x61);
	}
	else
#endif
	{
		switch (freq)
		{
		case EVE_SYSCLK_24M:
			EVE_Host_pllFreqSelect(phost, EVE_PLL_24M);
			break;
		case EVE_SYSCLK_36M:
			EVE_Host_pllFreqSelect(phost, EVE_PLL_36M);
			break;
		default:
			eve_printf_debug("Invalid sys clk frequency selected (%i)\n", (int)freq);
			/* no break */
		case EVE_SYSCLK_48M:
			EVE_Host_pllFreqSelect(phost, EVE_PLL_48M);
			break;
		}
	}
}

#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
/**
 * @brief Power off a component
 * 
 * @param phost Pointer to Hal context
 * @param val Component number
 */
EVE_HAL_EXPORT void EVE_Host_powerOffComponents(EVE_HalContext *phost, uint8_t val)
{
	EVE_Hal_hostCommandExt3(phost, (uint32_t)0x49 | (val << 8));
}

/**
 * @brief Set the drive strength for various pins
 * 
 * @param phost Pointer to Hal context
 * @param strength Drive strength
 * @param group Pin group to set
 */
EVE_HAL_EXPORT void EVE_Host_padDriveStrength(EVE_HalContext *phost, EVE_81X_GPIO_DRIVE_STRENGTH_T strength, EVE_81X_GPIO_GROUP_T group)
{
	EVE_Hal_hostCommandExt3(phost, (uint32_t)0x70 | (group << 8) | (strength << 8));
}

/**
 * @brief Hold the device in reset state
 * 
 * @param phost Pointer to Hal context
 */
EVE_HAL_EXPORT void EVE_Host_resetActive(EVE_HalContext *phost)
{
	EVE_Hal_hostCommandExt3(phost, EVE_81X_RESET_ACTIVE);
}

/**
 * @brief Exit reset state, Eve will power on and enter into its default state
 * 
 * @param phost Pointer to Hal context
 */
EVE_HAL_EXPORT void EVE_Host_resetRemoval(EVE_HalContext *phost)
{
	EVE_Hal_hostCommandExt3(phost, EVE_81X_RESET_REMOVAL);
}
#endif

/**
 * @brief Display a fullscreen debug message using TEXT8X8. Uses the back of RAM_G.
 * 
 * @param phost Pointer to Hal context
 * @param str Error message to show
 * @param size Size of the message
 */
void EVE_Hal_displayMessage(EVE_HalContext *phost, const char *str, uint16_t size)
{
	uint32_t round = ((size + 31U) & ~31U);
	uint32_t addr = RAM_G + RAM_G_SIZE - round;
	uint32_t dl = 0;

	/* Abuse back of RAM_G to store error */
	/* May invalidate user data... */
	EVE_Hal_wrMem(phost, addr, (uint8_t *)str, size);

	/* Empty remaining space after text */
	EVE_Hal_startTransfer(phost, EVE_TRANSFER_WRITE, addr + size);
	for (uint32_t i = size; i < round; ++i)
		EVE_Hal_transfer8(phost, 0);
	EVE_Hal_endTransfer(phost);

	/* Generate bluescreen */
	EVE_Hal_wr32(phost, RAM_DL + ((dl++) << 2), CLEAR_COLOR_RGB(0x00, 0x20, 0x40));
	EVE_Hal_wr32(phost, RAM_DL + ((dl++) << 2), CLEAR(1, 1, 1));
	EVE_Hal_wr32(phost, RAM_DL + ((dl++) << 2), BITMAP_HANDLE(15)); /* Scratch handle will reset anyway after reset */
	EVE_Hal_wr32(phost, RAM_DL + ((dl++) << 2), BITMAP_SOURCE(addr));
	EVE_Hal_wr32(phost, RAM_DL + ((dl++) << 2), BITMAP_SIZE_H(0, 0));
	EVE_Hal_wr32(phost, RAM_DL + ((dl++) << 2), BITMAP_SIZE(NEAREST, BORDER, BORDER, 256, (round >> 2)));
	EVE_Hal_wr32(phost, RAM_DL + ((dl++) << 2), BITMAP_LAYOUT_H(0, 0));
	EVE_Hal_wr32(phost, RAM_DL + ((dl++) << 2), BITMAP_LAYOUT(TEXT8X8, 32, (round >> 2)));
	EVE_Hal_wr32(phost, RAM_DL + ((dl++) << 2), BEGIN(BITMAPS));
	EVE_Hal_wr32(phost, RAM_DL + ((dl++) << 2), VERTEX2II(32, 32, 15, 0));
#if ENABLE_ERR_REPORT
	EVE_Hal_wr32(phost, RAM_DL + ((dl++) << 2), BITMAP_SOURCE(RAM_ERR_REPORT));
	EVE_Hal_wr32(phost, RAM_DL + ((dl++) << 2), VERTEX2II(32, 96, 15, 0));
#endif
	EVE_Hal_wr32(phost, RAM_DL + ((dl++) << 2), DISPLAY());
	EVE_Hal_wr8(phost, REG_DLSWAP, DLSWAP_FRAME);
}

/* end of file */
