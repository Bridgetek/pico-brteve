/**
 * @file EVE_Config.h
 * @brief This file processes the input definitions
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

#ifndef EVE_CONFIG__H
#define EVE_CONFIG__H

#include "EVE_IntTypes.h"

/*

This file processes the input definitions, 
the available list of which is specified further below for ESD using these macros.
- ESD_TARGET_GRAPHICS(definition, ...)
- ESD_TARGET_DISPLAY(definition, ...)
- ESD_TARGET_PLATFORM(definition, ...)
- ESD_TARGET_FLASH(definition, ...)

*/

#define ESD_TARGET_GRAPHICS(name, ...)
#define ESD_TARGET_DISPLAY(name, ...)
#define ESD_TARGET_PLATFORM(name, ...)
#define ESD_TARGET_FLASH(name, ...)
#define ESD_TARGET_TOUCH(name, ...)

// Chip IDs with EVE generation
#define EVE_FT800 0x10800
#define EVE_FT801 0x10801
#define EVE_FT810 0x20810
#define EVE_FT811 0x20811
#define EVE_FT812 0x20812
#define EVE_FT813 0x20813
#define EVE_BT880 0x20880
#define EVE_BT881 0x20881
#define EVE_BT882 0x20882
#define EVE_BT883 0x20883
#define EVE_BT815 0x30815
#define EVE_BT816 0x30816
#define EVE_BT817 0x40817
#define EVE_BT817A 0x4817A
#define EVE_BT818 0x40818

// EVE generations
#define EVE1 1 /* FT800 and FT801 */
#define EVE2 2 /* FT810 to FT813, and BT880 to BT883 */
#define EVE3 3 /* BT815 and BT816 */
#define EVE4 4 /* BT817, BT817A and BT818 */

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

/*

Uses regex matching for supported targets.
See https://regexr.com/ for help on regex.

To match only PANL70:
\bPANL70\b
To match platforms starting with MM9
\bMM9
\bMM9\w*\b
Match anything (default):
\b\w+\b
Match nothing:
(?=a)b

IntegratedPlatform specifies that the platform chip is part of the target, the specified string is a display name
IntegratedFlash specifies that the flash chip is part of the target, the specified string is a display name
OverrideSupport specifies that the supported flash of the target overrides the Supported option
of any other target, overriding the Integrated options as well

Example:
SupportedPlatforms = "8XXEMU|\bMM9|FT4222|MPSSE"
SupportedFlash = "\bW25Q\w*\b|\bMX25L\w*\b

*/

ESD_TARGET_GRAPHICS(ME810A_HV35R, DisplayName = "ME810A-HV35R", IntegratedDisplay = "ILI9488 (320x480)", SupportedFlash = "(?=a)b", SupportedTouch = "\b\w+RESISTIVE\w*\b", LibraryTargets = "\b(FT81X|FT810)\b")
ESD_TARGET_GRAPHICS(ME812A_WH50R, DisplayName = "ME812A-WH50R", SupportedDisplays = "\b\w+WVGA\w*\b", SupportedFlash = "(?=a)b", SupportedTouch = "\b\w+RESISTIVE\w*\b", LibraryTargets = "\b(FT81X|FT812)\b")
ESD_TARGET_GRAPHICS(ME813A_WH50C, DisplayName = "ME813A-WH50C", SupportedDisplays = "\b\w+WVGA\w*\b", SupportedFlash = "(?=a)b", SupportedTouch = "\b\w+FOCAL\w*\b", LibraryTargets = "\b(FT81X|FT813)\b")
// ESD_TARGET_GRAPHICS(ME810AU_HV35R, DisplayName = "ME810AU-HV35R", IntegratedDisplay = "ILI9488 (320x480)", IntegratedPlatform = "FT4222", SupportedArchitectures = "\bFT4222\b", SupportedFlash = "(?=a)b", LibraryTargets = "\b(FT81X|FT810)\b")
// ESD_TARGET_GRAPHICS(ME812AU_WH50R, DisplayName = "ME812AU-WH50R", SupportedDisplays = "\b\w+WVGA\w*\b", IntegratedPlatform = "FT4222", SupportedArchitectures = "\bFT4222\b", SupportedFlash = "(?=a)b", LibraryTargets = "\b(FT81X|FT812)\b")
// ESD_TARGET_GRAPHICS(ME813AU_WH50C, DisplayName = "ME813AU-WH50C", SupportedDisplays = "\b\w+WVGA\w*\b", IntegratedPlatform = "FT4222", SupportedArchitectures = "\bFT4222\b", SupportedFlash = "(?=a)b", LibraryTargets = "\b(FT81X|FT813)\b")
// ESD_TARGET_GRAPHICS(PANL35, DisplayName = "PanL35", IntegratedDisplay = "KD2401 (320x480)", IntegratedPlatform = "FT903", SupportedArchitectures = "\bFT32\b", SupportedFlash = "(?=a)b", LibraryTargets = "\b(FT81X|FT811)\b")
// ESD_TARGET_GRAPHICS(PANL50, DisplayName = "PanL50", SupportedDisplays = "\b\w+WVGA\w*\b", IntegratedPlatform = "MM900EV1B", SupportedArchitectures = "\bFT32\b", SupportedFlash = "(?=a)b", LibraryTargets = "\b(FT81X|FT813)\b")
// ESD_TARGET_GRAPHICS(PANL70, DisplayName = "PanL70", IntegratedDisplay = "WVGA (800x480)", IntegratedPlatform = "FT930", SupportedArchitectures = "\bFT32\b", SupportedFlash = "(?=a)b", LibraryTargets = "\b(FT81X|FT811)\b")
// ESD_TARGET_GRAPHICS(PANL70PLUS, DisplayName = "PanL70 Plus", IntegratedDisplay = "WVGA (800x480)", IntegratedPlatform = "FT930", SupportedArchitectures = "\bFT32\b", SupportedFlash = "(?=a)b", LibraryTargets = "\b(FT81X|FT811)\b")
// ESD_TARGET_GRAPHICS(EVE_GRAPHICS_VM800C, DisplayName = "VM800C", SupportedFlash = "(?=a)b", LibraryTargets = "\b(FT80X|FT800)\b")
ESD_TARGET_GRAPHICS(EVE_GRAPHICS_VM810C, DisplayName = "VM810C", SupportedFlash = "(?=a)b", SupportedTouch = "\b\w+RESISTIVE\w*\b", LibraryTargets = "\b(FT81X|FT810)\b")
ESD_TARGET_GRAPHICS(EVE_GRAPHICS_VM816C, DisplayName = "VM816C", IntegratedFlash = "W25Q128", SupportedTouch = "\b\w+RESISTIVE\w*\b", LibraryTargets = "\b(BT81X|BT816)\b", FirmwareFolder = "BT815")
ESD_TARGET_GRAPHICS(EVE_GRAPHICS_ME817EV, DisplayName = "ME817EV", IntegratedFlash = "W25Q128", SupportedTouch = "\b\w+FOCAL\w*\b|\b\w+GOODIX\w*\b", LibraryTargets = "\b(BT81X|BT818)\b", FirmwareFolder = "BT817")

ESD_TARGET_GRAPHICS(EVE_GRAPHICS_GD3X_DAZZLER, DisplayName = "GD3X Dazzler", IntegratedDisplay = "HDMI 720p (1280x720)", IntegratedFlash = "W25Q64", SupportedTouch = "\b\w+DISABLED\w*\b", LibraryTargets = "\b(BT81X|BT815)\b", FirmwareFolder = "BT815")
ESD_TARGET_GRAPHICS(EVE_GRAPHICS_IDM2040, DisplayName = "IDM2040", IntegratedFlash = "W25Q128", SupportedTouch = "\b\w+FOCAL\w*\b|\b\w+GOODIX\w*\b", IntegratedPlatform = "IDM2040EV", LibraryTargets = "\b(BT81X|BT818)\b", FirmwareFolder = "BT817")

