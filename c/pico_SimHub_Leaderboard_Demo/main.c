/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "pico/stdlib.h"

/**
  @file main.c
 */
/*
 * ============================================================================
 * History
 * =======
 * Apr 2022		Initial version
 *
 *
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
 * http://brtchip.com/BRTSourceCodeLicenseAgreement/ ("the Licence Terms").
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
 * appliance. There are exclusions of FTDI liability for certain types of loss
 * such as: special loss or damage; incidental loss or damage; indirect or
 * consequential loss or damage; loss of income; loss of business; loss of
 * profits; loss of revenue; loss of contracts; business interruption; loss of
 * the use of money or anticipated savings; loss of information; loss of
 * opportunity; loss of goodwill or reputation; and/or loss of, damage to or
 * corruption of data.
 * There is a monetary cap on FTDI's liability.
 * The Software may have subsequently been amended by another user and then
 * distributed by that other user ("Adapted Software").  If so that user may
 * have additional licence terms that apply to those amendments. However, FTDI
 * has no liability in relation to those amendments.
 * ============================================================================
 */

/* INCLUDES *****************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "EVE.h"
#include "eve_ui.h"
#include "eve_ram_g.h"

#include "assets.h"

/* EXTERNALS ****************************************************************/

extern const Image_header_t img_bar_format[];
extern const uint8_t img_bar[];
extern const Image_header_t img_orbitron_format[];
extern const uint8_t font_orbitron[];
extern const uint32_t Theme_main;
extern const uint32_t Theme_colours[];

/* CONSTANTS ****************************************************************/

// Do not send any debug information to the stdio. The stdio channel is connected
// to the host via the USB CDC_ACM interface and is used as a channel for game data.
#define DEBUG

// Define to enable a screenshot when a touchscreen event detected.
#undef ENABLE_SCREENSHOT

// Allow the use of the pico LED for feedback.
const uint LED_PIN = PICO_DEFAULT_LED_PIN;

// SimHub device name. Used as a response to the 'N' command.
const char DEVICE_NAME[] = "pico EVE Dash";

// Enable the serial interface for SimHub on stdio.
#define SIMHUB_STDIO_ENABLE

/* Commands from SimHub:
Simulate with python script
	>python simhubtest.py
Identify driver:
	!PD;<car number>;<driver name>;<time>
Examples:
	!PD;99;27;Jimmy McRae;0.000

Notify new lap time:
	!PL;<lap number>;<lap time>
Examples:
	!PL;1;34:35.369
	!PL;2;32:31.467

Update rankings:
	Start ranking update: !PS 
	Rank ahead: !PA;<places ahead>;<car number>;<driver name>;<time difference>
	Rank behind: !PB;<places ahead>;<car number>;<driver name>;<time difference>
	Commit ranking update: !PU
Examples:
	!PS
	!PA;4;1;Tony Pond;-01.490
	!PA;3;2;Russell Brooks;-03.436
	!PA;2;3;Jim Clark;-03.464
	!PA;1;4;Jackie Stewart;-02.441
	!PA;0;5;Stirling Moss;-01.122
	!PB;0;6;Mike Hawthorn;+00.101
	!PB;1;7;Nigel Mansell;+00.339
	!PB;2;8;Graham Hill;+01.044
	!PB;3;9;James Hunt;+01.504
	!PB;4;10;John Surtees;+03.418
	!PU
*/

// Data header marker from SimHub. Alternate marker is for use by a human to
// test the interface.
#define MESSAGE_HEADER 0x03
#define MESSAGE_HEADER_ALT '!'

// Maximum expected message length.
#define MESSAGE_MAX 256

// Definitions of order of messages in the rank custom string.
enum {
	MESSAGE_RANK_POSITION = 1,
	MESSAGE_RANK_CAR,
	MESSAGE_RANK_DRIVER,
	//MESSAGE_RANK_TEAM,
	MESSAGE_RANK_TIME,
	MESSAGE_RANK_TOTAL
};

