/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Ha Thach for Adafruit Industries
 * Copyright (c) 2021 Bridgetek Pte Ltd
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef BOARD_H_
#define BOARD_H_

#define UF2_VERSION_BASE    "0.3.0"
#define UF2_VERSION         UF2_VERSION_BASE " for Bridgetek EVE"

extern unsigned char g_UsbProduct[64];
extern uint32_t g_Uf2FamilyId;
extern unsigned char g_DeviceName[64];

#define MATRIX_RAIN 1

//--------------------------------------------------------------------+
// USB UF2
//--------------------------------------------------------------------+

#define USB_VID_FTDI      0x0403
#define USB_PID_MASS_STORAGE 0x0fd5

#define USB_VID           USB_VID_FTDI
#define USB_PID           USB_PID_MASS_STORAGE
#define USB_MANUFACTURER  "Bridgetek Pte Ltd"
#define USB_PRODUCT       g_UsbProduct

#define UF2_PRODUCT_NAME  "Bridgetek BT8XX"
#define UF2_BOARD_ID      "BT8XX-RP2040-v0.0"
#define UF2_VOLUME_LABEL  "BTEVE-FLASH"
#define UF2_INDEX_URL     "https://brtchip.com"

#define BOARD_UF2_FAMILY_ID g_Uf2FamilyId

#define DISPLAY_TITLE     g_DeviceName
#define DISPLAY_WIDTH     240
#define DISPLAY_HEIGHT    320

// Size of firmware that is device family specific
#define EVE_FLASH_FIRMWARE_SIZE 4096

typedef struct spi_pins
{
    uint32_t MISO ;
    uint32_t CS   ;
    uint32_t SCK  ;
    uint32_t MOSI ;
    uint32_t INT  ;
    uint32_t PWD  ;
    uint32_t IO2  ;
    uint32_t IO3  ;
}spi_pins;

#endif
