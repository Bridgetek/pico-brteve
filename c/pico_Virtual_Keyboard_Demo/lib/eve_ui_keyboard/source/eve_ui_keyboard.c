/**
  @file eve_ui_keyboard.c
 */
/*
 * ============================================================================
 * History
 * =======
 * 2017-03-15 : Created
 *
 * (C) Copyright Bridgetek Pte Ltd
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

#include "eve_ui.h"
#include "eve_ram_g.h"
#include "eve_ui_keyboard.h"

#undef USE_EXTRA_SCREEN
#undef USE_SPECIAL_SCREEN

/**
 * @brief Keyboard colours.
 * @details The highlight colour is used for an active keypress on a button or
 * a selected option (e.g. caps lock). The alpha
 */
//@{
#define KEY_COLOUR_HIGHLIGHT EVE_COLOUR_HIGHLIGHT
#define KEY_COLOUR_CONTROL EVE_COLOUR_BG_1
#define KEY_COLOUR_ALPHANUM EVE_COLOUR_BG_2
#define KEY_COLOUR_TOP EVE_COLOUR_FG_1
//@}

#define PACK __attribute__((__packed__))

/**
 * @brief Simple state values for detecting a keypress and acting on it.
 * @details A modifier key will only affect the next scan code sent in this
 * example. However, a scan key will send a report to the host.
 * The modifier keys (shift, alt, ctrl, Gui (Windows Key) behave like this
 * because the resistive touchscreen displays support one touch only so
 * holding shift and another key cannot be detected.
 */
//@{
#define KEY_PRESS_NONE 0
#define KEY_PRESS_MODIFIER 1
#define KEY_PRESS_SCAN 2
//@}

/** @name Display section definitions.
 * @brief Macros to simplify selecting locations on the display for buttons,
 * text and indicators.
 * @details The display is split into 2 areas: at the top is a status area with
 * LED indicators for Caps, Num and Scroll Lock and mode selection buttons;
 * the lower portion is the keyboard buttons.
 */
//@{
/**
 * @brief Key dimensions for LEDs and layout switches.
 * @details The top 1/4 of the display is reserved for persistent objects.
 * These are the LED status indicators and buttons to switch the layout
 * of the keyboard.
 * There are 2 rows of 10 positions in the matrix. Both start counting at
 * zero.
 */
//@{
#define KEY_SPACER_STATUS (EVE_DISP_WIDTH / 120)

#define KEY_ROWS_STATUS 2
#define KEY_COLS_STATUS 10

#define KEY_WIDTH_STATUS(a) (((EVE_DISP_WIDTH * ((a)*8)) / (KEY_COLS_STATUS * 8)) - KEY_SPACER_STATUS)
#define KEY_HEIGHT_STATUS (((EVE_DISP_HEIGHT / 4) / KEY_ROWS_STATUS) - KEY_SPACER_STATUS)

#define KEY_ROW_STATUS(a) (0 + ((a) * (KEY_HEIGHT_STATUS + KEY_SPACER_STATUS)))
#define KEY_COL_STATUS(a) ((a) * (EVE_DISP_WIDTH / KEY_COLS_STATUS))
//@}

/**
 * @brief Key dimensions for alphanumeric keys.
 * @details This defines a block of 15 columns and 6 rows for the alphanumeric
 * keys. Some keys are positioned a fraction offset to simulate a real
 * keyboard.
 * The top of this region is defined by: KEY_ROW_STATUS(KEY_ROWS_STATUS)
 */
//@{
#define KEY_SPACER_ALPHA (EVE_DISP_WIDTH / 120)

#define KEY_ROWS_ALPHA 6
#define KEY_COLS_ALPHA 15

#define KEY_WIDTH_ALPHA(a) (((EVE_DISP_WIDTH * ((a)*8)) / (KEY_COLS_ALPHA * 8)) - KEY_SPACER_ALPHA)
#define KEY_HEIGHT_ALPHA (((EVE_DISP_HEIGHT - KEY_ROW_STATUS(KEY_ROWS_STATUS)) / KEY_ROWS_ALPHA) - KEY_SPACER_ALPHA)

#define KEY_ROW_ALPHA(a) (KEY_ROW_STATUS(KEY_ROWS_STATUS) + ((a) * (KEY_HEIGHT_ALPHA + KEY_SPACER_ALPHA)))
#define KEY_COL_ALPHA(a) ((a) * (EVE_DISP_WIDTH / KEY_COLS_ALPHA))
//@}

/**
 * @brief Key dimensions for keypad keys.
 * @details This defines a block of 12 columns and 5 rows for the numeric and
 * control keys.
 * The top of this region is defined by: KEY_ROW_STATUS(KEY_ROWS_STATUS)
 */
//@{
#define KEY_SPACER_KEYPAD (EVE_DISP_WIDTH / 120)

#define KEY_ROWS_KEYPAD 6
#define KEY_COLS_KEYPAD 12

#define KEY_WIDTH_KEYPAD(a) (((EVE_DISP_WIDTH * ((a)*8)) / (KEY_COLS_KEYPAD * 8)) - KEY_SPACER_KEYPAD)
#define KEY_HEIGHT_KEYPAD(a) ((((EVE_DISP_HEIGHT - KEY_ROW_STATUS(KEY_ROWS_STATUS)) * (a)) / KEY_ROWS_KEYPAD) - KEY_SPACER_KEYPAD)

#define KEY_ROW_KEYPAD(a) (KEY_ROW_STATUS(KEY_ROWS_STATUS) + ((a) * (KEY_HEIGHT_KEYPAD(1) + KEY_SPACER_KEYPAD)))
#define KEY_COL_KEYPAD(a) ((a) * (EVE_DISP_WIDTH / KEY_COLS_KEYPAD))
//@}
//@}

/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/

/**
 * @brief Current state of keyboard.
 */
static struct key_state key_state;

/**
 * @brief Current keyboard screen displayed.
 */
static uint8_t keyboard_screen = KEYBOARD_SCREEN_ALPHANUMERIC;

/**
 * @brief Current keyboard screen displayed.
 */
static uint8_t keyboard_layout = KEYBOARD_LAYOUT_PC_UK_ALPHA;

/**
 * @brief Current keyboard components displayed.
 */
static uint32_t keyboard_components = KEYBOARD_COMPONENTS_FULL;

/**
 * @brief Map of ASCII codes assigned to tagged buttons.
 */
//@{
static const uint8_t keymap_num_row_non_us[] = {
	TAG_TILDE,
	TAG_1,
	TAG_2,
	TAG_3,
	TAG_4,
	TAG_5,
	TAG_6,
	TAG_7,
	TAG_8,
	TAG_9,
	TAG_0,
	TAG_MINUS,
	TAG_EQUALS,
};
static const uint8_t keymap_top_row_non_us[] = {
	TAG_Q,
	TAG_W,
	TAG_E,
	TAG_R,
	TAG_T,
	TAG_Y,
	TAG_U,
	TAG_I,
	TAG_O,
	TAG_P,
	TAG_SQB_OPEN,
	TAG_BRACKET_CLS,
};
static const uint8_t keymap_mid_row_non_us[] = {
	TAG_A,
	TAG_S,
	TAG_D,
	TAG_F,
	TAG_G,
	TAG_H,
	TAG_J,
	TAG_K,
	TAG_L,
	TAG_SEMICOLON,
	TAG_SQUOTE,
	TAG_HASH,
};
static const uint8_t keymap_bot_row_non_us[] = {
	TAG_BACKSLASH,
	TAG_Z,
	TAG_X,
	TAG_C,
	TAG_V,
	TAG_B,
	TAG_N,
	TAG_M,
	TAG_COMMA,
	TAG_DOT,
	TAG_SLASH,
};

static const uint8_t keymap_num_row_us[] = {
	TAG_TILDE,
	TAG_1,
	TAG_2,
	TAG_3,
	TAG_4,
	TAG_5,
	TAG_6,
	TAG_7,
	TAG_8,
	TAG_9,
	TAG_0,
	TAG_MINUS,
	TAG_EQUALS,
};
static const uint8_t keymap_top_row_us[] = {
	TAG_Q,
	TAG_W,
	TAG_E,
	TAG_R,
	TAG_T,
	TAG_Y,
	TAG_U,
	TAG_I,
	TAG_O,
	TAG_P,
	TAG_SQB_OPEN,
	TAG_BRACKET_CLS,
	TAG_BACKSLASH,
};
static const uint8_t keymap_mid_row_us[] = {
	TAG_A,
	TAG_S,
	TAG_D,
	TAG_F,
	TAG_G,
	TAG_H,
	TAG_J,
	TAG_K,
	TAG_L,
	TAG_SEMICOLON,
	TAG_SQUOTE,
};
static const uint8_t keymap_bot_row_us[] = {
	TAG_Z,
	TAG_X,
	TAG_C,
	TAG_V,
	TAG_B,
	TAG_N,
	TAG_M,
	TAG_COMMA,
	TAG_DOT,
	TAG_SLASH,
};
//@}

/**
 @brief Dimensions of custom images.
 */
//@{
uint16_t img_bridgetek_logo_width = 0;
uint16_t img_bridgetek_logo_height = 0;
uint16_t img_settings_width = 0;
uint16_t img_settings_height = 0;
uint16_t img_cancel_width = 0;
uint16_t img_cancel_height = 0;
uint16_t img_tick_width = 0;
uint16_t img_tick_height = 0;
uint16_t img_refresh_width = 0;
uint16_t img_refresh_height = 0;
uint16_t img_keypad_width = 0;
uint16_t img_keypad_height = 0;
uint16_t img_keyboard_width = 0;
uint16_t img_keyboard_height = 0;
uint16_t img_media_width = 0;
uint16_t img_media_height = 0;
//@}

/**
 * @brief Extern links to fonts which are used here.
 */
extern const uint8_t font_arial_ttf_15_L4[];
extern const uint32_t font_arial_ttf_15_L4_size;
/**
 * @brief Extern links to images which are used here.
 */
extern const uint8_t img_bridgetek_logo_jpg[] asm("img_bridgetek_logo_jpg");
extern const uint8_t img_settings_jpg[] asm("img_settings_jpg");
extern const uint8_t img_cancel_jpg[] asm("img_cancel_jpg");
extern const uint8_t img_tick_jpg[] asm("img_z_jpg");
extern const uint8_t img_refresh_jpg[] asm("img_refresh_jpg");
extern const uint8_t img_keypad_jpg[] asm("img_keypad_jpg");
extern const uint8_t img_keyboard_jpg[] asm("img_keyboard_jpg");
extern const uint8_t img_media_jpg[] asm("img_media_jpg");

/* MACROS **************************************************************************/

/* LOCAL FUNCTIONS / INLINES *******************************************************/