// Definitions of order of messages in the rank custom string.
enum {
	MESSAGE_LAP_NUMBER = 1,
	MESSAGE_LAP_TIME,
	MESSAGE_LAP_TOTAL
};

#define MESSAGE_TOTAL_MAX ((MESSAGE_RANK_TOTAL>MESSAGE_LAP_TOTAL)?MESSAGE_RANK_TOTAL:MESSAGE_LAP_TOTAL)

// Bitmap handles for EVE.
#define HANDLE_BAR     1
#define HANDLE_FONT_1  27

// Number of entries in the leaderboard.
#define LEADERBOARD_LAP_DISPLAY 16
// Number of slots for rank display (including one space for the current driver).
#define LEADERBOARD_RANK_DISPLAY 11

// Number of slots ahead of driver.
#define LEADERBOARD_RANK_BEFORE 5
// Number of slots after driver.
#define LEADERBOARD_RANK_AFTER 5
// Position in rank table for this driver.
// Undefine to draw a line at the driver position.
//#define LEADERBOARD_RANK_FIXED 5
#undef LEADERBOARD_RANK_FIXED
// Total number of before and after drivers.
#define LEADERBOARD_RANK_TOTAL (LEADERBOARD_RANK_AFTER + LEADERBOARD_RANK_BEFORE)
// Place in rank array for 
#define LEADERBOARD_RANK_DRIVER LEADERBOARD_RANK_TOTAL

// Size of strings used for display purposes.
#define LEADERBOARD_NAME 64
#define LEADERBOARD_TIME 16

// Spacing and gap for leaderboard.
#define LEADERBOARD_LAP_SPACING 3
#define LEADERBOARD_LAP_HEIGHT 30
#define LEADERBOARD_RANK_SPACING 30
#define LEADERBOARD_RANK_HEIGHT 40
#define LEADERBOARD_SPACING 3
// Number of steps to use to animate leaderboard changes.
#define LEADERBOARD_ANIMATE_STEPS 20
// Milliseconds per step for animation.
#define LEADERBOARD_ANIMATE_SPEED 5

/* TYPEDEFS *****************************************************************/

/* GLOBALS ******************************************************************/

struct leaderboard {
	int carNum;
	char driver[LEADERBOARD_NAME];
	char time[LEADERBOARD_TIME];
};

struct leaderboard leaderboard[LEADERBOARD_RANK_DISPLAY];
struct leaderboard prevLeaderboard[LEADERBOARD_RANK_DISPLAY];

struct leadchanges {
	int prevPos;
	int animate;
} leadchanges[LEADERBOARD_LAP_DISPLAY];

struct lapdata {
	char lap[LEADERBOARD_TIME];
	char time[LEADERBOARD_TIME];
};

struct lapdata lapTimes[LEADERBOARD_LAP_DISPLAY];
int lapAnimate;
int lapCount = 0;

/* LOCAL FUNCTIONS / INLINES ************************************************/

// Forward declaration of board setup.
static void setup(void);
static void led_state(uint8_t state);