// ESD_TARGET_GRAPHICS(EVE_GRAPHICS_FT800, DisplayName = "FT800 (Generic)", SupportedDisplays = "\b\w+(QVGA|HVGA|AT043B35)\w*\b", SupportedTouch = "\b\w+RESISTIVE\w*\b", LibraryTargets="\b(FT80X|FT800)\b")
// ESD_TARGET_GRAPHICS(EVE_GRAPHICS_FT801, DisplayName = "FT801 (Generic)", SupportedDisplays = "\b\w+(QVGA|HVGA|AT043B35)\w*\b", SupportedTouch = "\b\w+FOCAL\w*\b", LibraryTargets="\b(FT80X|FT801)\b")
// ESD_TARGET_GRAPHICS(EVE_GRAPHICS_FT810, DisplayName = "FT810 (Generic)", SupportedTouch = "\b\w+RESISTIVE\w*\b", LibraryTargets="\b(FT81X|FT810)\b")
// ESD_TARGET_GRAPHICS(EVE_GRAPHICS_FT811, DisplayName = "FT811 (Generic)", SupportedTouch = "\b\w+FOCAL\w*\b", LibraryTargets="\b(FT81X|FT811)\b")
// ESD_TARGET_GRAPHICS(EVE_GRAPHICS_FT812, DisplayName = "FT812 (Generic)", SupportedTouch = "\b\w+RESISTIVE\w*\b", LibraryTargets="\b(FT81X|FT812)\b")
// ESD_TARGET_GRAPHICS(EVE_GRAPHICS_FT813, DisplayName = "FT813 (Generic)", SupportedTouch = "\b\w+FOCAL\w*\b", LibraryTargets="\b(FT81X|FT813)\b")
// ESD_TARGET_GRAPHICS(EVE_GRAPHICS_BT815, DisplayName = "BT815 (Generic)", SupportedTouch = "\b\w+FOCAL\w*\b|\b\w+GOODIX\w*\b", LibraryTargets="\b(BT81X|BT815)\b")
// ESD_TARGET_GRAPHICS(EVE_GRAPHICS_BT816, DisplayName = "BT816 (Generic)", SupportedTouch = "\b\w+RESISTIVE\w*\b", LibraryTargets="\b(BT81X|BT816)\b")

/* Landscape */
ESD_TARGET_DISPLAY(EVE_DISPLAY_QVGA, DisplayName = "QVGA (320x240)")
ESD_TARGET_DISPLAY(EVE_DISPLAY_WQVGA, DisplayName = "WQVGA (480x272)")
ESD_TARGET_DISPLAY(EVE_DISPLAY_WVGA, DisplayName = "WVGA (800x480)")
ESD_TARGET_DISPLAY(EVE_DISPLAY_WSVGA, DisplayName = "WSVGA (1024x600)")
ESD_TARGET_DISPLAY(EVE_DISPLAY_WXGA, DisplayName = "WXGA (1280x800)")
// ESD_TARGET_DISPLAY(EVE_DISPLAY_AT043B35, DisplayName = "AT043B35 (480x272)")

/* Portrait */
// ESD_TARGET_DISPLAY(EVE_DISPLAY_ILI9488_HVGA_PORTRAIT, DisplayName = "ILI9488 (320x480)")
// ESD_TARGET_DISPLAY(EVE_DISPLAY_KD2401_HVGA_PORTRAIT, DisplayName = "KD2401 (320x480)")

ESD_TARGET_PLATFORM(EVE_PLATFORM_BT8XXEMU, DisplayName = "Emulator", Icon = ":/icons/game-monitor.png", OverrideSupport, SupportedFlash = "\b\w*_W25Q\w*\b|\b\w*_MX25L\w*\b", SupportedArchitectures = "\bBT8XXEMU\b")
ESD_TARGET_PLATFORM(MM900EV1A, SupportedArchitectures = "\bFT32\b")
ESD_TARGET_PLATFORM(MM900EV1B, SupportedArchitectures = "\bFT32\b")
ESD_TARGET_PLATFORM(MM900EV2A, SupportedArchitectures = "\bFT32\b")
ESD_TARGET_PLATFORM(MM900EV3A, SupportedArchitectures = "\bFT32\b")
ESD_TARGET_PLATFORM(MM900EV_LITE, DisplayName = "MM900EV-Lite", SupportedArchitectures = "\bFT32\b")
// ESD_TARGET_PLATFORM(MM930MINI, DisplayName = "MM930Mini", SupportedArchitectures = "\bFT32\b")
// ESD_TARGET_PLATFORM(MM930LITE, DisplayName = "MM930Lite", SupportedArchitectures = "\bFT32\b")
// ESD_TARGET_PLATFORM(MM932LC, DisplayName = "MM932LC", SupportedArchitectures = "\bFT32\b")
ESD_TARGET_PLATFORM(EVE_PLATFORM_FT4222, DisplayName = "FT4222", Icon = ":/icons/terminal.png", SupportedArchitectures = "\bFT4222\b")
ESD_TARGET_PLATFORM(EVE_PLATFORM_MPSSE, DisplayName = "MPSSE", Icon = ":/icons/terminal.png", SupportedArchitectures = "\bMPSSE\b")
ESD_TARGET_PLATFORM(EVE_PLATFORM_RP2040, DisplayName = "Rapberry Pi Pico", SupportedArchitectures = "\bPICO\b")
ESD_TARGET_PLATFORM(EVE_PLATFORM_MM2040EV, DisplayName = "MM2040EV", SupportedArchitectures = "\bPICO\b")

ESD_TARGET_FLASH(EVE_FLASH_W25Q16, DisplayName = "W25Q16")
ESD_TARGET_FLASH(EVE_FLASH_W25Q32, DisplayName = "W25Q32")
ESD_TARGET_FLASH(EVE_FLASH_W25Q64, DisplayName = "W25Q64")
ESD_TARGET_FLASH(EVE_FLASH_W25Q128, DisplayName = "W25Q128")

ESD_TARGET_FLASH(EVE_FLASH_MX25L16, DisplayName = "MX25L16")
ESD_TARGET_FLASH(EVE_FLASH_MX25L32, DisplayName = "MX25L32")
ESD_TARGET_FLASH(EVE_FLASH_MX25L64, DisplayName = "MX25L64")
ESD_TARGET_FLASH(EVE_FLASH_MX25L128, DisplayName = "MX25L128")
ESD_TARGET_FLASH(EVE_FLASH_MX25L256, DisplayName = "MX25L256", FlashFirmware = "unified.blob")
ESD_TARGET_FLASH(EVE_FLASH_MX25L512, DisplayName = "MX25L512", FlashFirmware = "unified.blob")
ESD_TARGET_FLASH(EVE_FLASH_MX25L1024, DisplayName = "MX25L1024", FlashFirmware = "unified.blob")
ESD_TARGET_FLASH(EVE_FLASH_MX25L2048, DisplayName = "MX25L2048", FlashFirmware = "unified.blob")

ESD_TARGET_TOUCH(EVE_TOUCH_FOCAL, DisplayName = "FocalTech")
ESD_TARGET_TOUCH(EVE_TOUCH_GOODIX, DisplayName = "Goodix")
ESD_TARGET_TOUCH(EVE_TOUCH_RESISTIVE, DisplayName = "Resistive")
ESD_TARGET_TOUCH(EVE_TOUCH_DISABLED, DisplayName = "Disabled")