static void eve_ui_keyboard_header_bar(uint32_t options)
{
	uint32_t x = EVE_SPACER;

	EVE_TAG(TAG_NO_ACTION);
	EVE_COLOR_RGB(128, 128, 128);
	EVE_BEGIN(EVE_BEGIN_RECTS);
	// Draw a bar across the header area.
	EVE_VERTEX2F(0 * 16, 0 * 16);
	// Standard height bar, 1/8 of the screen height.
	EVE_VERTEX2F(EVE_DISP_WIDTH * 16, (EVE_DISP_HEIGHT / 8) * 16);

	if ((options & EVE_HEADER_LOGO) && img_bridgetek_logo_width)
	{
		EVE_TAG(TAG_LOGO);
		EVE_BEGIN(EVE_BEGIN_BITMAPS);
		EVE_VERTEX_TRANSLATE_X(((EVE_DISP_WIDTH/2)-((uint32_t)img_bridgetek_logo_width/2)) * 16);
		EVE_VERTEX2II(0, 0, BITMAP_BRIDGETEK_LOGO, 0);
	}
	EVE_VERTEX_TRANSLATE_Y(EVE_SPACER * 16);
	if ((options & EVE_HEADER_SETTINGS_BUTTON) && img_settings_width)
	{
		EVE_TAG(TAG_SETTINGS);
		EVE_BEGIN(EVE_BEGIN_BITMAPS);
		EVE_VERTEX_TRANSLATE_X(x * 16);
		EVE_VERTEX2II(0, 0, BITMAP_SETTINGS, 0);
		x += (img_settings_width + EVE_SPACER);
	}
	if ((options & EVE_HEADER_REFRESH_BUTTON) && img_refresh_width)
	{
		EVE_TAG(TAG_REFRESH);
		EVE_BEGIN(EVE_BEGIN_BITMAPS);
		EVE_VERTEX_TRANSLATE_X(x * 16);
		EVE_VERTEX2II(0, 0, BITMAP_REFRESH, 0);
		x += (img_refresh_width + EVE_SPACER);
	}
	if ((options & EVE_HEADER_CANCEL_BUTTON) && img_cancel_width)
	{
		EVE_TAG(TAG_CANCEL);
		EVE_BEGIN(EVE_BEGIN_BITMAPS);
		EVE_VERTEX_TRANSLATE_X(x * 16);
		EVE_VERTEX2II(0, 0, BITMAP_CANCEL, 0);
		x += (img_cancel_width + EVE_SPACER);
	}
	if ((options & EVE_HEADER_SAVE_BUTTON) && img_tick_width)
	{
		EVE_TAG(TAG_SAVE);
		EVE_BEGIN(EVE_BEGIN_BITMAPS);
		EVE_VERTEX_TRANSLATE_X(x * 16);
		EVE_VERTEX2II(0, 0, BITMAP_SAVE, 0);
		x += (img_tick_width + EVE_SPACER);
	}

	x = EVE_DISP_WIDTH - EVE_SPACER;
	if ((options & EVE_HEADER_KEYPAD_BUTTON) && img_keypad_width)
	{
		EVE_TAG(TAG_KEYPAD);
		EVE_BEGIN(EVE_BEGIN_BITMAPS);
		x -= (img_keypad_width + EVE_SPACER);
		EVE_VERTEX_TRANSLATE_X(x * 16);
		EVE_VERTEX2II(0, 0, BITMAP_KEYPAD, 0);
	}
	if ((options & EVE_HEADER_KEYBOARD_BUTTON) && img_keyboard_width)
	{
		EVE_TAG(TAG_KEYBOARD);
		EVE_BEGIN(EVE_BEGIN_BITMAPS);
		x -= (img_keyboard_width + EVE_SPACER);
		EVE_VERTEX_TRANSLATE_X(x * 16);
		EVE_VERTEX2II(0, 0, BITMAP_KEYBOARD, 0);
	}
	if ((options & EVE_HEADER_EXTRA_BUTTON) && img_media_width)
	{
		EVE_TAG(TAG_MEDIA);
		EVE_BEGIN(EVE_BEGIN_BITMAPS);
		x -= (img_media_width + EVE_SPACER);
		EVE_VERTEX_TRANSLATE_X(x * 16);
		EVE_VERTEX2II(0, 0, BITMAP_MEDIA, 0);
	}
	if ((options & EVE_HEADER_SPECIAL_BUTTON) && img_tick_width)
	{
		EVE_TAG(TAG_SPECIAL);
		EVE_BEGIN(EVE_BEGIN_BITMAPS);
		x -= (img_tick_width + EVE_SPACER);
		EVE_VERTEX_TRANSLATE_X(x * 16);
		EVE_VERTEX2II(0, 0, BITMAP_SAVE, 0);
	}
	EVE_VERTEX_TRANSLATE_X(0);
	EVE_VERTEX_TRANSLATE_Y(0);
}