static void screenshot(void)
{
#ifdef ENABLE_SCREENSHOT
	uint32_t img_end_address = malloc_ram_g(EVE_DISP_WIDTH * 4);
	int row_num = 0;
	int line_counter = 0;
	uint32_t line_data[EVE_DISP_WIDTH] __attribute__((aligned(4)));
	uint32_t pixel_data;

	printf("PPM start\n"); // Use this marker to identify the start of the image.
	stdio_set_translate_crlf(&stdio_usb, false);
	
	// PPM Header
	printf("P6\n%d\n%d\n255\n", EVE_DISP_WIDTH, EVE_DISP_HEIGHT);
	while (row_num < EVE_DISP_HEIGHT) 
	{
		// Write screenshot into RAM_G
		EVE_LIB_BeginCoProList();
		EVE_CMD_DLSTART();
		// ARGB8 format (0x20) is not equivalent to a BITMAP_FORMAT
		EVE_CMD_SNAPSHOT2(0x20, img_end_address, 0, row_num, (EVE_DISP_WIDTH * 2), 1);
		EVE_LIB_EndCoProList();
		EVE_LIB_AwaitCoProEmpty();

		eve_ui_arch_sleepms(20); // ensure co-pro has finished taking snapshot

		EVE_LIB_ReadDataFromRAMG((uint8_t *)line_data, EVE_DISP_WIDTH * 4, img_end_address);
		line_counter = 0;
		while (line_counter < EVE_DISP_WIDTH)
		{
			pixel_data = line_data[line_counter];
			putchar_raw((uint8_t)(pixel_data >> 16));
			putchar_raw((uint8_t)(pixel_data >> 8));
			putchar_raw((uint8_t)(pixel_data >> 0));
			line_counter++; // increment address
		}
		fflush(stdout);
		row_num++;
	}
	stdio_set_translate_crlf(&stdio_usb, true);
	printf("\nPPM end\n"); // Marker to identify the end of the image.

	free_ram_g(img_end_address);
	
	eve_ui_splash("Screenshot completed...");
	eve_ui_arch_sleepms(2000);

#endif // ENABLE_SCREENSHOT
}

static void copy_token(char *dst, const char *src, int max_len, int tok_len)
{
	memcpy(dst, src, max_len);
	if (tok_len >= max_len)
	{
		tok_len = max_len - 1;
	}
	dst[tok_len] = '\0';
}

static void mark_rank_changes()
{
	struct leaderboard *leaderboard_entry = leaderboard;
	struct leaderboard *leaderboard_prev = prevLeaderboard;
	struct leaderboard *leaderboard_search;
	struct leadchanges *changes = leadchanges;
	int i, j;

	for (i = 0; i < (LEADERBOARD_RANK_TOTAL); i++)
	{
		// Default is no change.
		changes->prevPos = i;
		changes->animate = 0;

		// If there is a change in position work out the change.
		if (leaderboard_entry->carNum != leaderboard_prev->carNum)
		{
			// Find the previous position of the entry.
			leaderboard_search = prevLeaderboard;

			for (j = 0; j < (LEADERBOARD_RANK_TOTAL); j++)
			{
				// Compare car number in old v new ranking table.
				if (leaderboard_entry->carNum == leaderboard_search->carNum)
				{
					changes->animate = LEADERBOARD_ANIMATE_STEPS;
					break;
				}
				else if (leaderboard_search->carNum != -1)
				{
				}
	
				leaderboard_search++;
			}

			// Move from j to i.
			changes->prevPos = j;
		}

		changes++;
		leaderboard_entry++;
		leaderboard_prev++;
	}
}

