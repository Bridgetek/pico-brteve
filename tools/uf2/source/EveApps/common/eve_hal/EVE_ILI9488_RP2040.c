/**
 * @file EVE_ILI9488_RP2040.c
 * @brief ILI9488 driver source
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

#if defined(RP2040_PLATFORM) \
    && (defined(ENABLE_ILI9488_HVGA_PORTRAIT) || defined(ENABLE_KD2401_HVGA_PORTRAIT))

#define GPIO_ILI9488_CS 9
#define GPIO_ILI9488_DCX 8

int16_t ILI9488_SPI_WriteCmd(EVE_HalContext *phost, uint8_t cmd, uint8_t data)
{
	gpio_put(GPIO_ILI9488_CS, 0);
	gpio_put(GPIO_ILI9488_DCX, 0);

	uint8_t hcmd[1] = { 0 };
	hcmd[0] = cmd;

	spi_write_blocking(phost->SpiPort, hcmd, 1);

	uint8_t hdata[1] = { 0 };
	hdata[0] = data;

	gpio_put(GPIO_ILI9488_DCX, 1);

	spi_write_blocking(phost->SpiPort, hdata, 1);

	gpio_put(GPIO_ILI9488_CS, 1);

	return 0;
}

int16_t ILI9488_SPI_WriteCmdN(EVE_HalContext *phost, uint8_t cmd, uint8_t bytecount, const uint8_t *pbuff)
{

	gpio_put(GPIO_ILI9488_CS, 0);
	gpio_put(GPIO_ILI9488_DCX, 0);

	uint8_t hcmd[1] = { 0 };
	hcmd[0] = cmd;

	spi_write_blocking(phost->SpiPort, hcmd, 1);

	gpio_put(GPIO_ILI9488_DCX, 1);

	int byteWritten = spi_write_blocking(phost->SpiPort, pbuff, bytecount);

	gpio_put(GPIO_ILI9488_CS, 1);

	return byteWritten;
}

void EVE_ILI9488_RP2040_bootup(EVE_HalContext *phost)
{
	gpio_put(phost->SpiCsPin, 1);

	gpio_init(GPIO_ILI9488_DCX);
	gpio_set_dir(GPIO_ILI9488_DCX, GPIO_OUT); //gpios for ili9488 - dcx
	gpio_put(GPIO_ILI9488_DCX, 0);

	gpio_init(GPIO_ILI9488_CS);
	gpio_set_dir(GPIO_ILI9488_CS, GPIO_OUT); //gpios for ili9488 - cs
	gpio_put(GPIO_ILI9488_CS, 1);

	uint8_t arraytemp[16];

	ILI9488_SPI_WriteCmd(phost, ILI9488_CMD_SOFTWARE_RESET, 0);
	EVE_sleep(1);

	//colomn address set - 0 to 319
	arraytemp[0] = 0x00;
	arraytemp[1] = 0x00;
	arraytemp[2] = 0x01;
	arraytemp[3] = 0x3f;
	ILI9488_SPI_WriteCmdN(phost, ILI9488_CMD_COLOMNADDR, 4, arraytemp);

	//row address set - 0 to 479
	arraytemp[0] = 0x00;
	arraytemp[1] = 0x00;
	arraytemp[2] = 0x01;
	arraytemp[3] = 0xdf;
	ILI9488_SPI_WriteCmdN(phost, ILI9488_CMD_ROWADDR, 4, arraytemp);

	//Frame rate 70HZ
	ILI9488_SPI_WriteCmd(phost, ILI9488_CMD_FRAME_RATE_CONTROL, 0xB0);

	//adjust control 3
	arraytemp[0] = 0xa9;
	arraytemp[1] = 0x51;
	arraytemp[2] = 0x2c;
	arraytemp[3] = 0x82;
	ILI9488_SPI_WriteCmdN(phost, 0XF7, 4, arraytemp);

	ILI9488_SPI_WriteCmd(phost, ILI9488_CMD_INTERFACE_MODE_CONTROL, 2);
	ILI9488_SPI_WriteCmd(phost, ILI9488_CMD_INTERFACE_PIXEL_FORMAT, ILI9488_INTERFACE_PIXEL_FORMAT_18BIT_DPI);
	ILI9488_SPI_WriteCmd(phost, ILI9488_CMD_IMAGEFUNCTION, 0);
	ILI9488_SPI_WriteCmd(phost, ILI9488_CMD_WRITE_CONTROL_DISPLAY, 0x2c);

	ILI9488_SPI_WriteCmd(phost, ILI9488_CMD_MADCTRL, 0x48); //bgr connection and colomn address order

	arraytemp[0] = 0x30;
	arraytemp[1] = 0x02;
	arraytemp[2] = 0x3b;

	ILI9488_SPI_WriteCmdN(phost, ILI9488_CMD_DISPLAY_FUNCTION_CONTROL, 3, arraytemp);

	ILI9488_SPI_WriteCmd(phost, ILI9488_CMD_SLEEP_OUT, 0);
	EVE_sleep(1);

	ILI9488_SPI_WriteCmd(phost, ILI9488_CMD_DISPLAYON, 0);
	ILI9488_SPI_WriteCmd(phost, 0x2c, 0xB0);
}

#endif

/*end of file*/