/*

The following values are set based on the input definitions.
Do not set these values manually; instead, use the specific user definitions.

Graphics target:
- EVE_SUPPORT_CHIPID=EVE_FT800
- EVE_SUPPORT_CHIPID=EVE_FT801
- EVE_SUPPORT_CHIPID=EVE_FT810
- EVE_SUPPORT_CHIPID=EVE_FT811
- EVE_SUPPORT_CHIPID=EVE_FT812
- EVE_SUPPORT_CHIPID=EVE_FT813
- EVE_SUPPORT_CHIPID=EVE_BT880
- EVE_SUPPORT_CHIPID=EVE_BT881
- EVE_SUPPORT_CHIPID=EVE_BT882
- EVE_SUPPORT_CHIPID=EVE_BT883
- EVE_SUPPORT_CHIPID=EVE_BT815
- EVE_SUPPORT_CHIPID=EVE_BT816
- EVE_SUPPORT_CHIPID=EVE_BT817
- EVE_SUPPORT_CHIPID=EVE_BT817A
- EVE_SUPPORT_CHIPID=EVE_BT818

Platform target:
- BT8XXEMU_PLATFORM (set EVE_PLATFORM_BT8XXEMU)
- FT9XX_PLATFORM (set MM900EV1A, etc.)
- FT4222_PLATFORM (set EVE_PLATFORM_FT4222)
- MPSSE_PLATFORM (set EVE_PLATFORM_MPSSE)
- RP2040_PLATFORM (set EVE_PLATFORM_RP2040)

Display resolution:
- DISPLAY_RESOLUTION_QVGA
- DISPLAY_RESOLUTION_WQVGA
- DISPLAY_RESOLUTION_WVGA
- DISPLAY_RESOLUTION_WSVGA
- DISPLAY_RESOLUTION_WXGA
- DISPLAY_RESOLUTION_HVGA_PORTRAIT

Flash, with size in megabytes:
- EVE_FLASH_AVAILABLE
- EVE_FLASH_SIZE=16

Additionally, the following support flags are set:
- EVE_SUPPORT_FLASH (Set even if EVE_FLASH_AVAILABLE is not set)
- EVE_SUPPORT_UNICODE
- EVE_SUPPORT_ASTC
- EVE_SUPPORT_PNG
- EVE_SUPPORT_VIDEO
- EVE_SUPPORT_CMDB
- EVE_SUPPORT_MEDIAFIFO
- EVE_SUPPORT_CAPACITIVE
- EVE_SUPPORT_RESISTIVE

*/

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

/*

Remap options.

*/

#ifdef ME810AU_HV35R
#define ME810A_HV35R
#endif
#ifdef ME812AU_WH50R
#define ME812A_WH50R
#endif
#ifdef ME813AU_WV7C
#define ME813A_WV7C
#endif
#ifdef ME813AU_WH50C
#define ME813A_WH50C
#endif
#ifdef VM800C35A_D /* This module has 1 integrated LCD*/
#define VM800C35A_N /* This module is same but no integrated LCD*/
#endif
#ifdef VM800C43A_D /* This module has 1 integrated LCD*/
#define VM800C43A_N /* This module is same but no integrated LCD*/
#endif
#ifdef VM816C50A_D /* This module has 1 integrated LCD*/
#define VM816C50A_N /* This module is same but no integrated LCD*/
#endif
#ifdef VM816CU50A_D /* This module has 1 integrated LCD*/
#define VM816CU50A_N /* This module is same but no integrated LCD*/
#endif
#ifdef VM810C50A_D /* This module has 1 more integrated LCD*/
#define VM810C50A_N /* This module is same but no integrated LCD*/
#endif

/* 

Remap manual use of internal macros, internal macros are mapped at the end of this file (due to ESD_SIMULATION overrides)
(Conditional code should check the internal macros MPSSE_PLATFORM, etc. and not the user-specified macros.)

*/
#if defined(BT8XXEMU_PLATFORM)
#undef BT8XXEMU_PLATFORM
#define EVE_PLATFORM_BT8XXEMU
#elif defined(MPSSE_PLATFORM)
#undef MPSSE_PLATFORM
#define EVE_PLATFORM_MPSSE
#elif defined(FT4222_PLATFORM)
#undef FT4222_PLATFORM
#define EVE_PLATFORM_FT4222
#endif

/*

Validate the configured options.

*/
#if defined(VM800B35A_BK) || defined(VM800B43A_BK)                   \
    || defined(VM800B50A_BK) || defined(VM801B43A_BK)                \
    || defined(VM801B50A_BK) || defined(VM800C35A_N)                 \
    || defined(VM800C43A_N) || defined(VM800C35A_D)                  \
    || defined(VM800C43A_D) || defined(VM800C50A_D)                  \
    || defined(ME812A_WH50R) || defined(ME812AU_WH50R)               \
    || defined(ME813A_WH50C) || defined(ME813AU_WH50C)               \
    || defined(VM810C50A_D) || defined(VM810C50A_N)                  \
    || defined(ME810A_HV35R) || defined(VM816C50A_D)                 \
    || defined(VM816C50A_N) || defined(VM816CU50A_D)                 \
    || defined(VM816CU50A_N) || defined(ME810AU_HV35R)               \
    || defined(ME813A_WV7C) || defined(EVE_MODULE_PANL)              \
    || defined(PANL50)                                               \
    || defined(EVE_GRAPHICS_VM800C) || defined(EVE_GRAPHICS_VM810C)  \
    || defined(EVE_GRAPHICS_VM816C) || defined(EVE_GRAPHICS_ME817EV) \
    || defined(EVE_GRAPHICS_FT800) || defined(EVE_GRAPHICS_FT801)    \
    || defined(EVE_GRAPHICS_FT810) || defined(EVE_GRAPHICS_FT811)    \
    || defined(EVE_GRAPHICS_FT812) || defined(EVE_GRAPHICS_FT813)    \
    || defined(EVE_GRAPHICS_BT880) || defined(EVE_GRAPHICS_BT881)    \
    || defined(EVE_GRAPHICS_BT882) || defined(EVE_GRAPHICS_BT883)    \
    || defined(EVE_GRAPHICS_BT815) || defined(EVE_GRAPHICS_BT816)    \
    || defined(EVE_GRAPHICS_BT817) || defined(EVE_GRAPHICS_BT817A)   \
	|| defined(EVE_GRAPHICS_BT818)                                   \
    || defined(EVE_GRAPHICS_GD3X_DAZZLER)                            \
    || defined(EVE_GRAPHICS_IDM2040)
#define EVE_GRAPHICS_AVAILABLE
#endif

#if defined(EVE_DISPLAY_QVGA) || defined(EVE_DISPLAY_WQVGA)    \
    || defined(EVE_DISPLAY_WVGA) || defined(EVE_DISPLAY_WSVGA) \
    || defined(EVE_DISPLAY_WXGA)                               \
    || defined(EVE_DISPLAY_ILI9488_HVGA_PORTRAIT)              \
    || defined(EVE_DISPLAY_KD2401_HVGA_PORTRAIT)
#define EVE_DISPLAY_AVAILABLE
#endif

#if defined(ESD_SIMULATION) || defined(EVE_PLATFORM_BT8XXEMU)      \
    || defined(MM900EV1A) || defined(MM900EV1B)                    \
    || defined(MM900EV2A) || defined(MM900EV3A)                    \
    || defined(MM900EV_LITE) || defined(MM930MINI)                 \
    || defined(MM930LITE) || defined(MM932LC)                      \
    || defined(EVE_PLATFORM_FT4222) || defined(EVE_PLATFORM_MPSSE) \
    || defined(EVE_PLATFORM_RP2040) || defined(EVE_PLATFORM_MM2040EV)
#define EVE_PLATFORM_AVAILABLE
#endif

#if defined(EVE_FLASH_W25Q16) || defined(EVE_FLASH_W25Q32)        \
    || defined(EVE_FLASH_W25Q64) || defined(EVE_FLASH_W25Q128)    \
    || defined(EVE_FLASH_MX25L16) || defined(EVE_FLASH_MX25L32)   \
    || defined(EVE_FLASH_MX25L64) || defined(EVE_FLASH_MX25L128)  \
    || defined(EVE_FLASH_MX25L256) || defined(EVE_FLASH_MX25L512) \
    || defined(EVE_FLASH_MX25L1024) || defined(EVE_FLASH_MX25L2048)
