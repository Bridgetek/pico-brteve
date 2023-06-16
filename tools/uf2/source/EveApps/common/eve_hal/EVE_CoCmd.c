/**
 * @file EVE_CoCmd.c
 * @brief EVE's co-processor commmands
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


#include "EVE_Platform.h"

/**********************************************************************
***********************************************************************
**********************************************************************/

EVE_HAL_EXPORT void EVE_CoCmd_d(EVE_HalContext *phost, uint32_t cmd)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, cmd, 0))
		return;
#endif

	EVE_Cmd_wr32(phost, cmd);
}

EVE_HAL_EXPORT void EVE_CoCmd_dd(EVE_HalContext *phost, uint32_t cmd, uint32_t d0)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, cmd, d0))
		return;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr32(phost, d0);
	EVE_Cmd_endFunc(phost);
}

EVE_HAL_EXPORT void EVE_CoCmd_ddd(EVE_HalContext *phost, uint32_t cmd, uint32_t d0, uint32_t d1)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, cmd, d0))
		return;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr32(phost, d0);
	EVE_Cmd_wr32(phost, d1);
	EVE_Cmd_endFunc(phost);
}

EVE_HAL_EXPORT void EVE_CoCmd_dddd(EVE_HalContext *phost, uint32_t cmd, uint32_t d0, uint32_t d1, uint32_t d2)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, cmd, d0))
		return;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr32(phost, d0);
	EVE_Cmd_wr32(phost, d1);
	EVE_Cmd_wr32(phost, d2);
	EVE_Cmd_endFunc(phost);
}

EVE_HAL_EXPORT void EVE_CoCmd_ddddd(EVE_HalContext *phost, uint32_t cmd, uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, cmd, d0))
		return;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr32(phost, d0);
	EVE_Cmd_wr32(phost, d1);
	EVE_Cmd_wr32(phost, d2);
	EVE_Cmd_wr32(phost, d3);
	EVE_Cmd_endFunc(phost);
}

EVE_HAL_EXPORT void EVE_CoCmd_ddww(EVE_HalContext *phost, uint32_t cmd,
    uint32_t d0, uint16_t w1, uint16_t w2)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, cmd, d0))
		return;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr32(phost, d0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_endFunc(phost);
}

EVE_HAL_EXPORT void EVE_CoCmd_ddwww(EVE_HalContext *phost, uint32_t cmd,
    uint32_t d0, uint16_t w1, uint16_t w2, uint16_t w3)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, cmd, d0))
		return;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr32(phost, d0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_wr16(phost, w3);
	EVE_Cmd_wr16(phost, 0);
	EVE_Cmd_endFunc(phost);
}

EVE_HAL_EXPORT void EVE_CoCmd_dddwwww(EVE_HalContext *phost, uint32_t cmd,
    uint32_t d0, uint32_t d1,
    uint16_t w2, uint16_t w3, uint16_t w4, uint16_t w5)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, cmd, d0))
		return;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr32(phost, d0);
	EVE_Cmd_wr32(phost, d1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_wr16(phost, w3);
	EVE_Cmd_wr16(phost, w4);
	EVE_Cmd_wr16(phost, w5);
	EVE_Cmd_endFunc(phost);
}

EVE_HAL_EXPORT void EVE_CoCmd_dwwdd(EVE_HalContext *phost, uint32_t cmd,
    uint16_t w0, uint16_t w1, uint32_t d2, uint32_t d3)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, cmd, 0))
		return;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr16(phost, w0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr32(phost, d2);
	EVE_Cmd_wr32(phost, d3);
	EVE_Cmd_endFunc(phost);
}

EVE_HAL_EXPORT void EVE_CoCmd_dwwdwwd(EVE_HalContext *phost, uint32_t cmd,
    uint16_t w0, uint16_t w1, uint32_t d2,
    uint16_t w3, uint16_t w4, uint32_t d5)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, cmd, 0))
		return;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr16(phost, w0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr32(phost, d2);
	EVE_Cmd_wr16(phost, w3);
	EVE_Cmd_wr16(phost, w4);
	EVE_Cmd_wr32(phost, d5);
	EVE_Cmd_endFunc(phost);
}

