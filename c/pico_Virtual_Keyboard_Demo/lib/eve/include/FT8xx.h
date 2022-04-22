/**
 @file FT8xx.h
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

#ifndef _FT8XX_H_
#define	_FT8XX_H_

/* Definitions used for target device selection */
#define FT800 8
#define FT801 9
#define FT810 10
#define FT811 11
#define FT812 12
#define FT813 13
#define BT815 15
#define BT816 16
#define BT817 17
#define BT818 18

/* For capacitive versions */
#define CTOUCH_MODE_COMPATIBILITY 1
#define CTOUCH_MODE_EXTENDED 0

#undef EVE1_ENABLE
#undef EVE2_ENABLE
#undef EVE3_ENABLE
#undef EVE4_ENABLE

#if (FT8XX_TYPE == FT800)
#define EVE1_ENABLE

#elif (FT8XX_TYPE == FT801)
#define EVE1_ENABLE

#elif (FT8XX_TYPE == FT810)
#define EVE2_ENABLE

#elif (FT8XX_TYPE == FT811)
#define EVE2_ENABLE

#elif (FT8XX_TYPE == FT812)
#define EVE2_ENABLE

#elif (FT8XX_TYPE == FT813)
#define EVE2_ENABLE

#elif (FT8XX_TYPE == BT815)
#define EVE3_ENABLE

#elif (FT8XX_TYPE == BT816)
#define EVE3_ENABLE

#elif (FT8XX_TYPE == BT817)
#define EVE4_ENABLE

#elif (FT8XX_TYPE == BT818)
#define EVE4_ENABLE

#else
#warning FT8XX_TYPE definition not recognised.
#endif


#if defined EVE1_ENABLE

/* For FT81x enable the switch in platform.h file */
/* Lower boundary of trimming */
//#define LOW_FREQ_BOUND  47040000L//98% of 48Mhz

#define EVE_RAM_G                	0x0UL			// RAM_G
#define EVE_RAM_G_SIZE           	(256*1024L)
#define EVE_ROM_CHIPID           	0x0C0000UL		// Chip ID
#define EVE_ROM_FONT			 	0x0BB23CUL		// Font table and bitmap
#define EVE_ROMFONT_TABLEADDRESS    0x0FFFFCUL		// Font table pointer address
#define EVE_RAM_DL               	0x100000UL		// RAM_DL
#define EVE_RAM_DL_SIZE          	(8*1024L)
#define EVE_RAM_PAL              	0x102000UL		// Palette RAM
#define EVE_RAM_REG              	0x102400UL		// Registers
#define EVE_RAM_CMD              	0x108000UL		// RAM_CMD
#define EVE_RAM_CMD_SIZE         	(4*1024L)

#define EVE_REG_PCLK             	0x10246cUL
#define EVE_REG_PCLK_POL         	0x102468UL
#define EVE_REG_CSPREAD          	0x102464UL
#define EVE_REG_SWIZZLE          	0x102460UL
#define EVE_REG_DITHER           	0x10245cUL
#define EVE_REG_OUTBITS          	0x102458UL
#define EVE_REG_ROTATE           	0x102454UL
#define EVE_REG_VSYNC1           	0x10244cUL
#define EVE_REG_VSYNC0           	0x102448UL
#define EVE_REG_VSIZE            	0x102444UL
#define EVE_REG_VOFFSET          	0x102440UL
#define EVE_REG_VCYCLE           	0x10243cUL
#define EVE_REG_HSYNC1           	0x102438UL
#define EVE_REG_HSYNC0           	0x102434UL
#define EVE_REG_HSIZE            	0x102430UL
#define EVE_REG_HOFFSET          	0x10242cUL
#define EVE_REG_HCYCLE           	0x102428UL

#define EVE_REG_TAP_MASK         	0x102424UL
#define EVE_REG_TAP_CRC          	0x102420UL

#define EVE_REG_DLSWAP           	0x102450UL

#define EVE_REG_TAG              	0x102478UL
#define EVE_REG_TAG_Y            	0x102474UL
#define EVE_REG_TAG_X            	0x102470UL

//---------------------------------------------------
// FT800 specific

#define EVE_REG_TOUCH_ADC_MODE   	0x1024f4UL
#define EVE_REG_TOUCH_SCREEN_XY  	0x102510UL
#define EVE_REG_TOUCH_RAW_XY     	0x102508UL
#define EVE_REG_TOUCH_DIRECT_XY  	0x102574UL
#define EVE_REG_TOUCH_DIRECT_Z1Z2 	0x102578UL
#define EVE_REG_ANALOG           	0x102538UL
#define EVE_REG_TOUCH_RZ         	0x10250cUL
#define EVE_REG_TOUCH_TRANSFORM_A 	0x10251cUL
#define EVE_REG_TOUCH_TRANSFORM_B 	0x102520UL
#define EVE_REG_TOUCH_TRANSFORM_C 	0x102524UL
#define EVE_REG_TOUCH_TRANSFORM_D 	0x102528UL
#define EVE_REG_TOUCH_TRANSFORM_E 	0x10252cUL
#define EVE_REG_TOUCH_TRANSFORM_F 	0x102530UL
#define EVE_REG_TOUCH_TAG        	0x102518UL
// FT801 specific mapped to same addresses as FT800 above
#define EVE_REG_CTOUCH_EXTENDED 	0x1024f4UL
#define EVE_REG_CTOUCH_TOUCH0_XY 	0x102510UL
#define EVE_REG_CTOUCH_RAW_XY 		0x102508UL	// FT801 Compatibility mode
#define EVE_REG_CTOUCH_TOUCH1_XY 	0x102508UL	// FT801 Extended mode
#define EVE_REG_CTOUCH_TOUCH2_XY 	0x102574UL	// FT801 Extended mode
#define EVE_REG_CTOUCH_TOUCH3_XY 	0x102578UL	// FT801 Extended mode
#define EVE_REG_CTOUCH_TOUCH4_X 	0x102538UL	// FT801 Extended mode
#define EVE_REG_CTOUCH_TOUCH4_Y 	0x10250cUL	// FT801 Extended mode
#define EVE_REG_CTOUCH_TRANSFORM_A 	0x10251cUL
#define EVE_REG_CTOUCH_TRANSFORM_B 	0x102520UL
#define EVE_REG_CTOUCH_TRANSFORM_C 	0x102524UL
#define EVE_REG_CTOUCH_TRANSFORM_D 	0x102528UL
#define EVE_REG_CTOUCH_TRANSFORM_E 	0x10252cUL
#define EVE_REG_CTOUCH_TRANSFORM_F 	0x102530UL
#define EVE_REG_CTOUCH_TAG        	0x102518UL

//---------------------------------------------------

#define EVE_REG_TOUCH_TAG_XY     	0x102514UL
#define EVE_REG_TOUCH_RZTHRESH  	0x102504UL
#define EVE_REG_TOUCH_OVERSAMPLE 	0x102500UL
#define EVE_REG_TOUCH_SETTLE     	0x1024fcUL
#define EVE_REG_TOUCH_CHARGE     	0x1024f8UL
#define EVE_REG_TOUCH_MODE       	0x1024f0UL

#define EVE_REG_PLAY             	0x102488UL
#define EVE_REG_SOUND            	0x102484UL
#define EVE_REG_VOL_SOUND        	0x102480UL
#define EVE_REG_VOL_PB           	0x10247cUL
#define EVE_REG_PLAYBACK_PLAY    	0x1024bcUL
#define EVE_REG_PLAYBACK_LOOP    	0x1024b8UL
#define EVE_REG_PLAYBACK_FORMAT  	0x1024b4UL
#define EVE_REG_PLAYBACK_FREQ    	0x1024b0UL
#define EVE_REG_PLAYBACK_READPTR 	0x1024acUL
#define EVE_REG_PLAYBACK_LENGTH  	0x1024a8UL
#define EVE_REG_PLAYBACK_START   	0x1024a4UL

#define EVE_REG_CMD_DL           	0x1024ecUL
#define EVE_REG_CMD_WRITE        	0x1024e8UL
#define EVE_REG_CMD_READ         	0x1024e4UL

#define EVE_REG_TRACK          		0x109000UL

#define EVE_REG_PWM_DUTY         	0x1024c4UL
#define EVE_REG_PWM_HZ           	0x1024c0UL

#define EVE_REG_INT_MASK         	0x1024a0UL
#define EVE_REG_INT_EN           	0x10249cUL
#define EVE_REG_INT_FLAGS        	0x102498UL
#define EVE_REG_GPIO             	0x102490UL
#define EVE_REG_GPIO_DIR         	0x10248cUL
#define EVE_REG_CPURESET         	0x10241cUL

#define EVE_REG_SCREENSHOT_READ		0x102554UL
#define EVE_REG_SCREENSHOT_BUSY		0x1024D8UL
#define EVE_REG_SCREENSHOT_START    0x102418UL	//
#define EVE_REG_SCREENSHOT_Y        0x102414UL	//
#define EVE_REG_SCREENSHOT_EN		0x102410UL	//

#define EVE_REG_FREQUENCY        	0x10240cUL
#define EVE_REG_CLOCK            	0x102408UL
#define EVE_REG_FRAMES           	0x102404UL
#define EVE_REG_ID               	0x102400UL
#define EVE_REG_TRIM             	0x10256cUL

#define EVE_REG_MACRO_0          	0x1024c8UL
#define EVE_REG_MACRO_1          	0x1024ccUL

#define EVE_REG_ROMSUB_SEL       	0x1024e0UL