static void draw_keypad(uint8_t tag)
{
	uint32_t button_colour;

	if (keyboard_components & KEYBOARD_COMPONENTS_KEYPAD_CONTROL)
	{
		button_colour = (tag == TAG_PRINT_SCREEN) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_PRINT_SCREEN); // Print Screen
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(0), KEY_ROW_KEYPAD(0) + (KEY_HEIGHT_KEYPAD(1) * 1) / 6,
			KEY_WIDTH_KEYPAD(1), (KEY_HEIGHT_KEYPAD(1) * 2) / 3,
			KEYBOARD_FONT, EVE_OPT_FLAT, "PrtScr");

		button_colour = (key_state.Scroll) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_SCROLL_LOCK); // Scroll Lock
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(1), KEY_ROW_KEYPAD(0) + (KEY_HEIGHT_KEYPAD(1) * 1) / 6,
			KEY_WIDTH_KEYPAD(1), (KEY_HEIGHT_KEYPAD(1) * 2) / 3,
			KEYBOARD_FONT, EVE_OPT_FLAT, "ScrLock");

		button_colour = (tag == TAG_PAUSE) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_PAUSE); // Pause
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(2), KEY_ROW_KEYPAD(0) + (KEY_HEIGHT_KEYPAD(1) * 1) / 6,
			KEY_WIDTH_KEYPAD(1), (KEY_HEIGHT_KEYPAD(1) * 2) / 3,
			KEYBOARD_FONT, EVE_OPT_FLAT, "Pause");

		button_colour = (tag == TAG_INSERT) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_INSERT); // Insert
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(0), KEY_ROW_KEYPAD(1),
			KEY_WIDTH_KEYPAD(1), KEY_HEIGHT_KEYPAD(1),
			KEYBOARD_FONT, EVE_OPT_FLAT, "Ins");

		button_colour = (tag == TAG_HOME) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_HOME); // Home
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(1), KEY_ROW_KEYPAD(1),
			KEY_WIDTH_KEYPAD(1), KEY_HEIGHT_KEYPAD(1),
			KEYBOARD_FONT, EVE_OPT_FLAT, "Home");

		button_colour = (tag == TAG_PAGE_UP) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_PAGE_UP); // Page Up
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(2), KEY_ROW_KEYPAD(1),
			KEY_WIDTH_KEYPAD(1), KEY_HEIGHT_KEYPAD(1),
			KEYBOARD_FONT, EVE_OPT_FLAT, "PgUp");

		button_colour = (tag == TAG_DEL) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_DEL); // Delete
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(0), KEY_ROW_KEYPAD(2),
			KEY_WIDTH_KEYPAD(1), KEY_HEIGHT_KEYPAD(1),
			KEYBOARD_FONT, EVE_OPT_FLAT, "Del");

		button_colour = (tag == TAG_END) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_END); // End
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(1), KEY_ROW_KEYPAD(2),
			KEY_WIDTH_KEYPAD(1), KEY_HEIGHT_KEYPAD(1),
			KEYBOARD_FONT, EVE_OPT_FLAT, "End");

		button_colour = (tag == TAG_PAGE_DOWN) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_PAGE_DOWN); // Page Down
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(2), KEY_ROW_KEYPAD(2),
			KEY_WIDTH_KEYPAD(1), KEY_HEIGHT_KEYPAD(1),
			KEYBOARD_FONT, EVE_OPT_FLAT, "PgDn");

		button_colour = (tag == TAG_UP_ARROW) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_UP_ARROW); // Up Arrow
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(1), KEY_ROW_KEYPAD(4),
			KEY_WIDTH_KEYPAD(1), KEY_HEIGHT_KEYPAD(1),
			KEYBOARD_FONT_EXTENDED, EVE_OPT_FLAT, "\x02");

		button_colour = (tag == TAG_LEFT_ARROW) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_LEFT_ARROW); // Left Arrow
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(0), KEY_ROW_KEYPAD(5),
			KEY_WIDTH_KEYPAD(1), KEY_HEIGHT_KEYPAD(1),
			KEYBOARD_FONT_EXTENDED, EVE_OPT_FLAT, "\x01");

		button_colour = (tag == TAG_DOWN_ARROW) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_DOWN_ARROW); // Down Arrow
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(1), KEY_ROW_KEYPAD(5),
			KEY_WIDTH_KEYPAD(1), KEY_HEIGHT_KEYPAD(1),
			KEYBOARD_FONT_EXTENDED, EVE_OPT_FLAT, "\x04");

		button_colour = (tag == TAG_RIGHT_ARROW) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_RIGHT_ARROW); // Right Arrow
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(2), KEY_ROW_KEYPAD(5),
			KEY_WIDTH_KEYPAD(1), KEY_HEIGHT_KEYPAD(1),
			KEYBOARD_FONT_EXTENDED, EVE_OPT_FLAT, "\x03");
	}
	else
	{
		button_colour = (tag == TAG_BACKSPACE) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_BACKSPACE); // Backspace
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(8), KEY_ROW_KEYPAD(1),
			KEY_WIDTH_KEYPAD(1), KEY_HEIGHT_KEYPAD(1),
			KEYBOARD_FONT_EXTENDED, EVE_OPT_FLAT, "\x01");
	}

	if (keyboard_components & KEYBOARD_COMPONENTS_KEYPAD_CONTROL)
	{
		button_colour = (key_state.Numeric) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_NUMBER_LOCK); // Num Lock
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(4), KEY_ROW_KEYPAD(1),
			KEY_WIDTH_KEYPAD(1), KEY_HEIGHT_KEYPAD(1),
			KEYBOARD_FONT, EVE_OPT_FLAT, "Num");
	}

	if (keyboard_components & KEYBOARD_COMPONENTS_KEYPAD_ARITH)
	{
		button_colour = (tag == TAG_PAD_DIV) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_ALPHANUM;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_PAD_DIV); //
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(5), KEY_ROW_KEYPAD(1),
			KEY_WIDTH_KEYPAD(1), KEY_HEIGHT_KEYPAD(1),
			KEYBOARD_FONT, EVE_OPT_FLAT, "/");

		button_colour = (tag == TAG_PAD_MUL) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_ALPHANUM;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_PAD_MUL); //
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(6), KEY_ROW_KEYPAD(1),
			KEY_WIDTH_KEYPAD(1), KEY_HEIGHT_KEYPAD(1),
			KEYBOARD_FONT, EVE_OPT_FLAT, "*");

		button_colour = (tag == TAG_PAD_MINUS) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_ALPHANUM;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_PAD_MINUS); //
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(7), KEY_ROW_KEYPAD(1),
			KEY_WIDTH_KEYPAD(1), KEY_HEIGHT_KEYPAD(1),
			KEYBOARD_FONT, EVE_OPT_FLAT, "-");

		button_colour = (tag == TAG_PAD_PLUS) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_ALPHANUM;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_PAD_PLUS); //
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(7), KEY_ROW_KEYPAD(2),
			KEY_WIDTH_KEYPAD(1), KEY_HEIGHT_KEYPAD(2),
			KEYBOARD_FONT, EVE_OPT_FLAT, "+");
	}

	button_colour = (tag == TAG_PAD_ENTER) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_PAD_ENTER); //
	EVE_CMD_BUTTON(
		KEY_COL_KEYPAD(7), KEY_ROW_KEYPAD(4),
		KEY_WIDTH_KEYPAD(1), KEY_HEIGHT_KEYPAD(2),
		KEYBOARD_FONT, EVE_OPT_FLAT, "Enter");

	if (key_state.Numeric)
	{
		button_colour = (tag == TAG_PAD_7) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_ALPHANUM;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_PAD_7); //
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(4), KEY_ROW_KEYPAD(2),
			KEY_WIDTH_KEYPAD(1), KEY_HEIGHT_KEYPAD(1),
			KEYBOARD_FONT, EVE_OPT_FLAT, "7");

		button_colour = (tag == TAG_PAD_8) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_ALPHANUM;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_PAD_8); //
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(5), KEY_ROW_KEYPAD(2),
			KEY_WIDTH_KEYPAD(1), KEY_HEIGHT_KEYPAD(1),
			KEYBOARD_FONT, EVE_OPT_FLAT, "8");

		button_colour = (tag == TAG_PAD_9) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_ALPHANUM;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_PAD_9); //
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(6), KEY_ROW_KEYPAD(2),
			KEY_WIDTH_KEYPAD(1), KEY_HEIGHT_KEYPAD(1),
			KEYBOARD_FONT, EVE_OPT_FLAT, "9");

		button_colour = (tag == TAG_PAD_4) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_ALPHANUM;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_PAD_4); //
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(4), KEY_ROW_KEYPAD(3),
			KEY_WIDTH_KEYPAD(1), KEY_HEIGHT_KEYPAD(1),
			KEYBOARD_FONT, EVE_OPT_FLAT, "4");

		button_colour = (tag == TAG_PAD_5) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_ALPHANUM;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_PAD_5); //
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(5), KEY_ROW_KEYPAD(3),
			KEY_WIDTH_KEYPAD(1), KEY_HEIGHT_KEYPAD(1),
			KEYBOARD_FONT, EVE_OPT_FLAT, "5");

		button_colour = (tag == TAG_PAD_6) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_ALPHANUM;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_PAD_6); //
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(6), KEY_ROW_KEYPAD(3),
			KEY_WIDTH_KEYPAD(1), KEY_HEIGHT_KEYPAD(1),
			KEYBOARD_FONT, EVE_OPT_FLAT, "6");

		button_colour = (tag == TAG_PAD_1) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_ALPHANUM;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_PAD_1); //
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(4), KEY_ROW_KEYPAD(4),
			KEY_WIDTH_KEYPAD(1), KEY_HEIGHT_KEYPAD(1),
			KEYBOARD_FONT, EVE_OPT_FLAT, "1");

		button_colour = (tag == TAG_PAD_2) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_ALPHANUM;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_PAD_2); //
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(5), KEY_ROW_KEYPAD(4),
			KEY_WIDTH_KEYPAD(1), KEY_HEIGHT_KEYPAD(1),
			KEYBOARD_FONT, EVE_OPT_FLAT, "2");

		button_colour = (tag == TAG_PAD_3) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_ALPHANUM;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_PAD_3); //
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(6), KEY_ROW_KEYPAD(4),
			KEY_WIDTH_KEYPAD(1), KEY_HEIGHT_KEYPAD(1),
			KEYBOARD_FONT, EVE_OPT_FLAT, "3");

		button_colour = (tag == TAG_PAD_0) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_ALPHANUM;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_PAD_0); //
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(4), KEY_ROW_KEYPAD(5),
			KEY_WIDTH_KEYPAD(2), KEY_HEIGHT_KEYPAD(1),
			KEYBOARD_FONT, EVE_OPT_FLAT, "0");

		if (keyboard_components & KEYBOARD_COMPONENTS_KEYPAD_DOT)
		{
			button_colour = (tag == TAG_PAD_DOT) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_ALPHANUM;
			EVE_CMD_FGCOLOR(button_colour);
			EVE_TAG(TAG_PAD_DOT); //
			EVE_CMD_BUTTON(
				KEY_COL_KEYPAD(6), KEY_ROW_KEYPAD(5),
				KEY_WIDTH_KEYPAD(1), KEY_HEIGHT_KEYPAD(1),
				KEYBOARD_FONT, EVE_OPT_FLAT, ".");
		}
	}
	else
	{
		button_colour = (tag == TAG_PAD_7) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_PAD_7); // Home
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(4), KEY_ROW_KEYPAD(2),
			KEY_WIDTH_KEYPAD(1), KEY_HEIGHT_KEYPAD(1),
			KEYBOARD_FONT, EVE_OPT_FLAT, "Home");

		button_colour = (tag == TAG_PAD_8) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_PAD_8); // Up Arrow
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(5), KEY_ROW_KEYPAD(2),
			KEY_WIDTH_KEYPAD(1), KEY_HEIGHT_KEYPAD(1),
			KEYBOARD_FONT_EXTENDED, EVE_OPT_FLAT, "\x02");

		button_colour = (tag == TAG_PAD_9) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_PAD_9); // Page Up
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(6), KEY_ROW_KEYPAD(2),
			KEY_WIDTH_KEYPAD(1), KEY_HEIGHT_KEYPAD(1),
			KEYBOARD_FONT, EVE_OPT_FLAT, "PgUp");

		button_colour = (tag == TAG_PAD_4) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_PAD_4); // Left Arrow
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(4), KEY_ROW_KEYPAD(3),
			KEY_WIDTH_KEYPAD(1), KEY_HEIGHT_KEYPAD(1),
			KEYBOARD_FONT_EXTENDED, EVE_OPT_FLAT, "\x01");

		button_colour = (tag == TAG_PAD_5) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_PAD_5); // Blank
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(5), KEY_ROW_KEYPAD(3),
			KEY_WIDTH_KEYPAD(1), KEY_HEIGHT_KEYPAD(1),
			KEYBOARD_FONT, EVE_OPT_FLAT, "");

		button_colour = (tag == TAG_PAD_6) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_PAD_6); // Right Arrow
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(6), KEY_ROW_KEYPAD(3),
			KEY_WIDTH_KEYPAD(1), KEY_HEIGHT_KEYPAD(1),
			KEYBOARD_FONT_EXTENDED, EVE_OPT_FLAT, "\x03");

		button_colour = (tag == TAG_PAD_1) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_PAD_1); // End
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(4), KEY_ROW_KEYPAD(4),
			KEY_WIDTH_KEYPAD(1), KEY_HEIGHT_KEYPAD(1),
			KEYBOARD_FONT, EVE_OPT_FLAT, "End");

		button_colour = (tag == TAG_PAD_2) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_PAD_2); // Down Arrow
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(5), KEY_ROW_KEYPAD(4),
			KEY_WIDTH_KEYPAD(1), KEY_HEIGHT_KEYPAD(1),
			KEYBOARD_FONT_EXTENDED, EVE_OPT_FLAT, "\x04");

		button_colour = (tag == TAG_PAD_3) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_PAD_3); // Page Down
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(6), KEY_ROW_KEYPAD(4),
			KEY_WIDTH_KEYPAD(1), KEY_HEIGHT_KEYPAD(1),
			KEYBOARD_FONT, EVE_OPT_FLAT, "PgDn");

		button_colour = (tag == TAG_PAD_0) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_PAD_0); // Insert
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(4), KEY_ROW_KEYPAD(5),
			KEY_WIDTH_KEYPAD(2), KEY_HEIGHT_KEYPAD(1),
			KEYBOARD_FONT, EVE_OPT_FLAT, "Ins");

		button_colour = (tag == TAG_PAD_DOT) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_PAD_DOT); // Delete
		EVE_CMD_BUTTON(
			KEY_COL_KEYPAD(6), KEY_ROW_KEYPAD(5),
			KEY_WIDTH_KEYPAD(1), KEY_HEIGHT_KEYPAD(1),
			KEYBOARD_FONT, EVE_OPT_FLAT, "Del");
	}
}

static void draw_function_keys(uint8_t tag)
{
	uint32_t button_colour;
	int i;

	if (keyboard_components & KEYBOARD_COMPONENTS_ESCAPE)
	{
		button_colour = (tag == TAG_ESCAPE) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_ESCAPE); // Escape
		EVE_CMD_BUTTON(
			KEY_COL_ALPHA(0), KEY_ROW_ALPHA(0) + (KEY_HEIGHT_ALPHA * 1) / 6,
			KEY_WIDTH_ALPHA(1), (KEY_HEIGHT_ALPHA * 2) / 3,
			KEYBOARD_FONT, EVE_OPT_FLAT, "Esc");
	}

	if (keyboard_components & KEYBOARD_COMPONENTS_FUNCTION)
	{
		for (i = 0; i < 12; i++)
		{
			char name[4];
			sprintf(name, "F%d", i + 1);
			button_colour = (tag == (TAG_F1 + i)) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
			EVE_CMD_FGCOLOR(button_colour);
			EVE_TAG(TAG_F1 + i); // F1 to F12
			EVE_CMD_BUTTON(
				KEY_COL_ALPHA(1 + i) + (i > 4 ? (KEY_WIDTH_ALPHA(1) * 0.5) : 0) + (i > 8 ? (KEY_WIDTH_ALPHA(1) * 0.5) : 0),
				KEY_ROW_ALPHA(0) + (KEY_HEIGHT_ALPHA * 1) / 6,
				KEY_WIDTH_ALPHA(1), (KEY_HEIGHT_ALPHA * 2) / 3,
				KEYBOARD_FONT, EVE_OPT_FLAT, name);
		}
	}
}