#define EVE_FLASH_AVAILABLE
#endif

#if defined(EVE_TOUCH_FOCAL) || defined(EVE_TOUCH_GOODIX) || defined(EVE_TOUCH_RESISTIVE) || defined(EVE_TOUCH_DISABLED)
#define EVE_TOUCH_AVAILABLE
#endif

#if !defined(EVE_PLATFORM_AVAILABLE) && defined(WIN32)
#define EVE_MULTI_PLATFORM_TARGET
#endif

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

/*

The selected graphics module below must set one of the following options.
(Users must select the graphics module definition, *not* one of these enable options.)
- FT800_ENABLE
- FT801_ENABLE
- FT810_ENABLE
- FT811_ENABLE
- FT812_ENABLE
- FT813_ENABLE
- BT880_ENABLE
- BT881_ENABLE
- BT882_ENABLE
- BT883_ENABLE
- BT815_ENABLE
- BT816_ENABLE
- BT817_ENABLE
- BT817A_ENABLE
- BT818_ENABLE

It may also set platform, display, and flash values if none are configured.

*/
#if defined(VM800P35A_BK)
#Error Host ATMEGA328P is not supported

#elif defined(VM800P43A_BK)
#Host ATMEGA328P is not supported

#elif defined(VM800P50A_BK)
#Host ATMEGA328P is not supported

#elif defined(VM801P43A_BK)
#Host ATMEGA328P is not supported

#elif defined(VM801P50A_BK)
#Host ATMEGA328P is not supported

#elif defined(VM800B35A_BK) /* EVE module */
#define FT800_ENABLE
#define ENABLE_SPI_SINGLE
#define RESISTANCE_THRESHOLD (1800)

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_QVGA
#endif

#ifndef EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_RESISTIVE
#endif

#if !defined(EVE_PLATFORM_AVAILABLE) && !defined(EVE_MULTI_PLATFORM_TARGET)
#define EVE_PLATFORM_AVAILABLE
#define EVE_PLATFORM_MPSSE
#endif

#elif defined(VM800B43A_BK) || defined(VM800B50A_BK) || defined(VM800C43A_D) || defined(VM800C50A_D) /* EVE module */
#define FT800_ENABLE
#define ENABLE_SPI_SINGLE
#define RESISTANCE_THRESHOLD (1800)

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_WQVGA
#endif

#ifndef EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_FOCAL
#endif

#if !defined(EVE_PLATFORM_AVAILABLE) && !defined(EVE_MULTI_PLATFORM_TARGET)
#define EVE_PLATFORM_AVAILABLE
#define EVE_PLATFORM_MPSSE
#endif

#elif defined(VM801B43A_BK) || defined(VM801B50A_BK) /* EVE module */
#define FT801_ENABLE
#define ENABLE_SPI_SINGLE

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_WQVGA
#endif

#ifndef EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_FOCAL
#endif

#if !defined(EVE_PLATFORM_AVAILABLE) && !defined(EVE_MULTI_PLATFORM_TARGET)
#define EVE_PLATFORM_AVAILABLE
#define EVE_PLATFORM_MPSSE
#endif

#elif defined(VM800C35A_N) /* EVE module */
/* This module has no integrated LCD */
#define FT800_ENABLE
#define ENABLE_SPI_SINGLE
#define RESISTANCE_THRESHOLD (1800)

#if !defined(EVE_PLATFORM_AVAILABLE) && !defined(EVE_MULTI_PLATFORM_TARGET)
#define EVE_PLATFORM_AVAILABLE
#define EVE_PLATFORM_MPSSE
#endif

#if defined(VM800C35A_D)

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_QVGA
#endif

#ifndef EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_RESISTIVE
#endif

#endif

#elif defined(VM800C43A_N) /* EVE module */
/* This module has no integrated LCD */
#define FT800_ENABLE
#define ENABLE_SPI_SINGLE
#define RESISTANCE_THRESHOLD (1800)

#if !defined(EVE_PLATFORM_AVAILABLE) && !defined(EVE_MULTI_PLATFORM_TARGET)
#define EVE_PLATFORM_AVAILABLE
#define EVE_PLATFORM_MPSSE
#endif

#elif defined(ME812A_WH50R) /* EVE 2 module */

#define FT812_ENABLE
#define ENABLE_SPI_QUAD
#define RESISTANCE_THRESHOLD (1800)
#define EVE_USE_INTERNAL_OSC

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_WVGA
#endif

#ifdef ME812AU_WH50R

#if !defined(EVE_PLATFORM_AVAILABLE) && !defined(EVE_MULTI_PLATFORM_TARGET)
#define EVE_PLATFORM_AVAILABLE
#define EVE_PLATFORM_FT4222
#endif

#ifndef EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_RESISTIVE
#endif

#endif

#elif defined(ME813A_WH50C) /* EVE 2 module */

#define FT813_ENABLE
#define ENABLE_SPI_QUAD
#define EVE_USE_INTERNAL_OSC

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_WVGA
#endif

#ifdef ME813AU_WH50C

#if !defined(EVE_PLATFORM_AVAILABLE) && !defined(EVE_MULTI_PLATFORM_TARGET)
#define EVE_PLATFORM_AVAILABLE
#define EVE_PLATFORM_FT4222
#endif

#ifndef EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_FOCAL
#endif

#endif

#elif defined(VM810C50A_N) /* EVE 2 module */

#define FT810_ENABLE
#define ENABLE_SPI_SINGLE

#if !defined(EVE_PLATFORM_AVAILABLE) && !defined(EVE_MULTI_PLATFORM_TARGET)
#define EVE_PLATFORM_AVAILABLE
#define EVE_PLATFORM_MPSSE
#endif

#if defined(VM810C50A_D)

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_WVGA
#endif

#ifndef EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_RESISTIVE
#endif

#endif

#elif defined(ME810A_HV35R) /* EVE 2 module */
#define FT810_ENABLE
#define ENABLE_SPI_QUAD
#define RESISTANCE_THRESHOLD (1800)
#define EVE_USE_INTERNAL_OSC

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_HVGA_PORTRAIT
#define ENABLE_ILI9488_HVGA_PORTRAIT
#endif

#ifdef ME810AU_HV35R

#if !defined(EVE_PLATFORM_AVAILABLE) && !defined(EVE_MULTI_PLATFORM_TARGET)
#define EVE_PLATFORM_AVAILABLE
#define EVE_PLATFORM_FT4222
#endif

#ifndef EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_RESISTIVE
#endif

#endif

#elif defined(VM816C50A_N) /* EVE 3 module */

#define BT816_ENABLE
#define ENABLE_SPI_SINGLE
#define RESISTANCE_THRESHOLD (1800)

#if !defined(EVE_PLATFORM_AVAILABLE) && !defined(EVE_MULTI_PLATFORM_TARGET)
#define EVE_PLATFORM_AVAILABLE
#define EVE_PLATFORM_MPSSE
#endif

#ifndef EVE_FLASH_AVAILABLE
#define EVE_FLASH_AVAILABLE
#define EVE_FLASH_MX25L128
#endif

#if defined(VM816C50A_D) /* EVE 3 module */

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_WVGA
#endif

#ifndef EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_RESISTIVE
#endif

#endif

#elif defined(VM816CU50A_N) /* EVE 3 module */

#define BT816_ENABLE
#define ENABLE_SPI_QUAD
#define RESISTANCE_THRESHOLD (1800)

#if !defined(EVE_PLATFORM_AVAILABLE) && !defined(EVE_MULTI_PLATFORM_TARGET)
#define EVE_PLATFORM_AVAILABLE
#define EVE_PLATFORM_FT4222
#endif

#ifndef EVE_FLASH_AVAILABLE
#define EVE_FLASH_AVAILABLE
#define EVE_FLASH_MX25L128
#endif