#define EVE_ENC_ALPHA_FUNC(func,ref) ((0x9UL << 24)|(((func) & 0x7UL) << 8)|(((ref) & 0xffUL) << 0))
#define EVE_ENC_BEGIN(prim) ((0x1fUL << 24)|(((prim) & 0xFUL) << 0))
#define EVE_ENC_BITMAP_HANDLE(handle) ((0x5UL << 24)|(((handle) & 0x1fUL) << 0))
#define EVE_ENC_BITMAP_LAYOUT(format,linestride,height) ((0x7UL << 24)|(((format) & 0x1fUL) << 19)|(((linestride) & 0x3ffUL) << 9)|(((height) & 0x1ffUL) << 0))
#define EVE_ENC_BITMAP_SIZE(filter,wrapx,wrapy,width,height) ((0x8UL << 24)|(((filter) & 0x1UL) << 20)|(((wrapx) & 0x1UL) << 19)|(((wrapy) & 0x1UL) << 18)|(((width) & 0x1ffUL) << 9)|(((height) & 0x1ffUL) << 0))
#define EVE_ENC_BITMAP_ADDR_MASK 1048575UL
#define EVE_ENC_BITMAP_SOURCE(addr) ((1UL << 24) | ((addr)&EVE_ENC_BITMAP_ADDR_MASK))
#define EVE_ENC_BITMAP_TRANSFORM_A(a) ((0x15UL << 24)|((((uint32_t)(a)) & 0x1FFFFUL) << 0))
#define EVE_ENC_BITMAP_TRANSFORM_B(b) ((0x16UL << 24)|((((uint32_t)(b)) & 0x1FFFFUL) << 0))
#define EVE_ENC_BITMAP_TRANSFORM_C(c) ((0x17UL << 24)|((((uint32_t)(c)) & 0xFFFFFFUL) << 0))
#define EVE_ENC_BITMAP_TRANSFORM_D(d) ((0x18UL << 24)|((((uint32_t)(d)) & 0x1FFFFUL) << 0))
#define EVE_ENC_BITMAP_TRANSFORM_E(e) ((0x19UL << 24)|((((uint32_t)(e)) & 0x1FFFFUL) << 0))
#define EVE_ENC_BITMAP_TRANSFORM_F(f) ((0x1aUL << 24)|((((uint32_t)(f)) & 0xFFFFFFUL) << 0))
#define EVE_ENC_BLEND_FUNC(src,dst) ((0xbUL << 24)|(((src) & 0x7UL) << 3)|(((dst) & 0x7UL) << 0))
#define EVE_ENC_CALL(dest) ((0x1dUL << 24)|(((dest) & 0xFFFFUL) << 0))
#define EVE_ENC_CELL(cell) ((0x6UL << 24)|(((cell) & 0x7fUL) << 0))
#define EVE_ENC_CLEAR_COLOR_A(alpha) ((0xfUL << 24)|(((alpha) & 0xffUL) << 0))
#define EVE_ENC_CLEAR_COLOR_RGB(red,green,blue) ((0x2UL << 24)|(((red) & 0xffUL) << 16)|(((green) & 0xffUL) << 8)|(((blue) & 0xffUL) << 0))
#define EVE_ENC_CLEAR_COLOR(c) ((0x2UL << 24)|(((uint32_t)(c)) & 0x00ffffffUL))
#define EVE_ENC_CLEAR_STENCIL(s) ((0x11UL << 24)|((((uint32_t)(s)) & 0xffUL) << 0))
#define EVE_ENC_CLEAR_TAG(s) ((0x12UL << 24)|((((uint32_t)(s)) & 0xffUL) << 0))
#define EVE_ENC_CLEAR(c,s,t) ((0x26UL << 24)|((((uint32_t)(c)) & 0x1UL) << 2)|((((uint32_t)(s)) & 0x1UL) << 1)|((((uint32_t)(t)) & 0x1UL) << 0))
#define EVE_ENC_COLOR_A(alpha) ((0x10UL << 24)|(((alpha) & 0xffUL) << 0))
#define EVE_ENC_COLOR_MASK(r,g,b,a) ((0x20UL << 24)|((((uint32_t)(r)) & 0x1UL) << 3)|((((uint32_t)(g)) & 0x1UL) << 2)|((((uint32_t)(b)) & 0x1UL) << 1)|((((uint32_t)(a)) & 0x1UL) << 0))
#define EVE_ENC_COLOR_RGB(red,green,blue) ((0x4UL << 24)|(((red) & 0xffUL) << 16)|(((green) & 0xffUL) << 8)|(((blue) & 0xffUL) << 0))
#define EVE_ENC_COLOR(c) ((0x4UL << 24)|(((uint32_t)(c)) & 0x00ffffffUL))
#define EVE_ENC_DISPLAY() ((0x0UL << 24))
#define EVE_ENC_END() ((0x21UL << 24))
#define EVE_ENC_JUMP(dest) ((0x1eUL << 24)|(((dest) & 0xFFFFUL) << 0))
#define EVE_ENC_LINE_WIDTH(width) ((0xeUL << 24)|(((width) & 0xFFFUL) << 0))
#define EVE_ENC_MACRO(m) ((0x25UL << 24)|((((uint32_t)(m)) & 0x1UL) << 0))
#define EVE_ENC_POINT_SIZE(size) ((0xdUL << 24)|(((size) & 0x1FFFUL) << 0))
#define EVE_ENC_RESTORE_CONTEXT() ((0x23UL << 24))
#define EVE_ENC_RETURN() ((0x24UL << 24))
#define EVE_ENC_SAVE_CONTEXT() ((0x22UL << 24))
#define EVE_ENC_SCISSOR_SIZE(width,height) ((0x1cUL << 24)|(((width) & 0x3ffUL) << 10)|(((height) & 0x3ffUL) << 0))
#define EVE_ENC_SCISSOR_XY(x,y) ((0x1bUL << 24)|((((uint32_t)(x)) & 0x1ffUL) << 9)|((((uint32_t)(y)) & 0x1ffUL) << 0))
#define EVE_ENC_STENCIL_FUNC(func,ref,mask) ((0xaUL << 24)|(((func) & 0x7UL) << 16)|(((ref) & 0xffUL) << 8)|(((mask) & 0xffUL) << 0))
#define EVE_ENC_STENCIL_MASK(mask) ((0x13UL << 24)|(((mask) & 0xffUL) << 0))
#define EVE_ENC_STENCIL_OP(sfail,spass) ((0xcUL << 24)|(((sfail) & 0x7UL) << 3)|(((spass) & 0x7UL) << 0))
#define EVE_ENC_TAG_MASK(mask) ((0x14UL << 24)|(((mask) & 0x1UL) << 0))
#define EVE_ENC_TAG(s) ((0x3UL << 24)|((((uint32_t)(s)) & 0xffUL) << 0))
#define EVE_ENC_VERTEX2F(x,y) ((0x1UL << 30)|((((uint32_t)(x)) & 0xffffUL) << 15)|((((uint32_t)(y)) & 0xffffUL) << 0))
#define EVE_ENC_VERTEX2II(x,y,handle,cell) ((0x2UL << 30)|((((uint32_t)(x)) & 0x1ffUL) << 21)|((((uint32_t)(y)) & 0x1ffUL) << 12)|(((handle) & 0x1fUL) << 7)|(((cell) & 0x7fUL) << 0))

#define EVE_ENC_CMD_APPEND           0xffffff1eUL
#define EVE_ENC_CMD_BGCOLOR          0xffffff09UL
#define EVE_ENC_CMD_BITMAP_TRANSFORM 0xffffff21UL
#define EVE_ENC_CMD_BUTTON           0xffffff0dUL
#define EVE_ENC_CMD_CALIBRATE        0xffffff15UL
#define EVE_ENC_CMD_CLOCK            0xffffff14UL
#define EVE_ENC_CMD_COLDSTART        0xffffff32UL
#define EVE_ENC_CMD_CRC              0xffffff03UL
#define EVE_ENC_CMD_CSKETCH    	     0xffffff35UL
#define EVE_ENC_CMD_DIAL             0xffffff2dUL
#define EVE_ENC_CMD_DLSTART          0xffffff00UL
#define EVE_ENC_CMD_EXECUTE          0xffffff07UL
#define EVE_ENC_CMD_FGCOLOR          0xffffff0aUL
#define EVE_ENC_CMD_GAUGE            0xffffff13UL
#define EVE_ENC_CMD_GETMATRIX        0xffffff33UL
#define EVE_ENC_CMD_GETPOINT         0xffffff08UL
#define EVE_ENC_CMD_GETPROPS         0xffffff25UL
#define EVE_ENC_CMD_GETPTR           0xffffff23UL
#define EVE_ENC_CMD_GRADCOLOR        0xffffff34UL
#define EVE_ENC_CMD_GRADIENT         0xffffff0bUL
#define EVE_ENC_CMD_HAMMERAUX        0xffffff04UL
#define EVE_ENC_CMD_IDCT             0xffffff06UL
#define EVE_ENC_CMD_INFLATE          0xffffff22UL
#define EVE_ENC_CMD_INTERRUPT        0xffffff02UL
#define EVE_ENC_CMD_KEYS             0xffffff0eUL
#define EVE_ENC_CMD_LOADIDENTITY     0xffffff26UL
#define EVE_ENC_CMD_LOADIMAGE        0xffffff24UL
#define EVE_ENC_CMD_LOGO             0xffffff31UL
#define EVE_ENC_CMD_MARCH            0xffffff05UL
#define EVE_ENC_CMD_MEMCPY           0xffffff1dUL
#define EVE_ENC_CMD_MEMCRC           0xffffff18UL
#define EVE_ENC_CMD_MEMSET           0xffffff1bUL
#define EVE_ENC_CMD_MEMWRITE         0xffffff1aUL
#define EVE_ENC_CMD_MEMZERO          0xffffff1cUL
#define EVE_ENC_CMD_NUMBER           0xffffff2eUL
#define EVE_ENC_CMD_PROGRESS         0xffffff0fUL
#define EVE_ENC_CMD_REGREAD          0xffffff19UL
#define EVE_ENC_CMD_ROTATE           0xffffff29UL
#define EVE_ENC_CMD_SCALE            0xffffff28UL
#define EVE_ENC_CMD_SCREENSAVER      0xffffff2fUL
#define EVE_ENC_CMD_SCROLLBAR        0xffffff11UL
#define EVE_ENC_CMD_SETFONT          0xffffff2bUL
#define EVE_ENC_CMD_SETMATRIX        0xffffff2aUL
#define EVE_ENC_CMD_SKETCH           0xffffff30UL
#define EVE_ENC_CMD_SLIDER           0xffffff10UL
#define EVE_ENC_CMD_SNAPSHOT         0xffffff1fUL
#define EVE_ENC_CMD_SPINNER          0xffffff16UL
#define EVE_ENC_CMD_STOP             0xffffff17UL
#define EVE_ENC_CMD_SWAP             0xffffff01UL
#define EVE_ENC_CMD_TEXT             0xffffff0cUL
#define EVE_ENC_CMD_TOGGLE           0xffffff12UL
#define EVE_ENC_CMD_TOUCH_TRANSFORM  0xffffff20UL
#define EVE_ENC_CMD_TRACK            0xffffff2cUL
#define EVE_ENC_CMD_TRANSLATE        0xffffff27UL

#define EVE_BEGIN_BITMAPS            0x1UL
#define EVE_BEGIN_EDGE_STRIP_A       0x7UL
#define EVE_BEGIN_EDGE_STRIP_B       0x8UL
#define EVE_BEGIN_EDGE_STRIP_L       0x6UL
#define EVE_BEGIN_EDGE_STRIP_R       0x5UL
#define EVE_BEGIN_LINE_STRIP         0x4UL
#define EVE_BEGIN_LINES              0x3UL
#define EVE_BEGIN_POINTS             0x2UL
#define EVE_BEGIN_RECTS              0x9UL

#define EVE_FORMAT_ARGB1555          0x0UL
#define EVE_FORMAT_ARGB2             0x5UL
#define EVE_FORMAT_ARGB4             0x6UL
#define EVE_FORMAT_BARGRAPH          0xbUL
#define EVE_FORMAT_L1                0x1UL
#define EVE_FORMAT_L4                0x2UL
#define EVE_FORMAT_L8                0x3UL
#define EVE_FORMAT_PALETTED          0x8UL
#define EVE_FORMAT_RGB332            0x4UL
#define EVE_FORMAT_RGB565            0x7UL
#define EVE_FORMAT_TEXT8X8           0x9UL
#define EVE_FORMAT_TEXTVGA           0xaUL

#define EVE_TEST_ALWAYS              0x7UL
#define EVE_TEST_EQUAL               0x5UL
#define EVE_TEST_GEQUAL              0x4UL
#define EVE_TEST_GREATER             0x3UL
#define EVE_TEST_LEQUAL              0x2UL
#define EVE_TEST_LESS                0x1UL
#define EVE_TEST_NEVER               0x0UL
#define EVE_TEST_NOTEQUAL            0x6UL

#define EVE_FILTER_BILINEAR          0x1UL
#define EVE_FILTER_NEAREST           0x0UL