static int decode_game_info(char *cmdstr, int cmdlen)
{
	// Update screen after this update.
	int screenUpdate = 0;
	// Array of pointers to custom message received from SimHub.
	const char *tokens[MESSAGE_TOTAL_MAX];
	int toklen[MESSAGE_TOTAL_MAX];
	int i;
	int tok;
	char cmd = cmdstr[0];

	tok = 0;
	memset(toklen, 0, sizeof(toklen));
	
	// Split the string into parts as simhubdata definition.
	tokens[tok] = &cmdstr[0];
	toklen[tok] = 0;

	for (i = 0; i < cmdlen; i++)
	{
		if ((tok >= MESSAGE_TOTAL_MAX) || (cmdstr[i] == '\0'))
		{
			break;
		}
		else if (cmdstr[i] == ';')
		{
			i++;
			tok++;
			tokens[tok] = &cmdstr[i];
			toklen[tok] = 0;
		}

		toklen[tok]++;
	}
	tok++;

	// Action lap time tokesn
	if (cmd == 'L')
	{
		char newLapTime[LEADERBOARD_TIME];

		// Make a copy of the newly supplied lap number.
		copy_token(newLapTime, tokens[1], LEADERBOARD_TIME, toklen[1]);
		// Compare the new lap number with the previous last lap recorded. 
		// This might be the same data retransmitted, if so, ignore.
		if (strncmp(lapTimes[0].lap, newLapTime, LEADERBOARD_TIME))
		{
			// Move existing lap times down one slot.
			for (i = (LEADERBOARD_LAP_DISPLAY - 1); i > 0 ; i--)
			{
				strncpy(lapTimes[i].time, lapTimes[i - 1].time, LEADERBOARD_TIME);
				strncpy(lapTimes[i].lap, lapTimes[i - 1].lap, LEADERBOARD_TIME);
			}
			lapAnimate = LEADERBOARD_ANIMATE_STEPS;

			for (i = 1; i < tok; i++)
			{
				if (i == MESSAGE_LAP_NUMBER) 
				{
					copy_token(lapTimes[0].lap, tokens[i], LEADERBOARD_TIME, toklen[i]);
				}
				if (i == MESSAGE_LAP_TIME)
				{
					copy_token(lapTimes[0].time, tokens[i], LEADERBOARD_TIME, toklen[i]);
				}
			}
		}
	}
	if ((cmd == 'A') || (cmd == 'B') || (cmd == 'D'))
	{
		int pos;

		// Parse the token array.
		for (i = 1; i < tok; i++)
		{
			if (i == MESSAGE_RANK_POSITION) 
			{
				// Initialise the driver's position.
				if (cmd == 'D')
				{
					pos = LEADERBOARD_RANK_DRIVER;
				}
				else
				{
					pos = atoi(tokens[i]);

					// Add in list ahead of the player's position.
					if (cmd == 'A')
					{
						// Only show close positions.
						if ((pos < 0) || (pos > LEADERBOARD_RANK_AFTER))
						{
							break;
						}
						
						pos = LEADERBOARD_RANK_AFTER - pos - 1;
					}
					// Add in list behind the player's position.
					else if (cmd == 'B')
					{
						// Only show close positions.
						if ((pos < 0) || (pos > LEADERBOARD_RANK_BEFORE))
						{
							break;
						}

						pos = LEADERBOARD_RANK_BEFORE + pos;
					}
				}
				memset(&leaderboard[pos], 0, sizeof(struct leaderboard));
			}
			if (i == MESSAGE_RANK_CAR) 
			{
				leaderboard[pos].carNum = atoi(tokens[i]);
			}
			if (i == MESSAGE_RANK_DRIVER) 
			{
				copy_token(leaderboard[pos].driver, tokens[i], LEADERBOARD_NAME, toklen[i]);
			}
			if (i == MESSAGE_RANK_TIME) 
			{
				copy_token(leaderboard[pos].time, tokens[i], LEADERBOARD_TIME, toklen[i]);
			}
		}
	}
	// Start a ranking update.
	if (cmd == 'S')
	{
		memcpy(prevLeaderboard, leaderboard, sizeof(prevLeaderboard));
	}
	// Commit a ranking update.
	if (cmd == 'U')
	{
		screenUpdate = 1;
	}

	return screenUpdate;
}