EVE_HAL_EXPORT void EVE_CoCmd_dwwwwdw(EVE_HalContext *phost, uint32_t cmd,
    uint16_t w0, uint16_t w1, uint16_t w2, uint16_t w3,
    uint32_t d4, uint16_t w5)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, cmd, 0))
		return;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr16(phost, w0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_wr16(phost, w3);
	EVE_Cmd_wr32(phost, d4);
	EVE_Cmd_wr16(phost, w5);
	EVE_Cmd_wr16(phost, 0);
	EVE_Cmd_endFunc(phost);
}

EVE_HAL_EXPORT void EVE_CoCmd_dwwwwdww(EVE_HalContext *phost, uint32_t cmd,
    uint16_t w0, uint16_t w1, uint16_t w2, uint16_t w3,
    uint32_t d4, uint16_t w5, uint16_t w6)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, cmd, 0))
		return;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr16(phost, w0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_wr16(phost, w3);
	EVE_Cmd_wr32(phost, d4);
	EVE_Cmd_wr16(phost, w5);
	EVE_Cmd_wr16(phost, w6);
	EVE_Cmd_endFunc(phost);
}

EVE_HAL_EXPORT void EVE_CoCmd_dwwww(EVE_HalContext *phost, uint32_t cmd,
    uint16_t w0, uint16_t w1, uint16_t w2, uint16_t w3)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, cmd, 0))
		return;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr16(phost, w0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_wr16(phost, w3);
	EVE_Cmd_endFunc(phost);
}

EVE_HAL_EXPORT void EVE_CoCmd_dwwwww(EVE_HalContext *phost, uint32_t cmd,
    uint16_t w0, uint16_t w1, uint16_t w2, uint16_t w3, uint16_t w4)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, cmd, 0))
		return;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr16(phost, w0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_wr16(phost, w3);
	EVE_Cmd_wr16(phost, w4);
	EVE_Cmd_wr16(phost, 0);
	EVE_Cmd_endFunc(phost);
}

EVE_HAL_EXPORT void EVE_CoCmd_dwwwwd(EVE_HalContext *phost, uint32_t cmd,
    uint16_t w0, uint16_t w1, uint16_t w2, uint16_t w3, uint32_t d4)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, cmd, 0))
		return;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr16(phost, w0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_wr16(phost, w3);
	EVE_Cmd_wr32(phost, d4);
	EVE_Cmd_endFunc(phost);
}

EVE_HAL_EXPORT void EVE_CoCmd_dwwwwwww(EVE_HalContext *phost, uint32_t cmd,
    uint16_t w0, uint16_t w1, uint16_t w2, uint16_t w3,
    uint16_t w4, uint16_t w5, uint16_t w6)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, cmd, 0))
		return;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr16(phost, w0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_wr16(phost, w3);
	EVE_Cmd_wr16(phost, w4);
	EVE_Cmd_wr16(phost, w5);
	EVE_Cmd_wr16(phost, w6);
	EVE_Cmd_wr16(phost, 0);
	EVE_Cmd_endFunc(phost);
}

EVE_HAL_EXPORT void EVE_CoCmd_dwwwwwwww(EVE_HalContext *phost, uint32_t cmd,
    uint16_t w0, uint16_t w1, uint16_t w2, uint16_t w3,
    uint16_t w4, uint16_t w5, uint16_t w6, uint16_t w7)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, cmd, 0))
		return;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr16(phost, w0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_wr16(phost, w3);
	EVE_Cmd_wr16(phost, w4);
	EVE_Cmd_wr16(phost, w5);
	EVE_Cmd_wr16(phost, w6);
	EVE_Cmd_wr16(phost, w7);
	EVE_Cmd_endFunc(phost);
}

