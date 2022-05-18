/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "pico/stdlib.h"

/**
  @file main.c
  @brief Free RTOS with D2XX
  An example of thread-safe D2XX library usage with FreeRTOS
 */
/*
 * ============================================================================
 * History
 * =======
 * Nov 2019		Initial beta for FT81x and FT80x
 * Mar 2020		Updated beta - added BT815/6 commands
 * Mar 2021     Beta with BT817/8 support added
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

/* INCLUDES ************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "EVE.h"
#include "eve_ui.h"

#include "example.h"

/* CONSTANTS ***********************************************************************/

/**
 * @brief Show debugging data on stdio.
 * 
 */
#define DEBUG

/**
 * @brief Pin which is connected to an LED to show activity to user.
 * 
 */
const uint LED_PIN = PICO_DEFAULT_LED_PIN;

/**
 * @brief Pins for GPIO for the controller.
 * 
 */
const uint FIRE_PIN = 28;
const uint LEFT_PIN = 27;
const uint RIGHT_PIN = 26;

/**
 @brief Page number in datalogger memory in Flash for touchscreen calibration
 values.
 */
#define CONFIG_PAGE_TOUCHSCREEN 0

/* GLOBALS *************************************************************************/

static struct repeating_timer timer;

/* LOCAL FUNCTIONS / INLINES *******************************************************/

void setup(void);

/* FUNCTIONS ***********************************************************************/

void led_state(uint8_t state)
{
    gpio_put(LED_PIN, state);
}

uint8_t fire_button_state(void)
{
    return gpio_get(FIRE_PIN);
}

uint8_t left_button_state(void)
{
    return gpio_get(LEFT_PIN);
}

uint8_t right_button_state(void)
{
    return gpio_get(RIGHT_PIN);
}

/* FUNCTIONS ***********************************************************************/

int main(void)
{
    uint8_t report_id;

    led_state(1);

    /* Setup UART */
    setup();
	
	/* Start example code. Includes EVE setup. */
	example_setup();

	/* Function never returns */
	for (;;)
	{
		example_loop();
	}
}

void setup(void)
{
    int ch = 0;

#ifdef DEBUG
    // Initialise stdio ports as configured in CMakeLists.txt
    stdio_init_all();
#endif

    // Turn on the pico LED to show activity
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 1);
    
    // Turn on the pico GPIO for buttons and joystick
    gpio_init(FIRE_PIN);
    gpio_set_dir(FIRE_PIN, GPIO_IN);
    gpio_set_pulls(FIRE_PIN, true, false);
    gpio_init(LEFT_PIN);
    gpio_set_dir(LEFT_PIN, GPIO_IN);
    gpio_set_pulls(LEFT_PIN, true, false);
    gpio_init(RIGHT_PIN);
    gpio_set_dir(RIGHT_PIN, GPIO_IN);
    gpio_set_pulls(RIGHT_PIN, true, false);
 
 #ifdef DEBUG
    /* Print out a welcome message... */
    printf ("(C) Copyright, Bridgetek Pte. Ltd. \r\n \r\n");
    printf ("---------------------------------------------------------------- \r\n");
    printf ("Welcome to Raspberry Pi Pico RP2040 Spaced Invaders\r\n");
    printf ("\n");
	printf ("Pin configuration for example:\n");
	printf ("Use SPI 0 hardware bus to match IDM2040-7A from Bridgetek\n");
	printf ("Pin 5 - MOSI (GPIO3)\n");
	printf ("Pin 6 - MISO (GPIO4)\n");
	printf ("Pin 4 - SCLK (GPIO2)\n");
	printf ("Pin 7 - CS (GPIO5) - Note this is not the SPI0_CS0 pin\n");
	printf ("Pin 20 - PD# (GPIO15) - Powerdown pin\n");
	printf ("Pin 34 - Fire/Start Button - pulled up (GPIO28)\n");
	printf ("Pin 32 - Left Button - pulled up (GPIO27)\n");
	printf ("Pin 31 - Right Button - pulled up (GPIO26)\n");
	printf ("Pin 40 - 5v supply for FT8xx\n");
	printf ("Pin 23 - signal GND for SPI\n");
#endif

	// Start millisecond timer.
	add_repeating_timer_us(1000, (repeating_timer_callback_t)example_timer, NULL, &timer); 
}