static void draw_leds()
{
	if (keyboard_components & KEYBOARD_COMPONENTS_LEDS)
	{
		if (key_state.Numeric)
		{
			EVE_COLOR_RGB(255, 255, 0);
		}
		else
		{
			EVE_COLOR_RGB(32, 32, 0);
		}
		EVE_POINT_SIZE(10 * 16);
		EVE_BEGIN(EVE_BEGIN_POINTS);
		EVE_VERTEX2F(
			KEY_COL_STATUS(7) * 16,
			(KEY_ROW_STATUS(1) + (KEY_HEIGHT_STATUS / 2)) * 16);

		if (key_state.Caps)
		{
			EVE_COLOR_RGB(255, 255, 0);
		}
		else
		{
			EVE_COLOR_RGB(32, 32, 0);
		}
		EVE_POINT_SIZE(10 * 16);
		EVE_BEGIN(EVE_BEGIN_POINTS);
		EVE_VERTEX2F(
			KEY_COL_STATUS(8) * 16,
			(KEY_ROW_STATUS(1) + (KEY_HEIGHT_STATUS / 2)) * 16);

		if (key_state.Scroll)
		{
			EVE_COLOR_RGB(255, 255, 0);
		}
		else
		{
			EVE_COLOR_RGB(32, 32, 0);
		}
		EVE_POINT_SIZE(10 * 16);
		EVE_BEGIN(EVE_BEGIN_POINTS);
		EVE_VERTEX2F(
			KEY_COL_STATUS(9) * 16,
			(KEY_ROW_STATUS(1) + (KEY_HEIGHT_STATUS / 2)) * 16);

		EVE_COLOR_RGB((KEY_COLOUR_TOP >> 16) & 0xff, (KEY_COLOUR_TOP >> 8) & 0xff, (KEY_COLOUR_TOP >> 0) & 0xff);
		EVE_CMD_TEXT(
			KEY_COL_STATUS(7) + 32, KEY_ROW_STATUS(1) + (KEY_HEIGHT_STATUS / 2),
			KEYBOARD_FONT, EVE_OPT_CENTERX | EVE_OPT_CENTERY, "Num");
		EVE_CMD_TEXT(
			KEY_COL_STATUS(8) + 32, KEY_ROW_STATUS(1) + (KEY_HEIGHT_STATUS / 2),
			KEYBOARD_FONT, EVE_OPT_CENTERX | EVE_OPT_CENTERY, "Caps");
		EVE_CMD_TEXT(
			KEY_COL_STATUS(9) + 32, KEY_ROW_STATUS(1) + (KEY_HEIGHT_STATUS / 2),
			KEYBOARD_FONT, EVE_OPT_CENTERX | EVE_OPT_CENTERY, "Scroll");
	}
}

#ifdef USE_EXTRA_SCREEN
static void draw_extra(uint8_t tag)
{
	uint32_t button_colour;

	EVE_CMD_TEXT(
		KEY_COL_STATUS(0), KEY_ROW_STATUS(3) + (KEY_HEIGHT_STATUS / 2),
		KEYBOARD_FONT, EVE_OPT_CENTERY, "Volume:");

	button_colour = (tag == TAG_CC_MUTE) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_ALPHANUM;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_CC_MUTE); // Volume mute
	EVE_CMD_BUTTON(
		KEY_COL_STATUS(1), KEY_ROW_STATUS(3),
		KEY_WIDTH_STATUS(1), KEY_HEIGHT_STATUS,
		KEYBOARD_FONT, EVE_OPT_FLAT, "Mute");

	button_colour = (tag == TAG_CC_VOL_DOWN) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_ALPHANUM;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_CC_VOL_DOWN); // Volume down
	EVE_CMD_BUTTON(
		KEY_COL_STATUS(3), KEY_ROW_STATUS(3),
		KEY_WIDTH_STATUS(1), KEY_HEIGHT_STATUS,
		KEYBOARD_FONT, EVE_OPT_FLAT, "Down");

	button_colour = (tag == TAG_CC_VOL_UP) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_ALPHANUM;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_CC_VOL_UP); // Volume up
	EVE_CMD_BUTTON(
		KEY_COL_STATUS(2), KEY_ROW_STATUS(3),
		KEY_WIDTH_STATUS(1), KEY_HEIGHT_STATUS,
		KEYBOARD_FONT, EVE_OPT_FLAT, "Up");

	button_colour = (tag == TAG_CC_PREV) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_ALPHANUM;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_CC_PREV); // Previous Track
	EVE_CMD_BUTTON(
		KEY_COL_STATUS(4.5), KEY_ROW_STATUS(3),
		KEY_WIDTH_STATUS(1), KEY_HEIGHT_STATUS,
		KEYBOARD_FONT, EVE_OPT_FLAT, "Prev");

	button_colour = (tag == TAG_CC_STOP) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_ALPHANUM;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_CC_STOP); // Stop Track
	EVE_CMD_BUTTON(
		KEY_COL_STATUS(5.5), KEY_ROW_STATUS(3),
		KEY_WIDTH_STATUS(1), KEY_HEIGHT_STATUS,
		KEYBOARD_FONT, EVE_OPT_FLAT, "Stop");

	button_colour = (tag == TAG_CC_PLAY) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_ALPHANUM;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_CC_PLAY); // Play/Pause Track
	EVE_CMD_BUTTON(
		KEY_COL_STATUS(6.5), KEY_ROW_STATUS(3),
		KEY_WIDTH_STATUS(2), KEY_HEIGHT_STATUS,
		KEYBOARD_FONT, EVE_OPT_FLAT, "Play/Pause");

	button_colour = (tag == TAG_CC_NEXT) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_ALPHANUM;
	EVE_CMD_FGCOLOR(button_colour); 
	EVE_TAG(TAG_CC_NEXT); // Next Track
	EVE_CMD_BUTTON(
		KEY_COL_STATUS(8.5), KEY_ROW_STATUS(3),
		KEY_WIDTH_STATUS(1), KEY_HEIGHT_STATUS,
		KEYBOARD_FONT, EVE_OPT_FLAT, "Next");

	/* EDIT controls are application specific */
	EVE_CMD_TEXT(
			KEY_COL_STATUS(0), KEY_ROW_STATUS(7) + (KEY_HEIGHT_STATUS / 2),
			KEYBOARD_FONT, EVE_OPT_CENTERY,"Edit:");

	button_colour = (tag == TAG_CUT)?KEY_COLOUR_HIGHLIGHT:KEY_COLOUR_ALPHANUM;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_CUT); // Cut
	EVE_CMD_BUTTON(
			KEY_COL_STATUS(1), KEY_ROW_STATUS(7),
			KEY_WIDTH_STATUS(1), KEY_HEIGHT_STATUS,
			KEYBOARD_FONT, EVE_OPT_FLAT, "Cut");

	button_colour = (tag == TAG_COPY)?KEY_COLOUR_HIGHLIGHT:KEY_COLOUR_ALPHANUM;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_COPY); // Copy
	EVE_CMD_BUTTON(
			KEY_COL_STATUS(2), KEY_ROW_STATUS(7),
			KEY_WIDTH_STATUS(1), KEY_HEIGHT_STATUS,
			KEYBOARD_FONT, EVE_OPT_FLAT, "Copy");

	button_colour = (tag == TAG_PASTE)?KEY_COLOUR_HIGHLIGHT:KEY_COLOUR_ALPHANUM;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_PASTE); // Paste
	EVE_CMD_BUTTON(
			KEY_COL_STATUS(3), KEY_ROW_STATUS(7),
			KEY_WIDTH_STATUS(1), KEY_HEIGHT_STATUS,
			KEYBOARD_FONT, EVE_OPT_FLAT, "Paste");

	button_colour = (tag == TAG_UNDO)?KEY_COLOUR_HIGHLIGHT:KEY_COLOUR_ALPHANUM;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_UNDO); // Undo
	EVE_CMD_BUTTON(
			KEY_COL_STATUS(4), KEY_ROW_STATUS(7),
			KEY_WIDTH_STATUS(1), KEY_HEIGHT_STATUS,
			KEYBOARD_FONT, EVE_OPT_FLAT, "Undo");

	button_colour = (tag == TAG_REDO)?KEY_COLOUR_HIGHLIGHT:KEY_COLOUR_ALPHANUM;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_REDO); // Redo
	EVE_CMD_BUTTON(
			KEY_COL_STATUS(5), KEY_ROW_STATUS(7),
			KEY_WIDTH_STATUS(1), KEY_HEIGHT_STATUS,
			KEYBOARD_FONT, EVE_OPT_FLAT, "Redo");

	EVE_CMD_TEXT(
		KEY_COL_STATUS(0), KEY_ROW_STATUS(6) + (KEY_HEIGHT_STATUS / 2),
		KEYBOARD_FONT, EVE_OPT_CENTERY, "Other:");

	button_colour = (tag == TAG_FIND) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_ALPHANUM;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_FIND); // Find
	EVE_CMD_BUTTON(
		KEY_COL_STATUS(1), KEY_ROW_STATUS(6),
		KEY_WIDTH_STATUS(1), KEY_HEIGHT_STATUS,
		KEYBOARD_FONT, EVE_OPT_FLAT, "Find...");

	EVE_CMD_TEXT(
		KEY_COL_STATUS(0), KEY_ROW_STATUS(5) + (KEY_HEIGHT_STATUS / 2),
		KEYBOARD_FONT, EVE_OPT_CENTERY, "Power:");

	button_colour = (tag == TAG_SC_SLEEP) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_ALPHANUM;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_SC_SLEEP); // Power sleep
	EVE_CMD_BUTTON(
		KEY_COL_STATUS(1), KEY_ROW_STATUS(5),
		KEY_WIDTH_STATUS(1), KEY_HEIGHT_STATUS,
		KEYBOARD_FONT, EVE_OPT_FLAT, "Sleep");
}
#endif // USE_EXTRA_SCREEN

