/**
 @file EVE.h
 */
/*
 * ============================================================================
 * History
 * =======
 * Nov 2019		Initial beta for FT81x and FT80x
 * Mar 2020		Updated beta - added BT815/6 commands
 * Mar 2021		Beta with BT817/8 support added
 *
 *
 *
 *
 *
 * (C) Copyright,  Bridgetek Pte. Ltd.
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd
 * ("Bridgetek") subject to the licence terms set out
 * http://www.ftdichip.com/FTSourceCodeLicenceTerms.htm ("the Licence Terms").
 * You must read the Licence Terms before downloading or using the Software.
 * By installing or using the Software you agree to the Licence Terms. If you
 * do not agree to the Licence Terms then do not download or use the Software.
 *
 * Without prejudice to the Licence Terms, here is a summary of some of the key
 * terms of the Licence Terms (and in the event of any conflict between this
 * summary and the Licence Terms then the text of the Licence Terms will
 * prevail).
 *
 * The Software is provided "as is".
 * There are no warranties (or similar) in relation to the quality of the
 * Software. You use it at your own risk.
 * The Software should not be used in, or for, any medical device, system or
 * appliance. There are exclusions of Bridgetek liability for certain types of loss
 * such as: special loss or damage; incidental loss or damage; indirect or
 * consequential loss or damage; loss of income; loss of business; loss of
 * profits; loss of revenue; loss of contracts; business interruption; loss of
 * the use of money or anticipated savings; loss of information; loss of
 * opportunity; loss of goodwill or reputation; and/or loss of, damage to or
 * corruption of data.
 * There is a monetary cap on Bridgetek's liability.
 * The Software may have subsequently been amended by another user and then
 * distributed by that other user ("Adapted Software").  If so that user may
 * have additional licence terms that apply to those amendments. However, Bridgetek
 * has no liability in relation to those amendments.
 * ============================================================================
 */

#ifndef EVE_HEADER_H
#define	EVE_HEADER_H

// for Uint8/16/32 and Int8/16/32 data types.
#include <stdint.h>
// Include the configuration for this instance.

#include <EVE_config.h>
#include "FT8xx.h"

/**
 @brief Initialise EVE API.
 @details Initialise the EVE API layer, HAL layer and MCU-specific hardware
 	 layer.
 */
void EVE_Init(void);

/**
 @brief EVE API: Begin coprocessor list
 @details Starts a coprocessor list. Waits for the coprocessor to be idle
 	 before asserting chip select.
 */
void EVE_LIB_BeginCoProList(void);

/**
 @brief EVE API: End coprocessor list
 @details Ends a coprocessor list. Deasserts chip select.
 */
void EVE_LIB_EndCoProList(void);

/**
 @brief EVE API: Waits for coprocessor list to end
 @details Will poll the coprocessor command list until it has been completed.
 */
void EVE_LIB_AwaitCoProEmpty(void);

/**
 @brief EVE API: Write a buffer to memory mapped RAM
 @details Writes a block of data via SPI to the EVE.
 @param ImgData - Pointer to start of data buffer.
 @param DataSize - Number of bytes in buffer.
 @param DestAddress - 24 bit memory mapped address on EVE.
 */
void EVE_LIB_WriteDataToRAMG(const uint8_t *ImgData, uint32_t DataSize, uint32_t DestAddress);

/**
 @brief EVE API: Read a buffer from memory mapped RAM
 @details Reads a block of data via SPI from the EVE.
 @param ImgData - Pointer to start of receive data buffer.
 @param DataSize - Number of bytes to read (rounded up to be 32-bit aligned).
 @param DestAddress - 24 bit memory mapped address on EVE.
 */
void EVE_LIB_ReadDataFromRAMG(uint8_t *ImgData, uint32_t DataSize, uint32_t SrcAddress);

/**
 @brief EVE API: Write a buffer to the coprocessor command memory
 @details Writes a block of data via SPI to the EVE coprocessor.
 	 This must be part of a coprocessor list. It will typically be called
 	 after a coprocessor command to provide data for the operation.
 	 The data will be added to the coprocessor command list therefore the
 	 write will block on available space in this list.
 @param ImgData - Pointer to start of data buffer.
 @param DataSize - Number of bytes in buffer.
 */
void EVE_LIB_WriteDataToCMD(const uint8_t *ImgData, uint32_t DataSize);

/**
 @brief EVE API: Write a string the coprocessor command memory
 @details Writes a string via SPI to the EVE coprocessor.
 	 This must be part of a coprocessor list. It will typically be called
 	 after a coprocessor command to provide a string for the operation.
 	 The data will be added to the coprocessor command list therefore the
 	 write will block on available space in this list.
 @param ImgData - Pointer to start of data buffer.
 @param DataSize - Number of bytes in buffer.
 */
