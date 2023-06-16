/**
 * @file EVE_CoCmd_State.c
 * @brief EVE's co-processor command
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

EVE_HAL_EXPORT bool EVE_CoCmd_bitmapTransform(EVE_HalContext *phost, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t tx0, int32_t ty0, int32_t tx1, int32_t ty1, int32_t tx2, int32_t ty2, uint16_t *result)
{
	uint16_t resAddr;

#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, CMD_BITMAP_TRANSFORM, 0))
		return false;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_BITMAP_TRANSFORM);
	EVE_Cmd_wr32(phost, x0);
	EVE_Cmd_wr32(phost, y0);
	EVE_Cmd_wr32(phost, x1);
	EVE_Cmd_wr32(phost, y1);
	EVE_Cmd_wr32(phost, x2);
	EVE_Cmd_wr32(phost, y2);
	EVE_Cmd_wr32(phost, tx0);
	EVE_Cmd_wr32(phost, ty0);
	EVE_Cmd_wr32(phost, tx1);
	EVE_Cmd_wr32(phost, ty1);
	EVE_Cmd_wr32(phost, tx2);
	EVE_Cmd_wr32(phost, ty2);
	resAddr = EVE_Cmd_moveWp(phost, 4);
	EVE_Cmd_endFunc(phost);

#if EVE_DL_OPTIMIZE
	EVE_DL_STATE.BitmapTransform = true;
#endif

	/* Read result */
	if (result)
	{
		if (!EVE_Cmd_waitFlush(phost))
			return false;
		*result = EVE_Hal_rd16(phost, RAM_CMD + resAddr);
	}
	return true;
}

/* end of file */