static int serial_task(void)
{
	// Update to screen required now.
	int screenUpdate = 0;
	// Length of command line received.
	int cmdlen = 0;
	// Each command line from host can be MESSAGE_MAX bytes max.
	static char cmdstr[MESSAGE_MAX];
	static int cmdstrcount = 0;
	// Received character.
	static int recv;
	
	while (1)
	{
	 	recv = getchar_timeout_us(0);
		// No data received.
		if (recv == PICO_ERROR_TIMEOUT)
		{
			break;
		}

		// End of line or command. Accept \r or \n.
	 	if ((recv == '\r') || (recv == '\n'))
		{
			// Ignore empty lines.
			if (cmdstrcount > 0)
			{
				cmdstr[cmdstrcount] = '\0';
				cmdlen = cmdstrcount;
				cmdstrcount = 0;
				//printf("COMMAND: \"%s\"\r\n", cmdstr);
				break;
			}
			continue;
		}
		// Truncate message if it is too long.
		if (cmdstrcount < MESSAGE_MAX)
		{
			// Add received data to command.
			cmdstr[cmdstrcount] = (char)recv & 0x7f;
			cmdstrcount++;
		}
	}

	// Parse the message.
	if (cmdlen >= 2)
	{
		char header, opt;
		header = cmdstr[0];
		opt = cmdstr[1];

		// Messages always begin with the header character.
		if ((header == MESSAGE_HEADER) || (header == MESSAGE_HEADER_ALT))
		{
			//printf("COMMAND: %c\r\n", opt);

			// Hello command
			if (opt == '1') {
				sleep_ms(10);
				putchar('h');
				fflush(stdout);
			}

			// Set baudrate
			if (opt == '8') {
				if (cmdlen >=3){
				}
				putchar(1);
				fflush(stdout);
			}

			// Simple buttons count
			if (opt == 'J') {
				putchar(0);
				fflush(stdout);
			}

			//  Module count command
			if (opt == '2') {
				putchar(0);
				fflush(stdout);
			}

			//  SIMPLE Module count command
			if (opt == 'B') {
				putchar(0);
				fflush(stdout);
			}

			// ACQ Packet
			if (opt == 'A') {
				putchar(3);
				fflush(stdout);
			}

			// Buttons state
			if (opt == 'C') {
				//sendButtonState();
				//serialFlush();
			}

			// Device Name
			if (opt == 'N') {
				printf("%s", DEVICE_NAME);
				fflush(stdout);
			}
			//  RGBLED count command
			if (opt == '4') {
				putchar(0);
				fflush(stdout);
			}
			// Tachometer
			if (opt == 'T') {

			}

			// Boost gauge
			if (opt == 'U') {

			}

			if (opt == '!')
			{
				screenshot();
			}

			// Features command
			if (opt == '0') {

				// Gear
				printf("G");

				// Name
				printf("N");

				// Additional buttons
				//printf("J");

				// Custom Protocol Support
				printf("P");

				// End of response <CR>
				putchar(0xa);
				fflush(stdout);
			}

			// GEAR
			if (opt == 'G') {
				//simhubdata.gear = cmdstr[2];
				putchar(1);
				fflush(stdout);
			}

			// Custom protocol for end users (See Wiki for more infos : https://github.com/zegreatclan/AssettoCorsaTools/wiki/Custom-Arduino-hardware-support)
			if (opt == 'P') {
				//putchar(1);
				//fflush(stdout);
				screenUpdate = decode_game_info(&cmdstr[2], cmdlen - 2);
			}
		}
	}
	return screenUpdate;
}

/* EVE FUNCTIONS ************************************************************/

// Draw a bitmap at the required location.
static void draw_bitmap(uint8_t handle, int16_t x, int16_t y)
{
	EVE_BITMAP_HANDLE(handle);
	EVE_BEGIN(EVE_BEGIN_BITMAPS);
	EVE_VERTEX2F(x << 4, y << 4);
	EVE_END();
}   

static void draw_number(char *num)
{
	int pch;
	int dx = 0;
	const int w = ((img_orbitron_format->Width * 2) / 3) + 1;

	EVE_BEGIN(EVE_BEGIN_BITMAPS);

	for (pch = 0; pch < strlen(num); pch++)
	{
		char ch = num[pch];
			
		if (ch == '1') 
			EVE_VERTEX2II((w * 1 / 4) + dx, 0, HANDLE_FONT_1, ch);
		else
			EVE_VERTEX2II(dx, 0, HANDLE_FONT_1, ch);

		if ((ch == '.') || (ch == ':'))
			dx += ((w * 1) / 2);
		else
			dx += w;
	}
	EVE_END();
}   

