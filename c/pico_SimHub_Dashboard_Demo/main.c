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

extern const uint8_t img_background_jpg[];
extern const Image_header_t img_gbox_format[];
extern const uint8_t img_gbox[];
extern const Image_header_t img_needle_format[];
extern const uint8_t img_needle[];

/* CONSTANTS ****************************************************************/

// Do not send any debug information to the stdio. The stdio channel is connected
// to the host via the USB CDC_ACM interface and is used as a channel for game data.
#undef DEBUG

// Define to enable a screenshot when a touchscreen event detected.
#undef ENABLE_SCREENSHOT

// Allow the use of the pico LED for feedback.
const uint LED_PIN = PICO_DEFAULT_LED_PIN;

// SimHub device name. Used as a response to the 'N' command.
const char DEVICE_NAME[] = "pico EVE Dash";

// Enable the serial interface for SimHub on stdio.
#define SIMHUB_STDIO_ENABLE

// Data header marker from SimHub. Alternate marker is for use by a human to
// test the interface.
#define MESSAGE_HEADER 0x03
#define MESSAGE_HEADER_ALT '!'

// Maximum expected message length.
#define MESSAGE_MAX 255

// Definitions of order of messages in the custom string.
enum {
	MESSAGE_SPEED,
	MESSAGE_RPM,
	MESSAGE_GEAR,
	MESSAGE_TOTAL
};

// X, Y, and size definitions for gear selection indicator.
// This is matched to the graphic in background.jpg. A single box is 
// drawn around the gear number depending on the gear selected by SimHub.
#define GBOX_CENTRE_X 400
#define GBOX_CENTRE_Y 406
#define GBOX_BOX_WIDTH 36
#define GBOX_BOX_HEIGHT 36

#define GBOX_P_X (GBOX_CENTRE_X - (GBOX_BOX_WIDTH / 2) - (3 * GBOX_BOX_WIDTH))
#define GBOX_R_X (GBOX_CENTRE_X - (GBOX_BOX_WIDTH / 2) - (2 * GBOX_BOX_WIDTH))
#define GBOX_N_X (GBOX_CENTRE_X - (GBOX_BOX_WIDTH / 2) - (1 * GBOX_BOX_WIDTH))
#define GBOX_D_X (GBOX_CENTRE_X - (GBOX_BOX_WIDTH / 2))
#define GBOX_1_X (GBOX_CENTRE_X - (GBOX_BOX_WIDTH / 2) + (1 * GBOX_BOX_WIDTH))
#define GBOX_2_X (GBOX_CENTRE_X - (GBOX_BOX_WIDTH / 2) + (2 * GBOX_BOX_WIDTH))
#define GBOX_3_X (GBOX_CENTRE_X - (GBOX_BOX_WIDTH / 2) + (3 * GBOX_BOX_WIDTH))
#define GBOX_4_X (GBOX_CENTRE_X - (GBOX_BOX_WIDTH / 2) + (3 * GBOX_BOX_WIDTH))
#define GBOX_5_X (GBOX_CENTRE_X - (GBOX_BOX_WIDTH / 2) + (3 * GBOX_BOX_WIDTH))
#define GBOX_Y (GBOX_CENTRE_Y - (GBOX_BOX_HEIGHT / 2))

// X, Y location for the centre of the speedometer needle.
// This is matched to the graphic in background.jpg.
#define SPEED_CENTRE_X 400
#define SPEED_CENTRE_Y 223

// Maximum speed defined on the speedometer in background.jpg.
// Any speed above this will be "squashed" to keep the needle under
// the FSD point.
#define MAX_SPEED_BASE 160

// Definitions of "deceleration" arrays for damping the motion of the
// indicator needles. This stops the needles jumping and makes them look
// more "analogue".
#define     N_SPEED (0)
#define     N_RPM   (1)
#define     N_FUEL  (2)
#define     N_TEMP  (3)

// Macro to enact "deceleration" effect.
#define DEACCELARATION(target, prev, rate) 				\
do {								\
    target = (prev) + (rate) * ((target) - (prev));	\
    prev = (target);						\
} while (0)

// Bitmap handles for EVE.
#define HANDLE_GUI_1_BACKGROUND  (0)
#define HANDLE_GUI_1_GERA_BOX    (1)
#define HANDLE_GUI_1_NEEDLE      (2)