#ifdef USE_SPECIAL_SCREEN
static void draw_special(uint8_t tag)
{
	uint32_t button_colour;

	button_colour = (tag == TAG_SPACE) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_ALPHANUM;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_SPACE); // Space
	EVE_CMD_BUTTON(
		KEY_COL_ALPHA(4), KEY_ROW_ALPHA(4.5),
		KEY_WIDTH_ALPHA(6), (KEY_HEIGHT_ALPHA * 3) / 2,
		KEYBOARD_FONT, EVE_OPT_FLAT, "Power Up");

	button_colour = (tag == TAG_LEFT_ARROW) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_ALPHANUM;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_LEFT_ARROW); // Left Arrow
	EVE_CMD_BUTTON(
		KEY_COL_ALPHA(4), KEY_ROW_ALPHA(2.5),
		KEY_WIDTH_ALPHA(2), KEY_HEIGHT_ALPHA * 2,
		KEYBOARD_FONT, EVE_OPT_FLAT, "Turn Left");

	button_colour = (tag == TAG_DOWN_ARROW) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_ALPHANUM;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_DOWN_ARROW); // Down Arrow
	EVE_CMD_BUTTON(
		KEY_COL_ALPHA(6), KEY_ROW_ALPHA(2.5),
		KEY_WIDTH_ALPHA(2), KEY_HEIGHT_ALPHA * 2,
		KEYBOARD_FONT, EVE_OPT_FLAT, "U-Turn");

	button_colour = (tag == TAG_RIGHT_ARROW) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_ALPHANUM;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_RIGHT_ARROW); // Right Arrow
	EVE_CMD_BUTTON(
		KEY_COL_ALPHA(8), KEY_ROW_ALPHA(2.5),
		KEY_WIDTH_ALPHA(2), KEY_HEIGHT_ALPHA * 2,
		KEYBOARD_FONT, EVE_OPT_FLAT, "Turn Right");

	button_colour = (tag == TAG_PAGE_UP) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_PAGE_UP); // Page Up
	EVE_CMD_BUTTON(
		KEY_COL_ALPHA(13.5), KEY_ROW_ALPHA(1),
		KEY_WIDTH_KEYPAD(1.5), KEY_HEIGHT_KEYPAD(1),
		KEYBOARD_FONT, EVE_OPT_FLAT, "Hard");

	button_colour = (tag == TAG_PAGE_DOWN) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_PAGE_DOWN); // Page Down
	EVE_CMD_BUTTON(
		KEY_COL_ALPHA(13.5), KEY_ROW_ALPHA(2),
		KEY_WIDTH_KEYPAD(1.5), KEY_HEIGHT_KEYPAD(1),
		KEYBOARD_FONT, EVE_OPT_FLAT, "Easy");

	button_colour = (tag == TAG_F1) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_F1); // F1 to F12
	EVE_CMD_BUTTON(
		KEY_COL_ALPHA(0), KEY_ROW_ALPHA(0),
		KEY_WIDTH_ALPHA(3), KEY_HEIGHT_KEYPAD(1),
		KEYBOARD_FONT, EVE_OPT_FLAT, "Elbow");

	button_colour = (tag == TAG_F2) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_F2); // F1 to F12
	EVE_CMD_BUTTON(
		KEY_COL_ALPHA(3), KEY_ROW_ALPHA(0),
		KEY_WIDTH_ALPHA(3), KEY_HEIGHT_KEYPAD(1),
		KEYBOARD_FONT, EVE_OPT_FLAT, "Wave");

	button_colour = (tag == TAG_F3) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_F3); // F1 to F12
	EVE_CMD_BUTTON(
		KEY_COL_ALPHA(6), KEY_ROW_ALPHA(0),
		KEY_WIDTH_ALPHA(3), KEY_HEIGHT_KEYPAD(1),
		KEYBOARD_FONT, EVE_OPT_FLAT, "Ride On!");

	button_colour = (tag == TAG_F4) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_F4); // F1 to F12
	EVE_CMD_BUTTON(
		KEY_COL_ALPHA(9), KEY_ROW_ALPHA(0),
		KEY_WIDTH_ALPHA(3), KEY_HEIGHT_KEYPAD(1),
		KEYBOARD_FONT, EVE_OPT_FLAT, "Hammer Time!");

	button_colour = (tag == TAG_F5) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_F5); // F1 to F12
	EVE_CMD_BUTTON(
		KEY_COL_ALPHA(0), KEY_ROW_ALPHA(1),
		KEY_WIDTH_ALPHA(3), KEY_HEIGHT_KEYPAD(1),
		KEYBOARD_FONT, EVE_OPT_FLAT, "Nice!");

	button_colour = (tag == TAG_F6) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_F6); // F1 to F12
	EVE_CMD_BUTTON(
		KEY_COL_ALPHA(3), KEY_ROW_ALPHA(1),
		KEY_WIDTH_ALPHA(3), KEY_HEIGHT_KEYPAD(1),
		KEYBOARD_FONT, EVE_OPT_FLAT, "Bring It!");

	button_colour = (tag == TAG_F7) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_F7); // F1 to F12
	EVE_CMD_BUTTON(
		KEY_COL_ALPHA(6), KEY_ROW_ALPHA(1),
		KEY_WIDTH_ALPHA(3), KEY_HEIGHT_KEYPAD(1),
		KEYBOARD_FONT, EVE_OPT_FLAT, "I'm Toast!");

	button_colour = (tag == TAG_F8) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_F8); // F1 to F12
	EVE_CMD_BUTTON(
		KEY_COL_ALPHA(9), KEY_ROW_ALPHA(1),
		KEY_WIDTH_ALPHA(3), KEY_HEIGHT_KEYPAD(1),
		KEYBOARD_FONT, EVE_OPT_FLAT, "Bell");

	button_colour = (tag == TAG_F10) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_F10); // F1 to F12
	EVE_CMD_BUTTON(
		KEY_COL_ALPHA(0), KEY_ROW_ALPHA(3),
		KEY_WIDTH_ALPHA(2), KEY_HEIGHT_KEYPAD(1),
		KEYBOARD_FONT, EVE_OPT_FLAT, "Screenshot");

	button_colour = (tag == TAG_M) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_M); // 'm'
	EVE_CMD_BUTTON(
		KEY_COL_ALPHA(0), KEY_ROW_ALPHA(4),
		KEY_WIDTH_ALPHA(2), KEY_HEIGHT_KEYPAD(1),
		KEYBOARD_FONT, EVE_OPT_FLAT, "Message");
}
#endif // USE_EXTRA_SCREEN

static void draw_layout_selectors(void)
{
	uint32_t button_colour;

	EVE_CMD_TEXT(
		KEY_COL_STATUS(0), KEY_ROW_STATUS(3) + (KEY_HEIGHT_STATUS / 2),
		KEYBOARD_FONT, EVE_OPT_CENTERY, "Layout:");

	button_colour = (keyboard_layout == KEYBOARD_LAYOUT_PC_US_ALPHA) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_ALPHANUM;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_US_ALPHA); // US Keyboard Layout
	EVE_CMD_BUTTON(
		KEY_COL_STATUS(1), KEY_ROW_STATUS(3),
		KEY_WIDTH_STATUS(1), KEY_HEIGHT_STATUS,
		KEYBOARD_FONT, EVE_OPT_FLAT, "US");

	button_colour = (keyboard_layout == KEYBOARD_LAYOUT_PC_UK_ALPHA) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_ALPHANUM;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_UK_ALPHA); // UK Keyboard Layout
	EVE_CMD_BUTTON(
		KEY_COL_STATUS(2), KEY_ROW_STATUS(3),
		KEY_WIDTH_STATUS(1), KEY_HEIGHT_STATUS,
		KEYBOARD_FONT, EVE_OPT_FLAT, "UK");

	button_colour = (keyboard_layout == KEYBOARD_LAYOUT_PC_DE_ALPHA) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_ALPHANUM;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_DE_ALPHA); // German Keyboard Layout
	EVE_CMD_BUTTON(
		KEY_COL_STATUS(3), KEY_ROW_STATUS(3),
		KEY_WIDTH_STATUS(1), KEY_HEIGHT_STATUS,
		KEYBOARD_FONT, EVE_OPT_FLAT, "DE");
}

static void draw_keyboard_fixed_keys(uint8_t tag)
{
	uint32_t button_colour;

	button_colour = (tag == TAG_BACKSPACE) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_BACKSPACE); // Backspace
	EVE_CMD_BUTTON(
		KEY_COL_ALPHA(13), KEY_ROW_ALPHA(1),
		KEY_WIDTH_ALPHA(2), KEY_HEIGHT_ALPHA,
		KEYBOARD_FONT_EXTENDED, EVE_OPT_FLAT, "\x01");

	button_colour = (tag == TAG_TAB) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_TAB); // Tab
	if (key_state.ShiftL || key_state.ShiftR)
	{
		EVE_CMD_BUTTON(
			KEY_COL_ALPHA(0), KEY_ROW_ALPHA(2),
			KEY_WIDTH_ALPHA(1.5), KEY_HEIGHT_ALPHA,
			KEYBOARD_FONT_EXTENDED, EVE_OPT_FLAT, "\x05\x01");
	}
	else
	{
		EVE_CMD_BUTTON(
			KEY_COL_ALPHA(0), KEY_ROW_ALPHA(2),
			KEY_WIDTH_ALPHA(1.5), KEY_HEIGHT_ALPHA,
			KEYBOARD_FONT_EXTENDED, EVE_OPT_FLAT, "\x03\x05");
	}

	button_colour = (key_state.Caps) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_CAPS_LOCK); // Capslock
	EVE_CMD_BUTTON(
		KEY_COL_ALPHA(0), KEY_ROW_ALPHA(3),
		KEY_WIDTH_ALPHA(1.75), KEY_HEIGHT_ALPHA,
		KEYBOARD_FONT, EVE_OPT_FLAT, "CapsLock");

	button_colour = (tag == TAG_SPACE) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_ALPHANUM;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_SPACE); // Space
	EVE_CMD_BUTTON(
		KEY_COL_ALPHA(4), KEY_ROW_ALPHA(5),
		KEY_WIDTH_ALPHA(5.75), KEY_HEIGHT_ALPHA,
		KEYBOARD_FONT, EVE_OPT_FLAT, "Space");

	button_colour = (key_state.ShiftR) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_SHIFTR); // Shift Right
	EVE_CMD_BUTTON(
		KEY_COL_ALPHA(12.25), KEY_ROW_ALPHA(4),
		KEY_WIDTH_ALPHA(2.75), KEY_HEIGHT_ALPHA,
		KEYBOARD_FONT, EVE_OPT_FLAT, "Shift");

	if (keyboard_components & KEYBOARD_COMPONENTS_MODIFIERS)
	{
		button_colour = (key_state.CtrlL) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_CTRLL); // Ctrl Left
		EVE_CMD_BUTTON(
			KEY_COL_ALPHA(0), KEY_ROW_ALPHA(5),
			KEY_WIDTH_ALPHA(1.5), KEY_HEIGHT_ALPHA,
			KEYBOARD_FONT, EVE_OPT_FLAT, "Ctrl");

		button_colour = (key_state.WinL) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_WINL); // Win Left
		EVE_CMD_BUTTON(
			KEY_COL_ALPHA(1.5), KEY_ROW_ALPHA(5),
			KEY_WIDTH_ALPHA(1.25), KEY_HEIGHT_ALPHA,
			KEYBOARD_FONT, EVE_OPT_FLAT, "Gui");

		button_colour = (key_state.Alt) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_ALT); // Alt Left
		EVE_CMD_BUTTON(
			KEY_COL_ALPHA(2.75), KEY_ROW_ALPHA(5),
			KEY_WIDTH_ALPHA(1.25), KEY_HEIGHT_ALPHA,
			KEYBOARD_FONT, EVE_OPT_FLAT, "Alt");

		button_colour = (key_state.AltGr) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_ALTGR); // AltGr
		EVE_CMD_BUTTON(
			KEY_COL_ALPHA(9.75), KEY_ROW_ALPHA(5),
			KEY_WIDTH_ALPHA(1.25), KEY_HEIGHT_ALPHA,
			KEYBOARD_FONT, EVE_OPT_FLAT, "AltGr");

		button_colour = (key_state.WinR) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_WINR); // Win Right
		EVE_CMD_BUTTON(
			KEY_COL_ALPHA(11), KEY_ROW_ALPHA(5),
			KEY_WIDTH_ALPHA(1.25), KEY_HEIGHT_ALPHA,
			KEYBOARD_FONT, EVE_OPT_FLAT, "Gui");

		button_colour = (tag == TAG_APP) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_APP); // Context menu
		EVE_CMD_BUTTON(
			KEY_COL_ALPHA(12.25), KEY_ROW_ALPHA(5),
			KEY_WIDTH_ALPHA(1.25), KEY_HEIGHT_ALPHA,
			KEYBOARD_FONT, EVE_OPT_FLAT, "Menu");

		button_colour = (key_state.CtrlR) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
		EVE_CMD_FGCOLOR(button_colour);
		EVE_TAG(TAG_CTRLR); // Ctrl Right
		EVE_CMD_BUTTON(
			KEY_COL_ALPHA(13.5), KEY_ROW_ALPHA(5),
			KEY_WIDTH_ALPHA(1.5), KEY_HEIGHT_ALPHA,
			KEYBOARD_FONT, EVE_OPT_FLAT, "Ctrl");
	}
}