#define EVE_WRAP_BORDER              0x0UL
#define EVE_WRAP_REPEAT              0x1UL

#define EVE_BLEND_ZERO               0x0UL
#define EVE_BLEND_ONE                0x1UL
#define EVE_BLEND_SRC_ALPHA          0x2UL
#define EVE_BLEND_DST_ALPHA          0x3UL
#define EVE_BLEND_ONE_MINUS_SRC_ALPHA    0x4UL
#define EVE_BLEND_ONE_MINUS_DST_ALPHA    0x5UL

#define EVE_STENCIL_DECR             0x4UL
#define EVE_STENCIL_INCR             0x3UL
#define EVE_STENCIL_INVERT           0x5UL
#define EVE_STENCIL_KEEP             0x1UL
#define EVE_STENCIL_REPLACE          0x2UL
#define EVE_STENCIL_ZERO             0x0UL

#define EVE_OPT_3D		             0x0UL
#define EVE_OPT_RGB565	             0x0UL
#define EVE_OPT_MONO                 0x1UL
#define EVE_OPT_NODL                 0x2UL
#define EVE_OPT_FLAT                 0x100UL
#define EVE_OPT_SIGNED               0x100UL

#define EVE_OPT_CENTERX              0x200UL
#define EVE_OPT_CENTERY              0x400UL
#define EVE_OPT_CENTER               0x600UL
#define EVE_OPT_RIGHTX               0x800UL
#define EVE_OPT_NOBACK               0x1000UL
#define EVE_OPT_NOTICKS              0x2000UL
#define EVE_OPT_NOHM                 0x4000UL
#define EVE_OPT_NOPOINTER            0x4000UL
#define EVE_OPT_NOSECS               0x8000UL
#define EVE_OPT_NOHANDS              0xc000UL

#define EVE_DLSWAP_DONE              0x0UL
#define EVE_DLSWAP_FRAME             0x2UL
#define EVE_DLSWAP_LINE              0x1UL

#define EVE_INT_CMDEMPTY             0x20UL
#define EVE_INT_CMDFLAG              0x40UL
#define EVE_INT_CONVCOMPLETE         0x80UL
#define EVE_INT_PLAYBACK             0x10UL
#define EVE_INT_SOUND                0x8UL
#define EVE_INT_SWAP                 0x1UL
#define EVE_INT_TAG                  0x4UL
#define EVE_INT_TOUCH                0x2UL


#define EVE_TOUCHMODE_CONTINUOUS     0x3UL
#define EVE_TOUCHMODE_FRAME          0x2UL
#define EVE_TOUCHMODE_OFF            0x0UL
#define EVE_TOUCHMODE_ONESHOT        0x1UL

#define EVE_LINEAR_SAMPLES           0x0UL	// Audio playback
#define EVE_ULAW_SAMPLES             0x1UL
#define EVE_ADPCM_SAMPLES            0x2UL
#define EVE_VOL_ZERO                 0x0UL

#define EVE_ADC_DIFFERENTIAL         0x1UL
#define EVE_ADC_SINGLE_ENDED         0x0UL

//#################################################################################################################
//#####################################  EVE 2 ####################################################################
//#################################################################################################################

#elif defined EVE2_ENABLE

/* For FT801 enable the switch in platform.h file */
/* Lower boundary of trimming */
#define LOW_FREQ_BOUND  58800000L//98% of 60Mhz

#define EVE_RAM_CMD              0x308000UL
#define EVE_RAM_CMD_SIZE         (4*1024L)
#define EVE_RAM_DL               0x300000UL
#define EVE_RAM_DL_SIZE          (8*1024L)
#define EVE_RAM_G                0x0UL
#define EVE_RAM_G_SIZE           (1024*1024L)
#define EVE_RAM_REG              0x302000UL
#define EVE_RAM_ROMSUB           0x30a000UL

#define EVE_ROMFONT_TABLEADDRESS 0x2ffffcUL
#define EVE_REG_ANA_COMP         0x302184UL
#define EVE_REG_ANALOG           0x30216cUL
#define EVE_REG_BIST_EN          0x302174UL
#define EVE_REG_BUSYBITS         0x3020e8UL
#define EVE_REG_CLOCK            0x302008UL
#define EVE_REG_CMD_DL           0x302100UL
#define EVE_REG_CMD_READ         0x3020f8UL
#define EVE_REG_CMD_WRITE        0x3020fcUL
#define EVE_REG_CMDB_SPACE       0x302574UL
#define EVE_REG_CMDB_WRITE       0x302578UL
#define EVE_REG_CPURESET         0x302020UL
#define EVE_REG_CRC              0x302178UL
#define EVE_REG_CSPREAD          0x302068UL
#define EVE_REG_CTOUCH_EXTENDED  0x302108UL
#define EVE_REG_CTOUCH_TOUCH0_XY 0x302124UL
#define EVE_REG_CTOUCH_TOUCH1_XY 0x30211cUL
#define EVE_REG_CTOUCH_TOUCH2_XY 0x30218cUL
#define EVE_REG_CTOUCH_TOUCH3_XY 0x302190UL
#define EVE_REG_CTOUCH_TOUCH4_X  0x30216cUL
#define EVE_REG_CTOUCH_TOUCH4_Y  0x302120UL
#define EVE_REG_CYA_TOUCH        0x302168UL
#define EVE_REG_DATESTAMP        0x302564UL
#define EVE_REG_DITHER           0x302060UL
#define EVE_REG_DLSWAP           0x302054UL
#define EVE_REG_FRAMES           0x302004UL
#define EVE_REG_FREQUENCY        0x30200cUL
#define EVE_REG_GPIO             0x302094UL
#define EVE_REG_GPIO_DIR         0x302090UL
#define EVE_REG_GPIOX            0x30209cUL
#define EVE_REG_GPIOX_DIR        0x302098UL
#define EVE_REG_HCYCLE           0x30202cUL
#define EVE_REG_HOFFSET          0x302030UL
#define EVE_REG_HSIZE            0x302034UL
#define EVE_REG_HSYNC0           0x302038UL
#define EVE_REG_HSYNC1           0x30203cUL
#define EVE_REG_ID               0x302000UL
#define EVE_REG_INT_EN           0x3020acUL
#define EVE_REG_INT_FLAGS        0x3020a8UL
#define EVE_REG_INT_MASK         0x3020b0UL
#define EVE_REG_MACRO_0          0x3020d8UL
#define EVE_REG_MACRO_1          0x3020dcUL
#define EVE_REG_MEDIAFIFO_READ   0x309014UL
#define EVE_REG_MEDIAFIFO_WRITE  0x309018UL
#define EVE_REG_OUTBITS          0x30205cUL
#define EVE_REG_PATCHED_ANALOG   0x302170UL
#define EVE_REG_PATCHED_TOUCH_FAULT  0x30216cUL
#define EVE_REG_PCLK             0x302070UL
#define EVE_REG_PCLK_POL         0x30206cUL
#define EVE_REG_PLAY             0x30208cUL
#define EVE_REG_PLAYBACK_FORMAT  0x3020c4UL
#define EVE_REG_PLAYBACK_FREQ    0x3020c0UL
#define EVE_REG_PLAYBACK_LENGTH  0x3020b8UL
#define EVE_REG_PLAYBACK_LOOP    0x3020c8UL
#define EVE_REG_PLAYBACK_PLAY    0x3020ccUL
#define EVE_REG_PLAYBACK_READPTR 0x3020bcUL
#define EVE_REG_PLAYBACK_START   0x3020b4UL
#define EVE_REG_PWM_DUTY         0x3020d4UL
#define EVE_REG_PWM_HZ           0x3020d0UL
#define EVE_REG_RENDERMODE       0x302010UL
#define EVE_REG_ROMSUB_SEL       0x3020f0UL
#define EVE_REG_ROTATE           0x302058UL
#define EVE_REG_SNAPFORMAT       0x30201cUL
#define EVE_REG_SNAPSHOT         0x302018UL
#define EVE_REG_SNAPY            0x302014UL
#define EVE_REG_SOUND            0x302088UL
#define EVE_REG_SPI_EARLY_TX     0x30217cUL
#define EVE_REG_SPI_WIDTH        0x302188UL
#define EVE_REG_SWIZZLE          0x302064UL
#define EVE_REG_TAG              0x30207cUL
#define EVE_REG_TAG_X            0x302074UL
#define EVE_REG_TAG_Y            0x302078UL
#define EVE_REG_TAP_CRC          0x302024UL
#define EVE_REG_TAP_MASK         0x302028UL
#define EVE_REG_TOUCH_ADC_MODE   0x302108UL
#define EVE_REG_TOUCH_CHARGE     0x30210cUL
#define EVE_REG_TOUCH_DIRECT_XY  0x30218cUL
#define EVE_REG_TOUCH_DIRECT_Z1Z2    0x302190UL
#define EVE_REG_TOUCH_FAULT      0x302170UL
#define EVE_REG_TOUCH_MODE       0x302104UL
#define EVE_REG_TOUCH_OVERSAMPLE 0x302114UL
#define EVE_REG_TOUCH_RAW_XY     0x30211cUL
#define EVE_REG_TOUCH_RZ         0x302120UL
#define EVE_REG_TOUCH_RZTHRESH   0x302118UL
#define EVE_REG_TOUCH_SCREEN_XY  0x302124UL
#define EVE_REG_TOUCH_SETTLE     0x302110UL
#define EVE_REG_TOUCH_TAG        0x30212cUL
#define EVE_REG_TOUCH_TAG_XY     0x302128UL
#define EVE_REG_TOUCH_TAG1       0x302134UL
#define EVE_REG_TOUCH_TAG1_XY    0x302130UL
#define EVE_REG_TOUCH_TAG2       0x30213cUL
#define EVE_REG_TOUCH_TAG2_XY    0x302138UL
#define EVE_REG_TOUCH_TAG3       0x302144UL
#define EVE_REG_TOUCH_TAG3_XY    0x302140UL
#define EVE_REG_TOUCH_TAG4       0x30214cUL
#define EVE_REG_TOUCH_TAG4_XY    0x302148UL
#define EVE_REG_TOUCH_TRANSFORM_A    0x302150UL
#define EVE_REG_TOUCH_TRANSFORM_B    0x302154UL
#define EVE_REG_TOUCH_TRANSFORM_C    0x302158UL
#define EVE_REG_TOUCH_TRANSFORM_D    0x30215cUL
#define EVE_REG_TOUCH_TRANSFORM_E    0x302160UL
#define EVE_REG_TOUCH_TRANSFORM_F    0x302164UL
#define EVE_REG_TRACKER          0x309000UL
#define EVE_REG_TRACKER_1        0x309004UL
#define EVE_REG_TRACKER_2        0x309008UL
#define EVE_REG_TRACKER_3        0x30900cUL
#define EVE_REG_TRACKER_4        0x309010UL
#define EVE_REG_TRIM             0x302180UL
#define EVE_REG_VCYCLE           0x302040UL
#define EVE_REG_VOFFSET          0x302044UL
#define EVE_REG_VOL_PB           0x302080UL
#define EVE_REG_VOL_SOUND        0x302084UL
#define EVE_REG_VSIZE            0x302048UL
#define EVE_REG_VSYNC0           0x30204cUL
#define EVE_REG_VSYNC1           0x302050UL

