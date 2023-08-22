/**
 * @file EVE_CoCmd_Widgets.c
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

#include <stdarg.h>

#if (EVE_SUPPORT_CHIPID >= EVE_BT815)
/* Count number of arguments in Cmd_Text for string format*/
static uint8_t countArgs(const char *str)
{
	uint8_t count = 0;
	const char *tmp = str;

	while ((tmp = strstr(tmp, "%")))
	{
		if (*(tmp + 1) == '%')
		{
			tmp += 2;
		}
		else
		{
			count++;
			tmp++;
		}
	}
	return count;
}
#endif

EVE_HAL_EXPORT void EVE_CoCmd_text(EVE_HalContext *phost, int16_t x, int16_t y, int16_t font, uint16_t options, const char *s, ...)
{
	va_list args;
#if (EVE_SUPPORT_CHIPID >= EVE_BT815)
	uint8_t num;
#endif

#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, CMD_TEXT, 0))
		return;
#endif

	if (font >= 32)
	{
		if (font != 63)
			eve_printf_debug("Invalid font handle specified: %i\n", (int)font);
		return;
	}
	va_start(args, s);
#if (EVE_SUPPORT_CHIPID >= EVE_BT815) /* OPT_FORMAT not defined in FT8xx chip */
	num = (options & OPT_FORMAT) ? (countArgs(s)) : (0); /* Only check % characters if option OPT_FORMAT is set */
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_TEXT);
	EVE_Cmd_wr16(phost, x);
	EVE_Cmd_wr16(phost, y);
	EVE_Cmd_wr16(phost, font);
	EVE_Cmd_wr16(phost, options);
	EVE_Cmd_wrString(phost, s, EVE_CMD_STRING_MAX);
#if (EVE_SUPPORT_CHIPID >= EVE_BT815)
	if (EVE_CHIPID >= EVE_BT815)
	{
		for (uint8_t i = 0; i < num; i++)
			EVE_Cmd_wr32(phost, (uint32_t)va_arg(args, uint32_t));
	}
#endif
	EVE_Cmd_endFunc(phost);
	va_end(args);

#if (EVE_DL_OPTIMIZE)
	phost->DlPrimitive = 0;
#endif
}

EVE_HAL_EXPORT void EVE_CoCmd_text_s(EVE_HalContext *phost, int16_t x, int16_t y, int16_t font, uint16_t options, const char *s, uint32_t length)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, CMD_TEXT, 0))
		return;
#endif

	if (font >= 32)
	{
		if (font != 63)
			eve_printf_debug("Invalid font handle specified: %i\n", (int)font);
		return;
	}

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_TEXT);
	EVE_Cmd_wr16(phost, x);
	EVE_Cmd_wr16(phost, y);
	EVE_Cmd_wr16(phost, font);
	EVE_Cmd_wr16(phost, options);
	EVE_Cmd_wrString(phost, s, length);
	EVE_Cmd_endFunc(phost);

#if (EVE_DL_OPTIMIZE)
	phost->DlPrimitive = 0;
#endif
}

EVE_HAL_EXPORT void EVE_CoCmd_text_ex(EVE_HalContext *phost, int16_t x, int16_t y, int16_t font, uint16_t options, bool bottom, int16_t baseLine, int16_t capsHeight, int16_t xOffset, const char *s)
{
	int16_t yOffset;
	if (options & OPT_CENTERY)
		yOffset = baseLine - (capsHeight >> 1);
	else if (bottom)
		yOffset = baseLine;
	else
		yOffset = baseLine - capsHeight;
	/*
	if (options & OPT_RIGHTX)
		xOffset = 0;
	else if (options & OPT_CENTERX)
		xOffset >>= 1;
	*/
	EVE_CoCmd_text(phost, x - xOffset, y - yOffset, font, options & ~OPT_CENTERY, s);
}

EVE_HAL_EXPORT void EVE_CoCmd_button(EVE_HalContext *phost, int16_t x, int16_t y, int16_t w, int16_t h, int16_t font, uint16_t options, const char *s, ...)
{
	va_list args;
#if (EVE_SUPPORT_CHIPID >= EVE_BT815)
	uint8_t num;
#endif

#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, CMD_BUTTON, 0))
		return;
#endif

	if (font >= 32)
	{
		if (font != 63)
			eve_printf_debug("Invalid font handle specified: %i\n", (int)font);
		return;
	}

	va_start(args, s);
#if (EVE_SUPPORT_CHIPID >= EVE_BT815) /* OPT_FORMAT not defined in FT8xx chip */
	num = (options & OPT_FORMAT) ? (countArgs(s)) : (0); /* Only check % characters if option OPT_FORMAT is set */
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_BUTTON);
	EVE_Cmd_wr32(phost, (((uint32_t)y << 16) | (x & 0xffff)));
	EVE_Cmd_wr32(phost, (((uint32_t)h << 16) | (w & 0xffff)));
	EVE_Cmd_wr32(phost, (((uint32_t)options << 16) | (font & 0xffff)));
	EVE_Cmd_wrString(phost, s, EVE_CMD_STRING_MAX);