static uint8_t draw_keys(int16_t x, int16_t y,
						 uint16_t options, const char *display, const uint8_t *tag_array, uint8_t offset_map)
{
	uint32_t button_colour;
	int i;
	int count = strlen(display);
	char str[2] = {0, 0};
	int16_t use_font;
	uint8_t cdisp;
	uint8_t cmap;

	for (i = 0; i < count; i++)
	{
		cdisp = (uint8_t)display[i];
		cmap = (uint8_t)tag_array[i];

		button_colour = (cmap == (uint8_t)options) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_ALPHANUM;
		EVE_CMD_FGCOLOR(button_colour);

		EVE_TAG(cmap);

		use_font = KEYBOARD_FONT;
		if (cdisp >= 0x80)
		{
			// Special encoding in the custom font file
			use_font = KEYBOARD_FONT_EXTENDED;
			switch (cdisp)
			{
			case (uint8_t)'\xAC': // not sign
				cdisp = '\x10';
				break;
			case (uint8_t)'\xA3': // UKP sign
				cdisp = '\x11';
				break;
			case (uint8_t)'\x80': // Euro sign
				cdisp = '\x12';
				break;
			case (uint8_t)'\xA2': // Cent sign
				cdisp = '\x13';
				break;
			case (uint8_t)'\xA5': // Yen sign
				cdisp = '\x14';
				break;
			case (uint8_t)'\xA7': // section sign
				cdisp = '\x20';
				break;
			case (uint8_t)'\xDC': // capital U umlaut/diaeresis
				cdisp = '\x21';
				break;
			case (uint8_t)'\xFC': // lower u umlaut/diaeresis
				cdisp = '\x22';
				break;
			case (uint8_t)'\xD6': // capital O umlaut/diaeresis
				cdisp = '\x23';
				break;
			case (uint8_t)'\xF6': // lower o umlaut/diaeresis
				cdisp = '\x24';
				break;
			case (uint8_t)'\xC4': // capital A umlaut/diaeresis
				cdisp = '\x25';
				break;
			case (uint8_t)'\xE4': // lower a umlaut/diaeresis
				cdisp = '\x26';
				break;
			case (uint8_t)'\xB0': // degree symbol
				cdisp = '\x27';
				break;
			case (uint8_t)'\x60': // grave accent
				cdisp = '\x28';
				break;
			case (uint8_t)'\xB4': // accute accent
				cdisp = '\x29';
				break;
			case (uint8_t)'\xDF': // eszett
				cdisp = '\x2a';
				break;
			default:
				cdisp = '\x0f';
			}
		}

		str[0] = (char)cdisp;
		EVE_CMD_BUTTON(
			x + KEY_COL_ALPHA(i), y,
			KEY_WIDTH_ALPHA(1), KEY_HEIGHT_ALPHA,
			use_font, EVE_OPT_FLAT, str);
		offset_map++;
	}

	return offset_map;
}

static void draw_fixed_keys_uk_de(uint8_t tag)
{
	uint32_t button_colour;

	button_colour = (tag == TAG_ENTER) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_ENTER); // Enter
	EVE_CMD_BUTTON(
		KEY_COL_ALPHA(13.75), KEY_ROW_ALPHA(2),
		KEY_WIDTH_ALPHA(1.25), KEY_HEIGHT_ALPHA * 2 + KEY_SPACER_ALPHA,
		KEYBOARD_FONT, EVE_OPT_FLAT, "Enter");

	button_colour = (key_state.ShiftL) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_SHIFTL); // Shift Left
	EVE_CMD_BUTTON(
		KEY_COL_ALPHA(0), KEY_ROW_ALPHA(4),
		KEY_WIDTH_ALPHA(1.25), KEY_HEIGHT_ALPHA,
		KEYBOARD_FONT, EVE_OPT_FLAT, "Shift");
}

/* For safety escaped characters are separated within the strings which describe the keys.
 * This prevents escape sequences being incorrectly encoded by the compiler...
 * e.g. \x60ASDF to encode grave, A, S D F, will encode as \x60A, S D F.
 * Adding in quotes around each escaped character ensures compiler accuracy.
 */

static void draw_de_keyboard(uint8_t tag)
{
	uint8_t offset = 1;

	draw_fixed_keys_uk_de(tag);

	if (key_state.ShiftL || key_state.ShiftR)
	{
		offset = draw_keys(KEY_COL_ALPHA(0), KEY_ROW_ALPHA(1),
						   tag, "\xB0" "!\"" "\xA7" "$%&/()=?" "\x60", keymap_num_row_non_us, offset);

		offset = draw_keys(KEY_COL_ALPHA(1.5), KEY_ROW_ALPHA(2),
						   tag, "QWERTZUIOP" "\xDC" "*", keymap_top_row_non_us, offset);

		offset = draw_keys(KEY_COL_ALPHA(1.75), KEY_ROW_ALPHA(3),
						   tag, "ASDFGHJKL" "\xD6\xC4" "'", keymap_mid_row_non_us, offset);

		offset = draw_keys(KEY_COL_ALPHA(1.25), KEY_ROW_ALPHA(4),
						   tag, ">YXCVBNM;:_", keymap_bot_row_non_us, offset);
	}
	else
	{
		offset = draw_keys(KEY_COL_ALPHA(0), KEY_ROW_ALPHA(1),
						   tag, "^1234567890""\xDF""\xB4", keymap_num_row_non_us, offset);

		if (key_state.Caps)
		{
			offset = draw_keys(KEY_COL_ALPHA(1.5), KEY_ROW_ALPHA(2),
							   tag, "QWERTZUIOP" "\xDC" "+", keymap_top_row_non_us, offset);

			offset = draw_keys(KEY_COL_ALPHA(1.75), KEY_ROW_ALPHA(3),
							   tag, "ASDFGHJKL" "\xD6\xC4" "#", keymap_mid_row_non_us, offset);

			offset = draw_keys(KEY_COL_ALPHA(1.25), KEY_ROW_ALPHA(4),
							   tag, "<YXCVBNM,.-", keymap_bot_row_non_us, offset);
		}
		else
		{
			offset = draw_keys(KEY_COL_ALPHA(1.5), KEY_ROW_ALPHA(2),
							   tag, "qwertzuiop" "\xFC" "+", keymap_top_row_non_us, offset);

			offset = draw_keys(KEY_COL_ALPHA(1.75), KEY_ROW_ALPHA(3),
							   tag, "asdfghjkl" "\xF6\xE4" "#", keymap_mid_row_non_us, offset);

			offset = draw_keys(KEY_COL_ALPHA(1.25), KEY_ROW_ALPHA(4),
							   tag, "<yxcvbnm,.-", keymap_bot_row_non_us, offset);
		}
	}
	draw_function_keys(tag);
	draw_keyboard_fixed_keys(tag);
}

static void draw_uk_keyboard(uint8_t tag)
{
	uint8_t offset = 1;

	draw_fixed_keys_uk_de(tag);

	if (key_state.ShiftL || key_state.ShiftR)
	{
		offset = draw_keys(KEY_COL_ALPHA(0), KEY_ROW_ALPHA(1),
						   tag, "\xAC" "!\"" "\xA3" "$%^&*()_+", keymap_num_row_non_us, offset);

		offset = draw_keys(KEY_COL_ALPHA(1.5), KEY_ROW_ALPHA(2),
						   tag, "QWERTYUIOP{}", keymap_top_row_non_us, offset);

		offset = draw_keys(KEY_COL_ALPHA(1.75), KEY_ROW_ALPHA(3),
						   tag, "ASDFGHJKL:@~", keymap_mid_row_non_us, offset);

		offset = draw_keys(KEY_COL_ALPHA(1.25), KEY_ROW_ALPHA(4),
						   tag, "|ZXCVBNM<>?", keymap_bot_row_non_us, offset);
	}
	else
	{
		offset = draw_keys(KEY_COL_ALPHA(0), KEY_ROW_ALPHA(1),
						   tag, "`1234567890-=", keymap_num_row_non_us, offset);

		if (key_state.Caps)
		{
			offset = draw_keys(KEY_COL_ALPHA(1.5), KEY_ROW_ALPHA(2),
							   tag, "QWERTYUIOP[]", keymap_top_row_non_us, offset);

			offset = draw_keys(KEY_COL_ALPHA(1.75), KEY_ROW_ALPHA(3),
							   tag, "ASDFGHJKL;'#", keymap_mid_row_non_us, offset);

			offset = draw_keys(KEY_COL_ALPHA(1.25), KEY_ROW_ALPHA(4),
							   tag, "\\ZXCVBNM,./", keymap_bot_row_non_us, offset);
		}
		else
		{
			offset = draw_keys(KEY_COL_ALPHA(1.5), KEY_ROW_ALPHA(2),
							   tag, "qwertyuiop[]", keymap_top_row_non_us, offset);

			offset = draw_keys(KEY_COL_ALPHA(1.75), KEY_ROW_ALPHA(3),
							   tag, "asdfghjkl;'#", keymap_mid_row_non_us, offset);

			offset = draw_keys(KEY_COL_ALPHA(1.25), KEY_ROW_ALPHA(4),
							   tag, "\\zxcvbnm,./", keymap_bot_row_non_us, offset);
		}
	}
	draw_function_keys(tag);
	draw_keyboard_fixed_keys(tag);
}