static int draw_leaderboard()
{
	struct leadchanges *changes = leadchanges;
	struct leaderboard *leaderboard_entry = leaderboard;
	char num[34];
	int i;
	int16_t cx, cy;
	uint16_t x, y;
	int action = 0;

	for (i = 0; i < (LEADERBOARD_RANK_TOTAL); i++)
	{
		// Target location.
		x = 0;
		y = (LEADERBOARD_RANK_HEIGHT * i) + LEADERBOARD_RANK_SPACING;
#ifdef LEADERBOARD_RANK_FIXED
		if (i >= LEADERBOARD_RANK_FIXED)
		{
			y += (LEADERBOARD_RANK_HEIGHT * 1);
		}
#endif
		if (i != changes->prevPos)
		{
			if (changes->animate > 0)
			{
				action++;
				
				if (changes->animate >= (LEADERBOARD_ANIMATE_STEPS / 2))
				{
					cx = LEADERBOARD_ANIMATE_STEPS - changes->animate;
				}
				else
				{
					cx = changes->animate;
				}

				if (changes->prevPos > i)
				{
					x += cx;
				}
				else
				{
					x -= cx;
				}

				cy = (LEADERBOARD_RANK_HEIGHT * (changes->prevPos - i));
				cy = cy * changes->animate / LEADERBOARD_ANIMATE_STEPS;
				y += cy;

				changes->animate--;
			}
		}
		changes++;

		EVE_VERTEX_TRANSLATE_X(x << 4);
		EVE_VERTEX_TRANSLATE_Y(y << 4);

		EVE_COLOR(Theme_colours[leaderboard_entry->carNum % 16]);
		draw_bitmap(HANDLE_BAR, 0, 0);
		
		EVE_COLOR_RGB(255, 255, 255);

		if (leaderboard_entry->driver[0])
		{
			sprintf(num, "%d", leaderboard_entry->carNum); 
			EVE_CMD_TEXT(40, 0, HANDLE_FONT_1, 0, num);
			EVE_CMD_TEXT(80, 0, HANDLE_FONT_1, 0, leaderboard_entry->driver);

			EVE_VERTEX_TRANSLATE_X((x + 370) << 4);
			draw_number(leaderboard_entry->time);
		}
		leaderboard_entry++;
	}

#ifndef LEADERBOARD_RANK_FIXED
	// Draw a green line to seperate the before and after drivers.
	EVE_BEGIN(EVE_BEGIN_LINES);
	y = (LEADERBOARD_RANK_HEIGHT * LEADERBOARD_RANK_BEFORE) + LEADERBOARD_RANK_SPACING -
		 ((LEADERBOARD_RANK_HEIGHT - img_bar_format->Height) / 2);
	EVE_COLOR_RGB(0, 255, 0);
	EVE_VERTEX_TRANSLATE_X(0);
	EVE_VERTEX_TRANSLATE_Y(y << 4);
	EVE_VERTEX2F(0 << 4, 0 << 4);
	EVE_VERTEX2F(img_bar_format->Width << 4, 0);
	EVE_END();
#else // LEADERBOARD_RANK_FIXED
	// Add an entry for the current driver name and number.
	x = 0;
	y = (LEADERBOARD_RANK_HEIGHT * LEADERBOARD_RANK_FIXED) + LEADERBOARD_RANK_SPACING;
	EVE_VERTEX_TRANSLATE_X(x << 4);
	EVE_VERTEX_TRANSLATE_Y(y << 4);

	EVE_COLOR(Theme_colours[leaderboard[LEADERBOARD_RANK_DRIVER].carNum % 16]);
	draw_bitmap(HANDLE_BAR, 0, 0);
	
	EVE_COLOR_RGB(255, 255, 255);

	if (leaderboard[LEADERBOARD_RANK_DRIVER].driver[0])
	{
		sprintf(num, "%d", leaderboard[LEADERBOARD_RANK_DRIVER].carNum); 
		EVE_CMD_TEXT(40, 0, HANDLE_FONT_1, 0, num);
		EVE_CMD_TEXT(80, 0, HANDLE_FONT_1, 0, leaderboard[LEADERBOARD_RANK_DRIVER].driver);
	}
#endif  // LEADERBOARD_RANK_FIXED

	return action;
}