#if defined(VM816CU50A_D) /* EVE 3 module */

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_WVGA
#endif

#ifndef EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_RESISTIVE
#endif

#endif

#elif defined(ME813A_WV7C)

#define FT813_ENABLE
#define ENABLE_SPI_QUAD

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_WVGA
#endif

#ifdef ME813AU_WV7C

#if !defined(EVE_PLATFORM_AVAILABLE) && !defined(EVE_MULTI_PLATFORM_TARGET)
#define EVE_PLATFORM_AVAILABLE
#define EVE_PLATFORM_FT4222
#endif

#ifndef EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_FOCAL
#endif

#endif

#elif defined(EVE_GRAPHICS_VM816C)

#define BT816_ENABLE
#define ENABLE_SPI_QUAD
#define RESISTANCE_THRESHOLD (1800)

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_WVGA
#endif

#if !defined(EVE_PLATFORM_AVAILABLE) && !defined(EVE_MULTI_PLATFORM_TARGET)
#define EVE_PLATFORM_AVAILABLE
#define EVE_PLATFORM_FT4222
#endif

#ifndef EVE_FLASH_AVAILABLE
#define EVE_FLASH_AVAILABLE
#define EVE_FLASH_W25Q128
#endif

#ifndef EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_RESISTIVE
#endif

#elif defined(EVE_GRAPHICS_ME817EV)

#define BT817_ENABLE
#define ENABLE_SPI_QUAD

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_WVGA
#endif

#if !defined(EVE_PLATFORM_AVAILABLE) && !defined(EVE_MULTI_PLATFORM_TARGET)
#define EVE_PLATFORM_AVAILABLE
#define EVE_PLATFORM_FT4222
#endif

#ifndef EVE_FLASH_AVAILABLE
#define EVE_FLASH_AVAILABLE
#define EVE_FLASH_W25Q128
#endif

#ifndef EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_AVAILABLE
#ifdef DISPLAY_RESOLUTION_WXGA
#define EVE_TOUCH_GOODIX
#else
#define EVE_TOUCH_FOCAL
#endif
#endif

#elif defined(EVE_GRAPHICS_VM810C)

#define FT810_ENABLE
#define ENABLE_SPI_QUAD
#define RESISTANCE_THRESHOLD (1800)

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_WVGA
#endif

#if !defined(EVE_PLATFORM_AVAILABLE) && !defined(EVE_MULTI_PLATFORM_TARGET)
#define EVE_PLATFORM_AVAILABLE
#define EVE_PLATFORM_FT4222
#endif

#ifndef EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_RESISTIVE
#endif

#elif defined(EVE_GRAPHICS_VM800C)

#define FT800_ENABLE
#define ENABLE_SPI_QUAD
#define RESISTANCE_THRESHOLD (1800)

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_WQVGA
#endif

#if !defined(EVE_PLATFORM_AVAILABLE) && !defined(EVE_MULTI_PLATFORM_TARGET)
#define EVE_PLATFORM_AVAILABLE
#define EVE_PLATFORM_MPSSE
#endif

#ifndef EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_RESISTIVE
#endif

#elif defined(EVE_GRAPHICS_GD3X_DAZZLER)

#define BT815_ENABLE
#define ENABLE_SPI_QUAD
#define EVE_USE_INTERNAL_OSC

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_HDTV
#endif

#if !defined(EVE_PLATFORM_AVAILABLE) && !defined(EVE_MULTI_PLATFORM_TARGET)
#define EVE_PLATFORM_AVAILABLE
#define EVE_PLATFORM_RP2040
#endif

#ifndef EVE_FLASH_AVAILABLE
#define EVE_FLASH_AVAILABLE
#define EVE_FLASH_W25Q64
#endif

#ifndef EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_DISABLED
#endif

#elif defined(EVE_GRAPHICS_IDM2040)

#define BT817_ENABLE
#define ENABLE_SPI_QUAD

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_WVGA
#endif

#if !defined(EVE_PLATFORM_AVAILABLE) && !defined(EVE_MULTI_PLATFORM_TARGET)
#define EVE_PLATFORM_AVAILABLE
#define EVE_PLATFORM_RP2040
#endif

#ifndef EVE_FLASH_AVAILABLE
#define EVE_FLASH_AVAILABLE
#define EVE_FLASH_W25Q128
#endif

#ifndef EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_AVAILABLE
#ifdef DISPLAY_RESOLUTION_WXGA
#define EVE_TOUCH_GOODIX
#else
#define EVE_TOUCH_FOCAL
#endif
#endif

#elif defined(PANL35)

#define FT811_ENABLE
#define ENABLE_SPI_QUAD

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_HVGA_PORTRAIT
#define ENABLE_KD2401_HVGA_PORTRAIT
#endif

#if !defined(EVE_PLATFORM_AVAILABLE) && !defined(EVE_MULTI_PLATFORM_TARGET)
#define EVE_PLATFORM_AVAILABLE
#ifndef FT900_PLATFORM
#define FT900_PLATFORM
#endif // FT900_PLATFORM
#define EVE_MODULE_PANL
#define malloc pvPortMalloc
#define calloc vPortCalloc
#define free vPortFree
#define PANL_HEAP_SIZE (20 * 1024)
#define TINYPRINTF_OVERRIDE_LIBC (0)
#endif

#elif defined(PANL70)

#define FT811_ENABLE
#define ENABLE_SPI_QUAD

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_WVGA
#endif

#if !defined(EVE_PLATFORM_AVAILABLE) && !defined(EVE_MULTI_PLATFORM_TARGET)
#define EVE_PLATFORM_AVAILABLE
#ifndef FT900_PLATFORM
#define FT900_PLATFORM
#endif // FT900_PLATFORM
#define EVE_MODULE_PANL
#define malloc pvPortMalloc
#define calloc vPortCalloc
#define free vPortFree
#define PANL_HEAP_SIZE (20 * 1024)
#define TINYPRINTF_OVERRIDE_LIBC (0)
#endif

#elif defined(PANL70PLUS)

#define FT811_ENABLE
#define ENABLE_SPI_QUAD

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_WVGA
#endif

#if !defined(EVE_PLATFORM_AVAILABLE) && !defined(EVE_MULTI_PLATFORM_TARGET)
#define EVE_PLATFORM_AVAILABLE
#ifndef FT900_PLATFORM
#define FT900_PLATFORM
#endif // FT900_PLATFORM
#define EVE_MODULE_PANL
#define malloc pvPortMalloc
#define calloc vPortCalloc
#define free vPortFree
#define PANL_HEAP_SIZE (20 * 1024)
#define TINYPRINTF_OVERRIDE_LIBC (0)
#endif

#elif defined(PANL50)
#define FT813_ENABLE
#define ENABLE_SPI_QUAD

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_WVGA
#endif

#if !defined(EVE_PLATFORM_AVAILABLE) && !defined(EVE_MULTI_PLATFORM_TARGET)
#define EVE_PLATFORM_AVAILABLE
#define PANL_APPLET
#ifndef FT900_PLATFORM
#define FT900_PLATFORM
#endif // FT900_PLATFORM
#define EVE_MODULE_PANL
#define malloc pvPortMalloc
#define calloc vPortCalloc
#define free vPortFree

#define printf tfp_printf
#define iprintf tfp_printf
#define PANL_HEAP_SIZE (20 * 1024)
#define TINYPRINTF_OVERRIDE_LIBC (0)
#endif

#elif defined(EVE_GRAPHICS_FT800)

#define FT800_ENABLE
#define ENABLE_SPI_QUAD
#define RESISTANCE_THRESHOLD (1800)

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_QVGA
#endif

#ifndef EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_RESISTIVE
#endif

#elif defined(EVE_GRAPHICS_FT801)

#define FT801_ENABLE

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_WQVGA
#endif

#elif defined(EVE_GRAPHICS_FT810)

#define FT810_ENABLE

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_QVGA
#endif