#define EVE_ENC_ALPHA_FUNC(func,ref) ((0x9UL << 24)|(((func) & 0x7UL) << 8)|(((ref) & 0xffUL) << 0))
#define EVE_ENC_BEGIN(prim) ((0x1fUL << 24)|(((prim)&15UL) << 0))
#define EVE_ENC_BITMAP_HANDLE(handle) ((0x5UL << 24)|(((handle) & 0x1fUL) << 0))
#define EVE_ENC_BITMAP_LAYOUT_H(linestride,height) ((0x28UL << 24)|(((linestride) & 0x3UL) << 2)|(((height) & 0x3UL) << 0))
#define EVE_ENC_BITMAP_LAYOUT(format,linestride,height) ((0x7UL << 24)|(((format) & 0x1fUL) << 19)|(((linestride) & 0x3ffUL) << 9)|(((height) & 0x1ffUL) << 0))
#define EVE_ENC_BITMAP_SIZE_H(width,height) ((0x29UL << 24)|(((width) & 0x3UL) << 2)|(((height) & 0x3UL) << 0))
#define EVE_ENC_BITMAP_SIZE(filter,wrapx,wrapy,width,height) ((0x8UL << 24)|(((filter) & 0x1UL) << 20)|(((wrapx) & 0x1UL) << 19)|(((wrapy) & 0x1UL) << 18)|(((width) & 0x1ffUL) << 9)|(((height) & 0x1ffUL) << 0))

#define EVE_ENC_BITMAP_SOURCE(addr) ((0x1UL << 24)|(((addr) & 0x3FFFFFUL) << 0))

#define EVE_ENC_BITMAP_TRANSFORM_A(a) ((0x15UL << 24)|((((uint32_t)(a)) & 0x1FFFFUL) << 0))
#define EVE_ENC_BITMAP_TRANSFORM_B(b) ((0x16UL << 24)|((((uint32_t)(b)) & 0x1FFFFUL) << 0))
#define EVE_ENC_BITMAP_TRANSFORM_C(c) ((0x17UL << 24)|((((uint32_t)(c)) & 0xFFFFFFUL) << 0))
#define EVE_ENC_BITMAP_TRANSFORM_D(d) ((0x18UL << 24)|((((uint32_t)(d)) & 0x1FFFFUL) << 0))
#define EVE_ENC_BITMAP_TRANSFORM_E(e) ((0x19UL << 24)|((((uint32_t)(e)) & 0x1FFFFUL) << 0))
#define EVE_ENC_BITMAP_TRANSFORM_F(f) ((0x1aUL << 24)|((((uint32_t)(f)) & 0xFFFFFFUL) << 0))
#define EVE_ENC_BLEND_FUNC(src,dst) ((0xbUL << 24)|(((src) & 0x7UL) << 3)|(((dst) & 0x7UL) << 0))
#define EVE_ENC_CALL(dest) ((0x1dUL << 24)|(((dest) & 0xFFFFUL) << 0))
#define EVE_ENC_CELL(cell) ((0x6UL << 24)|(((cell) & 0x7fUL) << 0))
#define EVE_ENC_CLEAR_COLOR_A(alpha) ((0xfUL << 24)|(((alpha) & 0xffUL) << 0))
#define EVE_ENC_CLEAR_COLOR_RGB(red,green,blue) ((0x2UL << 24)|(((red) & 0xffUL) << 16)|(((green) & 0xffUL) << 8)|(((blue) & 0xffUL) << 0))
#define EVE_ENC_CLEAR_COLOR(c) ((0x2UL << 24)|(((uint32_t)(c)) & 0x00ffffffUL))
#define EVE_ENC_CLEAR_STENCIL(s) ((0x11UL << 24)|((((uint32_t)(s)) & 0xffUL) << 0))
#define EVE_ENC_CLEAR_TAG(s) ((0x12UL << 24)|((((uint32_t)(s)) & 0xffUL) << 0))
#define EVE_ENC_CLEAR(c,s,t) ((0x26UL << 24)|((((uint32_t)(c)) & 0x1UL) << 2)|((((uint32_t)(s)) & 0x1UL) << 1)|((((uint32_t)(t)) & 0x1UL) << 0))
#define EVE_ENC_COLOR_A(alpha) ((0x10UL << 24)|(((alpha) & 0xffUL) << 0))
#define EVE_ENC_COLOR_MASK(r,g,b,a) ((0x20UL << 24)|((((uint32_t)(r)) & 0x1UL) << 3)|((((uint32_t)(g)) & 0x1UL) << 2)|((((uint32_t)(b)) & 0x1UL) << 1)|((((uint32_t)(a)) & 0x1UL) << 0))
#define EVE_ENC_COLOR_RGB(red,green,blue) ((0x4UL << 24)|(((red) & 0xffUL) << 16)|(((green) & 0xffUL) << 8)|(((blue) & 0xffUL) << 0))
#define EVE_ENC_COLOR(c) ((0x4UL << 24)|(((uint32_t)(c)) & 0x00ffffffUL))
#define EVE_ENC_DISPLAY() ((0x0UL << 24))
#define EVE_ENC_END() ((0x21UL << 24))
#define EVE_ENC_JUMP(dest) ((0x1eUL << 24)|(((dest) & 0xFFFFUL) << 0))
#define EVE_ENC_LINE_WIDTH(width) ((0xeUL << 24)|(((width) & 0xFFFUL) << 0))
#define EVE_ENC_MACRO(m) ((0x25UL << 24)|((((uint32_t)(m)) & 0x1UL) << 0))
#define EVE_ENC_NOP() ((0x2dUL << 24))
#define EVE_ENC_PALETTE_SOURCE(addr) ((0x2aUL << 24)|(((addr) & 0x3FFFFFUL) << 0))
#define EVE_ENC_POINT_SIZE(size) ((0xdUL << 24)|(((size) & 0x1FFFUL) << 0))
#define EVE_ENC_RESTORE_CONTEXT() ((0x23UL << 24))
#define EVE_ENC_RETURN() ((0x24UL << 24))
#define EVE_ENC_SAVE_CONTEXT() ((0x22UL << 24))
#define EVE_ENC_SCISSOR_SIZE(width,height) ((0x1cUL << 24)|(((width) & 0xFFFUL) << 12)|(((height) & 0xFFFUL) << 0))
#define EVE_ENC_SCISSOR_XY(x,y) ((0x1bUL << 24)|((((uint32_t)(x)) & 0x7FFUL) << 11)|((((uint32_t)(y)) & 0x7FFUL) << 0))
#define EVE_ENC_STENCIL_FUNC(func,ref,mask) ((0xaUL << 24)|(((func) & 0x7UL) << 16)|(((ref) & 0xffUL) << 8)|(((mask) & 0xffUL) << 0))
#define EVE_ENC_STENCIL_MASK(mask) ((0x13UL << 24)|(((mask) & 0xffUL) << 0))
#define EVE_ENC_STENCIL_OP(sfail,spass) ((0xcUL << 24)|(((sfail) & 0x7UL) << 3)|(((spass) & 0x7UL) << 0))
#define EVE_ENC_TAG_MASK(mask) ((0x14UL << 24)|(((mask) & 0x1UL) << 0))
#define EVE_ENC_TAG(s) ((0x3UL << 24)|((((uint32_t)(s)) & 0xffUL) << 0))
#define EVE_ENC_VERTEX_FORMAT(frac) ((0x27UL << 24)|(((frac) & 0x7UL) << 0))
#define EVE_ENC_VERTEX_TRANSLATE_X(x) ((0x2bUL << 24)|((((uint32_t)(x)) & 0x1FFFFUL) << 0))
#define EVE_ENC_VERTEX_TRANSLATE_Y(y) ((0x2cUL << 24)|((((uint32_t)(y)) & 0x1FFFFUL) << 0))
#define EVE_ENC_VERTEX2F(x,y) ((0x1UL << 30)|((((uint32_t)(x)) & 0xffffUL) << 15)|((((uint32_t)(y)) & 0xffffUL) << 0))
#define EVE_ENC_VERTEX2II(x,y,handle,cell) ((0x2UL << 30)|((((uint32_t)(x)) & 0x1ffUL) << 21)|((((uint32_t)(y)) & 0x1ffUL) << 12)|(((handle) & 0x1fUL) << 7)|(((cell) & 0x7fUL) << 0))

#define EVE_ENC_CMD_APPEND           0xffffff1eUL
#define EVE_ENC_CMD_BGCOLOR          0xffffff09UL
#define EVE_ENC_CMD_BITMAP_TRANSFORM 0xffffff21UL
#define EVE_ENC_CMD_BUTTON           0xffffff0dUL
#define EVE_ENC_CMD_CALIBRATE        0xffffff15UL
#define EVE_ENC_CMD_CLOCK            0xffffff14UL
#define EVE_ENC_CMD_COLDSTART        0xffffff32UL
#define EVE_ENC_CMD_CRC              0xffffff03UL
#define EVE_ENC_CMD_CSKETCH          0xffffff35UL
#define EVE_ENC_CMD_DIAL             0xffffff2dUL
#define EVE_ENC_CMD_DLSTART          0xffffff00UL
#define EVE_ENC_CMD_EXECUTE          0xffffff07UL
#define EVE_ENC_CMD_FGCOLOR          0xffffff0aUL
#define EVE_ENC_CMD_GAUGE            0xffffff13UL
#define EVE_ENC_CMD_GETMATRIX        0xffffff33UL
#define EVE_ENC_CMD_GETPOINT         0xffffff08UL
#define EVE_ENC_CMD_GETPROPS         0xffffff25UL
#define EVE_ENC_CMD_GETPTR           0xffffff23UL
#define EVE_ENC_CMD_GRADCOLOR        0xffffff34UL
#define EVE_ENC_CMD_GRADIENT         0xffffff0bUL
#define EVE_ENC_CMD_HAMMERAUX        0xffffff04UL
#define EVE_ENC_CMD_IDCT_DELETED     0xffffff06UL
#define EVE_ENC_CMD_INFLATE          0xffffff22UL
#define EVE_ENC_CMD_INT_RAMSHARED    0xffffff3dUL
#define EVE_ENC_CMD_INT_SWLOADIMAGE  0xffffff3eUL
#define EVE_ENC_CMD_INTERRUPT        0xffffff02UL
#define EVE_ENC_CMD_KEYS             0xffffff0eUL
#define EVE_ENC_CMD_LOADIDENTITY     0xffffff26UL
#define EVE_ENC_CMD_LOADIMAGE        0xffffff24UL
#define EVE_ENC_CMD_LOGO             0xffffff31UL
#define EVE_ENC_CMD_MARCH            0xffffff05UL
#define EVE_ENC_CMD_MEDIAFIFO        0xffffff39UL
#define EVE_ENC_CMD_MEMCPY           0xffffff1dUL
#define EVE_ENC_CMD_MEMCRC           0xffffff18UL
#define EVE_ENC_CMD_MEMSET           0xffffff1bUL
#define EVE_ENC_CMD_MEMWRITE         0xffffff1aUL
#define EVE_ENC_CMD_MEMZERO          0xffffff1cUL
#define EVE_ENC_CMD_NUMBER           0xffffff2eUL
#define EVE_ENC_CMD_PLAYVIDEO        0xffffff3aUL
#define EVE_ENC_CMD_PROGRESS         0xffffff0fUL
#define EVE_ENC_CMD_REGREAD          0xffffff19UL
#define EVE_ENC_CMD_ROMFONT          0xffffff3fUL
#define EVE_ENC_CMD_ROTATE           0xffffff29UL
#define EVE_ENC_CMD_SCALE            0xffffff28UL
#define EVE_ENC_CMD_SCREENSAVER      0xffffff2fUL
#define EVE_ENC_CMD_SCROLLBAR        0xffffff11UL
#define EVE_ENC_CMD_SETBASE          0xffffff38UL
#define EVE_ENC_CMD_SETBITMAP        0xffffff43UL
#define EVE_ENC_CMD_SETFONT          0xffffff2bUL
#define EVE_ENC_CMD_SETFONT2         0xffffff3bUL
#define EVE_ENC_CMD_SETMATRIX        0xffffff2aUL
#define EVE_ENC_CMD_SETROTATE        0xffffff36UL
#define EVE_ENC_CMD_SETSCRATCH       0xffffff3cUL
#define EVE_ENC_CMD_SKETCH           0xffffff30UL
#define EVE_ENC_CMD_SLIDER           0xffffff10UL
#define EVE_ENC_CMD_SNAPSHOT         0xffffff1fUL
#define EVE_ENC_CMD_SNAPSHOT2        0xffffff37UL
#define EVE_ENC_CMD_SPINNER          0xffffff16UL
#define EVE_ENC_CMD_STOP             0xffffff17UL
#define EVE_ENC_CMD_SWAP             0xffffff01UL
#define EVE_ENC_CMD_SYNC             0xffffff42UL
#define EVE_ENC_CMD_TEXT             0xffffff0cUL
#define EVE_ENC_CMD_TOGGLE           0xffffff12UL
#define EVE_ENC_CMD_TOUCH_TRANSFORM  0xffffff20UL
#define EVE_ENC_CMD_TRACK            0xffffff2cUL
#define EVE_ENC_CMD_TRANSLATE        0xffffff27UL
#define EVE_ENC_CMD_VIDEOFRAME       0xffffff41UL
#define EVE_ENC_CMD_VIDEOSTART       0xffffff40UL

