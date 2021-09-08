/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include "hardware/spi.h"

#include "me817ev_1024x600.h"
#include "eve.h"
#include "transport.h"

#define UNIT_OF_PIXEL_PRECISION         16 // With fractional = 4 by default
#define PRECISION(x)                    (UNIT_OF_PIXEL_PRECISION * (x))

void widgets_demo() {
	const uint32_t point_x = WIDTH / 4 + 150;
	const uint32_t point_y = HEIGHT / 2;
	const uint32_t point_size = 40;

	const uint32_t rect_x = WIDTH / 8 + 10;
	const uint32_t rect_y = HEIGHT / 2 + 20;
	const uint32_t rect_size = 80;

	const uint32_t line_x = WIDTH / 8;
	const uint32_t line_y = HEIGHT / 2 + HEIGHT / 4;
	const uint32_t line_w = WIDTH / 4;
	const uint32_t line_h = 10;

	const uint32_t button_x = WIDTH / 2;
	const uint32_t button_y = HEIGHT / 4;
	const uint32_t button_w = 150;
	const uint32_t button_h = 50;

	const uint32_t toggle_x = WIDTH / 2 + 40;
	const uint32_t toggle_y = HEIGHT / 4 + 100;
	const uint32_t toggle_w = 90;

	const uint32_t slider_x = WIDTH / 2 + 40;
	const uint32_t slider_y = HEIGHT / 4 + 200;
	const uint32_t slider_w = 300;
	const uint32_t slider_h = 30;

	const uint32_t progress_x = WIDTH / 2 + 40;
	const uint32_t progress_y = HEIGHT / 4 + 300;
	const uint32_t progress_w = 400;
	const uint32_t progress_h = 30;

	eve_transfer_begin(EVE_TRANSFER_WRITE, REG_CMDB_WRITE);

	eve_cocmd_dlStart();
	eve_cmd_32(CLEAR_COLOR_RGB(255, 255, 255));
	eve_cmd_32(CLEAR(1, 1, 1));
	eve_cmd_32(COLOR_RGB(255, 255, 255));

	eve_cocmd_gradient(0, 0, 0x0000FF, WIDTH, HEIGHT, 0x00FF00);
	eve_cocmd_text((uint16_t) WIDTH / 4, (uint16_t) HEIGHT / 4 - 20, 31, 0, "Text");
	eve_cocmd_text((uint16_t) WIDTH / 4, (uint16_t) HEIGHT / 4 + 50, 31, 0, "42");

	eve_cmd_32(BEGIN(POINTS));
	eve_cmd_32(POINT_SIZE(PRECISION(point_size)));
	eve_cmd_32(VERTEX2F(PRECISION(point_x), PRECISION(point_y)));

	eve_cmd_32(BEGIN(RECTS));
	eve_cmd_32(VERTEX2F(PRECISION(rect_x), PRECISION(rect_y)));
	eve_cmd_32(VERTEX2F(PRECISION(rect_x + rect_size), PRECISION(rect_y + rect_size)));

	eve_cmd_32(BEGIN(RECTS));
	eve_cmd_32(VERTEX2F(PRECISION(line_x), PRECISION(line_y)));
	eve_cmd_32(VERTEX2F(PRECISION(line_x + line_w), PRECISION(line_y + line_h)));

	eve_cocmd_button(button_x, button_y, button_w, button_h, 30, 0, "Button");
	eve_cocmd_toggle(toggle_x, toggle_y, toggle_w, 30, 0, 0, "On");
	eve_cocmd_slider(slider_x, slider_y, slider_w, slider_h, 0, 100, 500);
	eve_cocmd_progress(progress_x, progress_y, progress_w, progress_h, 0, 150, 600);

	eve_cmd_32(DISPLAY());
	eve_cocmd_swap();

	eve_transfer_end();
}

int main() {
	/// Enable UART so we can print status output
	stdio_init_all();

	/// Power up
	gpio_init(PIN_PDN);
	gpio_set_dir(PIN_PDN, GPIO_OUT);
	gpio_put(PIN_PDN, 0);

	/// Setup SPI
	spi_setup();
	gpio_put(PIN_PDN, 1);

	sleep_ms(1000);

	eve_config config;

	config.Dither = DITHER;
	config.Swizzle = SWIZZLE;
	config.CSpread = CSPREAD;
	config.OutBitsR = OUTBITSR;
	config.OutBitsG = OUTBITSG;
	config.OutBitsB = OUTBITSB;
	config.PCLKPol = PCLKPOL;

	config.Width = WIDTH;
	config.Height = HEIGHT;
	config.HCycle = HCYCLE;
	config.HOffset = HOFFSET;
	config.HSync0 = HSYNC0;
	config.HSync1 = HSYNC1;
	config.VCycle = VCYCLE;
	config.VOffset = VOFFSET;
	config.VSync0 = VSYNC0;
	config.VSync1 = VSYNC1;
	config.PCLK = PCLK_;
	config.PCLKFreq = PCLKFREQ;

	eve_bootup bootup;
	bootup.ExternalOsc = 1;
	bootup.SystemClock = EVE_SYSCLK_72M;

#ifdef HOST_PLATFORM_GAMEDUINO_3X_DAZZLER_1280X720
	bootup.Skip_ROM_CHIPID_Check = 1;
#else
	bootup.Skip_ROM_CHIPID_Check = 0;
#endif

	while (!eve_init(&bootup, &config)) {
		sleep_ms(1000);
	}

	/// Blocking call, return when calibrate finish
#ifndef HOST_PLATFORM_GAMEDUINO_3X_DAZZLER_1280X720 // GAMEDUINO_3X_DAZZLER has no touch
	eve_calibrate(WIDTH, HEIGHT); 
#endif

	while (true) {
		widgets_demo();
		sleep_ms(100 * 1000);
	}
}