#ifndef EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_RESISTIVE
#endif

#elif defined(EVE_GRAPHICS_FT811)

#define FT811_ENABLE
#define ENABLE_SPI_QUAD

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_WVGA
#endif

#elif defined(EVE_GRAPHICS_FT812)
#define FT812_ENABLE
#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_QVGA
#endif

#ifndef EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_RESISTIVE
#endif

#elif defined(EVE_GRAPHICS_FT813)
#define FT813_ENABLE
#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_QVGA
#endif

#elif defined(EVE_GRAPHICS_BT880)

#define BT880_ENABLE

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_QVGA
#endif

#ifndef EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_RESISTIVE
#endif

#elif defined(EVE_GRAPHICS_BT881)

#define BT881_ENABLE
#define ENABLE_SPI_QUAD

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_WVGA
#endif

#elif defined(EVE_GRAPHICS_BT882)
#define BT882_ENABLE
#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_QVGA
#endif

#ifndef EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_RESISTIVE
#endif

#elif defined(EVE_GRAPHICS_BT883)
#define BT883_ENABLE
#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_QVGA
#endif

#elif defined(EVE_GRAPHICS_BT815)

#define BT815_ENABLE
#define ENABLE_SPI_QUAD

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_WVGA
#endif

#ifndef EVE_FLASH_AVAILABLE
#define EVE_FLASH_AVAILABLE
#define EVE_FLASH_MX25L128
#endif
#ifndef EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_FOCAL
#endif

#elif defined(EVE_GRAPHICS_BT816)

#define BT816_ENABLE
#define ENABLE_SPI_QUAD
#define RESISTANCE_THRESHOLD (-1)

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_WVGA
#endif

#ifndef EVE_FLASH_AVAILABLE
#define EVE_FLASH_AVAILABLE
#define EVE_FLASH_MX25L128
#endif

#ifndef EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_RESISTIVE
#endif

#elif defined(EVE_GRAPHICS_BT817)

#define BT817_ENABLE
#define ENABLE_SPI_QUAD

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_WVGA
#endif

#ifndef EVE_FLASH_AVAILABLE
#define EVE_FLASH_AVAILABLE
#define EVE_FLASH_MX25L128
#endif

#ifndef EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_FOCAL
#endif

#elif defined(EVE_GRAPHICS_BT817A)

#define BT817A_ENABLE
#define ENABLE_SPI_QUAD

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_WVGA
#endif

#ifndef EVE_FLASH_AVAILABLE
#define EVE_FLASH_AVAILABLE
#define EVE_FLASH_MX25L128
#endif

#ifndef EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_FOCAL
#endif

#elif defined(EVE_GRAPHICS_BT818)

#define BT818_ENABLE
#define ENABLE_SPI_QUAD
#define RESISTANCE_THRESHOLD (1800)

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_WVGA
#endif

#ifndef EVE_FLASH_AVAILABLE
#define EVE_FLASH_AVAILABLE
#define EVE_FLASH_MX25L128
#endif

#ifndef EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_AVAILABLE
#define EVE_TOUCH_RESISTIVE
#endif

#endif

/// Re-Mapping FT800 Series to FT80X
#if defined(FT800_ENABLE) || defined(FT801_ENABLE)
#define FT80X_ENABLE
#endif

/// Re-Mapping FT810 Series to FT81X
#if defined(FT810_ENABLE) || defined(FT811_ENABLE) || defined(FT812_ENABLE) || defined(FT813_ENABLE)
#define FT81X_ENABLE
#endif

/// Re-Mapping BT880 Series to BT88X
#if defined(BT880_ENABLE) || defined(BT881_ENABLE) || defined(BT882_ENABLE) || defined(BT883_ENABLE)
#define BT88X_ENABLE
#endif

/// Re-Mapping BT815 Series to BT81X
#if defined(BT815_ENABLE) || defined(BT816_ENABLE)
#define BT81X_ENABLE
#endif

/// Re-Mapping BT817 Series to BT81XA
#if defined(BT817_ENABLE) || defined(BT817A_ENABLE) || defined(BT818_ENABLE)
#define BT81XA_ENABLE
#endif

/// Model numbered macro for versioning convenience.
#if defined(FT800_ENABLE)
#define EVE_SUPPORT_CHIPID EVE_FT800
#define EVE_SUPPORT_GEN EVE1
#define FT_800_ENABLE
#elif defined(FT801_ENABLE)
#define EVE_SUPPORT_CHIPID EVE_FT801
#define EVE_SUPPORT_GEN EVE1
#define FT_801_ENABLE
#elif defined(FT810_ENABLE)
#define EVE_SUPPORT_CHIPID EVE_FT810
#define EVE_SUPPORT_GEN EVE2
#define FT_810_ENABLE
#elif defined(FT811_ENABLE)
#define EVE_SUPPORT_CHIPID EVE_FT811
#define EVE_SUPPORT_GEN EVE2
#define FT_811_ENABLE
#elif defined(FT812_ENABLE)
#define EVE_SUPPORT_CHIPID EVE_FT812
#define EVE_SUPPORT_GEN EVE2
#define FT_812_ENABLE
#elif defined(FT813_ENABLE)
#define EVE_SUPPORT_CHIPID EVE_FT813
#define EVE_SUPPORT_GEN EVE2
#define FT_813_ENABLE
#elif defined(BT880_ENABLE)
#define EVE_SUPPORT_CHIPID EVE_BT880
#define EVE_SUPPORT_GEN EVE2
#define BT_880_ENABLE
#elif defined(BT881_ENABLE)
#define EVE_SUPPORT_CHIPID EVE_BT881
#define EVE_SUPPORT_GEN EVE2
#define BT_881_ENABLE
#elif defined(BT882_ENABLE)
#define EVE_SUPPORT_CHIPID EVE_BT882
#define EVE_SUPPORT_GEN EVE2
#define BT_882_ENABLE
#elif defined(BT883_ENABLE)
#define EVE_SUPPORT_CHIPID EVE_BT883
#define EVE_SUPPORT_GEN EVE2
#define BT_883_ENABLE
#elif defined(BT815_ENABLE)
#define EVE_SUPPORT_CHIPID EVE_BT815
#define EVE_SUPPORT_GEN EVE3
#define BT_815_ENABLE
#elif defined(BT816_ENABLE)
#define EVE_SUPPORT_CHIPID EVE_BT816
#define EVE_SUPPORT_GEN EVE3
#define BT_816_ENABLE
#elif defined(BT817_ENABLE)
#define EVE_SUPPORT_CHIPID EVE_BT817
#define EVE_SUPPORT_GEN EVE4
#define BT_817_ENABLE
#elif defined(BT817A_ENABLE)
#define EVE_SUPPORT_CHIPID EVE_BT817A
#define EVE_SUPPORT_GEN EVE4
#define BT_817_ENABLE
#elif defined(BT818_ENABLE)
#define EVE_SUPPORT_CHIPID EVE_BT818
#define EVE_SUPPORT_GEN EVE4
#define BT_818_ENABLE
#endif

#if defined(FT80X_ENABLE)
#define FT_80X_ENABLE
#elif defined(FT81X_ENABLE)
#define FT_81X_ENABLE
#elif defined(BT88X_ENABLE)
#define BT_88X_ENABLE
#elif defined(BT81X_ENABLE)
#define BT_81X_ENABLE
#elif defined(BT81XA_ENABLE)
#define BT_81XA_ENABLE
#endif

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#if defined(EVE_DISPLAY_QVGA)
#define DISPLAY_RESOLUTION_QVGA
#elif defined(EVE_DISPLAY_WQVGA)
#define DISPLAY_RESOLUTION_WQVGA
#elif defined(EVE_DISPLAY_WVGA)
#define DISPLAY_RESOLUTION_WVGA
#elif defined(EVE_DISPLAY_WSVGA)
#define DISPLAY_RESOLUTION_WSVGA
#elif defined(EVE_DISPLAY_WXGA)
#define DISPLAY_RESOLUTION_WXGA
#elif defined(EVE_DISPLAY_ILI9488_HVGA_PORTRAIT)
#define DISPLAY_RESOLUTION_HVGA_PORTRAIT
#define ENABLE_ILI9488_HVGA_PORTRAIT
#elif defined(EVE_DISPLAY_KD2401_HVGA_PORTRAIT)
#define DISPLAY_RESOLUTION_HVGA_PORTRAIT
#define ENABLE_KD2401_HVGA_PORTRAIT
#endif