#define EVE_BEGIN_BITMAPS            0x1UL
#define EVE_BEGIN_EDGE_STRIP_A       0x7UL
#define EVE_BEGIN_EDGE_STRIP_B       0x8UL
#define EVE_BEGIN_EDGE_STRIP_L       0x6UL
#define EVE_BEGIN_EDGE_STRIP_R       0x5UL
#define EVE_BEGIN_LINE_STRIP         0x4UL
#define EVE_BEGIN_LINES              0x3UL
#define EVE_BEGIN_POINTS             0x2UL
#define EVE_BEGIN_RECTS              0x9UL
#define EVE_BLEND_DST_ALPHA          0x3UL
#define EVE_BLEND_ONE                0x1UL
#define EVE_BLEND_ONE_MINUS_DST_ALPHA    0x5UL
#define EVE_BLEND_ONE_MINUS_SRC_ALPHA    0x4UL
#define EVE_BLEND_SRC_ALPHA          0x2UL
#define EVE_BLEND_ZERO               0x0UL
#define EVE_DLSWAP_DONE              0x0UL
#define EVE_DLSWAP_FRAME             0x2UL
#define EVE_DLSWAP_LINE              0x1UL
#define EVE_FILTER_BILINEAR          0x1UL
#define EVE_FILTER_NEAREST           0x0UL
#define EVE_FORMAT_ARGB1555          0x0UL
#define EVE_FORMAT_ARGB2             0x5UL
#define EVE_FORMAT_ARGB4             0x6UL
#define EVE_FORMAT_BARGRAPH          0xbUL
#define EVE_FORMAT_L1                0x1UL
#define EVE_FORMAT_L2                0x11UL
#define EVE_FORMAT_L4                0x2UL
#define EVE_FORMAT_L8                0x3UL
#define EVE_FORMAT_PALETTED          0x8UL
#define EVE_FORMAT_PALETTED4444      0xfUL
#define EVE_FORMAT_PALETTED565       0xeUL
#define EVE_FORMAT_PALETTED8         0x10UL
#define EVE_FORMAT_RGB332            0x4UL
#define EVE_FORMAT_RGB565            0x7UL
#define EVE_FORMAT_TEXT8X8           0x9UL
#define EVE_FORMAT_TEXTVGA           0xaUL
#define EVE_INT_CMDEMPTY             0x20UL
#define EVE_INT_CMDFLAG              0x40UL
#define EVE_INT_CONVCOMPLETE         0x80UL
#define EVE_INT_G8                   0x12UL
#define EVE_INT_L8C                  0xcUL
#define EVE_INT_PLAYBACK             0x10UL
#define EVE_INT_SOUND                0x8UL
#define EVE_INT_SWAP                 0x1UL
#define EVE_INT_TAG                  0x4UL
#define EVE_INT_TOUCH                0x2UL
#define EVE_INT_VGA                  0xdUL
#define EVE_LINEAR_SAMPLES           0x0UL
#define EVE_OPT_CENTER               0x600UL
#define EVE_OPT_CENTERX              0x200UL
#define EVE_OPT_CENTERY              0x400UL
#define EVE_OPT_FLAT                 0x100UL
#define EVE_OPT_FULLSCREEN           0x8UL
#define EVE_OPT_MEDIAFIFO            0x10UL
#define EVE_OPT_MONO                 0x1UL
#define EVE_OPT_NOBACK               0x1000UL
#define EVE_OPT_NODL                 0x2UL
#define EVE_OPT_NOHANDS              0xc000UL
#define EVE_OPT_NOHM                 0x4000UL
#define EVE_OPT_NOPOINTER            0x4000UL
#define EVE_OPT_NOSECS               0x8000UL
#define EVE_OPT_NOTEAR               0x4UL
#define EVE_OPT_NOTICKS              0x2000UL
#define EVE_OPT_RIGHTX               0x800UL
#define EVE_OPT_SIGNED               0x100UL
#define EVE_OPT_SOUND                0x20UL
#define EVE_STENCIL_DECR             0x4UL
#define EVE_STENCIL_INCR             0x3UL
#define EVE_STENCIL_INVERT           0x5UL
#define EVE_STENCIL_KEEP             0x1UL
#define EVE_STENCIL_REPLACE          0x2UL
#define EVE_STENCIL_ZERO             0x0UL
#define EVE_TEST_ALWAYS              0x7UL
#define EVE_TEST_EQUAL               0x5UL
#define EVE_TEST_GEQUAL              0x4UL
#define EVE_TEST_GREATER             0x3UL
#define EVE_TEST_LEQUAL              0x2UL
#define EVE_TEST_LESS                0x1UL
#define EVE_TEST_NEVER               0x0UL
#define EVE_TEST_NOTEQUAL            0x6UL
#define EVE_TOUCHMODE_CONTINUOUS     0x3UL
#define EVE_TOUCHMODE_FRAME          0x2UL
#define EVE_TOUCHMODE_OFF            0x0UL
#define EVE_TOUCHMODE_ONESHOT        0x1UL
#define EVE_ULAW_SAMPLES             0x1UL
#define EVE_VOL_ZERO                 0x0UL
#define EVE_WRAP_BORDER              0x0UL
#define EVE_WRAP_REPEAT              0x1UL
#define EVE_ADC_DIFFERENTIAL         0x1UL //?
#define EVE_ADC_SINGLE_ENDED         0x0UL //?
#define EVE_ADPCM_SAMPLES            0x2UL //?

//#################################################################################################################
//#####################################  EVE 3/4 ####################################################################
//#################################################################################################################

#elif (defined (EVE3_ENABLE) || defined (EVE4_ENABLE))

#define EVE_LOW_FREQ_BOUND  58800000L//98% of 60Mhz

#define EVE_RAM_G_SIZE              1024*1024L
#define EVE_RAM_BIST                3670016UL
#define EVE_RAM_CMD                 3178496UL
#define EVE_RAM_DL                  3145728UL
#define EVE_RAM_G                   0UL
#define EVE_RAM_REG                 3153920UL
#define EVE_RAM_ROMSUB              3186688UL
#define EVE_RAM_TOP                 3162112UL
#define EVE_CMDBUF_SIZE             4096UL
#define EVE_RAM_CMD_SIZE            (4*1024L)
#define EVE_RAM_ERR_REPORT          0x309800
#define EVE_ROMFONT_TABLEADDRESS    3145724UL

