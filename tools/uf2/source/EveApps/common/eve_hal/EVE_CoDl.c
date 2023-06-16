/**
 * @file EVE_CoDl.c
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

#include "EVE_CoDl.h"

#if defined(_MSC_VER) && (_MSC_VER < 1800)
#include <stdio.h> /* memset */
#endif

EVE_HAL_EXPORT void EVE_CoDlImpl_resetDlState(EVE_HalContext *phost)
{
#if (EVE_DL_OPTIMIZE) || (EVE_DL_CACHE_SCISSOR) || (EVE_SUPPORT_CHIPID < EVE_FT810) || defined(EVE_MULTI_GRAPHICS_TARGET)
	phost->DlStateIndex = 0;
#if defined(_MSC_VER) && (_MSC_VER < 1800)
	/* Designated initializers not supported in older Visual Studio versions */
	memset(&phost->DlState[0], 0, sizeof(EVE_HalDlState));
#if (EVE_DL_OPTIMIZE)
	phost->DlState[0].LineWidth = 16;
	phost->DlState[0].PointSize = 16;
	phost->DlState[0].ColorRGB = 0xFFFFFF;
	phost->DlState[0].ColorA = 0xFF;
	phost->DlState[0].Handle = 0x3F; /* Invalid value */
#endif
	phost->DlState[0].VertexFormat = 4;
#else
	phost->DlState[0] = (EVE_HalDlState)
	{
#if (EVE_DL_OPTIMIZE)
		.LineWidth = 16,
		.PointSize = 16,
		.ColorRGB = 0xFFFFFF,
#endif
#if (EVE_DL_CACHE_SCISSOR)
		.ScissorWidth = (uint16_t)phost->Width,
		.ScissorHeight = (uint16_t)phost->Height,
#endif
#if (EVE_DL_OPTIMIZE)
		.ColorA = 0xFF,
		.Handle = 0x3F, /* Invalid value */
#endif
#if (EVE_DL_OPTIMIZE) || (EVE_SUPPORT_CHIPID < EVE_FT810) || defined(EVE_MULTI_GRAPHICS_TARGET)
		    .VertexFormat = 4,
#elif (EVE_DL_CACHE_SCISSOR)
		0
#endif
	};
#endif
#endif

#if (EVE_DL_OPTIMIZE)
	phost->DlPrimitive = 0;
#endif
}

EVE_HAL_EXPORT void EVE_CoDlImpl_resetCoState(EVE_HalContext *phost)
{
#if (EVE_DL_OPTIMIZE)
	phost->CoFgColor = 0x003870;
	phost->CoBgColor = 0x002040;
	phost->CoBitmapTransform = false;
#endif

#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
	if (EVE_CHIPID >= EVE_FT810)
	{
		phost->CoScratchHandle = 15;
	}
#endif
}

#if (EVE_SUPPORT_CHIPID < EVE_FT810) || defined(EVE_MULTI_GRAPHICS_TARGET)

EVE_HAL_EXPORT void EVE_CoDlImpl_vertex2ii_translate(EVE_HalContext *phost, uint16_t x, uint16_t y, uint8_t handle, uint8_t cell)
{
	int16_t xf = (x << 4) + EVE_DL_STATE.VertexTranslateX;
	int16_t yf = (y << 4) + EVE_DL_STATE.VertexTranslateY;
	if (xf < 0 || yf < 0 || xf & 0xF || yf & 0xF)
	{
#if (EVE_DL_OPTIMIZE) || (!EVE_DL_END_PRIMITIVE)
		if (phost->DlPrimitive == BITMAPS
#if (EVE_DL_OPTIMIZE)
		    && (EVE_DL_STATE.Handle != handle || EVE_DL_STATE.Cell != cell)
#endif
		)
#endif
		{
			EVE_CoCmd_dl(phost, SAVE_CONTEXT());
			EVE_CoCmd_dl(phost, BITMAP_HANDLE(handle));
			EVE_CoCmd_dl(phost, CELL(cell));
			EVE_CoCmd_dl(phost, VERTEX2F(xf, yf));
			EVE_CoCmd_dl(phost, RESTORE_CONTEXT());
		}
#if (EVE_DL_OPTIMIZE) || (!EVE_DL_END_PRIMITIVE)
		else
		{
			EVE_CoCmd_dl(phost, VERTEX2F(xf, yf));
		}
#endif
	}
	else
	{
		EVE_CoCmd_dl(phost, VERTEX2II(xf >> 4, yf >> 4, handle, cell));
	}
}

#endif

/* Nothing beyond this */