static void draw_us_keyboard(uint8_t tag)
{
	uint32_t button_colour;
	uint8_t offset = 1;

	button_colour = (tag == TAG_ENTER) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_ENTER); // Enter
	EVE_CMD_BUTTON(
		KEY_COL_ALPHA(12.75), KEY_ROW_ALPHA(3),
		KEY_WIDTH_ALPHA(2.25), KEY_HEIGHT_ALPHA,
		KEYBOARD_FONT, EVE_OPT_FLAT, "Enter");

	button_colour = (key_state.ShiftL) ? KEY_COLOUR_HIGHLIGHT : KEY_COLOUR_CONTROL;
	EVE_CMD_FGCOLOR(button_colour);
	EVE_TAG(TAG_SHIFTL); // Shift Left
	EVE_CMD_BUTTON(
		KEY_COL_ALPHA(0), KEY_ROW_ALPHA(4),
		KEY_WIDTH_ALPHA(2.25), KEY_HEIGHT_ALPHA,
		KEYBOARD_FONT, EVE_OPT_FLAT, "Shift");

	if (key_state.ShiftL || key_state.ShiftR)
	{
		offset = draw_keys(KEY_COL_ALPHA(0), KEY_ROW_ALPHA(1),
						   tag, "~!@#$%^&*()_+", keymap_num_row_us, offset);

		offset = draw_keys(KEY_COL_ALPHA(1.5), KEY_ROW_ALPHA(2),
						   tag, "QWERTYUIOP{}|", keymap_top_row_us, offset);

		offset = draw_keys(KEY_COL_ALPHA(1.75), KEY_ROW_ALPHA(3),
						   tag, "ASDFGHJKL:\"", keymap_mid_row_us, offset);

		offset = draw_keys(KEY_COL_ALPHA(2.25), KEY_ROW_ALPHA(4),
						   tag, "ZXCVBNM<>?", keymap_bot_row_us, offset);
	}
	else
	{
		offset = draw_keys(KEY_COL_ALPHA(0), KEY_ROW_ALPHA(1),
						   tag, "\x60""1234567890-=", keymap_num_row_us, offset);

		if (key_state.Caps)
		{
			offset = draw_keys(KEY_COL_ALPHA(1.5), KEY_ROW_ALPHA(2),
							   tag, "QWERTYUIOP[]\\", keymap_top_row_us, offset);

			offset = draw_keys(KEY_COL_ALPHA(1.75), KEY_ROW_ALPHA(3),
							   tag, "ASDFGHJKL;\'", keymap_mid_row_us, offset);

			offset = draw_keys(KEY_COL_ALPHA(2.25), KEY_ROW_ALPHA(4),
							   tag, "ZXCVBNM,./", keymap_bot_row_us, offset);
		}
		else
		{
			offset = draw_keys(KEY_COL_ALPHA(1.5), KEY_ROW_ALPHA(2),
							   tag, "qwertyuiop[]\\", keymap_top_row_us, offset);

			offset = draw_keys(KEY_COL_ALPHA(1.75), KEY_ROW_ALPHA(3),
							   tag, "asdfghjkl;'", keymap_mid_row_us, offset);

			offset = draw_keys(KEY_COL_ALPHA(2.25), KEY_ROW_ALPHA(4),
							   tag, "zxcvbnm,./", keymap_bot_row_us, offset);
		}
	}
	draw_function_keys(tag);
	draw_keyboard_fixed_keys(tag);
}

static void draw_keyboard(const char *toast, uint32_t options, const char *edit, uint8_t tag)
{
	// Display List start
	EVE_LIB_BeginCoProList();
	EVE_CMD_DLSTART();

	EVE_CLEAR_COLOR_RGB(0, 0, 0);
	EVE_CLEAR(1, 1, 1);
	EVE_COLOR_RGB((KEY_COLOUR_TOP >> 16) & 0xff, (KEY_COLOUR_TOP >> 8) & 0xff, (KEY_COLOUR_TOP >> 0) & 0xff);
	// EVE_CLEAR_TAG(TAG_NO_ACTION);
	// EVE_TAG_MASK(1);

	// Colour for Special Function Keys
	EVE_CMD_FGCOLOR(KEY_COLOUR_CONTROL);
	EVE_CMD_BGCOLOR(KEY_COLOUR_CONTROL);

	if (keyboard_screen == KEYBOARD_SCREEN_SETTINGS)
	{
		draw_layout_selectors();
	}
	else if (keyboard_screen == KEYBOARD_SCREEN_ALPHANUMERIC)
	{
		if (keyboard_layout == KEYBOARD_LAYOUT_PC_UK_ALPHA)
		{
			draw_uk_keyboard(tag);
		}
		else if (keyboard_layout == KEYBOARD_LAYOUT_PC_US_ALPHA)
		{
			draw_us_keyboard(tag);
		}
		else if (keyboard_layout == KEYBOARD_LAYOUT_PC_DE_ALPHA)
		{
			draw_de_keyboard(tag);
		}
	}
#ifdef USE_EXTRA_SCREEN
	else if (keyboard_screen == KEYBOARD_SCREEN_EXTRA)
	{
		draw_extra(tag);
	}
#endif // USE_EXTRA_SCREEN
#ifdef USE_SPECIAL_SCREEN
	else if (keyboard_screen == KEYBOARD_SCREEN_SPECIAL)
	{
		draw_special(tag);
	}
#endif // USE_SPECIAL_SCREEN
	else if (keyboard_screen == KEYBOARD_SCREEN_KEYPAD)
	{
		draw_keypad(tag);
	}

	if (keyboard_screen != KEYBOARD_SCREEN_SETTINGS)
	{
		draw_leds();

		if (keyboard_components & KEYBOARD_COMPONENTS_TOAST)
		{
			EVE_TAG(0);
			EVE_CMD_TEXT(
				KEY_COL_STATUS(1) + KEY_SPACER_STATUS, KEY_ROW_STATUS(1) + (KEY_HEIGHT_STATUS / 2),
				KEYBOARD_FONT, EVE_OPT_CENTERY, toast);
		}

		if (keyboard_components & KEYBOARD_COMPONENTS_EDIT)
		{
			EVE_TAG(0);
			EVE_COLOR_RGB((KEY_COLOUR_ALPHANUM >> 16) & 0xff, (KEY_COLOUR_ALPHANUM >> 8) & 0xff, (KEY_COLOUR_ALPHANUM >> 0) & 0xff);
			EVE_BEGIN(EVE_BEGIN_RECTS);
			EVE_VERTEX2F(KEY_COL_STATUS(1) * 16, KEY_ROW_STATUS(2) * 16);
			EVE_VERTEX2F(KEY_COL_STATUS(9) * 16, (KEY_ROW_STATUS(2) + KEY_HEIGHT_STATUS) * 16);
			EVE_COLOR_RGB((KEY_COLOUR_TOP >> 16) & 0xff, (KEY_COLOUR_TOP >> 8) & 0xff, (KEY_COLOUR_TOP >> 0) & 0xff);
			EVE_CMD_TEXT(
				KEY_COL_STATUS(1) + KEY_SPACER_STATUS, KEY_ROW_STATUS(2) + (KEY_HEIGHT_STATUS / 2),
				KEYBOARD_FONT, EVE_OPT_CENTERY, edit);
		}

		// Draw header bar without unnecessary buttons
		if (keyboard_screen == KEYBOARD_SCREEN_ALPHANUMERIC)
		{
			eve_ui_keyboard_header_bar(options & (~(EVE_HEADER_KEYBOARD_BUTTON)));
		}
		else if (keyboard_screen == KEYBOARD_SCREEN_KEYPAD)
		{
			eve_ui_keyboard_header_bar(options & (~(EVE_HEADER_KEYPAD_BUTTON)));
		}
		else if (keyboard_screen == KEYBOARD_SCREEN_EXTRA)
		{
			eve_ui_keyboard_header_bar(options & (~(EVE_HEADER_EXTRA_BUTTON)));
		}
		else if (keyboard_screen == KEYBOARD_SCREEN_SPECIAL)
		{
			eve_ui_keyboard_header_bar(options & (~(EVE_HEADER_SPECIAL_BUTTON)));
		}
	}
	else // (keyboard_screen == KEYBOARD_SCREEN_SETTINGS)
	{
		eve_ui_keyboard_header_bar(options & (~(EVE_HEADER_KEYBOARD_BUTTON | EVE_HEADER_KEYPAD_BUTTON | EVE_HEADER_EXTRA_BUTTON | EVE_HEADER_SPECIAL_BUTTON)));
	}

	EVE_DISPLAY();
	EVE_CMD_SWAP();
	EVE_LIB_EndCoProList();
	EVE_LIB_AwaitCoProEmpty();
}