#define EVE_REG_ADAPTIVE_FRAMERATE  0x30257C //3155324UL
#define EVE_REG_ANALOG              3154284UL
#define EVE_REG_ANA_COMP            3154308UL
#define EVE_REG_ANIM_ACTIVE         3182636UL
#define EVE_REG_BIST_EN             3154292UL
#define EVE_REG_BUSYBITS            3154152UL
#define EVE_REG_CLOCK               3153928UL
#define EVE_REG_CMDB_SPACE          3155316UL
#define EVE_REG_CMDB_WRITE          3155320UL
#define EVE_REG_CMD_DL              3154176UL
#define EVE_REG_CMD_READ            3154168UL
#define EVE_REG_CMD_WRITE           3154172UL
#define EVE_REG_CPURESET            3153952UL
#define EVE_REG_CRC                 3154296UL
#define EVE_REG_CSPREAD             3154024UL
#define EVE_REG_CTOUCH_EXTENDED     3154184UL
#define EVE_REG_CTOUCH_TOUCH0_XY    3154212UL
#define EVE_REG_CTOUCH_TOUCH4_X     3154284UL
#define EVE_REG_CTOUCH_TOUCH4_Y     3154208UL
#define EVE_REG_CTOUCH_TOUCH1_XY    3154204UL
#define EVE_REG_CTOUCH_TOUCH2_XY    3154316UL
#define EVE_REG_CTOUCH_TOUCH3_XY    3154320UL
#define EVE_REG_TOUCH_CONFIG        3154280UL
#define EVE_REG_DATESTAMP           3155300UL
#define EVE_REG_DF_TUNED            3182640UL
#define EVE_REG_DITHER              3154016UL
#define EVE_REG_DLSWAP              3154004UL
#define EVE_REG_EHOST_TOUCH_ACK     3154288UL
#define EVE_REG_EHOST_TOUCH_ID      3154196UL
#define EVE_REG_EHOST_TOUCH_X       3154188UL
#define EVE_REG_EHOST_TOUCH_Y       3154200UL
#define EVE_REG_EJPG_ACC            3154776UL
#define EVE_REG_EJPG_BUSY           3154328UL
#define EVE_REG_EJPG_DAT            3154332UL
#define EVE_REG_EJPG_DCC            3154752UL
#define EVE_REG_EJPG_DEBUG          3155292UL
#define EVE_REG_EJPG_DST            3154340UL
#define EVE_REG_EJPG_FORMAT         3154352UL
#define EVE_REG_EJPG_H              3154348UL
#define EVE_REG_EJPG_HT             3154496UL
#define EVE_REG_EJPG_OPTIONS        3154336UL
#define EVE_REG_EJPG_Q              3154368UL
#define EVE_REG_EJPG_READY          3154324UL
#define EVE_REG_EJPG_RI             3154356UL
#define EVE_REG_EJPG_SCALE          3155288UL
#define EVE_REG_EJPG_TDA            3154364UL
#define EVE_REG_EJPG_TQ             3154360UL
#define EVE_REG_EJPG_W              3154344UL
#define EVE_REG_ESPIM_ADD           3155356UL
#define EVE_REG_ESPIM_COUNT         3155360UL
#define EVE_REG_ESPIM_DUMMY         3155428UL
#define EVE_REG_ESPIM_READSTART     3155336UL
#define EVE_REG_ESPIM_SEQ           3155340UL
#define EVE_REG_ESPIM_TRIG          3155432UL
#define EVE_REG_ESPIM_WINDOW        3155364UL
#define EVE_REG_FLASH_SIZE          3182628UL
#define EVE_REG_FLASH_STATUS        3155440UL
#define EVE_REG_FRAMES              3153924UL
#define EVE_REG_FREQUENCY           3153932UL
#define EVE_REG_FULLBUSYBITS        3155444UL
#define EVE_REG_GPIO                3154068UL
#define EVE_REG_GPIOX               3154076UL
#define EVE_REG_GPIOX_DIR           3154072UL
#define EVE_REG_GPIO_DIR            3154064UL
#define EVE_REG_HCYCLE              3153964UL
#define EVE_REG_HOFFSET             3153968UL
#define EVE_REG_HSIZE               3153972UL
#define EVE_REG_HSYNC0              3153976UL
#define EVE_REG_HSYNC1              3153980UL
#define EVE_REG_ID                  3153920UL
#define EVE_REG_INT_EN              3154092UL
#define EVE_REG_INT_FLAGS           3154088UL
#define EVE_REG_INT_MASK            3154096UL
#define EVE_REG_MACRO_0             3154136UL
#define EVE_REG_MACRO_1             3154140UL
#define EVE_REG_MEDIAFIFO_BASE      3182620UL
#define EVE_REG_MEDIAFIFO_READ      3182612UL
#define EVE_REG_MEDIAFIFO_SIZE      3182624UL
#define EVE_REG_MEDIAFIFO_WRITE     3182616UL
#define EVE_REG_OUTBITS             3154012UL
#define EVE_REG_PCLK                3154032UL
#define EVE_REG_PCLK_POL            3154028UL
#define EVE_REG_PLAY                3154060UL
#define EVE_REG_PLAYBACK_FORMAT     3154116UL
#define EVE_REG_PLAYBACK_FREQ       3154112UL
#define EVE_REG_PLAYBACK_LENGTH     3154104UL
#define EVE_REG_PLAYBACK_LOOP       3154120UL
#define EVE_REG_PLAYBACK_PAUSE      3155436UL
#define EVE_REG_PLAYBACK_PLAY       3154124UL
#define EVE_REG_PLAYBACK_READPTR    3154108UL
#define EVE_REG_PLAYBACK_START      3154100UL
#define EVE_REG_PLAY_CONTROL        3182926UL
#define EVE_REG_COPRO_PATCH_PTR     3182946UL
#define EVE_REG_PWM_DUTY            3154132UL
#define EVE_REG_PWM_HZ              3154128UL
#define EVE_REG_RAM_FOLD            3154164UL
#define EVE_REG_RASTERY             3155296UL
#define EVE_REG_RENDERMODE          3153936UL
#define EVE_REG_ROMSUB_SEL          3154160UL
#define EVE_REG_ROTATE              3154008UL
#define EVE_REG_SNAPFORMAT          3153948UL
#define EVE_REG_SNAPSHOT            3153944UL
#define EVE_REG_SNAPY               3153940UL
#define EVE_REG_SOUND               3154056UL
#define EVE_REG_SPIM                3155332UL
#define EVE_REG_SPIM_DIR            3155328UL
#define EVE_REG_SPI_EARLY_TX        3154300UL
#define EVE_REG_SPI_WIDTH           3154312UL
#define EVE_REG_SWIZZLE             3154020UL
#define EVE_REG_TAG                 3154044UL
#define EVE_REG_TAG_X               3154036UL
#define EVE_REG_TAG_Y               3154040UL
#define EVE_REG_TAP_CRC             3153956UL
#define EVE_REG_TAP_MASK            3153960UL
#define EVE_REG_TOUCH_ADC_MODE      3154184UL
#define EVE_REG_TOUCH_CHARGE        3154188UL
#define EVE_REG_TOUCH_DIRECT_XY     3154316UL
#define EVE_REG_TOUCH_DIRECT_Z1Z2   3154320UL
#define EVE_REG_TOUCH_FAULT         3154288UL
#define EVE_REG_TOUCH_MODE          3154180UL
#define EVE_REG_TOUCH_OVERSAMPLE    3154196UL
#define EVE_REG_TOUCH_RAW_XY        3154204UL
#define EVE_REG_TOUCH_RZ            3154208UL
#define EVE_REG_TOUCH_RZTHRESH      3154200UL
#define EVE_REG_TOUCH_SCREEN_XY     3154212UL
#define EVE_REG_TOUCH_SETTLE        3154192UL
#define EVE_REG_TOUCH_TAG           3154220UL
#define EVE_REG_TOUCH_TAG1          3154228UL
#define EVE_REG_TOUCH_TAG1_XY       3154224UL
#define EVE_REG_TOUCH_TAG2          3154236UL
#define EVE_REG_TOUCH_TAG2_XY       3154232UL
#define EVE_REG_TOUCH_TAG3          3154244UL
#define EVE_REG_TOUCH_TAG3_XY       3154240UL
#define EVE_REG_TOUCH_TAG4          3154252UL
#define EVE_REG_TOUCH_TAG4_XY       3154248UL
#define EVE_REG_TOUCH_TAG_XY        3154216UL
#define EVE_REG_TOUCH_TRANSFORM_A   3154256UL
#define EVE_REG_TOUCH_TRANSFORM_B   3154260UL
#define EVE_REG_TOUCH_TRANSFORM_C   3154264UL
#define EVE_REG_TOUCH_TRANSFORM_D   3154268UL
#define EVE_REG_TOUCH_TRANSFORM_E   3154272UL
#define EVE_REG_TOUCH_TRANSFORM_F   3154276UL
#define EVE_REG_TRACKER             3182592UL
#define EVE_REG_TRACKER_1           3182596UL
#define EVE_REG_TRACKER_2           3182600UL
#define EVE_REG_TRACKER_3           3182604UL
#define EVE_REG_TRACKER_4           3182608UL
#define EVE_REG_TRIM                3154304UL
#define EVE_REG_VCYCLE              3153984UL
#define EVE_REG_VOFFSET             3153988UL
#define EVE_REG_VOL_PB              3154048UL
#define EVE_REG_VOL_SOUND           3154052UL
#define EVE_REG_VSIZE               3153992UL
#define EVE_REG_VSYNC0              3153996UL
#define EVE_REG_VSYNC1              3154000UL

#define EVE_ENC_CMD_ANIMDRAW        4294967126UL
#define EVE_ENC_CMD_ANIMFRAME       4294967130UL
#define EVE_ENC_CMD_ANIMSTART       4294967123UL
#define EVE_ENC_CMD_ANIMSTOP        4294967124UL
#define EVE_ENC_CMD_ANIMXY          4294967125UL
#define EVE_ENC_CMD_APPEND          4294967070UL
#define EVE_ENC_CMD_APPENDF         4294967129UL
#define EVE_ENC_CMD_BGCOLOR         4294967049UL
#define EVE_ENC_CMD_BITMAP_TRANSFORM    4294967073UL
#define EVE_ENC_CMD_BUTTON          4294967053UL
#define EVE_ENC_CMD_CALIBRATE       4294967061UL
#define EVE_ENC_CMD_CLEARCACHE      4294967119UL
#define EVE_ENC_CMD_CLOCK           4294967060UL
#define EVE_ENC_CMD_COLDSTART       4294967090UL
#define EVE_ENC_CMD_CRC             4294967043UL
#define EVE_ENC_CMD_DEPRECATED_CSKETCH    4294967093UL
#define EVE_ENC_CMD_DIAL            4294967085UL
#define EVE_ENC_CMD_DLSTART         4294967040UL
#define EVE_ENC_CMD_EXECUTE         4294967047UL
#define EVE_ENC_CMD_FGCOLOR         4294967050UL
#define EVE_ENC_CMD_FILLWIDTH       4294967128UL
#define EVE_ENC_CMD_FLASHATTACH     4294967113UL
#define EVE_ENC_CMD_FLASHDETACH     4294967112UL
#define EVE_ENC_CMD_FLASHERASE      4294967108UL
#define EVE_ENC_CMD_FLASHFAST       4294967114UL
#define EVE_ENC_CMD_FLASHREAD       4294967110UL
#define EVE_ENC_CMD_FLASHSOURCE     4294967118UL
#define EVE_ENC_CMD_FLASHSPIDESEL   4294967115UL
#define EVE_ENC_CMD_FLASHSPIRX      4294967117UL
#define EVE_ENC_CMD_FLASHSPITX      4294967116UL
#define EVE_ENC_CMD_FLASHUPDATE     4294967111UL
#define EVE_ENC_CMD_FLASHWRITE      4294967109UL
#define EVE_ENC_CMD_GAUGE           4294967059UL
#define EVE_ENC_CMD_GETMATRIX       4294967091UL
#define EVE_ENC_CMD_GETPOINT        4294967048UL
#define EVE_ENC_CMD_GETPROPS        4294967077UL
#define EVE_ENC_CMD_GETPTR          4294967075UL
#define EVE_ENC_CMD_GRADCOLOR       4294967092UL
#define EVE_ENC_CMD_GRADIENT        4294967051UL
#define EVE_ENC_CMD_GRADIENTA       4294967127UL
#define EVE_ENC_CMD_HAMMERAUX       4294967044UL
#define EVE_ENC_CMD_HMAC            4294967133UL
#define EVE_ENC_CMD_IDCT_DELETED    4294967046UL
#define EVE_ENC_CMD_INFLATE         4294967074UL
#define EVE_ENC_CMD_INFLATE2        4294967120UL
#define EVE_ENC_CMD_INTERRUPT       4294967042UL
#define EVE_ENC_CMD_INT_RAMSHARED   4294967101UL
#define EVE_ENC_CMD_INT_SWLOADIMAGE 4294967102UL
#define EVE_ENC_CMD_KEYS            4294967054UL
#define EVE_ENC_CMD_LAST_           4294967134UL
#define EVE_ENC_CMD_LOADIDENTITY    4294967078UL
#define EVE_ENC_CMD_LOADIMAGE       4294967076UL
#define EVE_ENC_CMD_LOGO            4294967089UL
#define EVE_ENC_CMD_MARCH           4294967045UL
#define EVE_ENC_CMD_MEDIAFIFO       4294967097UL
#define EVE_ENC_CMD_MEMCPY          4294967069UL
#define EVE_ENC_CMD_MEMCRC          4294967064UL
#define EVE_ENC_CMD_MEMSET          4294967067UL
#define EVE_ENC_CMD_MEMWRITE        4294967066UL
#define EVE_ENC_CMD_MEMZERO         4294967068UL
#define EVE_ENC_CMD_NOP             4294967131UL
#define EVE_ENC_CMD_NUMBER          4294967086UL
#define EVE_ENC_CMD_PLAYVIDEO       4294967098UL
#define EVE_ENC_CMD_PROGRESS        4294967055UL
#define EVE_ENC_CMD_REGREAD         4294967065UL
#define EVE_ENC_CMD_RESETFONTS      4294967122UL
#define EVE_ENC_CMD_ROMFONT         4294967103UL
#define EVE_ENC_CMD_ROTATE          4294967081UL
#define EVE_ENC_CMD_ROTATEAROUND    4294967121UL
#define EVE_ENC_CMD_SCALE           4294967080UL
#define EVE_ENC_CMD_SCREENSAVER     4294967087UL
#define EVE_ENC_CMD_SCROLLBAR       4294967057UL
#define EVE_ENC_CMD_SETBASE         4294967096UL
#define EVE_ENC_CMD_SETBITMAP       4294967107UL
#define EVE_ENC_CMD_SETFONT         4294967083UL
#define EVE_ENC_CMD_SETFONT2        4294967099UL
#define EVE_ENC_CMD_SETMATRIX       4294967082UL
#define EVE_ENC_CMD_SETROTATE       4294967094UL
#define EVE_ENC_CMD_SETSCRATCH      4294967100UL
#define EVE_ENC_CMD_SHA1            4294967132UL
#define EVE_ENC_CMD_SKETCH          4294967088UL
#define EVE_ENC_CMD_SLIDER          4294967056UL
#define EVE_ENC_CMD_SNAPSHOT        4294967071UL
#define EVE_ENC_CMD_SNAPSHOT2       4294967095UL
#define EVE_ENC_CMD_SPINNER         4294967062UL
#define EVE_ENC_CMD_STOP            4294967063UL
#define EVE_ENC_CMD_SWAP            4294967041UL
#define EVE_ENC_CMD_SYNC            4294967106UL
#define EVE_ENC_CMD_TEXT            4294967052UL
#define EVE_ENC_CMD_TOGGLE          4294967058UL
#define EVE_ENC_CMD_TOUCH_TRANSFORM 4294967072UL
#define EVE_ENC_CMD_TRACK           4294967084UL
#define EVE_ENC_CMD_TRANSLATE       4294967079UL
#define EVE_ENC_CMD_VIDEOFRAME      4294967105UL
#define EVE_ENC_CMD_VIDEOSTART      4294967104UL
#define EVE_ENC_CMD_VIDEOSTARTF     4294967135UL