#ifndef EVE_DISPLAY_REFRESH
#define EVE_DISPLAY_REFRESH 0
#endif

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

/* The following are only used in ESD to configure the most appropriate flash emulation */
#if defined(ESD_SIMULATION) || defined(EVE_DEFINE_FLASH)
#if defined(EVE_FLASH_W25Q16)
#define EVE_FLASH_W25Q
#define EVE_FLASH_SIZE 2
#elif defined(EVE_FLASH_W25Q32)
#define EVE_FLASH_W25Q
#define EVE_FLASH_SIZE 4
#elif defined(EVE_FLASH_W25Q64)
#define EVE_FLASH_W25Q
#define EVE_FLASH_SIZE 8
#elif defined(EVE_FLASH_W25Q128)
#define EVE_FLASH_W25Q
#define EVE_FLASH_SIZE 16
#elif defined(EVE_FLASH_GOODIX)
#define EVE_FLASH_SIZE 16
#elif defined(EVE_FLASH_MX25L16)
#define EVE_FLASH_MX25L
#define EVE_FLASH_SIZE 2
#elif defined(EVE_FLASH_MX25L32)
#define EVE_FLASH_MX25L
#define EVE_FLASH_SIZE 4
#elif defined(EVE_FLASH_MX25L64)
#define EVE_FLASH_MX25L
#define EVE_FLASH_SIZE 8
#elif defined(EVE_FLASH_MX25L128)
#define EVE_FLASH_MX25L
#define EVE_FLASH_SIZE 16
#elif defined(EVE_FLASH_MX25L256)
#define EVE_FLASH_MX25L
#define EVE_FLASH_SIZE 32
#elif defined(EVE_FLASH_MX25L512)
#define EVE_FLASH_MX25L
#define EVE_FLASH_SIZE 64
#elif defined(EVE_FLASH_MX25L1024)
#define EVE_FLASH_MX25L
#define EVE_FLASH_SIZE 128
#elif defined(EVE_FLASH_MX25L2048)
#define EVE_FLASH_MX25L
#define EVE_FLASH_SIZE 256
#endif
#endif
#if defined(ESD_SIMULATION)
#if (EVE_SUPPORT_CHIPID >= EVE_BT817)
#ifdef _WIN32
#define EVE_FLASH_FIRMWARE L"BT817/unified.blob"
#else
#define EVE_FLASH_FIRMWARE "BT817/unified.blob"
#endif
#else
#ifdef _WIN32
#define EVE_FLASH_FIRMWARE L"BT815/unified.blob"
#else
#define EVE_FLASH_FIRMWARE "BT815/unified.blob"
#endif
#endif
#endif

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

/*

Ultimately, the platform selection must set one of the following internal platform flags.
- BT8XXEMU_PLATFORM
- FT9XX_PLATFORM
- FT4222_PLATFORM
- MPSSE_PLATFORM
- RP2040_PLATFORM
These may only be set by one of the platform target definitions, and should not be set manually by the user.

*/

#if defined(ESD_SIMULATION) || defined(EVE_PLATFORM_BT8XXEMU) /* ESD_SIMULATION overrides any other platform specifications */
#define BT8XXEMU_PLATFORM
#define EVE_HOST EVE_HOST_BT8XXEMU

#elif defined(__FT900__) || defined(MM900EV1A) || defined(MM900EV1B) || defined(MM900EV2A) || defined(MM900EV3A) || defined(MM900EV_LITE)
#ifndef FT900_PLATFORM
#define FT900_PLATFORM
#endif // FT900_PLATFORM

#elif defined(__FT930__) || defined(MM930MINI) || defined(MM930LITE) || defined(MM932LC)
#ifndef FT93X_PLATFORM
#define FT93X_PLATFORM
#endif // FT93X_PLATFORM

#elif defined(EVE_PLATFORM_FT4222)
#define FT4222_PLATFORM

#elif defined(EVE_PLATFORM_MPSSE)
#define MPSSE_PLATFORM

#elif defined(EVE_PLATFORM_RP2040) || defined(EVE_PLATFORM_MM2040EV)
#define RP2040_PLATFORM

#endif

#if defined(FT4222_PLATFORM)
#define MSVC_PLATFORM
#define EVE_HOST EVE_HOST_FT4222
#endif

#if defined(MPSSE_PLATFORM)
#define MSVC_PLATFORM
#define EVE_HOST EVE_HOST_MPSSE
#endif

#if defined(FT900_PLATFORM) || defined(FT93X_PLATFORM)
#define FT9XX_PLATFORM
#define EMBEDDED_PLATFORM
#define EVE_HOST EVE_HOST_EMBEDDED
#endif

#if defined(RP2040_PLATFORM)
#define EMBEDDED_PLATFORM
#define EVE_HOST EVE_HOST_EMBEDDED
#endif

#define EVE_CONFIG__STR(x) #x
#define EVE_CONFIG_STR(x) EVE_CONFIG__STR(x)

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#if defined(FT9XX_PLATFORM)       \
    || defined(FT4222_PLATFORM)   \
    || defined(MPSSE_PLATFORM)    \
    || defined(BT8XXEMU_PLATFORM) \
    || defined(RP2040_PLATFORM)
#define EVE_PLATFORM_AVAILABLE
#endif

#if defined(EVE_MULTI_PLATFORM_TARGET) && defined(EVE_PLATFORM_AVAILABLE)
#pragma message(__FILE__ "(" EVE_CONFIG_STR(__LINE__) "): warning PLATFORM: " \
                                                      "Multi platform incorrectly overridden by defaults")
#endif

#if !defined(EVE_SUPPORT_CHIPID)
#define EVE_MULTI_GRAPHICS_TARGET
#endif

#if defined(EVE_MULTI_PLATFORM_TARGET) || defined(EVE_MULTI_GRAPHICS_TARGET)
#define EVE_MULTI_TARGET
#endif

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

// Under multi target, the build should create a dynamic library
// Multi target is used by EVE Screen Editor for uploading to a device
#if defined(EVE_MULTI_TARGET) && defined(_WIN32)
#ifdef EVE_HAL_EXPORT
#undef EVE_HAL_EXPORT
#define EVE_HAL_EXPORT _declspec(dllexport)
#else
#define EVE_HAL_EXPORT _declspec(dllimport)
#endif
#else
#ifndef EVE_HAL_EXPORT
#define EVE_HAL_EXPORT
#endif
#endif

// Under multi target, enable all desktop functionality.
// None should have been defined previously.
#ifdef EVE_MULTI_PLATFORM_TARGET
#define FT4222_PLATFORM
#define MPSSE_PLATFORM
#define BT8XXEMU_PLATFORM
#define EVE_HOST phost->Host
#endif
#ifdef EVE_MULTI_GRAPHICS_TARGET
#define EVE_FLASH_AVAILABLE
#define EVE_SUPPORT_HSF
#define EVE_SUPPORT_FLASH
#define EVE_SUPPORT_UNICODE
#define EVE_SUPPORT_ASTC
#define EVE_SUPPORT_PNG
#define EVE_SUPPORT_VIDEO
#define EVE_SUPPORT_CMDB
#define EVE_SUPPORT_CAPACITIVE
#define EVE_SUPPORT_RESISTIVE
#define EVE_SUPPORT_CHIPID EVE_BT818
#define EVE_SUPPORT_GEN EVE4
#define RESISTANCE_THRESHOLD (1800)
#define EVE_CHIPID phost->ChipId
#define EVE_GEN EVE_gen(EVE_CHIPID)
#else
#define EVE_CHIPID EVE_SUPPORT_CHIPID
#define EVE_GEN EVE_SUPPORT_GEN
#endif

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

