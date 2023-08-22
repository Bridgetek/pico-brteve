/**
 * @file Image.c
 * @brief Image helper functions
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

#include "Platform.h"
#include "EVE_CoCmd.h"

#include "Common.h"
#include "Image.h"
#include "EVE_CoCmd.h"

/// Status stores
static float mangle = 0;
static uint32_t mrootx = 0;
static uint32_t mrooty = 0;
static int misRotate = 0;

/// Configuration
#define USE_COCMD_SETBITMAP 1
#define ENABLE_ROTATEAROUND 1
/// Graphics definitions
#define MAX_ANGLE           360
#define CIRCLE_MAX          65536

/// Graphics unity
#define ANGLE(x)            (x * 100 * CIRCLE_MAX / (MAX_ANGLE * 100))

#ifndef VP
#define VP(x) (x*16)
#endif

static uint32_t getformatW(uint32_t format){
	switch (format){
	case COMPRESSED_RGBA_ASTC_4x4_KHR  : return 4 ;
	case COMPRESSED_RGBA_ASTC_5x4_KHR  : return 5 ;
	case COMPRESSED_RGBA_ASTC_5x5_KHR  : return 5 ;
	case COMPRESSED_RGBA_ASTC_6x5_KHR  : return 6 ;
	case COMPRESSED_RGBA_ASTC_6x6_KHR  : return 6 ;
	case COMPRESSED_RGBA_ASTC_8x5_KHR  : return 8 ;
	case COMPRESSED_RGBA_ASTC_8x6_KHR  : return 8 ;
	case COMPRESSED_RGBA_ASTC_8x8_KHR  : return 8 ;
	case COMPRESSED_RGBA_ASTC_10x5_KHR : return 10;
	case COMPRESSED_RGBA_ASTC_10x6_KHR : return 10;
	case COMPRESSED_RGBA_ASTC_10x8_KHR : return 10;
	case COMPRESSED_RGBA_ASTC_10x10_KHR: return 10;
	case COMPRESSED_RGBA_ASTC_12x10_KHR: return 12;
	case COMPRESSED_RGBA_ASTC_12x12_KHR: return 12;
	default:
		return 4;
	}
}

static uint32_t getformatH(uint32_t format){
	switch (format){
	case COMPRESSED_RGBA_ASTC_4x4_KHR  : return 4;
	case COMPRESSED_RGBA_ASTC_5x4_KHR  : return 4;
	case COMPRESSED_RGBA_ASTC_5x5_KHR  : return 5;
	case COMPRESSED_RGBA_ASTC_6x5_KHR  : return 5;
	case COMPRESSED_RGBA_ASTC_6x6_KHR  : return 6;
	case COMPRESSED_RGBA_ASTC_8x5_KHR  : return 5;
	case COMPRESSED_RGBA_ASTC_8x6_KHR  : return 6;
	case COMPRESSED_RGBA_ASTC_8x8_KHR  : return 8;
	case COMPRESSED_RGBA_ASTC_10x5_KHR : return 5;
	case COMPRESSED_RGBA_ASTC_10x6_KHR : return 6;
	case COMPRESSED_RGBA_ASTC_10x8_KHR : return 8;
	case COMPRESSED_RGBA_ASTC_10x10_KHR: return 10;
	case COMPRESSED_RGBA_ASTC_12x10_KHR: return 10;
	case COMPRESSED_RGBA_ASTC_12x12_KHR: return 12;
	default:
		return 4;
	}
}

static void setupBitmap(EVE_HalContext *phost, Img_t *image) {
	uint32_t address = image->addressRamg;

#if USE_COCMD_SETBITMAP
	if (image->isFlash) {
		address = ATFLASH(image->addressFlash);
	}
	EVE_CoCmd_setBitmap(phost, address, image->bitmapLayout, image->w, image->h);
#else
	uint32_t frameSize = image->w * 2;
	uint32_t frameW = image->w;
	uint32_t frameH = image->h;
	
	if (image->w < image->h) {
		frameSize = image->h * 2;
	}
	if (misRotate) {
		frameW = frameH = frameSize;
	}

	// Only support ASTC
	uint16_t lh, linestride;
	const uint16_t atscBlockSize = 16; /// ASTC blocks represent between 4x4 to 12x12 pixels. Each block is 16 bytes in memory.
	uint16_t formatW = getformatW(image->bitmapLayout), formatH = getformatH(image->bitmapLayout);

	linestride = atscBlockSize * ((image->w + formatW - 1) / formatW);
	lh = (image->h + formatH - 1) / formatH;

	App_WrCoCmd_Buffer(phost, BITMAP_SOURCE(address));
	App_WrCoCmd_Buffer(phost, BITMAP_LAYOUT(image->bitmapLayout, linestride, lh));
	App_WrCoCmd_Buffer(phost, BITMAP_LAYOUT_H(linestride >> 10, lh >> 9));
	App_WrCoCmd_Buffer(phost, BITMAP_EXT_FORMAT(image->extFormat));
	App_WrCoCmd_Buffer(phost, BITMAP_SIZE(NEAREST, BORDER, BORDER, frameW, frameH));
	App_WrCoCmd_Buffer(phost, BITMAP_SIZE_H(frameW >> 9, frameH >> 9));
#endif
}

static void rotateBitmap(EVE_HalContext *phost, Img_t *image) {
	uint16_t translateX = 1, translateY = image->h - image->w;

	if (image->w < image->h) {
		translateX = image->h - image->w;
		translateY = 1;
	}
#if ENABLE_ROTATEAROUND
	EVE_CoCmd_loadIdentity(phost);
	EVE_CoCmd_rotateAround(phost, mrootx, mrooty, (int32_t)ANGLE(mangle), CIRCLE_MAX);
	EVE_CoCmd_setMatrix(phost);
#else
	// Use rotate and translate
	EVE_CoCmd_loadIdentity(phost);
	EVE_CoCmd_translate(phost, (translateX + mrootx) * CIRCLE_MAX,
			(translateY + mrooty) * CIRCLE_MAX);
	EVE_CoCmd_rotate(phost, ANGLE(mangle));
	EVE_CoCmd_translate(phost, -(0 + mrootx) * CIRCLE_MAX,
			-(0 + mrooty) * CIRCLE_MAX);
	EVE_CoCmd_setMatrix(phost);
#endif
}

static void drawBitmap(EVE_HalContext *phost, Img_t *image) {
	if (image->tag != 0) { // set TAG if the image need to process gesture events
		App_WrCoCmd_Buffer(phost, TAG(image->tag));
	} else {
		App_WrCoCmd_Buffer(phost, TAG(0));
	}
	App_WrCoCmd_Buffer(phost, BEGIN(BITMAPS));
	App_WrCoCmd_Buffer(phost, VERTEX2F(VP( image->x), VP(image->y)));
}

int Image_Setup_Rotate(float angle, uint32_t rootx, uint32_t rooty) {
	misRotate = 1;
	mangle = angle;
	mrootx = rootx;
	mrooty = rooty;

	return 1;
}

int Image_Copy_To_RamG(EVE_HalContext *phost, Img_t *image, uint32_t isRestart) {
	static uint32_t ramgAddr = RAM_G;

	if (isRestart == 1) {
		ramgAddr = 0;
	}

	image->isFlash = 0;
	image->addressRamg = ramgAddr;

	EVE_CoCmd_flashRead(phost, ramgAddr, image->addressFlash, image->size);

	ramgAddr += image->size;

	return 1;
}

int Image_Copy_To_RamG_And_Draw_Image(EVE_HalContext *phost, Img_t *image, uint32_t isRestart) {
	static uint32_t ramgAddr = RAM_G;

	if (isRestart == 1) {
		ramgAddr = 0;
	}

	image->isFlash = 0;
	image->addressRamg = ramgAddr;

	EVE_CoCmd_flashRead(phost, ramgAddr, image->addressFlash, image->size);
	Image_Draw(phost, image);

	ramgAddr += image->size;

	return 1;
}

int Image_Draw(EVE_HalContext *phost, Img_t *image) {
	App_WrCoCmd_Buffer(phost, SAVE_CONTEXT());
	setupBitmap(phost, image);
	if (misRotate) {
		rotateBitmap(phost, image);
		misRotate = 0;
	}
	drawBitmap(phost, image);

	App_WrCoCmd_Buffer(phost, RESTORE_CONTEXT());

	return 1;
}

int Image_Draw_From_RAM_G(EVE_HalContext *phost, uint32_t address, uint32_t x, uint32_t y, uint32_t w, uint32_t h, 
	uint16_t bitmapLayout, uint32_t extFormat, uint8_t tag) {
	Img_t image;
	image.isFlash = 0;
	image.addressRamg = address;
	image.bitmapLayout = bitmapLayout;
	image.extFormat = extFormat;
	image.x = x;
	image.y = y;
	image.h = h;
	image.w = w;
	image.index = 0;
	image.tag = tag;
	return Image_Draw(phost, &image);
}
int Image_Draw_From_Flash(EVE_HalContext *phost, uint32_t address, uint32_t x, uint32_t y, uint32_t w, uint32_t h, 
	uint16_t bitmapLayout, uint32_t extFormat, uint8_t tag) {
	Img_t image;
	image.isFlash = 1;
	image.addressFlash = address;
	image.bitmapLayout = bitmapLayout;
	image.extFormat = extFormat;
	image.x = x;
	image.y = y;
	image.w = w;
	image.h = h;
	image.index = 0;
	image.tag = tag;
	return Image_Draw(phost, &image);
}