#define EVE_ENC_VERTEX2F(x,y)                                ((1UL<<30)|(((x)&32767UL)<<15)|(((y)&32767UL)<<0))
#define EVE_ENC_VERTEX2II(x,y,handle,cell)                   ((2UL<<30)|(((x)&511UL)<<21)|(((y)&511UL)<<12)|(((handle)&31UL)<<7)|(((cell)&127UL)<<0))

#define EVE_ENC_BITMAP_ADDR_MASK 16777215UL
#define EVE_ENC_BITMAP_SOURCE(addr)                     ((1UL << 24) | ((addr)&EVE_ENC_BITMAP_ADDR_MASK))
#define EVE_ENC_BITMAP_SOURCE2(flash_or_ram, addr)      ((1UL << 24) | ((flash_or_ram) << 23) | (((addr) & (EVE_ENC_BITMAP_ADDR_MASK >> 1)) << 0))
#define EVE_ENC_CLEAR_COLOR_RGB(red,green,blue)              ((2UL<<24)|(((red)&255UL)<<16)|(((green)&255UL)<<8)|(((blue)&255UL)<<0))
#define EVE_ENC_TAG(s)                                       ((3UL<<24)|(((s)&255UL)<<0))
#define EVE_ENC_COLOR_RGB(red,green,blue)                    ((4UL<<24)|(((red)&255UL)<<16)|(((green)&255UL)<<8)|(((blue)&255UL)<<0))
#define EVE_ENC_BITMAP_HANDLE(handle)                        ((5UL<<24)|(((handle)&31UL)<<0))
#define EVE_ENC_CELL(cell)                                   ((6UL<<24)|(((cell)&127UL)<<0))
#define EVE_ENC_BITMAP_LAYOUT(format,linestride,height)      ((7UL<<24)|(((format)&31UL)<<19)|(((linestride)&1023UL)<<9)|(((height)&511UL)<<0))
#define EVE_ENC_BITMAP_SIZE(filter,wrapx,wrapy,width,height) ((8UL<<24)|(((filter)&1UL)<<20)|(((wrapx)&1UL)<<19)|(((wrapy)&1UL)<<18)|(((width)&511UL)<<9)|(((height)&511UL)<<0))
#define EVE_ENC_ALPHA_FUNC(func,ref)        ((9UL<<24)|(((func)&7UL)<<8)|(((ref)&255UL)<<0))
#define EVE_ENC_STENCIL_FUNC(func,ref,mask) ((10UL<<24)|(((func)&7UL)<<16)|(((ref)&255UL)<<8)|(((mask)&255UL)<<0))
#define EVE_ENC_BLEND_FUNC(src,dst)         ((11UL<<24)|(((src)&7UL)<<3)|(((dst)&7UL)<<0))
#define EVE_ENC_STENCIL_OP(sfail,spass)     ((12UL<<24)|(((sfail)&7UL)<<3)|(((spass)&7UL)<<0))
#define EVE_ENC_POINT_SIZE(size)            ((13UL<<24)|(((size)&8191UL)<<0))
#define EVE_ENC_LINE_WIDTH(width)           ((14UL<<24)|(((width)&4095UL)<<0))
#define EVE_ENC_CLEAR_COLOR(c)              ((2UL<<24)|(((uint32_t)(c)) & 0x00ffffffUL))
#define EVE_ENC_CLEAR_COLOR_A(alpha)        ((15UL<<24)|(((alpha)&255UL)<<0))
#define EVE_ENC_COLOR(c)                    ((4UL<<24)|(((uint32_t)(c)) & 0x00ffffffUL))
#define EVE_ENC_COLOR_A(alpha)              ((16UL<<24)|(((alpha)&255UL)<<0))
#define EVE_ENC_CLEAR_STENCIL(s)            ((17UL<<24)|(((s)&255UL)<<0))
#define EVE_ENC_CLEAR_TAG(s)                ((18UL<<24)|(((s)&255UL)<<0))
#define EVE_ENC_STENCIL_MASK(mask)          ((19UL<<24)|(((mask)&255UL)<<0))
#define EVE_ENC_TAG_MASK(mask)              ((20UL<<24)|(((mask)&1UL)<<0))
#define EVE_ENC_BITMAP_TRANSFORM_A(p,v)     ((21UL<<24)|(((p)&1UL)<<17)|(((v)&131071UL)<<0))
#define EVE_ENC_BITMAP_TRANSFORM_B(p,v)     ((22UL<<24)|(((p)&1UL)<<17)|(((v)&131071UL)<<0))
#define EVE_ENC_BITMAP_TRANSFORM_C(c)       ((23UL<<24)|(((c)&16777215UL)<<0))
#define EVE_ENC_BITMAP_TRANSFORM_D(p,v)     ((24UL<<24)|(((p)&1UL)<<17)|(((v)&131071UL)<<0))
#define EVE_ENC_BITMAP_TRANSFORM_E(p,v)     ((25UL<<24)|(((p)&1UL)<<17)|(((v)&131071UL)<<0))
#define EVE_ENC_BITMAP_TRANSFORM_F(f)       ((26UL<<24)|(((f)&16777215UL)<<0))
#define EVE_ENC_SCISSOR_XY(x,y)             ((27UL<<24)|(((x)&2047UL)<<11)|(((y)&2047UL)<<0))
#define EVE_ENC_SCISSOR_SIZE(width,height)  ((28UL<<24)|(((width)&4095UL)<<12)|(((height)&4095UL)<<0))
#define EVE_ENC_CALL(dest)                  ((29UL<<24)|(((dest)&65535UL)<<0))
#define EVE_ENC_JUMP(dest)                  ((30UL<<24)|(((dest)&65535UL)<<0))
#define EVE_ENC_BEGIN(prim)                 ((31UL<<24)|(((prim)&15UL)<<0))
#define EVE_ENC_COLOR_MASK(r,g,b,a)         ((32UL<<24)|(((r)&1UL)<<3)|(((g)&1UL)<<2)|(((b)&1UL)<<1)|(((a)&1UL)<<0))
#define EVE_ENC_END()                       ((33UL<<24))
#define EVE_ENC_SAVE_CONTEXT()              ((34UL<<24))
#define EVE_ENC_RESTORE_CONTEXT()           ((35UL<<24))
#define EVE_ENC_RETURN()                    ((36UL<<24))
#define EVE_ENC_MACRO(m)                    ((37UL<<24)|(((m)&1UL)<<0))
#define EVE_ENC_CLEAR(c,s,t)                ((38UL<<24)|(((c)&1UL)<<2)|(((s)&1UL)<<1)|(((t)&1UL)<<0))
#define EVE_ENC_VERTEX_FORMAT(frac)         ((39UL<<24)|(((frac)&7UL)<<0))
#define EVE_ENC_BITMAP_LAYOUT_H(linestride,height) ((40UL<<24)|(((linestride)&3UL)<<2)|(((height)&3UL)<<0))
#define EVE_ENC_BITMAP_SIZE_H(width,height) ((41UL<<24)|(((width)&3UL)<<2)|(((height)&3UL)<<0))
#define EVE_ENC_PALETTE_SOURCE(addr)        ((42UL<<24)|(((addr)&4194303UL)<<0))
#define EVE_ENC_VERTEX_TRANSLATE_X(x)       ((43UL<<24)|(((x)&131071UL)<<0))
#define EVE_ENC_VERTEX_TRANSLATE_Y(y)       ((44UL<<24)|(((y)&131071UL)<<0))
#define EVE_ENC_NOP()                       ((45UL<<24))
#define EVE_ENC_BITMAP_EXT_FORMAT(format)   ((46UL<<24)|(((format)&65535UL)<<0))
#define EVE_ENC_BITMAP_SWIZZLE(r,g,b,a)     ((47UL<<24)|(((r)&7UL)<<9)|(((g)&7UL)<<6)|(((b)&7UL)<<3)|(((a)&7UL)<<0))
#define EVE_ENC_DISPLAY()                   ((0UL<<24))

#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_10x10_KHR 37819UL
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_10x5_KHR  37816UL
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_10x6_KHR  37817UL
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_10x8_KHR  37818UL
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_12x10_KHR 37820UL
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_12x12_KHR 37821UL
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_4x4_KHR   37808UL
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_5x4_KHR   37809UL
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_5x5_KHR   37810UL
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_6x5_KHR   37811UL
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_6x6_KHR   37812UL
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_8x5_KHR   37813UL
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_8x6_KHR   37814UL
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_8x8_KHR   37815UL

//#define EVE_CTOUCH_MODE_COMPATIBILITY 1UL
//#define EVE_CTOUCH_MODE_EXTENDED      0UL