/// Feature support.
/// Avoid hardcoding specific EVE models throughout the libraries.
/// Allows disabling specific features for debugging purposes.
#if (EVE_SUPPORT_CHIPID >= EVE_BT817)
#define EVE_SUPPORT_HSF
#endif
#if (EVE_SUPPORT_CHIPID >= EVE_BT815)
#define EVE_SUPPORT_FLASH
#define EVE_SUPPORT_UNICODE
#define EVE_SUPPORT_ASTC
#endif
#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
#define EVE_SUPPORT_PNG
#define EVE_SUPPORT_VIDEO
#define EVE_SUPPORT_CMDB
#define EVE_SUPPORT_MEDIAFIFO
#if !(EVE_SUPPORT_CHIPID >= EVE_BT880 && EVE_SUPPORT_CHIPID <= EVE_BT883)
#define EVE_SUPPORT_LARGEFONT
#endif
#endif
#ifndef EVE_MULTI_GRAPHICS_TARGET
#if (((EVE_SUPPORT_CHIPID & 0x01) == 0x01) || (EVE_SUPPORT_CHIPID == EVE_BT817A))
#define EVE_SUPPORT_CAPACITIVE
#else
#define EVE_SUPPORT_RESISTIVE
#endif
#endif

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

/// Other options
#define EVE_DL_OPTIMIZE 1 /* Keep cache of displaylist values that don't often change but are generally set by every widget to reduce display list size */
#define EVE_DL_CACHE_SCISSOR 1 /* Keep cache of current scissor */
#define EVE_DL_END_PRIMITIVE 0 /* Whether the END command is sent */
#define EVE_DL_STATE_STACK_SIZE 4
#define EVE_DL_STATE_STACK_MASK 3

#define EVE_CMD_HOOKS 0 /* Allow adding a callback hook into EVE_CoCmd calls using CoCmdHook in EVE_HalContext */

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#if defined(FT900_PLATFORM) || defined(FT93X_PLATFORM)
#define eve_progmem __flash__ const
#define eve_progmem_const __flash__ const
#else
#define eve_progmem
#define eve_progmem_const const
#endif

typedef eve_progmem int8_t eve_prog_int8_t;
typedef eve_progmem uint8_t eve_prog_uint8_t;
typedef eve_progmem uint16_t eve_prog_uint16_t;

#ifndef EVE_TCHAR_DEFINED
#define EVE_TCHAR_DEFINED
#ifdef _WIN32
typedef wchar_t eve_tchar_t;
#else
typedef char eve_tchar_t;
#endif
#endif

#ifndef _MSC_VER
/* strcpy_s is not available in GCC */
#define strcpy_s(dst, sz, src) strcpy(dst, src)
#endif

#ifdef _MSC_VER
#define inline __inline
#endif

#if !defined(NDEBUG) && !defined(_DEBUG)
#define _DEBUG 1
#endif

#if defined(__GNUC__)
#define DO_PRAGMA_(x) _Pragma(#x)
#define eve_pragma_warning(msg) DO_PRAGMA_("GCC warning \"" msg "\"")
#elif defined(_MSC_VER)
#define eve_pragma_warning(msg) __pragma(message(__FILE__ "(" EVE_CONFIG_STR(__LINE__) "): warning EVE_Hal: " msg))
#else
#define eve_pragma_warning(msg)
#endif

#if defined(__GNUC__)
#define eve_pragma_error(msg) _Pragma("GCC error \"" msg "\"")
#elif defined(_MSC_VER)
#define eve_pragma_error(msg) __pragma(message(__FILE__ "(" EVE_CONFIG_STR(__LINE__) "): error EVE_Hal: " msg))
#else
#define eve_pragma_error(msg)
#endif

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

/* Set implementation options */
#if defined(FT4222_PLATFORM) || defined(MPSSE_PLATFORM)
#define EVE_BUFFER_WRITES
#endif

/* Disable unsupported options */
#if defined(ENABLE_ILI9488_HVGA_PORTRAIT) && !defined(FT9XX_PLATFORM) && !defined(RP2040_PLATFORM)
#undef ENABLE_ILI9488_HVGA_PORTRAIT
#endif
#if defined(ENABLE_KD2401_HVGA_PORTRAIT) && !defined(FT9XX_PLATFORM) && !defined(RP2040_PLATFORM)
#undef ENABLE_KD2401_HVGA_PORTRAIT
#endif

/* Enable FatFS by default on supported platforms */
#if defined(FT9XX_PLATFORM) || defined(RP2040_PLATFORM)
#ifndef EVE_ENABLE_FATFS
#define EVE_ENABLE_FATFS 1
#endif
#endif

#if defined(ESD_SIMULATION)
#ifdef EVE_ENABLE_FATFS
#undef EVE_ENABLE_FATFS
#endif
#endif

#ifndef EVE_ENABLE_FATFS
#define EVE_ENABLE_FATFS 0
#endif

/* Remove unsupported options */
#ifdef RP2040_PLATFORM
#ifdef ENABLE_SPI_QUAD
#undef ENABLE_SPI_QUAD
#endif
#ifdef ENABLE_SPI_DUAL
#undef ENABLE_SPI_DUAL
#endif
#endif

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

/// Configuration sanity checks
#if defined(PANL_APPLET) && defined(EVE_MULTI_TARGET)
#pragma message(__FILE__ "(" EVE_CONFIG_STR(__LINE__) "): error PANL_APPLET: " \
                                                      "Cannot target PANL_APPLET with EVE_MULTI_TARGET")
#endif
#if !defined(EVE_SUPPORT_CHIPID) && !defined(EVE_MULTI_GRAPHICS_TARGET)
#pragma message(__FILE__ "(" EVE_CONFIG_STR(__LINE__) "): error EVE_SUPPORT_CHIPID: " \
                                                      "No EVE device model has been selected")
#endif
#if !defined(EVE_DISPLAY_AVAILABLE) && !defined(EVE_MULTI_GRAPHICS_TARGET)
#pragma message(__FILE__ "(" EVE_CONFIG_STR(__LINE__) "): error EVE_DISPLAY_AVAILABLE: " \
                                                      "No display model has been selected")
#endif
#if !defined(EVE_SUPPORT_FLASH) && defined(EVE_FLASH_AVAILABLE)
#pragma message(__FILE__ "(" EVE_CONFIG_STR(__LINE__) "): warning EVE_FLASH_AVAILABLE: " \
                                                      "Cannot enable flash on EVE model which doesn't support flash")
#undef EVE_FLASH_AVAILABLE
#endif
#ifndef EVE_MULTI_PLATFORM_TARGET
#if ((defined(FT9XX_PLATFORM) ? 1 : 0)     \
    + (defined(FT4222_PLATFORM) ? 1 : 0)   \
    + (defined(MPSSE_PLATFORM) ? 1 : 0)    \
    + (defined(BT8XXEMU_PLATFORM) ? 1 : 0) \
    + (defined(RP2040_PLATFORM) ? 1 : 0))  \
    > 1
#pragma message(__FILE__ "(" EVE_CONFIG_STR(__LINE__) "): warning PLATFORM: " \
                                                      "More than one platform has been selected")
#endif
#endif
#if !defined(EVE_PLATFORM_AVAILABLE) && !defined(EVE_MULTI_PLATFORM_TARGET)
#pragma message(__FILE__ "(" EVE_CONFIG_STR(__LINE__) "): warning PLATFORM: " \
                                                      "No platform was selected")
#endif

#endif /* EVE_CONFIG__H */

/* end of file */