static uint8_t eve_loop(uint8_t *tag)
{
	static uint8_t key_pressed = KEY_PRESS_NONE;
	uint8_t ret = -1;
	uint8_t tag_code = 0;
	uint8_t key_detect = 0;
	uint8_t key_change = 0;
	uint8_t led_change = 0;
	uint8_t screen_change = 0;

	*tag = 0;
	key_detect = eve_ui_read_tag(&tag_code);

	if (key_detect)
	{
		// A key is currently pressed. If it was not pressed the last time
		// we came past then signal a new keypress.
		if (key_pressed == KEY_PRESS_NONE)
		{
			eve_ui_play_sound(0x51, 100);

			// Assume all valid keys are scan keys to start with.
			if (tag_code >= TAG_RESERVED_START)
			{
				switch (tag_code)
				{
				case TAG_UK_ALPHA:
					keyboard_layout = KEYBOARD_LAYOUT_PC_UK_ALPHA;
					screen_change = 1;
					break;

				case TAG_US_ALPHA:
					keyboard_layout = KEYBOARD_LAYOUT_PC_US_ALPHA;
					screen_change = 1;
					break;

				case TAG_DE_ALPHA:
					keyboard_layout = KEYBOARD_LAYOUT_PC_DE_ALPHA;
					screen_change = 1;
					break;

				case TAG_KEYPAD:
					keyboard_screen = KEYBOARD_SCREEN_KEYPAD;
					screen_change = 1;
					break;

				case TAG_KEYBOARD:
					keyboard_screen = KEYBOARD_SCREEN_ALPHANUMERIC;
					screen_change = 1;
					break;

				case TAG_MEDIA:
					keyboard_screen = KEYBOARD_SCREEN_EXTRA;
					screen_change = 1;
					break;

				case TAG_SPECIAL:
					keyboard_screen = KEYBOARD_SCREEN_SPECIAL;
					screen_change = 1;
					break;

				case TAG_SETTINGS:
					if (keyboard_screen == KEYBOARD_SCREEN_SETTINGS)
					{
						// Toggle to previous screen.
						keyboard_screen = KEYBOARD_SCREEN_ALPHANUMERIC;
					}
					else
					{
						keyboard_screen = KEYBOARD_SCREEN_SETTINGS;
					}
					screen_change = 1;
					break;

				case TAG_CANCEL:
					if (keyboard_screen == KEYBOARD_SCREEN_SETTINGS)
					{
						// Toggle to previous screen.
						keyboard_screen = KEYBOARD_SCREEN_ALPHANUMERIC;
						screen_change = 1;
					}
					else
					{
						ret = TAG_CANCEL;
					}
					break;

				case TAG_LOGO:
					ret = TAG_LOGO;
					eve_ui_screenshot();
					break;
				case TAG_SAVE:
					ret = TAG_SAVE;
					break;
				case TAG_REFRESH:
					ret = TAG_REFRESH;
					break;
				}
				key_pressed = KEY_PRESS_MODIFIER;
			}
			else if (tag_code != 0)
			{
				key_pressed = KEY_PRESS_SCAN;
				key_change = 1;

				// Check for modifiers.
				// Modifier keys are 'sticky' for resistive touchscreens as only one
				// touch can be registered at a time.
				// Turn scan keys into modifiers if appropriate.
				switch (tag_code)
				{
				case TAG_SHIFTL:
					key_state.ShiftL ^= 1; // toggle the shift button on when the key detect
					key_pressed = KEY_PRESS_MODIFIER;
					break;

				case TAG_SHIFTR:
					key_state.ShiftR ^= 1; // toggle the shift button on when the key detect
					key_pressed = KEY_PRESS_MODIFIER;
					break;

				case TAG_CTRLL:
					key_state.CtrlL ^= 1; // toggle the Ctrl button on when the key detect
					key_pressed = KEY_PRESS_MODIFIER;
					break;

				case TAG_CTRLR:
					key_state.CtrlR ^= 1; // toggle the Ctrl button on when the key detect
					key_pressed = KEY_PRESS_MODIFIER;
					break;

				case TAG_ALT:
					key_state.Alt ^= 1; // toggle the Alt button on when the key detect
					key_pressed = KEY_PRESS_MODIFIER;
					break;

				case TAG_ALTGR:
					key_state.AltGr ^= 1; // toggle the AltGr button on when the key detect
					key_pressed = KEY_PRESS_MODIFIER;
					break;

				case TAG_WINL:
					key_state.WinL ^= 1; // toggle the Windows button on when the key detect
					key_pressed = KEY_PRESS_MODIFIER;
					break;

				case TAG_WINR:
					key_state.WinR ^= 1; // toggle the Windows button on when the key detect
					key_pressed = KEY_PRESS_MODIFIER;
					break;

				case TAG_CAPS_LOCK:
					led_change = 1;
					if (key_state.Caps == 0)
					{
						key_state.Caps = 1;
					}
					else
					{
						key_state.Caps = 0;
					}
					break;

				case TAG_SCROLL_LOCK:
					led_change = 1;
					if (key_state.Scroll == 0)
					{
						key_state.Scroll = 1;
					}
					else
					{
						key_state.Scroll = 0;
					}
					break;

				case TAG_NUMBER_LOCK:
					led_change = 1;
					if (key_state.Numeric == 0)
					{
						key_state.Numeric = 1;
					}
					else
					{
						key_state.Numeric = 0;
					}
					break;
				}
				if (key_pressed == KEY_PRESS_SCAN)
				{
					*tag = tag_code;
				}
			}
		}
	}
	else
	{
		// No key is currently pressed. If one was pressed the last time we
		// came past then signal a key off event.
		if (key_pressed == KEY_PRESS_SCAN)
		{
			key_change = 1;
		}
		key_pressed = KEY_PRESS_NONE;
	}

	if ((key_change) || (led_change) || (screen_change))
	{
		ret = 0;
	}

	return ret;
}

/* FUNCTIONS ***********************************************************************/

#define KEYBOARD_COMPONENTS (KEYBOARD_COMPONENTS_ALPHANUMERIC | KEYBOARD_COMPONENTS_FUNCTION | KEYBOARD_COMPONENTS_MODIFIERS | \
		KEYBOARD_COMPONENTS_ESCAPE | KEYBOARD_COMPONENTS_LEDS | KEYBOARD_COMPONENTS_KEYPAD_DOT | \
		KEYBOARD_COMPONENTS_KEYPAD_CONTROL | KEYBOARD_COMPONENTS_KEYPAD_ARITH)

#ifndef USE_EXTRA_SCREEN
#define KEYBOARD_OPTIONS (EVE_HEADER_KEYBOARD_BUTTON | EVE_HEADER_KEYPAD_BUTTON | EVE_HEADER_LOGO | EVE_HEADER_SETTINGS_BUTTON)
#else
#ifndef USE_SPECIAL_SCREEN
#define KEYBOARD_OPTIONS (EVE_HEADER_KEYBOARD_BUTTON | EVE_HEADER_KEYPAD_BUTTON | EVE_HEADER_LOGO | EVE_HEADER_SETTINGS_BUTTON | \
		EVE_HEADER_EXTRA_BUTTON)
#else
#define KEYBOARD_OPTIONS (EVE_HEADER_KEYBOARD_BUTTON | EVE_HEADER_KEYPAD_BUTTON | EVE_HEADER_LOGO | EVE_HEADER_SETTINGS_BUTTON | \
		EVE_HEADER_EXTRA_BUTTON | EVE_HEADER_SPECIAL_BUTTON)
#endif // USE_SPECIAL_SCREEN
#endif // USE_EXTRA_SCREEN

void eve_ui_keyboard_set_layout(uint8_t layout)
{
	keyboard_layout = layout;
}

uint8_t eve_ui_keyboard_get_layout(void)
{
	return keyboard_layout;
}

void eve_ui_keyboard_set_components(uint32_t components)
{
	keyboard_components = components;
}

uint32_t eve_ui_keyboard_get_components(void)
{
	return keyboard_components;
}

void eve_ui_keyboard_set_screen(uint8_t screen)
{
	static uint8_t push_screen = KEYBOARD_SCREEN_SETTINGS;

	if ((keyboard_screen == KEYBOARD_SCREEN_SETTINGS)
			&& (screen == KEYBOARD_SCREEN_SETTINGS))
	{
		keyboard_screen = push_screen;
	}
	else if (screen == KEYBOARD_SCREEN_SETTINGS)
	{
		push_screen = keyboard_screen;
		keyboard_screen = screen;
	}
	else
	{
		keyboard_screen = screen;
	}
}

uint8_t eve_ui_keyboard_get_screen(void)
{
	return keyboard_screen;
}

void eve_ui_keyboard_splash(char *toast, uint32_t options)
{
	EVE_LIB_BeginCoProList();
	EVE_CMD_DLSTART();
	EVE_CLEAR_COLOR_RGB(0, 0, 0);
	EVE_CLEAR(1,1,1);
	//EVE_CLEAR_TAG(TAG_NO_ACTION);
	EVE_COLOR_RGB(255, 255, 255);
	eve_ui_keyboard_header_bar(EVE_HEADER_LOGO);
	EVE_CMD_TEXT(EVE_DISP_WIDTH/2, EVE_DISP_HEIGHT/2,
			FONT_HEADER, EVE_OPT_CENTERX | EVE_OPT_CENTERY, toast);
	//eve_ui_keyboard_header_bar(options);
	EVE_DISPLAY();
	EVE_CMD_SWAP();
	EVE_LIB_EndCoProList();
	EVE_LIB_AwaitCoProEmpty();
}

void eve_ui_keyboard_start(void)
{
	uint32_t img_address;

	img_address = eve_ui_load_font(font_arial_ttf_15_L4, font_arial_ttf_15_L4_size, KEYBOARD_FONT_EXTENDED);

	// Decode JPEG images from flash into RAM_DL on FT8xx.
	// Start at RAM_G after fonts (as font addresses must be fixed).
	init_ram_g(img_address);

	// Load images statically and sequentially.
	eve_ui_load_jpg(img_bridgetek_logo_jpg, BITMAP_BRIDGETEK_LOGO, 
				&img_bridgetek_logo_width, &img_bridgetek_logo_height);

	eve_ui_load_jpg(img_settings_jpg, BITMAP_SETTINGS, 
				&img_settings_width, &img_settings_height);

	eve_ui_load_jpg(img_cancel_jpg, BITMAP_CANCEL, 
				&img_cancel_width, &img_cancel_height);

	eve_ui_load_jpg(img_tick_jpg, BITMAP_SAVE, 
				&img_tick_width, &img_tick_height);

	eve_ui_load_jpg(img_refresh_jpg, BITMAP_REFRESH, 
				&img_refresh_width, &img_refresh_height);

	eve_ui_load_jpg(img_keypad_jpg, BITMAP_KEYPAD, 
				&img_keypad_width, &img_keypad_height);

	eve_ui_load_jpg(img_keyboard_jpg, BITMAP_KEYBOARD, 
				&img_keyboard_width, &img_keyboard_height);

	eve_ui_load_jpg(img_media_jpg, BITMAP_MEDIA, 
				&img_media_width, &img_media_height);

	keyboard_components = KEYBOARD_COMPONENTS;
	keyboard_screen = KEYBOARD_SCREEN_ALPHANUMERIC;

	draw_keyboard(NULL, KEYBOARD_OPTIONS, NULL, 0);
}

int8_t eve_ui_keyboard_loop(struct key_state *state, uint8_t *tag)
{
	uint8_t key_change = 0;
	uint8_t led_change = 0;
	uint8_t screen_change = 0;
	
	if (*tag == TAG_RESERVED_START)
	{
		screen_change = 1;
	}

	if ((state->Caps != key_state.Caps) || (state->Scroll != key_state.Scroll) || (state->Numeric != key_state.Numeric))
	{
		led_change = 1;

		key_state.Caps = state->Caps;
		key_state.Scroll = state->Scroll;
		key_state.Numeric = state->Numeric;
	}

	key_change = eve_loop(tag);

	memcpy(state, &key_state, sizeof(struct key_state));

	if ((key_change == 0) || (led_change) || (screen_change))
	{
		draw_keyboard(NULL, KEYBOARD_OPTIONS, NULL, *tag);
	}

	if (key_change == 0)
	{
		return 0;
	}

	return -1;
}
