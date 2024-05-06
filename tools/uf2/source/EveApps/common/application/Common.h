/**
 * @file Common.h
 * @brief Common functions
 *
 * @author Tuan Nguyen <tuan.nguyen@brtchip.com>
 *
 * @date 2019
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

#ifndef COMMON_H_
#define COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "EVE_Hal.h"

#include "FT_Gpu.h" /* Compatibility */
#include "Gpu.h" /* Compatibility */

#include "Common.h"
#include "FileIo.h"
#include "FileTransfer.h"
#include "Gesture.h"
#include "Logo.h"
#include "Maths.h"
#include "Image.h"
#include "FlashHelper.h"

// Fat32 - 3rd party library to read SDcard from FT9X platform
#if defined(FT900_PLATFORM) || defined(FT93X_PLATFORM)
#include "ff.h"
#endif

/// Alignment
#define __ALIGN_MASK(x,mask)                (((x)+(mask))&~(mask))
#define ALIGN(x,a)                          __ALIGN_MASK(x, a - 1)

/// Flash address
#define FLASH_ADDRESS                       (0x800000)
#define ATFLASH(x)                          (FLASH_ADDRESS | x / 32)
#define VP(x)                               ((x) * VertextPrecision)

extern uint32_t VertextFormat;
extern uint32_t VertextPrecision;

/** macro: PRINTF_LEVEL
 *    range: 0-3
 *    0: Disable printf
 *    1: Only APP_INF messages and printf function calls
 *    2: Only APP_ERR messages
 *    3: APP_ERR messages and APP_DBG messages
 */
#define PRINTF_LEVEL                        3
#define APP_PRINTF_INFO(M, ...)             printf("[INF] " M "\r\n", ##__VA_ARGS__)
#define APP_PRINTF_ERROR(M, ...)            printf("[ERROR] %s:%d: " M "\r\n", __func__, __LINE__, ##__VA_ARGS__)
#define APP_PRINTF_DEBUG(M, ...)            printf("[DBG] %s:%s:%d: " M "\r\n", __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#if   PRINTF_LEVEL == 1
#define APP_INF                             APP_PRINTF_INFO
#define APP_ERR(M,...)                      /* nothing */
#define APP_DBG(M,...)                      /* nothing */
#elif PRINTF_LEVEL == 2
#define APP_INF                             APP_PRINTF_INFO
#define APP_ERR                             APP_PRINTF_ERROR
#define APP_DBG(M,...)                      /* nothing */
#elif PRINTF_LEVEL == 3
#define APP_INF                             APP_PRINTF_INFO
#define APP_ERR                             APP_PRINTF_ERROR
#define APP_DBG                             APP_PRINTF_DEBUG
#else
#define APP_INF(M,...)                      /* nothing */
#define APP_ERR(M,...)                      /* nothing */
#define APP_DBG(M,...)                      /* nothing */
#define printf(M,...)                       /* nothing */
#endif
#define APP_DBG_H2(x)                        APP_DBG(#x ": 0x%02x", (x))
#define APP_DBG_H8(x)                        APP_DBG(#x ": 0x%08x", (x))
#define APP_DBG_D(x)                        APP_DBG(#x ": %d"    , (x))
#define APP_DBG_U(x)                        APP_DBG(#x ": %u"    , (x))
#define APP_DBG_F8(x)                       APP_DBG(#x ": 0x%08f", (x))
#define APP_DBG_F2(x)                       APP_DBG(#x ": 0x%02f", (x))
#define APP_DBG_S(x)                        APP_DBG(#x ": %s"    , (x))
#define INPUT_YES                           1
#define INPUT_NO                            0

extern char textbuffer[1000];
#define Draw_Text_Format(phost, M, ...)          \
{                                                    \
	sprintf(textbuffer, M, ##__VA_ARGS__);      \
	Draw_Text(phost, textbuffer);     \
}

#define Draw_Text_Format2(phost, ms, M, ...)     \
{                                                    \
	sprintf(textbuffer, M, ##__VA_ARGS__);      \
	Draw_Text2(phost, textbuffer, ms);\
}


void Gpu_Init(EVE_HalContext *phost);
void Gpu_Release(EVE_HalContext *phost);
uint32_t Get_SystemClock(EVE_HalContext *phost);
uint32_t Draw_Point(EVE_HalContext *phost, uint32_t Rx, uint32_t Ry, uint32_t R);
#if defined(BT817_ENABLE) || defined(BT817A_ENABLE)|| defined(BT818_ENABLE)
uint32_t Draw_Image(EVE_HalContext *phost, const char *filePath, uint32_t format);
#endif

void Calibration_Restore(EVE_HalContext *phost);
void Calibration_Save(EVE_HalContext *phost);
void Display_Start(EVE_HalContext *phost);
void Display_StartColor(EVE_HalContext* phost, uint8_t* bgColor, uint8_t* textColor);
void Display_End(EVE_HalContext *phost);
void Draw_TextColor(EVE_HalContext* phost, const char8_t* str, uint8_t* bgColor, uint8_t* textColor);
void Draw_Text(EVE_HalContext *phost, const char8_t *str);
void Draw_Text2(EVE_HalContext* phost, const char8_t* str, int ms);
void Play_MuteSound(EVE_HalContext *phost);
void Get_VC1Dump(EVE_HalContext *phost, const char *filename);
void Play_Sound(EVE_HalContext *phost, uint8_t sound, uint8_t vol, uint8_t midi);
void Setup_Precision(uint32_t format);
uint8_t Show_Diaglog_YesNo(EVE_HalContext *phost, const uint8_t* title, const uint8_t *msg);
void Show_Diaglog_Info(EVE_HalContext *phost, const uint8_t *msg);
uint8_t isSDcardDetected();
void hintScreen(EVE_HalContext *phost, const uint8_t *msg);
void Flash_Init(EVE_HalContext* phost, const uint8_t *filePath, const uint8_t *fileName);
void WelcomeScreen(Gpu_Hal_Context_t* phost, char* info[]);

#endif /* COMMON_H_ */