/* TYPEDEFS *****************************************************************/

// Data received from SimHub in the "P" command.
//  speed;rpm;gear
//ex: P165;3000;D<cr>
typedef struct {
	int speed;
	int rpm;
	char gear; //  = "N", "P", "R", "D", "1", "2", etc;
	// For future use:
	int fuel;
	int temp;
	char pos[2] ;  // = "1";
	char lap[2] ; // = "0";
	char curLap[10] ; //  = "00:00.000";
} CarInfo;

// Currently displayed dashboard data.
typedef struct
{
	float speed;
	float rpm;
	char gear; /* P R N 1 2 3... */
	// For future use:
	uint16_t temperature; /* 0-100% */
	uint16_t fuel; /* 0-100% */
} SystemStatus_t;

/* GLOBALS ******************************************************************/

// Currently shown data on the dashboard.
SystemStatus_t    SystemStatus;
// New data received from SimHub.
CarInfo           carinfo;

// Array of pointers to custom message received from SimHub.
char *tokens[MESSAGE_TOTAL];

// De-accelaration the needle movement: speed, rpm, temp, fuel */
float NeedleDeaccelaration[4] = {0}; 

/* LOCAL FUNCTIONS / INLINES ************************************************/

// Forward declaration of board setup.
static void setup(void);
static void led_state(uint8_t state);
static void screenshot(void);

static void Get_GameInfo(char *cmdstr, uint16_t len)
{
	int i, j;
	int tok;
	char tmp_cmdstr[MESSAGE_MAX];

	if (len <= 2) return;
	if (len > MESSAGE_MAX)
	{
		len = MESSAGE_MAX;
	}

	// Remove all characters which are not part of the message body.
	for(i = 2, j = 0; i < len; i++)
	{
		char cmdchr = cmdstr[i]; 

		// Allowable characters. Number and gear selectors.
		if ((('0' <= cmdchr) && (cmdchr <= '9')) || 
			(cmdchr == ';') || cmdchr =='N' || cmdchr =='R' || cmdchr =='P' || cmdchr =='D')
		{
			tmp_cmdstr[j++] = cmdchr;
		}
		if (cmdchr == '\0') break;
		if (cmdchr == 'G') return; //Add it to prevent stop (bug)
	}
	tmp_cmdstr[j] = '\0'; // Terminate the string

	tok = 0;	
	// Split the string into parts as carinfo definition.
	tokens[tok++] = &tmp_cmdstr[0];
	for (i = 0; i < j; i++)
	{
		if (tok >= MESSAGE_TOTAL)
		{
			break;
		}

		if (tmp_cmdstr[i] == ';')
		{
			tokens[tok++] = &tmp_cmdstr[i + 1];
		}
	}

	// Parse the token array.
	for (i = 0; i < tok; i++)
	{
		uint16_t num = 	atoi(tokens[i]);
		if (i == MESSAGE_SPEED) 
		{
			carinfo.speed = num;
		}
		if (i == MESSAGE_RPM) 
		{
			carinfo.rpm = num;
		}
		if (i == MESSAGE_GEAR) 
		{
			carinfo.gear = **(tokens + i); 
		}
	}
}