#define EVE_STENCIL_DECR              4UL
#define EVE_DLSWAP_DONE               0UL
#define EVE_DLSWAP_FRAME              2UL
#define EVE_DLSWAP_LINE               1UL
#define EVE_BLEND_DST_ALPHA           3UL
#define EVE_BEGIN_EDGE_STRIP_A        7UL
#define EVE_BEGIN_EDGE_STRIP_B        8UL
#define EVE_BEGIN_EDGE_STRIP_L        6UL
#define EVE_BEGIN_EDGE_STRIP_R        5UL
#define EVE_TEST_EQUAL                5UL
#define EVE_FLASH_STATUS_BASIC        2UL
#define EVE_FLASH_STATUS_DETACHED     1UL
#define EVE_FLASH_STATUS_FULL         3UL
#define EVE_FLASH_STATUS_INIT         0UL
#define EVE_TEST_GEQUAL               4UL
#define EVE_GLFORMAT                  31UL
#define EVE_TEST_GREATER              3UL
#define EVE_GREEN                     3UL
#define EVE_STENCIL_INCR              3UL
#define EVE_INT_CMDEMPTY              32UL
#define EVE_INT_CMDFLAG               64UL
#define EVE_INT_CONVCOMPLETE          128UL
#define EVE_INT_G8                    18UL
#define EVE_INT_L8C                   12UL
#define EVE_INT_PLAYBACK              16UL
#define EVE_INT_SOUND                 8UL
#define EVE_INT_SWAP                  1UL
#define EVE_INT_TAG                   4UL
#define EVE_INT_TOUCH                 2UL
#define EVE_INT_VGA                   13UL
#define EVE_STENCIL_INVERT            5UL

#define EVE_STENCIL_KEEP              1UL
#define EVE_FORMAT_L1                 1UL
#define EVE_FORMAT_L2                 17UL
#define EVE_FORMAT_L4                 2UL
#define EVE_FORMAT_L8                 3UL
#define EVE_TEST_LEQUAL               2UL
#define EVE_TEST_LESS                 1UL
#define EVE_LINEAR_SAMPLES            0UL
#define EVE_BEGIN_LINES               3UL
#define EVE_BEGIN_LINE_STRIP          4UL
#define EVE_FILTER_NEAREST            0UL
#define EVE_TEST_NEVER                0UL
#define EVE_TEST_NOTEQUAL             6UL
#define EVE_BLEND_ONE                 1UL
#define EVE_BLEND_ONE_MINUS_DST_ALPHA 5UL
#define EVE_BLEND_ONE_MINUS_SRC_ALPHA 4UL
#define EVE_OPT_CENTER                1536UL
#define EVE_OPT_CENTERX               512UL
#define EVE_OPT_CENTERY               1024UL
#define EVE_OPT_FILL                  8192UL
#define EVE_OPT_FLASH                 64UL
#define EVE_OPT_FLAT                  256UL
#define EVE_OPT_FORMAT                4096UL
#define EVE_OPT_FULLSCREEN            8UL
#define EVE_OPT_MEDIAFIFO             16UL
#define EVE_OPT_MONO                  1UL
#define EVE_OPT_NOBACK                4096UL
#define EVE_OPT_NODL                  2UL
#define EVE_OPT_NOHANDS               49152UL
#define EVE_OPT_NOHM                  16384UL
#define EVE_OPT_NOPOINTER             16384UL
#define EVE_OPT_NOSECS                32768UL
#define EVE_OPT_NOTEAR                4UL
#define EVE_OPT_NOTICKS               8192UL
#define EVE_OPT_OVERLAY               128UL
#define EVE_OPT_RIGHTX                2048UL
#define EVE_OPT_SIGNED                256UL
#define EVE_OPT_SOUND                 32UL
#define EVE_FORMAT_PALETTED           8UL
#define EVE_FORMAT_PALETTED4444       15UL
#define EVE_FORMAT_PALETTED565        14UL
#define EVE_FORMAT_PALETTED8          16UL
#define EVE_BEGIN_POINTS              2UL
#define EVE_BEGIN_RECTS               9UL
#define EVE_RED                       2UL
//#define PPC 16
//#define BANKS 16
//#define BANKW 16
#define EVE_ADC_DIFFERENTIAL          1UL
#define EVE_ADC_SINGLE_ENDED          0UL
#define EVE_ADPCM_SAMPLES             2UL
#define EVE_ALPHA                     5UL
#define EVE_TEST_ALWAYS               7UL
#define EVE_ANIM_HOLD                 2UL
#define EVE_ANIM_LOOP                 1UL
#define EVE_ANIM_ONCE                 0UL
#define EVE_FORMAT_ARGB1555           0UL
#define EVE_FORMAT_ARGB2              5UL
#define EVE_FORMAT_ARGB4              6UL
#define EVE_FORMAT_BARGRAPH           11UL
#define EVE_FILTER_BILINEAR           1UL
#define EVE_BEGIN_BITMAPS             1UL
#define EVE_BLUE                      4UL
#define EVE_WRAP_BORDER               0UL
#define EVE_WRAP_REPEAT               1UL
#define EVE_STENCIL_REPLACE           2UL
#define EVE_FORMAT_RGB332             4UL
#define EVE_FORMAT_RGB565             7UL
#define EVE_BLEND_SRC_ALPHA           2UL
#define EVE_SS_A0                     19UL
#define EVE_SS_A1                     20UL
#define EVE_SS_A2                     21UL
#define EVE_SS_A3                     22UL
#define EVE_SS_A4                     23UL
#define EVE_SS_A5                     24UL
#define EVE_SS_A6                     25UL
#define EVE_SS_A7                     26UL
#define EVE_SS_PAUSE                  18UL
#define EVE_SS_Q0                     0UL
#define EVE_SS_Q1                     1UL
#define EVE_SS_Q2                     2UL
#define EVE_SS_Q3                     3UL
#define EVE_SS_Q4                     4UL
#define EVE_SS_Q5                     5UL
#define EVE_SS_Q6                     6UL
#define EVE_SS_Q7                     7UL
#define EVE_SS_Q8                     8UL
#define EVE_SS_Q9                     9UL
#define EVE_SS_QA                     10UL
#define EVE_SS_QB                     11UL
#define EVE_SS_QC                     12UL
#define EVE_SS_QD                     13UL
#define EVE_SS_QE                     14UL
#define EVE_SS_QF                     15UL
#define EVE_SS_QI                     31UL
#define EVE_SS_S0                     16UL
#define EVE_SS_S1                     17UL
#define EVE_TEXT8EVE_SS               9UL
#define EVE_TEXTVGA                   10UL
#define EVE_TOUCHMODE_CONTINUOUS      3UL
#define EVE_TOUCHMODE_FRAME           2UL
#define EVE_TOUCHMODE_OFF             0UL
#define EVE_TOUCHMODE_ONESHOT         1UL
#define EVE_ULAW_SAMPLES              1UL
#define EVE_VOL_ZERO                  0UL

#define FTPOINTS                      POINTS

#undef EVE_ENC_BITMAP_TRANSFORM_A //New BT815 file do not compatible with the legacy code
#undef EVE_ENC_BITMAP_TRANSFORM_B //New BT815 file do not compatible with the legacy code
#undef EVE_ENC_BITMAP_TRANSFORM_D //New BT815 file do not compatible with the legacy code
#undef EVE_ENC_BITMAP_TRANSFORM_E //New BT815 file do not compatible with the legacy code

#define EVE_ENC_BITMAP_TRANSFORM_A_EXT(p,v) ((21UL<<24)|(((p)&1UL)<<17)|(((v)&131071UL)<<0))
#define EVE_ENC_BITMAP_TRANSFORM_B_EXT(p,v) ((22UL<<24)|(((p)&1UL)<<17)|(((v)&131071UL)<<0))
#define EVE_ENC_BITMAP_TRANSFORM_C(c)       ((23UL<<24)|(((c)&16777215UL)<<0))
#define EVE_ENC_BITMAP_TRANSFORM_D_EXT(p,v) ((24UL<<24)|(((p)&1UL)<<17)|(((v)&131071UL)<<0))
#define EVE_ENC_BITMAP_TRANSFORM_E_EXT(p,v) ((25UL<<24)|(((p)&1UL)<<17)|(((v)&131071UL)<<0))
#define EVE_ENC_BITMAP_TRANSFORM_F(f)       ((26UL<<24)|(((f)&16777215UL)<<0))

// Added to ensure previous macros are fine
#define EVE_ENC_BITMAP_TRANSFORM_A(a) EVE_ENC_BITMAP_TRANSFORM_A_EXT(0,a)
#define EVE_ENC_BITMAP_TRANSFORM_B(b) EVE_ENC_BITMAP_TRANSFORM_B_EXT(0,b)
#define EVE_ENC_BITMAP_TRANSFORM_D(d) EVE_ENC_BITMAP_TRANSFORM_D_EXT(0,d)
#define EVE_ENC_BITMAP_TRANSFORM_E(e) EVE_ENC_BITMAP_TRANSFORM_E_EXT(0,e)

#if defined (EVE4_ENABLE)

#define EVE_REG_UNDERRUN            0x30260C
#define EVE_REG_AH_HCYCLE_MAX       0x302610
#define EVE_REG_PCLK_FREQ           0x302614
#define EVE_REG_PCLK_2X             0x302618
#define EVE_ENC_CMD_ANIMFRAMERAM    0xFFFFFF6D
#define EVE_ENC_CMD_ANIMSTARTRAM    0xFFFFFF6E
#define EVE_ENC_CMD_RUNANIM         0xFFFFFF6F
#define EVE_ENC_CMD_APILEVEL        0xFFFFFF63
#define EVE_ENC_CMD_CALIBRATESUB    0xFFFFFF60
#define EVE_ENC_CMD_TESTCARD        0xFFFFFF61
#define EVE_ENC_CMD_WAIT            0xFFFFFF65
#define EVE_ENC_CMD_NEWLIST         0xFFFFFF68
#define EVE_ENC_CMD_ENDLIST         0xFFFFFF69
#define EVE_ENC_CMD_CALLLIST        0xFFFFFF67
#define EVE_ENC_CMD_RETURN          0xFFFFFF66
#define EVE_ENC_CMD_FONTCACHE       0xFFFFFF6B
#define EVE_ENC_CMD_FONTCACHEQUERY  0xFFFFFF6C
#define EVE_ENC_CMD_GETIMAGE        0xFFFFFF64
#define EVE_ENC_CMD_HSF             0xFFFFFF62
#define EVE_ENC_CMD_PCLKFREQ        0xFFFFFF6A

#endif    // defined EVE4_ENABLE

 //End BT81x parameters
#else

#error No EVE support level selected. EVE1_ENABLE, EVE2_ENABLE, EVE3_ENABLE or EVE4_ENABLE required.

#endif

#define FT_GPU_NUMCHAR_PERFONT (128)
#define FT_GPU_FONT_TABLE_SIZE (148)

/* BT81x, FT81x and FT80x font table structure */
/* Font table address in ROM can be found by reading the address from 0xFFFFC location. */
/* 16 font tables are present at the address read from location 0xFFFFC. */
/* On EVE4 this is called the Legacy Font Structure. */
typedef struct
{
	/* All the values are in bytes */
	/* Width of each character font from 0 to 127 */
	uint8_t	FontWidth[FT_GPU_NUMCHAR_PERFONT];
	/* Bitmap format of font wrt bitmap formats supported by FT800 - L1, L4, L8 */
	uint32_t	FontBitmapFormat;
	/* Font line stride in FT800 ROM */
	uint32_t	FontLineStride;
	/* Font width in pixels */
	uint32_t	FontWidthInPixels;
	/* Font height in pixels */
	uint32_t	FontHeightInPixels;
	/* Pointer to font graphics raw data */
	uint32_t	PointerToFontGraphicsData;
} EVE_GPU_FONT_HEADER;

/* Nothing beyond this */

#endif	/* _FT8XX_H_ */