EVE_HAL_EXPORT void EVE_CoCmd_dwwwwwwz(EVE_HalContext *phost, uint32_t cmd,
    uint16_t w0, uint16_t w1, uint16_t w2, uint16_t w3,
    uint16_t w4, uint16_t w5, const char *s)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, cmd, 0))
		return;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr16(phost, w0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_wr16(phost, w3);
	EVE_Cmd_wr16(phost, w4);
	EVE_Cmd_wr16(phost, w5);
	EVE_Cmd_wrString(phost, s, EVE_CMD_STRING_MAX);
	EVE_Cmd_endFunc(phost);
}

EVE_HAL_EXPORT void EVE_CoCmd_dwwwwwwz_s(EVE_HalContext *phost, uint32_t cmd,
    uint16_t w0, uint16_t w1, uint16_t w2, uint16_t w3,
    uint16_t w4, uint16_t w5, const char *s, uint32_t len)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, cmd, 0))
		return;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr16(phost, w0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_wr16(phost, w3);
	EVE_Cmd_wr16(phost, w4);
	EVE_Cmd_wr16(phost, w5);
	EVE_Cmd_wrString(phost, s, len);
	EVE_Cmd_endFunc(phost);
}

#if defined(_DEBUG) && defined(EVE_MULTI_GRAPHICS_TARGET)
EVE_HAL_EXPORT void EVE_CoCmd_debugUnsupported(const char *cmd, uint32_t chipId)
{
	eve_printf_debug("Coprocessor command %s is not supported on target platform %lx\n", cmd, (unsigned long)chipId);
}
#endif

/**********************************************************************
***********************************************************************
**********************************************************************/

EVE_HAL_EXPORT bool EVE_CoCmd_getMatrix(EVE_HalContext *phost, int32_t *m)
{
	uint16_t resAddr;
	int i;

#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, CMD_GETMATRIX, 0))
		return false;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_GETMATRIX);
	resAddr = EVE_Cmd_moveWp(phost, 6 * 4);
	EVE_Cmd_endFunc(phost);

	/* Read result */
	if (m)
	{
		if (!EVE_Cmd_waitFlush(phost))
			return false;
		EVE_Hal_startTransfer(phost, EVE_TRANSFER_READ, RAM_CMD + resAddr);
		for (i = 0; i < 6; ++i)
			m[i] = EVE_Hal_transfer32(phost, 0);
		EVE_Hal_endTransfer(phost);
	}
	return true;
}

EVE_HAL_EXPORT void EVE_CoCmd_setRotate(EVE_HalContext *phost, uint32_t r)
{
#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
	const bool swapXY = EVE_CHIPID >= EVE_FT810 ? r & 0x2 : false;
#endif

#if EVE_CMD_HOOKS
	/* Check hook */
	if (phost->CoCmdHook && phost->CoCmdHook(phost, CMD_SETROTATE, r))
		return;
#endif

#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
	if (EVE_CHIPID >= EVE_FT810)
	{
		/* Update cached width and height based on rotation */
		if (swapXY)
		{
			phost->Width = EVE_Hal_rd16(phost, REG_VSIZE);
			phost->Height = EVE_Hal_rd16(phost, REG_HSIZE);
		}
		else
		{
			phost->Width = EVE_Hal_rd16(phost, REG_HSIZE);
			phost->Height = EVE_Hal_rd16(phost, REG_VSIZE);
		}

		/* Command */
		EVE_Cmd_startFunc(phost);
		EVE_Cmd_wr32(phost, CMD_SETROTATE);
		EVE_Cmd_wr32(phost, r);
		EVE_Cmd_endFunc(phost);
	}
	else
#endif
	{
		/* Software based fallback for FT80X */
		EVE_Hal_wr32(phost, REG_ROTATE, r);

		/* TODO: Adjust touch transform matrix */
	}

	/* Don't keep this in the write buffer */
	EVE_Hal_flush(phost);
}

#if (EVE_SUPPORT_CHIPID >= EVE_BT817)