static int draw_laptimes()
{
	struct lapdata *lap_entry = lapTimes;
	int i;
	uint16_t x, y, cy;
	uint8_t r, g, b;
	int action = 0;

	r = Theme_main >> 16;
	g = Theme_main >> 8;
	b = Theme_main >> 0;

	for (i = 0; i < LEADERBOARD_LAP_DISPLAY; i++)
	{
		if ((lapAnimate == 0) || (i > 0))
		{
			// Target location.
			x = 480 + LEADERBOARD_SPACING + 20;
			y = (LEADERBOARD_LAP_HEIGHT * i) + LEADERBOARD_LAP_SPACING;
			if (lapAnimate > 0)
			{
				action++;
				
				if (i > 0)
				{
					cy = (lapAnimate * LEADERBOARD_LAP_HEIGHT) / LEADERBOARD_ANIMATE_STEPS;
					y -= cy;
				}
			}

			if (i > 0)
			{
				r = (int)r * ((LEADERBOARD_LAP_DISPLAY << 4) - (i << 2)) / (LEADERBOARD_LAP_DISPLAY << 4);
				g = (int)g * ((LEADERBOARD_LAP_DISPLAY << 4) - (i << 2)) / (LEADERBOARD_LAP_DISPLAY << 4);
				b = (int)b * ((LEADERBOARD_LAP_DISPLAY << 4) - (i << 2)) / (LEADERBOARD_LAP_DISPLAY << 4);
			}
			EVE_COLOR_RGB(r, g, b);

			EVE_VERTEX_TRANSLATE_X(x << 4);
			EVE_VERTEX_TRANSLATE_Y(y << 4);
			draw_bitmap(HANDLE_BAR, 0, 0);
			
			EVE_COLOR_RGB(255, 255, 255);
			EVE_VERTEX_TRANSLATE_X((x + 40) << 4);
			draw_number(lap_entry->lap);

			EVE_VERTEX_TRANSLATE_X((x + 160) << 4);
			draw_number(lap_entry->time);
		}

		lap_entry++;
	}

	if (lapAnimate > 0)
		lapAnimate--;
		
	return action;
}

static int screen_draw()
{
	int animate = 0;

	EVE_LIB_BeginCoProList();
	EVE_CMD_DLSTART();
	EVE_CLEAR_COLOR_RGB(125, 100, 75);
	EVE_CLEAR(1, 1, 1);
	EVE_COLOR_RGB(255, 255, 255);

	animate += draw_leaderboard();
	animate += draw_laptimes();

	EVE_DISPLAY();
	EVE_CMD_SWAP();
	EVE_LIB_EndCoProList();
	EVE_LIB_AwaitCoProEmpty();

	return animate;
}

static uint32_t load_image(const uint8_t *image_data, const Image_header_t *format, uint8_t image_handle)
{
	uint32_t img_offset;

	img_offset = malloc_ram_g(format->Size);
	if (img_offset)
	{
		eve_ui_arch_write_ram_from_pm(image_data, format->Size, img_offset);

		EVE_LIB_BeginCoProList();
		EVE_CMD_DLSTART();
		EVE_BEGIN(EVE_BEGIN_BITMAPS);
		EVE_BITMAP_HANDLE(image_handle);
		EVE_BITMAP_SOURCE(img_offset);
		EVE_BITMAP_LAYOUT(format->Format, format->Width, format->Height);
		EVE_BITMAP_LAYOUT_H((format->Width) >> 10, format->Height >> 9);
		EVE_BITMAP_SIZE(EVE_FILTER_NEAREST, EVE_WRAP_BORDER, EVE_WRAP_BORDER,
				format->Width, format->Height);
		EVE_BITMAP_SIZE_H(format->Width >> 9, format->Height >> 9);
		EVE_END();

		EVE_DISPLAY();
		EVE_CMD_SWAP();
		EVE_LIB_EndCoProList();
		EVE_LIB_AwaitCoProEmpty();

		img_offset += (format->Size);
	}

	return img_offset;
}