#ifdef SIMHUB_STDIO_ENABLE
static int serial_task(void)
{
	uint8_t read_bytes = 0;
	uint8_t header = 0;
	char opt, par;

	static char cmdstr[256];
	static int cmdstrlen = 0;

	static int recv;
	
	while (1)
	{
	 	recv = getchar_timeout_us(0);
		// No data received.
		if (recv == PICO_ERROR_TIMEOUT)
		{
			break;
		}
		// End of line or command.
	 	if (recv == '\r')
		{
			cmdstr[cmdstrlen] = '\0';
			read_bytes = cmdstrlen;
			cmdstrlen = 0;
			break;
		}
		// Add received data to command.
		cmdstr[cmdstrlen] = (char)recv & 0x7f;
		cmdstrlen++;
	}

	if (read_bytes >= 2)
	{
		char header, opt;
		header = cmdstr[0];
		opt = cmdstr[1];

		if ((header == MESSAGE_HEADER) || (header == MESSAGE_HEADER_ALT))
		{
			// Hello command
			if (opt == '1') {
				sleep_ms(10);
				putchar('h');
				fflush(stdout);
			}

			// Set baudrate
			if (opt == '8') {
				if (read_bytes >=3){
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
				carinfo.gear = cmdstr[2];
				putchar(1);
				fflush(stdout);
			}

			// Custom protocol for end users (See Wiki for more infos : https://github.com/zegreatclan/AssettoCorsaTools/wiki/Custom-Arduino-hardware-support)
			if (opt == 'P') {
				putchar(1);
				fflush(stdout);
				Get_GameInfo(cmdstr, read_bytes);
			}
		}
	}
	return read_bytes;
}
#endif // SIMHUB_STDIO_ENABLE

/* EVE FUNCTIONS ************************************************************/

// Draw a dial needle at a specific angle.
static void rotate_draw_image(uint8_t handle, float rotateAngle, int16_t x, int16_t y, uint16_t x_center, uint16_t y_center, int16_t x_translate_offset, int16_t y_translate_offset)
{
	EVE_BEGIN(EVE_BEGIN_BITMAPS);
	EVE_BITMAP_HANDLE((handle));
#define FX16(x) ((long)(65536L * x))
	EVE_CMD_LOADIDENTITY();
	EVE_CMD_TRANSLATE(FX16(x_translate_offset), FX16(y_translate_offset));
	EVE_CMD_ROTATE(rotateAngle);
	EVE_CMD_TRANSLATE( FX16(-x_center), FX16(-y_center));
	EVE_CMD_SETMATRIX();
	EVE_VERTEX_FORMAT(3);
	EVE_VERTEX2F((long)((x - x_translate_offset) * 8), (long)((y - y_translate_offset) * 8));
	EVE_VERTEX_FORMAT(4);
}

// Draw a bitmap at the required location.
static void Draw_Bitmap(uint8_t handle, int16_t x, int16_t y)
{
	EVE_BITMAP_HANDLE(handle);
	EVE_BEGIN(EVE_BEGIN_BITMAPS);
	EVE_VERTEX_FORMAT(3);
	EVE_VERTEX2F(8 * x, 8 * y);
	EVE_VERTEX_FORMAT(4);
	EVE_END();
}   

static void Draw_GUI_1()
{
	// Draw the dashboard background.
	Draw_Bitmap(HANDLE_GUI_1_BACKGROUND, 0, 0);

	// Draw the gearbox selector.
	switch (SystemStatus.gear)
	{
	case 'P':
		Draw_Bitmap(HANDLE_GUI_1_GERA_BOX, GBOX_P_X, GBOX_Y); //"P"
	    break;
	case 'R':
		Draw_Bitmap(HANDLE_GUI_1_GERA_BOX, GBOX_R_X, GBOX_Y); //"R"
		break;
	case 'N':
	default:
		Draw_Bitmap(HANDLE_GUI_1_GERA_BOX, GBOX_N_X, GBOX_Y); //"N"
		break;
	case 'D':
		Draw_Bitmap(HANDLE_GUI_1_GERA_BOX, GBOX_D_X, GBOX_Y); //"D"
		break;
	case '1':
		Draw_Bitmap(HANDLE_GUI_1_GERA_BOX, GBOX_1_X, GBOX_Y); //"1"
		break;
	case '2':
		Draw_Bitmap(HANDLE_GUI_1_GERA_BOX, GBOX_2_X, GBOX_Y); //"2"
		break;
	case '3':
	case '4':
	case '5':
		Draw_Bitmap(HANDLE_GUI_1_GERA_BOX, GBOX_3_X, GBOX_Y); //"3"
		break;
	}

	// Calculate speedometer needle angle.
	float angle;
	EVE_BITMAP_HANDLE(HANDLE_GUI_1_NEEDLE);
	EVE_BITMAP_SIZE(EVE_FILTER_BILINEAR, EVE_WRAP_BORDER, EVE_WRAP_BORDER, 400,400);

	const int16_t x_center = 5;
	const int16_t y_center = 21;
	const int16_t x_translate_offset = 212, y_translate_offset = 212;


	int16_t Init_Angle = 30;  // The needle angle when speed is 0.
	int16_t Speed_Range = MAX_SPEED_BASE; // From 0 to 160 km
	int16_t Angle_Range = 265; // The needle angle range from 0 to 160 km

	angle = SystemStatus.speed/Speed_Range * Angle_Range + Init_Angle;

	// The angle step is different when speed is over MAX_SPEED_BASE, handle it specially.
	if (SystemStatus.speed >= MAX_SPEED_BASE)
	{
		Init_Angle += Angle_Range; // The init angle is set to the position of MAX_SPEED_BASE

		Angle_Range = 17;
		Speed_Range = 20;

		angle = (SystemStatus.speed - MAX_SPEED_BASE) / Speed_Range * Angle_Range + Init_Angle;
	}

	DEACCELARATION(angle, NeedleDeaccelaration[N_SPEED], 0.04);

	int32_t rotateAngle = (int32_t)((angle)*65536/360);
	
	// Draw speed needle.
	rotate_draw_image(HANDLE_GUI_1_NEEDLE, rotateAngle, 
		SPEED_CENTRE_X, SPEED_CENTRE_Y, 
		x_center, y_center, 
		x_translate_offset, y_translate_offset); 
}

static void cluster_draw(void)
{
	EVE_LIB_BeginCoProList();
	EVE_CMD_DLSTART();
	EVE_CLEAR_COLOR_RGB(125, 125, 100);
	EVE_CLEAR(1, 1, 1);
	EVE_COLOR_RGB(255, 255, 255);

	Draw_GUI_1();

	EVE_DISPLAY();
	EVE_CMD_SWAP();
	EVE_LIB_EndCoProList();
	EVE_LIB_AwaitCoProEmpty();
}

static void cluster_setup(void)
{
	int i;

    EVE_LIB_BeginCoProList();
    EVE_CMD_DLSTART();
    EVE_DISPLAY();
    EVE_CMD_SWAP();
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();

	// Setup initial system.
	SystemStatus.rpm = 0;
	SystemStatus.speed = 0;
	SystemStatus.temperature = 50;

	// Load the bitmaps to RAM_G.
	eve_ui_load_argb1555(img_gbox, img_gbox_format->Size, HANDLE_GUI_1_GERA_BOX, img_gbox_format->Width, img_gbox_format->Height);
	eve_ui_load_argb1555(img_needle, img_needle_format->Size, HANDLE_GUI_1_NEEDLE, img_needle_format->Width, img_needle_format->Height);
	eve_ui_load_jpg(img_background_jpg, HANDLE_GUI_1_BACKGROUND, NULL, NULL);

	// Move dials to FSD and back again like a real car.
	SystemStatus.gear = 'P';
	SystemStatus.rpm = 9000;
	SystemStatus.speed = MAX_SPEED_BASE;

	for (i = 0; i < 140; i++)
	{
		cluster_draw();
	}

	// Return dial to zero.
	SystemStatus.gear = 'P';
	SystemStatus.rpm = 0;
	SystemStatus.speed = 0;
}

static void cluster_loop(void)
{
#ifdef SIMHUB_STDIO_ENABLE
	// Update the carinfo structure.
	if (serial_task())
	{
		SystemStatus.speed =  (float)carinfo.speed/10;
		SystemStatus.rpm = (float)carinfo.rpm/10;
		SystemStatus.gear = carinfo.gear;

#ifdef DEBUG
		printf("S=%f RPM=%f, G=%c\n",SystemStatus.speed , SystemStatus.rpm, SystemStatus.gear);
#endif // DEBUG
	}
#endif // SIMHUB_STDIO_ENABLE
	
	// Draw the dashboard.
	cluster_draw();
}

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
    uint8_t report_id;

    /* Setup UART */
    setup();
	eve_ui_setup();

    /* Start example code */
	cluster_setup();
    while (1)
    {			
		cluster_loop();
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

    // Turn on the pico LED to show activity
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);
    
#ifdef DEBUG
    /* Print out a welcome message... */
    printf ("(C) Copyright, Bridgetek Pte. Ltd. \r\n \r\n");
    printf ("---------------------------------------------------------------- \r\n");
    printf ("Welcome to BRT_AN_012 Example for Raspberry Pi Pico RP2040\r\n");
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