#if (EVE_SUPPORT_CHIPID >= EVE_BT815)
	if (EVE_CHIPID >= EVE_BT815)
	{
		for (uint8_t i = 0; i < num; i++)
			EVE_Cmd_wr32(phost, (uint32_t)va_arg(args, uint32_t));
	}
#endif
	EVE_Cmd_endFunc(phost);
	va_end(args);

#if (EVE_DL_OPTIMIZE)
	phost->DlPrimitive = 0;
#endif
}

EVE_HAL_EXPORT void EVE_CoCmd_keys(EVE_HalContext *phost, int16_t x, int16_t y, int16_t w, int16_t h, int16_t font, uint16_t options, const char *s)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, CMD_KEYS, 0))
		return;
#endif

	if (font >= 32)
	{
		if (font != 63)
			eve_printf_debug("Invalid font handle specified: %i\n", (int)font);
		return;
	}

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_KEYS);
	EVE_Cmd_wr16(phost, x);
	EVE_Cmd_wr16(phost, y);
	EVE_Cmd_wr16(phost, w);
	EVE_Cmd_wr16(phost, h);
	EVE_Cmd_wr16(phost, font);
	EVE_Cmd_wr16(phost, options);
	EVE_Cmd_wrString(phost, s, EVE_CMD_STRING_MAX);
	EVE_Cmd_endFunc(phost);

#if (EVE_DL_OPTIMIZE)
	phost->DlPrimitive = 0;
#endif
}

EVE_HAL_EXPORT void EVE_CoCmd_toggle(EVE_HalContext *phost, int16_t x, int16_t y, int16_t w, int16_t font, uint16_t options, uint16_t state, const char *s, ...)
{
	va_list args;
#if (EVE_SUPPORT_CHIPID >= EVE_BT815)
	uint8_t num;
#endif

#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, CMD_TOGGLE, 0))
		return;
#endif

	if (font >= 32)
	{
		if (font != 63)
			eve_printf_debug("Invalid font handle specified: %i\n", (int)font);
		return;
	}

	va_start(args, s);
#if (EVE_SUPPORT_CHIPID >= EVE_BT815) /* OPT_FORMAT not defined in FT8xx chip */
	num = (options & OPT_FORMAT) ? (countArgs(s)) : (0); //Only check % characters if option OPT_FORMAT is set
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_TOGGLE);
	EVE_Cmd_wr16(phost, x);
	EVE_Cmd_wr16(phost, y);
	EVE_Cmd_wr16(phost, w);
	EVE_Cmd_wr16(phost, font);
	EVE_Cmd_wr16(phost, options);
	EVE_Cmd_wr16(phost, state);
	EVE_Cmd_wrString(phost, s, EVE_CMD_STRING_MAX);
#if (EVE_SUPPORT_CHIPID >= EVE_BT815)
	if (EVE_CHIPID >= EVE_BT815)
	{
		for (uint8_t i = 0; i < num; i++)
			EVE_Cmd_wr32(phost, (uint32_t)va_arg(args, uint32_t));
	}
#endif
	EVE_Cmd_endFunc(phost);
	va_end(args);

#if (EVE_DL_OPTIMIZE)
	phost->DlPrimitive = 0;
#endif
}

EVE_HAL_EXPORT void EVE_CoCmd_number(EVE_HalContext *phost, int16_t x, int16_t y, int16_t font, uint16_t options, int32_t n)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, CMD_NUMBER, 0))
		return;
#endif

	if (font >= 32)
	{
		if (font != 63)
			eve_printf_debug("Invalid font handle specified: %i\n", (int)font);
		return;
	}

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_NUMBER);
	EVE_Cmd_wr16(phost, x);
	EVE_Cmd_wr16(phost, y);
	EVE_Cmd_wr16(phost, font);
	EVE_Cmd_wr16(phost, options);
	EVE_Cmd_wr32(phost, n);
	EVE_Cmd_endFunc(phost);

#if (EVE_DL_OPTIMIZE)
	phost->DlPrimitive = 0;
#endif
}

/* end of file */