static uint32_t load_font(const uint8_t *image_data, const Image_header_t *format, uint8_t image_handle)
{
	uint32_t img_offset;

	img_offset = malloc_ram_g(format->Size);
	if (img_offset)
	{
		eve_ui_arch_write_ram_from_pm(image_data, format->Size, img_offset);
		EVE_LIB_BeginCoProList();
		EVE_CMD_DLSTART();
		EVE_BEGIN(EVE_BEGIN_BITMAPS);
		EVE_BITMAP_HANDLE(image_handle);
		EVE_BITMAP_SOURCE(img_offset + 148 - (format->Stride * format->Height * format->First));
		EVE_BITMAP_LAYOUT_H(0, 0);
		EVE_BITMAP_LAYOUT(format->Format, format->Stride, format->Height);
		EVE_BITMAP_SIZE_H(0, 0);
		EVE_BITMAP_SIZE(EVE_FILTER_NEAREST, EVE_WRAP_BORDER, EVE_WRAP_BORDER, format->Width, format->Height);
		EVE_CMD_SETFONT(image_handle, img_offset);
		EVE_END();
		EVE_DISPLAY();
		EVE_CMD_SWAP();
		EVE_LIB_EndCoProList();
		EVE_LIB_AwaitCoProEmpty();
	}
}

static void screen_setup(void)
{
	int i;
	uint32_t ram_g_dummy;

    EVE_LIB_BeginCoProList();
    EVE_CMD_DLSTART();
    EVE_DISPLAY();
    EVE_CMD_SWAP();
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();

	// Load fonts to RAM_G.
	load_font(font_orbitron, img_orbitron_format, HANDLE_FONT_1);
	// Load the bitmaps to RAM_G.
	load_image(img_bar, img_bar_format, HANDLE_BAR);
}

static void screen_loop()
{
	mark_rank_changes();

	// Draw the dashboard.
	while (screen_draw()) {
		sleep_ms(LEADERBOARD_ANIMATE_SPEED);
	};
}

/* TINYUSB FUNCTIONS ********************************************************/

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
    (void) remote_wakeup_en;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
}

/* FUNCTIONS ****************************************************************/

int main(void)
{
	const char *tmp;
	int i;
	int rnd1, rnd2;

    // Setup UART.
    setup();
	// Setup EVE.
	eve_ui_setup();

    // Start example code.
	screen_setup();
	
	// Initial screen display.
	screen_loop();

	while (1)
    {
		if (serial_task())
			screen_loop();
    }

    // function never returns
    for (;;) ;
}

static void led_state(uint8_t state)
{
    gpio_put(LED_PIN, state);
}

static void setup(void)
{
    int ch = 0;

    // Initialise stdio ports as configured in CMakeLists.txt
    stdio_init_all();
	uart_set_hw_flow(uart0, true, true);
    gpio_set_function(18, GPIO_FUNC_UART); // CTS
    gpio_set_function(19, GPIO_FUNC_UART); // RTS

    // Turn on the pico LED to show activity
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 1);
    

#ifdef DEBUG
    /* Print out a welcome message... */
    printf ("(C) Copyright, Bridgetek Pte. Ltd. \r\n \r\n");
    printf ("---------------------------------------------------------------- \r\n");
    printf ("Welcome to EVE Example for Raspberry Pi Pico RP2040\r\n");
    printf ("\n");
	printf ("Pin configuration for example:\n");
	printf ("Use SPI 0 hardware bus to match IDM2040-7A from Bridgetek\n");
	printf ("Pin 5 - MOSI (GPIO3)\n");
	printf ("Pin 6 - MISO (GPIO4)\n");
	printf ("Pin 4 - SCLK (GPIO2)\n");
	printf ("Pin 7 - CS (GPIO5) - Note this is not the SPI0_CS0 pin\n");
	printf ("Pin 20 - PD# (GPIO15) - Powerdown pin\n");
	printf ("Pin 40 - 5v supply for FT8xx\n");
	printf ("Pin 8 - signal GND for SPI\n");
#endif
}