uint16_t EVE_LIB_SendString(const char* string);

/**
 @brief EVE API: Get properties of an CMD_LOADIMAGE operation
 @details Obtains the details of an image decoded by the CMD_LOADIMAGE
 	 coprocessor command. The properties of the image are taken from
 	 the coprocessor command list.
 @param addr - Pointer to variable to receive the image start address.
 @param width - Pointer to variable to receive the image width.
 @param height - Pointer to variable to receive the image height.
 */
void EVE_LIB_GetProps(uint32_t *addr, uint32_t *width, uint32_t *height);






//##################################################################################################
// Raw command interface write
void EVE_CMD(uint32_t c);
// Graphics instructions
void EVE_CLEAR_COLOR_RGB(uint8_t r, uint8_t g, uint8_t b);
void EVE_CLEAR_COLOR(uint32_t c);
void EVE_CLEAR(uint8_t c, uint8_t s, uint8_t t);
void EVE_COLOR_RGB(uint8_t r, uint8_t g, uint8_t b);
void EVE_COLOR(uint32_t c);
void EVE_VERTEX2F(int16_t x, int16_t y);
void EVE_VERTEX2II(uint16_t x, uint16_t y, uint8_t handle,uint8_t cell);
void EVE_BITMAP_HANDLE(uint8_t handle);
void EVE_BITMAP_SOURCE(int32_t addr);
void EVE_BITMAP_LAYOUT(uint8_t format, uint16_t linestride, uint16_t height);
void EVE_BITMAP_SIZE(uint8_t filter, uint8_t wrapx, uint8_t wrapy, uint16_t width, uint16_t height);
void EVE_CELL(uint8_t cell);
void EVE_TAG(uint8_t s);
void EVE_ALPHA_FUNC(uint8_t func, uint8_t ref);
void EVE_STENCIL_FUNC(uint8_t func, uint8_t ref, uint8_t mask);
void EVE_BLEND_FUNC(uint8_t src, uint8_t dst);
void EVE_STENCIL_OP(uint8_t sfail, uint8_t spass);
void EVE_POINT_SIZE(uint16_t size);
void EVE_LINE_WIDTH(uint16_t width);
void EVE_CLEAR_COLOR_A(uint8_t alpha);
void EVE_COLOR_A(uint8_t alpha);
void EVE_CLEAR_STENCIL(uint8_t s);
void EVE_CLEAR_TAG(uint8_t s);
void EVE_STENCIL_MASK(uint8_t mask);
void EVE_TAG_MASK(uint8_t mask);
void EVE_SCISSOR_XY(uint16_t x, uint16_t y);
void EVE_SCISSOR_SIZE(uint16_t width, uint16_t height);
void EVE_CALL(uint16_t dest);
void EVE_JUMP(uint16_t dest);
void EVE_BEGIN(uint8_t prim);
void EVE_COLOR_MASK(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void EVE_END(void);
void EVE_SAVE_CONTEXT(void);
void EVE_RESTORE_CONTEXT(void);
void EVE_RETURN(void);
void EVE_MACRO(uint8_t m);
void EVE_DISPLAY(void);









// Co-Processor Widgets
//void EVE_CMD_TEXT(int16_t x, int16_t y, int16_t font, uint16_t options, const char* string);
//void EVE_CMD_BUTTON(int16_t x, int16_t y, int16_t w, int16_t h, int16_t font, uint16_t options, const char* string);
void EVE_CMD_KEYS(int16_t x, int16_t y, int16_t w, int16_t h, int16_t font, uint16_t options, const char* string);
void EVE_CMD_NUMBER(int16_t x, int16_t y, int16_t font, uint16_t options, int32_t n);
void EVE_CMD_LOADIDENTITY(void);
//void EVE_CMD_TOGGLE(int16_t x, int16_t y, int16_t w, int16_t font, uint16_t options, uint16_t state, const char* string);
void EVE_CMD_GAUGE(int16_t x, int16_t y, int16_t r, uint16_t options, uint16_t major, uint16_t minor, uint16_t val, uint16_t range);
void EVE_CMD_REGREAD(uint32_t ptr, uint32_t result);
void EVE_CMD_GETPROPS(uint32_t ptr, uint32_t w, uint32_t h);
void EVE_CMD_MEMCPY(uint32_t dest, uint32_t src, uint32_t num);
void EVE_CMD_SPINNER(int16_t x, int16_t y, uint16_t style, uint16_t scale);
void EVE_CMD_BGCOLOR(uint32_t c);
void EVE_CMD_SWAP(void);
void EVE_CMD_INFLATE(uint32_t ptr);
void EVE_CMD_TRANSLATE(int32_t tx, int32_t ty);
void EVE_CMD_STOP(void);
void EVE_CMD_SLIDER(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t options, uint16_t val, uint16_t range);
void EVE_BITMAP_TRANSFORM_A(long a);
void EVE_BITMAP_TRANSFORM_B(long b);
void EVE_BITMAP_TRANSFORM_C(long c);
void EVE_BITMAP_TRANSFORM_D(long d);
void EVE_BITMAP_TRANSFORM_E(long e);
void EVE_BITMAP_TRANSFORM_F(long f);
void EVE_CMD_INTERRUPT(uint32_t ms);
void EVE_CMD_FGCOLOR(uint32_t c);
void EVE_CMD_ROTATE(int32_t a);
void EVE_CMD_MEMWRITE(uint32_t ptr, uint32_t num);
void EVE_CMD_SCROLLBAR(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t options, uint16_t val, uint16_t size, uint16_t range);
void EVE_CMD_GETMATRIX(int32_t a, int32_t b, int32_t c, int32_t d, int32_t e, int32_t f);
void EVE_CMD_SKETCH(int16_t x, int16_t y, uint16_t w, uint16_t h, uint32_t ptr, uint16_t format);
void EVE_CMD_MEMSET(uint32_t ptr, uint32_t value, uint32_t num);
void EVE_CMD_GRADCOLOR(uint32_t c);
void EVE_CMD_BITMAP_TRANSFORM(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t tx0, int32_t ty0, int32_t tx1, int32_t ty1, int32_t tx2, int32_t ty2, uint16_t result);
void EVE_CMD_CALIBRATE(uint32_t result);
void EVE_CMD_SETFONT(uint32_t font, uint32_t ptr);
void EVE_CMD_LOGO(void);
void EVE_CMD_APPEND(uint32_t ptr, uint32_t num);
void EVE_CMD_MEMZERO(uint32_t ptr, uint32_t num);
void EVE_CMD_SCALE(int32_t sx, int32_t sy);
void EVE_CMD_CLOCK(int16_t x, int16_t y, int16_t r, uint16_t options, uint16_t h, uint16_t m, uint16_t s, uint16_t ms);
void EVE_CMD_GRADIENT(int16_t x0, int16_t y0, uint32_t rgb0, int16_t x1, int16_t y1, uint32_t rgb1);
void EVE_CMD_SETMATRIX(void);
void EVE_CMD_TRACK(int16_t x, int16_t y, int16_t w, int16_t h, int16_t tag);
void EVE_CMD_GETPTR(uint32_t result);
void EVE_CMD_PROGRESS(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t options, uint16_t val, uint16_t range);
void EVE_CMD_COLDSTART(void);
void EVE_CMD_DIAL(int16_t x, int16_t y, int16_t r, uint16_t options, uint16_t val);
void EVE_CMD_LOADIMAGE(uint32_t ptr, uint32_t options);
void EVE_CMD_DLSTART(void);
void EVE_CMD_SNAPSHOT(uint32_t ptr);
void EVE_CMD_SCREENSAVER(void);
void EVE_CMD_MEMCRC(uint32_t ptr, uint32_t num, uint32_t result);
uint8_t COUNT_ARGS(const char* string);
void EVE_CMD_TEXT(int16_t x, int16_t y, int16_t font, uint16_t options, const char* string, ...);
void EVE_CMD_BUTTON(int16_t x, int16_t y, int16_t w, int16_t h, int16_t font, uint16_t options, const char* string, ...);
void EVE_CMD_TOGGLE(int16_t x, int16_t y, int16_t w, int16_t font, uint16_t options, uint16_t state, const char* string, ...);





#if defined (EVE2_ENABLE) || defined (EVE3_ENABLE) || defined (EVE4_ENABLE)
void EVE_VERTEX_FORMAT(uint8_t frac);
void EVE_BITMAP_LAYOUT_H(uint8_t linestride, uint8_t height);
void EVE_BITMAP_SIZE_H(uint8_t width, uint8_t height);
void EVE_PALETTE_SOURCE(uint32_t addr);
void EVE_VERTEX_TRANSLATE_X(uint32_t x);
void EVE_VERTEX_TRANSLATE_Y(uint32_t y);
void EVE_NOP(void);
// ---------------   CO-PRO   -------------
void EVE_CMD_SETROTATE(uint32_t r);
void EVE_CMD_SETFONT2(uint32_t font, uint32_t ptr, uint32_t firstchar);
void EVE_CMD_SNAPSHOT2(uint32_t fmt, uint32_t ptr, int16_t x, int16_t y, int16_t w, int16_t h);
void EVE_CMD_MEDIAFIFO(uint32_t ptr, uint32_t size);
void EVE_CMD_INT_SWLOADIMAGE(uint32_t ptr, uint32_t options);
void EVE_CMD_SYNC(void);
void EVE_CMD_ROMFONT(uint32_t font, uint32_t romslot);
void EVE_CMD_PLAYVIDEO(uint32_t options);
void EVE_CMD_VIDEOFRAME(uint32_t dst, uint32_t ptr);
void EVE_CMD_VIDEOSTART(void);
void EVE_CMD_SETBASE(uint32_t eve_base);
void EVE_CMD_SETBITMAP(uint32_t source, uint16_t fmt, uint16_t w, uint16_t h);
void EVE_CMD_SETSCRATCH(uint32_t handle);
#endif



#if defined (EVE3_ENABLE) || defined (EVE4_ENABLE)
void EVE_CMD_FILLWIDTH(uint32_t s);
void EVE_CMD_NOP();
void EVE_CMD_GETPOINT(int16_t x, int16_t y, uint32_t sx, uint32_t sy);
void EVE_CMD_INFLATE2(uint32_t ptr, uint32_t options);
void EVE_CMD_ROTATEAROUND(int32_t x, int32_t y, int32_t a, int32_t s);
void EVE_CMD_FLASHERASE();
void EVE_CMD_FLASHWRITEEXT(uint32_t dest, uint32_t num, uint8_t *data);
void EVE_CMD_FLASHWRITE(uint32_t ptr, uint32_t num);
void EVE_CMD_FLASHUPDATE(uint32_t dest, uint32_t src, uint32_t num);
void EVE_CMD_FLASHREAD(uint32_t dest, uint32_t src, uint32_t num);
void EVE_CMD_FLASHSOURCE(uint32_t ptr);
void EVE_CMD_FLASHSPITX(uint32_t num);
void EVE_CMD_FLASHFAST(uint32_t result);
void EVE_CMD_FLASHSPIRX(uint32_t ptr, uint32_t num);
void EVE_CMD_FLASHATTACH();
void EVE_CMD_FLASHDETATCH();
void EVE_CMD_FLASHSPIDESEL();
void EVE_CMD_CLEARCACHE();
void EVE_CMD_INTRAMSHARED(uint32_t ptr);
void EVE_CMD_SHA1(uint32_t src, uint32_t num, uint32_t hash);
void EVE_CMD_RESETFONTS();
void EVE_CMD_ANIMSTART(int32_t ch, uint32_t aoptr, uint32_t loop);
void EVE_CMD_GRADIENTA(int16_t x0, int16_t y0, uint32_t argb0, int16_t x1, int16_t y1, uint32_t argb1);
void EVE_CMD_ANIMSTOP(int32_t ch);
void EVE_CMD_ANIMXY(int32_t ch, int16_t x, int16_t y);
void EVE_CMD_ANIMDRAW(int32_t ch);
void EVE_CMD_ANIMFRAME(int16_t x, int16_t y, uint32_t aoptr, uint32_t frame);
void EVE_CMD_APPENDF(uint32_t ptr, uint32_t num);
void EVE_CMD_VIDEOSTARTF();
#endif


#if defined (EVE2_ENABLE)
//void EVE_CMD_TEXT(int16_t x, int16_t y, int16_t font, uint16_t options, const char* string);
//void EVE_CMD_BUTTON(int16_t x, int16_t y, int16_t w, int16_t h, int16_t font, uint16_t options, const char* string);
//void EVE_CMD_TOGGLE(int16_t x, int16_t y, int16_t w, int16_t font, uint16_t options, uint16_t state, const char* string);
void EVE_CMD_CSKETCH(int16_t x, int16_t y, uint16_t w, uint16_t h, uint32_t ptr, uint16_t format, uint16_t freq);
#endif




#if defined (EVE4_ENABLE)
void EVE_CMD_ANIMFRAMERAM(int16_t x, int16_t y, uint32_t aoptr, uint32_t frame );
void EVE_CMD_ANIMSTARTRAM(int32_t ch, uint32_t aoptr, uint32_t loop);
void EVE_CMD_RUNANIM(uint32_t waitmask, uint32_t play);
void EVE_CMD_APILEVEL(uint32_t level);
void EVE_CMD_CALIBRATESUB(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t result);
void EVE_CMD_TESTCARD();
void EVE_CMD_WAIT(uint32_t us);
void EVE_CMD_NEWLIST(uint32_t a);
void EVE_CMD_ENDLIST();
void EVE_CMD_CALLLIST(uint32_t a);
void EVE_CMD_RETURN();
void EVE_CMD_FONTCACHE(uint32_t font, int32_t ptr, uint32_t num);
void EVE_CMD_FONTCACHEQUERY(uint32_t total, int32_t used);
void EVE_CMD_GETIMAGE(uint32_t source, uint32_t fmt, uint32_t w, uint32_t h, uint32_t palette);
void EVE_CMD_HSF(uint32_t w );
void EVE_CMD_PCLKFREQ(uint32_t ftarget, int32_t rounding, uint32_t factual);
#endif

#endif	/* EVE_HEADER_H */