/* Returns new frequency, or 0 in case of failure */
EVE_HAL_EXPORT uint32_t EVE_CoCmd_pclkFreq(EVE_HalContext *phost, uint32_t ftarget, int32_t rounding)
{
	uint16_t resAddr;

	EVE_MULTI_TARGET_CHECK_RETURN(CMD_PCLKFREQ, EVE_CHIPID >= EVE_BT817, 0);

#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, CMD_PCLKFREQ, ftarget))
		return 0;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_PCLKFREQ);
	EVE_Cmd_wr32(phost, ftarget);
	EVE_Cmd_wr32(phost, rounding);
	resAddr = EVE_Cmd_moveWp(phost, 4);
	EVE_Cmd_endFunc(phost);

	/* Wait for the result */
	if (!EVE_Cmd_waitFlush(phost))
		return 0;
	return EVE_Hal_rd32(phost, RAM_CMD + resAddr);
}

#endif

/**********************************************************************
***********************************************************************
**********************************************************************/

EVE_HAL_EXPORT uint32_t EVE_CoCmd_calibrate(EVE_HalContext *phost)
{
	uint16_t resAddr;

#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, CMD_CALIBRATE, 0))
		return 0;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_CALIBRATE);
	resAddr = EVE_Cmd_moveWp(phost, 4);
	EVE_Cmd_endFunc(phost);

#if (EVE_DL_OPTIMIZE)
	phost->DlPrimitive = 0;
#endif

	/* Wait for the result */
	if (!EVE_Cmd_waitFlush(phost))
		return 0;
	return EVE_Hal_rd32(phost, RAM_CMD + resAddr);
}

#if (EVE_SUPPORT_CHIPID >= EVE_BT815)

EVE_HAL_EXPORT bool EVE_CoCmd_animStart(EVE_HalContext *phost, int32_t ch, uint32_t aoptr, uint32_t loop)
{
	uint32_t flashStatus;

	EVE_MULTI_TARGET_CHECK_RETURN(CMD_ANIMSTART, EVE_CHIPID >= EVE_BT815, false);

#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, CMD_ANIMSTART, ch))
		return false;
#endif

	if (!EVE_Cmd_waitFlush(phost))
		return false;
	flashStatus = EVE_Hal_rd32(phost, REG_FLASH_STATUS);
	if (flashStatus < FLASH_STATUS_FULL)
	{
		// Don't attempt to start animation without full flash speed, it hangs the coprocessor
		eve_printf_debug("Attempt to send CMD_ANIMSTART without FLASH_STATUS_FULL (REG_FLASH_STATUS: %i)\n", (int)flashStatus);
		return false;
	}

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_ANIMSTART);
	EVE_Cmd_wr32(phost, ch);
	EVE_Cmd_wr32(phost, aoptr);
	EVE_Cmd_wr32(phost, loop);
	EVE_Cmd_endFunc(phost);
	return EVE_Cmd_waitFlush(phost);
}

#endif

#if (EVE_SUPPORT_CHIPID >= EVE_BT817)

EVE_HAL_EXPORT uint32_t EVE_CoCmd_calibrateSub(EVE_HalContext *phost, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
	uint16_t resAddr;

	EVE_MULTI_TARGET_CHECK_RETURN(CMD_CALIBRATESUB, EVE_CHIPID >= EVE_BT817, 0);

#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, CMD_CALIBRATESUB, 0))
		return 0;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_CALIBRATESUB);
	EVE_Cmd_wr16(phost, x);
	EVE_Cmd_wr16(phost, y);
	EVE_Cmd_wr16(phost, w);
	EVE_Cmd_wr16(phost, h);
	resAddr = EVE_Cmd_moveWp(phost, 4);
	EVE_Cmd_endFunc(phost);

#if (EVE_DL_OPTIMIZE)
	phost->DlPrimitive = 0;
#endif

	/* Wait for the result */
	if (!EVE_Cmd_waitFlush(phost))
		return 0;
	return EVE_Hal_rd32(phost, RAM_CMD + resAddr);
}

#endif

/**********************************************************************
***********************************************************************
**********************************************************************/

/* Nothing beyond this */
