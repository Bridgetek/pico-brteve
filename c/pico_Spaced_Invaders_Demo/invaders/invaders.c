/* Sample application to demonstrat FT800 primitives, widgets and customized screen shots */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "eve_ui.h"

#include "example.h"

extern uint8_t right_button_state(void);
extern uint8_t fire_button_state(void);
extern uint8_t left_button_state(void);

// =================================================================================================================
// Screen Zoom factor
// =================================================================================================================
#if (DISPLAY_RES == WQVGA)
#define ZOOM 1
#elif (DISPLAY_RES == WVGA)
#define ZOOM 2
#endif

// =================================================================================================================
// Spaced Invaders - #defines
// =================================================================================================================
// Game state values ...
#define GAME_START_NEW false
#define GAME_NEXT_WAVE true

// Bitmap and cell handles for invaders / explosion ...
#define BH_INVADERSEXPLODE 0
#define CH_INVADERS 0
#define CH_INVADER1 0
#define CH_INVADER2 2
#define CH_INVADER3 4
#define CH_INVADER4 6
#define CH_INVADER5 8
#define CH_EXPLODES 10

// Bitmap and cell handles for shields ...
#define BH_SHIELDS 1
#define CH_SHIELD1 0
#define CH_SHIELD2 4
#define CH_SHIELD3 8
#define CH_SHIELD4 12
#define CH_SHIELD5 16

// Bitmap and cell handles for missiles ...
#define BH_BULLETS 2
#define CH_BLASTER 0  // player  missile
#define CH_TUMBLER 4  // invader missile 1
#define CH_ZIGZAGS 8  // invader missile 2
#define CH_STINGER 12 // invader missile 3

// Bitmap and cell handles for player / saucer ...
#define BH_PLAYERSAUCER 3
#define CH_PLAYER 0
#define CH_SAUCER 2
#define CH_EXPLODEX 4

// Game play area size.
#define GAME_AREA_WIDTH 224
#define GAME_AREA_HEIGHT 256

// Bottom of game area.
#define GAME_AREA_BOTTOM (GAME_AREA_HEIGHT - 18)

// Offsets to centre on display ...
#define DISPLAY_XOFF ((EVE_DISP_WIDTH - (GAME_AREA_WIDTH * ZOOM)) / 2)
#define DISPLAY_YOFF ((EVE_DISP_HEIGHT - (GAME_AREA_HEIGHT * ZOOM)) / 2)

// Player specific info ...
#define PLAYERS_TOTL 1 // Total number of players
#define PLAYERS_XPOS ((PLAYERS_XMIN + PLAYERS_XMAX)/2)
#define PLAYERS_YPOS (8 * 27)
#define PLAYERS_XPIX 15
#define PLAYERS_YPIX 8
#define PLAYERS_XSPD 1
#define PLAYERS_YSPD 0
#define PLAYERS_XMIN 4
#define PLAYERS_XMAX (GAME_AREA_WIDTH - 20)

// Shield specific info ...
#define SHIELDS_TOTL (4 * 10) // Total number of sheild sprites (10 sprites per sheild)
#define SHIELDS_XPOS 28
#define SHIELDS_YPOS 8 * 24
#define SHIELDS_XPIX 6
#define SHIELDS_YPIX 6
#define SHIELDS_XGRD (SHIELDS_XPIX * 4) + 24
#define SHIELDS_YGRD 0

// Invader specific info ...
#define INVADER_COLS 11
#define INVADER_ROWS 5
#define INVADER_TOTL (INVADER_COLS * INVADER_ROWS) // Total number of invaders

#define INVADER_XMIN 8
#define INVADER_XMAX (GAME_AREA_WIDTH - 12 - 8)
#define INVADER_YMIN 8 * 8
#define INVADER_YMAX PLAYERS_YPOS

#define INVADER_XPIX 11
#define INVADER_YPIX 8
#define INVADER_XGRD 16
#define INVADER_YGRD 16
#define INVADER_XSPD 2
#define INVADER_YSPD 8
#define INVADER_LEFT 0
#define INVADER_RIGHT 1

// Bullet specific info ...
#define BULLETS_XPIX 3
#define BULLETS_YPIX 8
#define BULLETS_YMIN (8 * 4)
#define BULLETS_YMAX (8 * 29)
#define BULLETS_YSPD 2
#define BLASTER_YSPD 8

// Saucers specific info ...
#define SAUCERS_XPIX 16
#define SAUCERS_YPIX 8
#define SAUCERS_XMIN 0
#define SAUCERS_XMAX (GAME_AREA_WIDTH - SAUCERS_XPIX)
#define SAUCERS_YMIN 32
#define SAUCERS_YMAX 32
#define SAUCERS_XPOS 16
#define SAUCERS_YPOS (8 * 5)

// Default refresh times for update of objects (in milliseconds) ...
#define TIMER_OFF 0
#define TIMER_NOW 1
#define TIMER_DISPLAY 20
#define TIMER_PLAYERS 10
#define TIMER_SAUCERS 40
#define TIMER_SAUCERS_NEXT 5000
#define TIMER_INVADER 10
#define TIMER_SHIELDS 0
#define TIMER_BLASTER 20
#define TIMER_MISSILE_DROPPING 20
#define TIMER_MISSILE_WAVE_START (60 * TIMER_MISSILE_DROPPING)
#define TIMER_MISSILE_UPTO_0200 (48 * TIMER_MISSILE_DROPPING)
#define TIMER_MISSILE_UPTO_1000 (16 * TIMER_MISSILE_DROPPING)
#define TIMER_MISSILE_UPTO_2000 (11 * TIMER_MISSILE_DROPPING)
#define TIMER_MISSILE_UPTO_3000 (8 * TIMER_MISSILE_DROPPING)
#define TIMER_MISSILE_OVER_3000 (7 * TIMER_MISSILE_DROPPING)
#define TIMER_BUTTONS 10
#define TIMER_EXPLODE 200
#define TIMER_SAUCER_SCORE 500
#define TIMER_SPAWN 100

// Number of animation frames for each object ...
#define FRAME_PLAYER 2
#define FRAME_SAUCER 2
#define FRAME_INVADER 2
#define FRAME_BLASTER 4
#define FRAME_MISSILE 4

// Buttons for control ...
#define BUTTON_NONE 0x00
#define BUTTON_LEFT 0x01
#define BUTTON_RIGHT 0x02
#define BUTTON_START 0x08
#define BUTTON_FIRE 0x08

#define FIXED_FONT 20

// Enumerations, structures, tables etc.

typedef struct bitmap_header
{
	uint8_t Format;
	uint8_t Width;
	uint8_t Height;
	uint8_t Stride;
	int32_t Offset;
} bitmap_header_t;

typedef struct invader_object
{
	uint8_t xcrd;
	uint8_t ycrd;
	uint8_t type;
	uint8_t life;
	uint8_t anim;
} invader_object_t;

typedef struct invader_text
{
	uint8_t   xoff;
	uint8_t   yoff;
	uint8_t * text;
} invader_text;

// Life states (used by all objects) ...
typedef enum
{
	LIFE_SPAWN = 0, // about to live
	LIFE_KAPUT,		// dead
	LIFE_TOAST,		// dying
	LIFE_ALIVE,		// 1 hit  left
	LIFE_ALIVE2,	// 2 hits left
	LIFE_ALIVE3,	// 3 hits left
	LIFE_SHIELD,	// 4 hits left
	LIFE_TOTAL
} eLIFESTATE;

// Display refresh types (used for update of objects)
typedef enum
{
	// Must be the first three entries (used as array index)...
	REFRESH_TUMBLER = 0,
	REFRESH_ZIGZAGS,
	REFRESH_STINGER,
	// The rest can be in any order ...
	REFRESH_DISPLAY,
	REFRESH_SAUCERS,
	REFRESH_INVADER,
	REFRESH_SHIELDS,
	REFRESH_PLAYERS,
	REFRESH_BLASTER,
	REFRESH_BUTTONS,
	REFRESH_MISSILE,
	REFRESH_GENERAL,
	// Must be last entry ...
	REFRESH_TOTAL
} eREFRESH;

// Header of raw data containing properties of bitmap
const bitmap_header_t InvadersExplodeImageInfo[1] = {
		// format,width,height,stride,arrayoffset
		{EVE_FORMAT_L1, 12, 96, 2, 0}
};

const uint8_t InvadersExplodeImageData[192] = {
		// 'resolution ', 12, 'x', 96, 'format ', 'L1', 'stride ', 2, ' total size ', 192
		6, 0, 15, 0, 31, 128, 54, 192, 63, 192, 22, 128, 32, 64, 16, 128,		  // invader 1-1
		6, 0, 15, 0, 31, 128, 54, 192, 63, 192, 9, 0, 22, 128, 41, 64,			  // invader 1-2
		16, 64, 8, 128, 31, 192, 59, 96, 127, 240, 95, 208, 80, 80, 13, 128,	  // invader 2-1
		16, 64, 72, 144, 95, 208, 118, 240, 63, 224, 31, 192, 16, 64, 32, 32,	  // invader 2-2
		16, 64, 8, 128, 31, 192, 59, 96, 127, 240, 95, 208, 80, 80, 13, 128,	  // invader 3-1
		16, 64, 72, 144, 95, 208, 118, 240, 63, 224, 31, 192, 16, 64, 32, 32,	  // invader 3-2
		15, 0, 127, 224, 255, 240, 206, 112, 255, 240, 57, 192, 102, 96, 48, 192, // invader 4-1
		15, 0, 127, 224, 255, 240, 231, 48, 255, 240, 25, 128, 54, 192, 192, 48,  // invader 4-2
		15, 0, 127, 224, 255, 240, 206, 112, 255, 240, 57, 192, 102, 96, 48, 192, // invader 5-1
		15, 0, 127, 224, 255, 240, 231, 48, 255, 240, 25, 128, 54, 192, 192, 48,  // invader 5-2
		0, 0, 16, 32, 1, 0, 64, 0, 2, 32, 8, 0, 0, 128, 0, 0,					  // explode 1-1
		8, 64, 200, 128, 32, 128, 4, 48, 194, 0, 16, 64, 17, 48, 33, 0			  // explode 1-2
};

// Header of raw data containing properties of bitmap
const bitmap_header_t ShieldsImageInfo[1] = {
		// format,width,height,stride,arrayoffset
		{EVE_FORMAT_L1, 6, 120, 1, 0}
};

const uint8_t ShieldsImageData[120] = {
		// 'resolution ', 6, 'x', 120, 'format ', 'L1', 'stride ', 1, ' total size ', 120
		28, 60, 124, 252, 252, 252,	  // shield 1-1
		28, 56, 44, 124, 204, 244,	  // shield 1-2
		20, 24, 32, 124, 136, 148,	  // shield 1-3
		0, 8, 32, 36, 136, 0,		  // shield 1-4
		224, 240, 248, 252, 252, 252, // shield 2-1
		128, 240, 40, 124, 204, 244,  // shield 2-2
		0, 80, 32, 124, 136, 148,	  // shield 2-3
		0, 64, 32, 36, 136, 0,		  // shield 2-4
		252, 252, 240, 224, 192, 128, // shield 3-1
		156, 248, 32, 96, 192, 128,	  // shield 3-2
		20, 88, 32, 96, 128, 128,	  // shield 3-3
		0, 72, 32, 32, 128, 0,		  // shield 3-4
		252, 252, 60, 28, 12, 4,	  // shield 4-1
		156, 248, 44, 28, 12, 4,	  // shield 4-2
		20, 88, 32, 28, 8, 4,		  // shield 4-3
		0, 72, 32, 4, 8, 0,			  // shield 4-4
		252, 252, 252, 252, 252, 252, // shield 5-1
		156, 248, 44, 124, 204, 244,  // shield 5-2
		20, 88, 32, 124, 136, 148,	  // shield 5-3
		0, 72, 32, 36, 136, 0,		  // shield 5-4
};

// Header of raw data containing properties of bitmap
const bitmap_header_t MissileImageInfo[1] = {
		// format,width,height,stride,arrayoffset
		{EVE_FORMAT_L1, 3, 128, 1, 0}
};

const uint8_t MissileImageData[128] = {
		// 'resolution ', 3, 'x', 128, 'format ', 'L1', 'stride ', 1, ' total size ', 128
		64, 64, 64, 64, 0, 0, 0, 0,		  // blaster 1-1
		64, 64, 64, 64, 0, 0, 0, 0,		  // blaster 1-2
		64, 64, 64, 64, 0, 0, 0, 0,		  // blaster 1-3
		64, 64, 64, 64, 0, 0, 0, 0,		  // blaster 1-4
		224, 64, 64, 64, 64, 64, 64, 64,  // tumbler 1-1
		64, 64, 224, 64, 64, 64, 64, 64,  // tumbler 1-2
		64, 64, 64, 64, 64, 224, 64, 64,  // tumbler 1-3
		64, 64, 64, 64, 64, 64, 64, 224,  // tumbler 1-4
		64, 32, 64, 128, 64, 32, 64, 128, // zigzags 1-1
		32, 64, 128, 64, 32, 64, 128, 64, // zigzags 1-2
		64, 128, 64, 32, 64, 128, 64, 32, // zigzags 1-3
		128, 64, 32, 64, 128, 64, 32, 64, // zigzags 1-4
		64, 32, 64, 128, 64, 64, 64, 64,  // stinger 1-1
		64, 64, 64, 64, 32, 64, 128, 64,  // stinger 1-2
		64, 64, 32, 64, 128, 64, 64, 64,  // stinger 1-3
		64, 64, 64, 32, 64, 128, 64, 64	  // stinger 1-4
};

// Header of raw data containing properties of bitmap
const bitmap_header_t PlayerSaucerImageInfo[1] = {
		// format,width,height,stride,arrayoffset
		{EVE_FORMAT_L1, 16, 40, 2, 0}
};

const uint8_t PlayerSaucerImageData[80] = {
		// 'resolution ', 16, 'x', 40, 'format ', 'L1', 'stride ', 2, ' total size ', 80
		1, 0, 3, 128, 3, 128, 127, 252, 255, 254, 255, 254, 255, 254, 255, 254, // player base 1-1
		0, 0, 2, 128, 2, 128, 126, 252, 255, 254, 255, 254, 255, 254, 255, 254, // player base 1-2
		0, 0, 7, 224, 31, 248, 63, 252, 109, 182, 255, 255, 57, 156, 16, 8,		// saucer 1-1
		0, 0, 7, 224, 31, 248, 63, 252, 114, 78, 255, 255, 57, 156, 16, 8,		// saucer 1-2
		0, 0, 36, 36, 18, 72, 8, 16, 96, 6, 8, 16, 18, 72, 36, 36				// explode X
};

const uint8_t invaderScore[5] =	{4, 2, 2, 1, 1};

// Count of player bullets kept which wraps around after 15 shots and is used to
// determine score for saucer, so to score 300 each time shoot 1st saucer with
// 23rd bullet and then every 15th bullet after that ...
const uint8_t saucersScore[15] = {
		//   1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
		10, 5, 10, 15, 10, 10, 5, 30, 10, 10, 10, 5, 15, 10, 5
};

// Text for displaying score for saucer hit ...
const uint8_t *textForScore[6] = {
		"50", "100", "150", "?", "?", "300"
};

const int32_t colours[24] =	{
		EVE_ENC_COLOR_RGB(255, 63, 0),
		EVE_ENC_COLOR_RGB(255, 127, 0), //  1
		EVE_ENC_COLOR_RGB(255, 191, 0),
		EVE_ENC_COLOR_RGB(255, 255, 0), //  2
		EVE_ENC_COLOR_RGB(191, 255, 0),
		EVE_ENC_COLOR_RGB(127, 255, 0), //  3
		EVE_ENC_COLOR_RGB(63, 255, 0),
		EVE_ENC_COLOR_RGB(0, 255, 0), //  4
		EVE_ENC_COLOR_RGB(0, 255, 63),
		EVE_ENC_COLOR_RGB(0, 255, 127), //  5
		EVE_ENC_COLOR_RGB(0, 255, 191),
		EVE_ENC_COLOR_RGB(0, 255, 255), //  6
		EVE_ENC_COLOR_RGB(0, 191, 255),
		EVE_ENC_COLOR_RGB(0, 127, 255), //  7
		EVE_ENC_COLOR_RGB(0, 63, 255),
		EVE_ENC_COLOR_RGB(0, 0, 255), //  8
		EVE_ENC_COLOR_RGB(63, 0, 255),
		EVE_ENC_COLOR_RGB(127, 0, 255), //  9
		EVE_ENC_COLOR_RGB(191, 0, 255),
		EVE_ENC_COLOR_RGB(255, 0, 255), // 10
		EVE_ENC_COLOR_RGB(255, 0, 191),
		EVE_ENC_COLOR_RGB(255, 0, 127), // 11
		EVE_ENC_COLOR_RGB(255, 0, 63),
		EVE_ENC_COLOR_RGB(255, 0, 0) // 12
};

// Data containing x,y positions and cell types that make up a shield ...
const uint8_t shieldXcrdData[10] = {
		0, 6, 12, 18, // top row
		0, 6, 12, 18, // middle row
		0, 18,		  // bottom row
};

const uint8_t shieldYcrdData[10] = {
		0, 0, 0, 0, // top row
		6, 6, 6, 6, // middle row
		12, 12		// bottom row
};

const uint8_t shieldBitsData[10] = {
		CH_SHIELD1, CH_SHIELD5, CH_SHIELD5, CH_SHIELD2, // top row
		CH_SHIELD5, CH_SHIELD3, CH_SHIELD4, CH_SHIELD5, // middle row
		CH_SHIELD5, CH_SHIELD5							// bottom row
};

// invader columns to drop missiles in ...
const uint8_t dropsTumbler[] = {1, 7, 1, 1, 1, 4, 11, 1, 6, 3, 1, 1, 11, 9, 2, 8, 0};
const uint8_t dropsZigZags[] = {11, 1, 6, 3, 1, 1, 11, 9, 2, 8, 2, 11, 4, 7, 10, 0};

// invader wave start height according to number of waves completed ...
const uint8_t invaderWave[] = {1, 2, 3, 3, 3, 4, 4, 4};

// =================================================================================================================
// Spaced Invaders - Global variables
// =================================================================================================================
invader_object_t sigodInvader[INVADER_TOTL]; // invader info
invader_object_t sigodShields[SHIELDS_TOTL]; // shields info
invader_object_t sigodPlayers;			   // player  info
invader_object_t sigodSaucers;			   // saucer  info
invader_object_t sigodBlaster;			   // missile info (player)
invader_object_t sigodMissile[3];			   // missile info (invader TUMBLER, ZIGZAGS and STINGER)

// need to keep this separate from the text list below as we write the score directly into it
char displayScore[] = "000000   000000   000000"; // line  1, offset  2

static const invader_text siTextList[10] = {
		{1, 1, "SCORE<1> HI-SCORE SCORE<2>"}, // offset  1, line  1
		//       "000000   000000   000000"
		{2, 3, displayScore},			  // offset  2, line  3
		{12, 8, "PLAY"},				  // offset 12, line  8
		{7, 11, "SPACE  INVADERS"},		  // offset  7, line 11
		{4, 15, "*SCORE ADVANCE TABLE*"}, // offset  4, line 15
		{10, 17, "=? MYSTERY"},			  // offset 10, line 17
		{10, 19, "=30 POINTS"},			  // offset 10, line 19
		{10, 21, "=20 POINTS"},			  // offset 10, line 21
		{10, 23, "=10 POINTS"},			  // offset 10, line 23
		{17, 30, "CREDIT 01"}			  // offset 17, line 30
};

bool invaderEdges;	  // flag to mark that an invader has reached left/right edge of display
bool invaderLands;	  // flag to mark that an invader has reached bottom of display
uint8_t invaderAlive; // current number of invaders still alive
uint8_t invaderStart; // Y position to start invader wave at
uint8_t invaderMoveX; // used to move invaders backwards and forwards
uint8_t invaderMoveY; // used to move invaders downwards
uint8_t invaderCheck; // next invader to check (move it if alive)

uint8_t playersXcord;
uint8_t playersYcord;
uint8_t playersMoveX;
uint8_t playersLives;
uint8_t playersSpawn; // Used to flash player on/off during spawning
uint8_t blasterCount; // Current number of player shots fired
int8_t saucersMoveX;

// Counters for update of objects ...
volatile uint16_t updateTimer[REFRESH_TOTAL];

// Scores ...
int16_t scoreCurrent = 0;
int16_t scoreHighest = 0;
uint8_t *scoreSaucer;

// Guess ...
uint8_t currentWave;

// Missile control indexing ...
uint8_t indexZigZags;
uint8_t indexTumbler;

// Addresses of bitmap data to be stored in FT800 graphics ram ...
int32_t ramInvadersExplode;
int32_t ramShields;
int32_t ramMissile;
int32_t ramPlayerSaucer;
int32_t ramNextBit;

// FUNCTIONS
// Object setup and VertexII function

static void setup_object(invader_object_t *data, uint8_t xcrd, uint8_t ycrd, uint8_t type, uint8_t life)
{
	data->xcrd = xcrd;
	data->ycrd = ycrd;
	data->type = type;
	data->life = life;
	data->anim = 0;
}

static inline void bitmap_draw(uint16_t xcrd, uint16_t ycrd, char hndl, char cell)
{
#if !defined (EVE1_ENABLE)
	EVE_VERTEX2II((xcrd * ZOOM), (ycrd * ZOOM), hndl, cell);
#else // !defined (EVE1_ENABLE)
	EVE_VERTEX2II(DISPLAY_XOFF + (xcrd * ZOOM), DISPLAY_YOFF + (ycrd * ZOOM), hndl, cell);
#endif // !defined (EVE1_ENABLE)
}

// Initialisation

static void begin_game(bool gamestate)
{
	invader_object_t *data;
	uint8_t rows;
	uint8_t cols;
	uint8_t xcrd;
	uint8_t ycrd;

	if (gamestate == GAME_START_NEW)
	{
		playersLives = 3;
		currentWave = 0;
		blasterCount = 0;
		scoreCurrent = 0;
		saucersMoveX = 2;
		indexTumbler = 0;
		indexZigZags = 0;
		setup_object(&sigodPlayers, PLAYERS_XPOS, PLAYERS_YPOS, CH_PLAYER, LIFE_ALIVE);
	}
	else
	{
		currentWave = ++currentWave % 8;
	}

	// determine vertical start position of invaders according to wave ...
	invaderStart = INVADER_YMIN + invaderWave[currentWave] * INVADER_YSPD;

	// Initialise saucer ...
	setup_object(&sigodSaucers, 0, 0, 0, LIFE_KAPUT);

	// Initialise invaders ...
	invaderAlive = INVADER_TOTL;
	invaderMoveX = INVADER_RIGHT;
	invaderMoveY = 0;
	invaderEdges = false; // no invaders have reached edges  of display
	invaderLands = false; // no invaders have reached bottom of display
	invaderCheck = 0;	  // next invader to update

	data = sigodInvader;
	ycrd = invaderStart + 7 * 8;
	for (rows = 0; rows < INVADER_ROWS; rows++)
	{
		xcrd = INVADER_XMIN;
		for (cols = 0; cols < INVADER_COLS; cols++)
		{
			setup_object(data, xcrd, ycrd, CH_INVADERS + (8 - (rows * 2)), LIFE_SPAWN);
			xcrd += INVADER_XGRD;
			data++;
		}
		ycrd -= INVADER_YGRD;
	}

	// Initialise shields ...
	data = sigodShields;
	xcrd = SHIELDS_XPOS;
	for (cols = 0; cols < 4; cols++)
	{
		for (rows = 0; rows < 10; rows++)
		{
			setup_object(data, xcrd + shieldXcrdData[rows], SHIELDS_YPOS + shieldYcrdData[rows], shieldBitsData[rows], LIFE_SHIELD);
			data++;
		}
		xcrd += SHIELDS_XGRD;
	}

	// Initialise player blaster and invader missiles ...
	setup_object(&sigodBlaster, 0, 0, CH_BLASTER, LIFE_KAPUT);
	setup_object(&sigodMissile[REFRESH_TUMBLER], 0, 0, CH_TUMBLER, LIFE_KAPUT);
	setup_object(&sigodMissile[REFRESH_ZIGZAGS], 0, 0, CH_ZIGZAGS, LIFE_KAPUT);
	setup_object(&sigodMissile[REFRESH_STINGER], 0, 0, CH_STINGER, LIFE_KAPUT);

	// Initialise timers for movement of objects  ...
	updateTimer[REFRESH_DISPLAY] = TIMER_NOW;
	updateTimer[REFRESH_SAUCERS] = TIMER_SAUCERS_NEXT;
	updateTimer[REFRESH_INVADER] = TIMER_NOW;
	updateTimer[REFRESH_SHIELDS] = TIMER_NOW;
	updateTimer[REFRESH_PLAYERS] = TIMER_NOW;
	updateTimer[REFRESH_BLASTER] = TIMER_OFF;
	updateTimer[REFRESH_TUMBLER] = TIMER_OFF;
	updateTimer[REFRESH_ZIGZAGS] = TIMER_OFF;
	updateTimer[REFRESH_STINGER] = TIMER_OFF;
	updateTimer[REFRESH_BUTTONS] = TIMER_NOW;
	// Timer REFRESH_MISSILE is used to determine when first missile is fired after the start of a wave,
	// and subsequently the period between missiles being fired (which decreases as the score increases).
	updateTimer[REFRESH_MISSILE] = TIMER_MISSILE_WAVE_START;
}

// Object collision

static uint8_t bullet_collide(uint8_t bx, uint8_t by, invader_object_t *item, uint8_t cw, uint8_t ch)
{
	uint8_t state = LIFE_ALIVE;
	uint8_t xmin;
	uint8_t xmax;
	uint8_t ymin;
	uint8_t ymax;

	if (item->life >= LIFE_ALIVE)
	{
		xmin = item->xcrd;
		xmax = xmin + cw;

		if ((bx >= xmin) && (bx <= xmax))
		{
			ymin = item->ycrd;
			ymax = ymin + ch;

			if ((by >= ymin) && (by <= ymax))
			{
				// kill bullet ...
				state = LIFE_TOAST;
				// decrement life of object hit (shields take multiple hits) ...
				item->life--;
			}
		}
	}

	return state;
}

static uint8_t hits_on_players(uint8_t bx, uint8_t by)
{
	return bullet_collide(bx, by, &sigodPlayers, PLAYERS_XPIX, PLAYERS_YPIX);
}

static uint8_t hits_on_shields(uint8_t bx, uint8_t by)
{
	uint8_t index = 0;
	uint8_t state = LIFE_ALIVE;

	do
	{
		state = bullet_collide(bx, by, &sigodShields[index], SHIELDS_XPIX, SHIELDS_YPIX);
	} while ((state == LIFE_ALIVE) && (++index < SHIELDS_TOTL));

	return state;
}

static uint8_t hits_on_invader(uint8_t bx, uint8_t by)
{
	uint8_t index = 0;
	uint8_t state = LIFE_ALIVE;

	do
	{
		state = bullet_collide(bx, by, &sigodInvader[index], INVADER_XPIX, INVADER_YPIX);

		if (state == LIFE_TOAST)
		{
			// decrement number of invaders still alive ...
			invaderAlive--;

			// update current score according to type of invader ...
			scoreCurrent += invaderScore[sigodInvader[index].type >> 1];

			// invader has been hit so will pause for a brief time ...
			updateTimer[REFRESH_INVADER] = TIMER_EXPLODE;
		}
	} while ((state == LIFE_ALIVE) && (++index < INVADER_TOTL));

	return state;
}

static uint8_t hits_on_saucer(uint8_t bx, uint8_t by)
{
	uint8_t state;

	state = bullet_collide(bx, by, &sigodSaucers, SAUCERS_XPIX, SAUCERS_YPIX);

	if (state == LIFE_TOAST)
	{
		// update current score according to number of blaster shots fired ...
		scoreCurrent += saucersScore[blasterCount];

		// make a note of the score and display it for a while ...
		scoreSaucer = (uint8_t *)textForScore[(saucersScore[blasterCount] / 5) - 1];
		updateTimer[REFRESH_SAUCERS] = TIMER_SAUCER_SCORE;
	}

	return state;
}

static uint8_t hits_on_tumbler(uint8_t bx, uint8_t by)
{
	return bullet_collide(bx, by, &sigodMissile[REFRESH_TUMBLER], BULLETS_XPIX, BULLETS_YPIX);
}

static uint8_t hits_on_zigzag(uint8_t bx, uint8_t by)
{
	return bullet_collide(bx, by, &sigodMissile[REFRESH_ZIGZAGS], BULLETS_XPIX, BULLETS_YPIX);
}

static uint8_t hits_on_stinger(uint8_t bx, uint8_t by)
{
	return bullet_collide(bx, by, &sigodMissile[REFRESH_STINGER], BULLETS_XPIX, BULLETS_YPIX);
}

// Update object positions

#define invaders_TIMER_EXPIRED(timer) updateTimer[timer] == 0

static uint8_t invader_column(uint8_t column)
{
	uint8_t find = 0;
	column -= 1;

	// check to see if an invader is alive in the column specified ...
	for (find = 0; find < 5; find++)
	{
		if (sigodInvader[column].life == LIFE_ALIVE)
		{
			return column;
		}
		column += INVADER_COLS;
	}

	return 255;
}

static uint8_t invader_nearest()
{
	uint8_t index;

	// find the nearest invader vertically above the player (within 4 pixels of centre) ...
	for (index = 0; index < INVADER_TOTL; index++)
	{
		if (sigodInvader[index].life == LIFE_ALIVE)
		{
			if (abs(sigodInvader[index].xcrd - sigodPlayers.xcrd) < 4)
			{
				return index;
			}
		}
	}

	return 255;
}

static void update_saucers()
{
	if (invaders_TIMER_EXPIRED(REFRESH_SAUCERS))
	{
		switch (sigodSaucers.life)
		{
		case LIFE_ALIVE:
			// update position and frame ...
			sigodSaucers.xcrd += saucersMoveX;
			sigodSaucers.anim = ++sigodSaucers.anim % FRAME_SAUCER;

			if ((sigodSaucers.xcrd <= SAUCERS_XMIN) || (sigodSaucers.xcrd >= SAUCERS_XMAX))
			{
				sigodSaucers.life = LIFE_KAPUT;					   // kill off saucer if it has reached edge of display
				updateTimer[REFRESH_SAUCERS] = TIMER_SAUCERS_NEXT; // delay before next update
			}
			else
			{
				updateTimer[REFRESH_SAUCERS] = TIMER_SAUCERS; // delay before next update
			}
			break;

		case LIFE_TOAST:
			sigodSaucers.life = LIFE_KAPUT;
			updateTimer[REFRESH_SAUCERS] = TIMER_SAUCERS_NEXT; // delay before next update
			break;

		case LIFE_KAPUT:
			if (saucersMoveX == 2)
			{
				setup_object(&sigodSaucers, SAUCERS_XMIN, SAUCERS_YPOS, CH_SAUCER, LIFE_ALIVE);
			}
			else
			{
				setup_object(&sigodSaucers, SAUCERS_XMAX, SAUCERS_YPOS, CH_SAUCER, LIFE_ALIVE);
			}

			updateTimer[REFRESH_SAUCERS] = TIMER_NOW; // display immediately
			break;
		}
	}
}

static void update_invaders()
{
	if (invaders_TIMER_EXPIRED(REFRESH_INVADER))
	{
		uint8_t index;

		// kill off any intruder which was hit the last time ...
		for (index = 0; index < INVADER_TOTL; index++)
		{
			if (sigodInvader[index].life == LIFE_TOAST)
			{
				sigodInvader[index].life = LIFE_KAPUT;
			}
		}

		// move next invader ...
		for (index = 0; index < INVADER_TOTL; index++)
		{
			// if last invader has been checked ...
			if (invaderCheck == INVADER_TOTL)
			{
				// if any invader has reached edge ...
				if (invaderEdges)
				{
					// reverse X direction and drop down a line  ...
					invaderMoveX = (invaderMoveX == INVADER_LEFT) ? INVADER_RIGHT : INVADER_LEFT;
					invaderMoveY = INVADER_YSPD;
					// reset edge reached ...
					invaderEdges = false;
				}
				else
				{
					invaderMoveY = 0;
				}

				// start checking from the first invader again ...
				invaderCheck = 0;
			}

			if (sigodInvader[invaderCheck].life == LIFE_SPAWN)
			{
				sigodInvader[invaderCheck].life = LIFE_ALIVE;
			}

			// update position of invaders accordingly ...
			if (sigodInvader[invaderCheck].life == LIFE_ALIVE)
			{
				if (invaderMoveX == INVADER_LEFT)
				{
					sigodInvader[invaderCheck].xcrd -= INVADER_XSPD;
				}
				else
				{
					sigodInvader[invaderCheck].xcrd += INVADER_XSPD;
				}

				sigodInvader[invaderCheck].ycrd += invaderMoveY;
				sigodInvader[invaderCheck].anim = ++sigodInvader[invaderCheck].anim % FRAME_INVADER;

				// determine if direction needs to change ...
				if ((sigodInvader[invaderCheck].xcrd <= INVADER_XMIN) || (sigodInvader[invaderCheck].xcrd >= INVADER_XMAX))
				{
					invaderEdges = true;
				}
				else if (sigodInvader[invaderCheck].ycrd >= INVADER_YMAX)
				{
					invaderLands = true;
				}

				// next invader to check (next time) ...
				invaderCheck++;
				break;
			}
			else
			{
				// next invader to check ...
				invaderCheck++;
			}
		}

		// set delay before next update ...
		updateTimer[REFRESH_INVADER] = TIMER_INVADER;
	}
}

static void update_players()
{
	if (invaders_TIMER_EXPIRED(REFRESH_PLAYERS))
	{
		// set delay before next update ...
		updateTimer[REFRESH_PLAYERS] = TIMER_PLAYERS;

		switch (sigodPlayers.life)
		{
		case LIFE_ALIVE:
			// update position ...
			sigodPlayers.xcrd += playersMoveX;

			// keep within limits ...
			if (sigodPlayers.xcrd <= PLAYERS_XMIN)
			{
				sigodPlayers.xcrd = PLAYERS_XMIN;
			}
			else if (sigodPlayers.xcrd >= PLAYERS_XMAX)
			{
				sigodPlayers.xcrd = PLAYERS_XMAX;
			}

			playersMoveX = 0;
			break;

		case LIFE_TOAST:
			sigodPlayers.life = LIFE_KAPUT;
			break;

		case LIFE_KAPUT:
			break;

		case LIFE_SPAWN:
			if (++playersSpawn == 9)
			{
				sigodPlayers.life = LIFE_ALIVE;
			}
			else
			{
				updateTimer[REFRESH_PLAYERS] = TIMER_SPAWN;
			}
			break;
		}
	}
}

static void update_blaster()
{
	if (invaders_TIMER_EXPIRED(REFRESH_BLASTER))
	{
		switch (sigodBlaster.life)
		{
		case LIFE_ALIVE:
			// update position and frame ...
			sigodBlaster.ycrd -= BLASTER_YSPD;
			sigodBlaster.anim = ++sigodBlaster.anim % FRAME_BLASTER;

			if (sigodBlaster.ycrd <= BULLETS_YMIN)
			{
				sigodBlaster.ycrd = BULLETS_YMIN;
				sigodBlaster.life = LIFE_TOAST; // kill off bullet if it has reached top of display
			}
			else
			{
				sigodBlaster.life = hits_on_tumbler(sigodBlaster.xcrd + 1, sigodBlaster.ycrd);
				if (sigodBlaster.life == LIFE_ALIVE)
				{
					sigodBlaster.life = hits_on_zigzag(sigodBlaster.xcrd + 1, sigodBlaster.ycrd);
					if (sigodBlaster.life == LIFE_ALIVE)
					{
						sigodBlaster.life = hits_on_stinger(sigodBlaster.xcrd + 1, sigodBlaster.ycrd);
						if (sigodBlaster.life == LIFE_ALIVE)
						{
							sigodBlaster.life = hits_on_shields(sigodBlaster.xcrd + 1, sigodBlaster.ycrd);
							if (sigodBlaster.life == LIFE_ALIVE)
							{
								sigodBlaster.life = hits_on_invader(sigodBlaster.xcrd + 1, sigodBlaster.ycrd);
								if (sigodBlaster.life == LIFE_ALIVE)
								{
									sigodBlaster.life = hits_on_saucer(sigodBlaster.xcrd + 1, sigodBlaster.ycrd);
								}
							}
						}
					}
				}
			}

			updateTimer[REFRESH_BLASTER] = (sigodBlaster.life == LIFE_ALIVE) ? TIMER_BLASTER : TIMER_EXPLODE;
			break;

		case LIFE_TOAST:
			sigodBlaster.life = LIFE_KAPUT;
			break;
		}
	}
}

static uint16_t missile_timer()
{
	// The higher the current score, the more frequent the missiles are dropped ...
	if (scoreCurrent < 20)
	{
		return TIMER_MISSILE_UPTO_0200;
	}
	else if (scoreCurrent < 100)
	{
		return TIMER_MISSILE_UPTO_1000;
	}
	else if (scoreCurrent < 200)
	{
		return TIMER_MISSILE_UPTO_2000;
	}
	else if (scoreCurrent < 300)
	{
		return TIMER_MISSILE_UPTO_3000;
	}

	return TIMER_MISSILE_OVER_3000;
}

static void update_missile(uint8_t type)
{
	// Handling of invader missiles (TUMBLER, ZIGZAGS and STINGER) ...
	uint8_t invader = 255;
	uint8_t missile = 0;

	if (invaders_TIMER_EXPIRED(type))
	{
		switch (sigodMissile[type].life)
		{
		case LIFE_ALIVE:
			// update position and frame ...
			sigodMissile[type].ycrd += BULLETS_YSPD;
			sigodMissile[type].anim = ++sigodMissile[type].anim % FRAME_MISSILE;

			if (sigodMissile[type].ycrd >= BULLETS_YMAX)
			{
				sigodMissile[type].ycrd = BULLETS_YMAX;
				sigodMissile[type].life = LIFE_TOAST; // kill off missile if it has reached bottom of display
			}
			else
			{
				sigodMissile[type].life = hits_on_shields(sigodMissile[type].xcrd + 1, sigodMissile[type].ycrd + 8);
				if (sigodMissile[type].life == LIFE_ALIVE)
				{
					sigodMissile[type].life = hits_on_players(sigodMissile[type].xcrd + 1, sigodMissile[type].ycrd + 8);
				}
			}

			updateTimer[type] = (sigodMissile[type].life == LIFE_ALIVE) ? TIMER_MISSILE_DROPPING : TIMER_EXPLODE;
			break;

		case LIFE_TOAST:
			sigodMissile[type].life = LIFE_KAPUT;
			// delay before next update ...
			updateTimer[type] = TIMER_MISSILE_DROPPING;
			break;

		case LIFE_KAPUT:
			if (invaders_TIMER_EXPIRED(REFRESH_MISSILE))
			{
				sigodMissile[type].life = LIFE_SPAWN;
				// delay before next update ...
				updateTimer[type] = TIMER_NOW;
				// delay before next missile can be fired ...
				updateTimer[REFRESH_MISSILE] = missile_timer();
			}
			break;

		case LIFE_SPAWN:
			switch (type)
			{
			case REFRESH_TUMBLER:
				// select next column to drop TUMBLER missile in (handle wrap around as well) ...
				if (dropsTumbler[++indexTumbler] == 0)
				{
					indexTumbler = 0;
				}

				// find the index of the first invader alive in the column specified (if any) ...
				invader = invader_column(dropsTumbler[indexTumbler]);
				missile = CH_TUMBLER;
				break;

			case REFRESH_ZIGZAGS:
				// select next column to drop ZIGZAGS missile in (handle wrap around as well) ...
				if (dropsZigZags[++indexZigZags] == 0)
				{
					indexZigZags = 0;
				}

				// find the index of the first invader alive in the column specified (if any) ...
				invader = invader_column(dropsZigZags[indexZigZags]);
				missile = CH_ZIGZAGS;
				break;

			case REFRESH_STINGER:
				// find the index of the first invader alive vertically above the player (if any) ...
				invader = invader_nearest();
				missile = CH_STINGER;
				break;
			}

			if (invader != 255)
			{
				// invader selected to drop missile ...
				setup_object(&sigodMissile[type], sigodInvader[invader].xcrd + 4, sigodInvader[invader].ycrd + 16, missile, LIFE_ALIVE);
			}

			updateTimer[type] = TIMER_MISSILE_DROPPING;
			break;
		}
	}
}

static void update_screen(void)
{
	update_saucers();
	update_invaders();
	update_players();
	update_blaster();
	update_missile(REFRESH_TUMBLER);
	update_missile(REFRESH_ZIGZAGS);
	update_missile(REFRESH_STINGER);
}

// Add objects to display list

static void invaders_render_text(uint16_t xcrd, uint16_t ycrd, uint8_t *text)
{
	// This version of text display requires 1 command per character (a shed load for the start screen), however
	// it allows us to display font 20 as a mono-spaced font (mind you some letters are a bit out of position).
	xcrd -= 1; // need to shift 1 pixel left to position character on 'x' correctly
	ycrd -= 3; // need to shift 3 pixels up  to position character on 'y' correctly

	// add text to display list ...
	do
	{
		if (*text == 'I')
		{
			// need to centre the letter 'I' ...
			bitmap_draw(xcrd + 2, ycrd, FIXED_FONT, *text);
		}
		else
		{
			bitmap_draw(xcrd, ycrd, FIXED_FONT, *text);
		}
		xcrd += 8;
	} while (*++text != 0);
}

static void invaders_RenderExplode1(uint8_t xcrd, uint8_t ycrd)
{
	bitmap_draw(xcrd - 2, ycrd, BH_PLAYERSAUCER, CH_EXPLODEX); // invader explosion
}

static void invaders_RenderExplode2(uint8_t xcrd, uint8_t ycrd)
{
	bitmap_draw(xcrd - 4, ycrd, BH_INVADERSEXPLODE, CH_EXPLODES); // other explosion
}

static void invaders_insert_score(uint16_t value, uint8_t xchar)
{
	uint8_t digit;
	uint8_t index;
	uint16_t power[5] = {10000, 1000, 100, 10, 1};

	for (index = 0; index < 5; index++)
	{
		digit = 48;
		while (value >= power[index])
		{
			digit++;
			value -= power[index];
		}

		// insert digit
		displayScore[xchar++] = digit;
	}
}

static void render_invaders()
{
	uint8_t index;
	uint8_t lastcolour = 255;
	uint8_t nextcolour;

	// render all invaders according to life state ...
	for (index = 0; index < INVADER_TOTL; index++)
	{
		// only add colour command if colour has changed (minimises commands to send)
		nextcolour = (sigodInvader[index].ycrd - INVADER_YMIN) / 8;

		if (nextcolour > 23)
		{
			nextcolour = 23;
		}

		if (lastcolour != nextcolour)
		{
			lastcolour = nextcolour;
			EVE_COLOR(colours[nextcolour]);
		}

		switch (sigodInvader[index].life)
		{
		case LIFE_ALIVE:
			// add appropriate invader sprite to display list (depending on row and animation state) ...
			bitmap_draw(sigodInvader[index].xcrd, sigodInvader[index].ycrd, BH_INVADERSEXPLODE, sigodInvader[index].type + sigodInvader[index].anim);
			break;

		case LIFE_TOAST:
			// add appropriate toastiness graphic to display list ...
			invaders_RenderExplode1(sigodInvader[index].xcrd, sigodInvader[index].ycrd);
			break;
		}
	}
}

static void render_sheilds()
{
	uint8_t index;

	EVE_COLOR_RGB(0, 255, 0);

	for (index = 0; index < SHIELDS_TOTL; index++)
	{
		if (sigodShields[index].life > LIFE_TOAST)
		{
			// add appropriate shield graphic to display list ...
			bitmap_draw(sigodShields[index].xcrd, sigodShields[index].ycrd, BH_SHIELDS, sigodShields[index].type + (LIFE_SHIELD - sigodShields[index].life));
		}
	}
}

static void render_players()
{
	uint8_t lives = 0;

	// render player accordingly ...
	switch (sigodPlayers.life)
	{
	case LIFE_ALIVE:
		EVE_COLOR_RGB(0, 255, 0);
		if (sigodBlaster.life == LIFE_KAPUT)
		{
			bitmap_draw(sigodPlayers.xcrd, sigodPlayers.ycrd, BH_PLAYERSAUCER, CH_PLAYER + 0);
		}
		else
		{
			bitmap_draw(sigodPlayers.xcrd, sigodPlayers.ycrd, BH_PLAYERSAUCER, CH_PLAYER + 1);
		}
		break;

	case LIFE_TOAST:
		EVE_COLOR_RGB(255, 0, 0);
		invaders_RenderExplode1(sigodPlayers.xcrd, sigodPlayers.ycrd);
		break;

	case LIFE_SPAWN:
		// only display player every 2nd timeout (makes it flash on/off) ...
		if ((playersSpawn % 2) == 0)
		{
			bitmap_draw(sigodPlayers.xcrd, sigodPlayers.ycrd, BH_PLAYERSAUCER, CH_PLAYER + 0);
		}
		break;
	}

	for (lives = 1; lives < playersLives; lives++)
	{
		bitmap_draw((lives * 20), 240, BH_PLAYERSAUCER, CH_PLAYER);
	}
}

static void render_saucers()
{
	// render saucer accordingly ...
	switch (sigodSaucers.life)
	{
	case LIFE_ALIVE:
		EVE_COLOR_RGB(255, 0, 0);
		bitmap_draw(sigodSaucers.xcrd, sigodSaucers.ycrd, BH_PLAYERSAUCER, CH_SAUCER + sigodSaucers.anim);
		break;

	case LIFE_TOAST:
		EVE_COLOR_RGB(255, 0, 0);
		invaders_render_text(sigodSaucers.xcrd, sigodSaucers.ycrd, scoreSaucer);
		break;
	}
}

static void render_missiles(invader_object_t *missile)
{
	// render bullet accordingly ...
	switch (missile->life)
	{
	case LIFE_ALIVE:
		bitmap_draw(missile->xcrd, missile->ycrd, BH_BULLETS, missile->type + missile->anim);
		break;

	case LIFE_TOAST:
		invaders_RenderExplode2(missile->xcrd, missile->ycrd);
		break;
	}
}

static void render_scoring()
{
	// update high score if necessary ...
	if (scoreCurrent > scoreHighest)
	{
		scoreHighest = scoreCurrent;
	}

	EVE_COLOR_RGB(255, 255, 255);
	invaders_insert_score(scoreCurrent, 0);
	invaders_insert_score(scoreHighest, 9);
	invaders_render_text(siTextList[0].xoff * 8, siTextList[0].yoff * 8, siTextList[0].text);
	invaders_render_text(siTextList[1].xoff * 8, siTextList[1].yoff * 8, siTextList[1].text);
	invaders_render_text(siTextList[9].xoff * 8, siTextList[9].yoff * 8, siTextList[9].text);
}

static void invaders_render_background()
{
	uint16_t size = EVE_DISP_WIDTH;
	uint8_t fade;

	// Render sky ...
	EVE_BEGIN(EVE_BEGIN_BITMAPS);
	EVE_CMD_GRADIENT(0, 0, EVE_ENC_COLOR_RGB(7, 22, 35), 0, EVE_DISP_HEIGHT, EVE_ENC_COLOR_RGB(0, 46, 51));
	EVE_END();

#if !defined(EVE1_ENABLE)
	// Render moon ... yes, I know ... it's not very moon like (needs some craters)!
	EVE_BEGIN(EVE_BEGIN_POINTS);
	// Add some alpha blending ...
	EVE_COLOR_A(128);

	for (fade = 0; fade < 16; fade++)
	{
		EVE_POINT_SIZE(size << 4);
		EVE_COLOR_RGB(100 - fade * 2, 70 - fade * 2, 32 - fade * 2);
		EVE_VERTEX2F((DISPLAY_XOFF + ((GAME_AREA_WIDTH / 2) * ZOOM)) << 4, (DISPLAY_YOFF + (EVE_DISP_HEIGHT + 100 + (fade * 8))) << 4);
		size += 3;
	}
	EVE_END();
#endif
#
	// Render rectangle outlining actual play area (aiming for a raised panel look) ...
	EVE_BEGIN(EVE_BEGIN_LINE_STRIP);
	EVE_COLOR_RGB(96, 96, 96);
	EVE_LINE_WIDTH(6);
	EVE_VERTEX2F((DISPLAY_XOFF) << 4, (DISPLAY_YOFF + (GAME_AREA_HEIGHT * ZOOM)) << 4);
	EVE_VERTEX2F((DISPLAY_XOFF) << 4, (DISPLAY_YOFF) << 4);
	EVE_VERTEX2F((DISPLAY_XOFF + (GAME_AREA_WIDTH * ZOOM)) << 4, (DISPLAY_YOFF) << 4);
	EVE_COLOR_RGB(0, 0, 0);
	EVE_VERTEX2F((DISPLAY_XOFF + (GAME_AREA_WIDTH * ZOOM)) << 4, (DISPLAY_YOFF + (GAME_AREA_HEIGHT * ZOOM)) << 4);
	EVE_VERTEX2F((DISPLAY_XOFF) << 4, (DISPLAY_YOFF + (GAME_AREA_HEIGHT * ZOOM)) << 4);
	// Reset to fully opaque ...
	EVE_COLOR_A(255);
	EVE_END();

	// Render line towards bottom of display ...
	EVE_COLOR_RGB(0, 255, 0);
	EVE_LINE_WIDTH((1 * ZOOM) << 4);
	EVE_BEGIN(EVE_BEGIN_LINES);
	EVE_VERTEX2F((DISPLAY_XOFF + (0 * ZOOM)) << 4, (DISPLAY_YOFF + (GAME_AREA_BOTTOM * ZOOM)) << 4);
	EVE_VERTEX2F((DISPLAY_XOFF + (GAME_AREA_WIDTH * ZOOM)) << 4, (DISPLAY_YOFF + (GAME_AREA_BOTTOM * ZOOM)) << 4);
	EVE_END();
}

static void render_play(void)
{
#if defined (EVE2_ENABLE) || defined (EVE3_ENABLE) || defined (EVE4_ENABLE)
	EVE_VERTEX_TRANSLATE_X(DISPLAY_XOFF << 4);
	EVE_VERTEX_TRANSLATE_Y(DISPLAY_YOFF << 4);
#else
	EVE_CMD_TRANSLATE(DISPLAY_XOFF << 16, DISPLAY_YOFF << 16);
#endif // defined (EVE2_ENABLE) || defined (EVE3_ENABLE) || defined (EVE4_ENABLE)

	EVE_BEGIN(EVE_BEGIN_BITMAPS);

	render_players();
	render_sheilds();
	render_invaders();
	render_saucers();
	render_scoring();
	render_missiles(&sigodBlaster);
	render_missiles(&sigodMissile[REFRESH_TUMBLER]);
	render_missiles(&sigodMissile[REFRESH_ZIGZAGS]);
	render_missiles(&sigodMissile[REFRESH_STINGER]);
};

static void render_buttons()
{
#if defined (EVE2_ENABLE) || defined (EVE3_ENABLE) || defined (EVE4_ENABLE)
	EVE_VERTEX_TRANSLATE_X(0 << 4);
	EVE_VERTEX_TRANSLATE_Y(0 << 4);
#else
	EVE_CMD_TRANSLATE(DISPLAY_XOFF << 16, DISPLAY_YOFF << 16);
#endif // defined (EVE2_ENABLE) || defined (EVE3_ENABLE) || defined (EVE4_ENABLE)

	// Add invisible control buttons to display list ...
	EVE_COLOR_MASK(0, 0, 0, 0);

	// Centre part of the screen for firing.
	EVE_TAG(BUTTON_FIRE);
	EVE_BEGIN(EVE_BEGIN_RECTS);
	EVE_VERTEX2F((EVE_DISP_WIDTH / 4) * 16, 0);
	EVE_VERTEX2F(((EVE_DISP_WIDTH * 3) / 4) * 16, EVE_DISP_HEIGHT * 16);

	// Left and right sides for moving.
	EVE_TAG(BUTTON_LEFT);
	EVE_BEGIN(EVE_BEGIN_RECTS);
	EVE_VERTEX2F(0, 0);
	EVE_VERTEX2F((EVE_DISP_WIDTH / 4) * 16, EVE_DISP_HEIGHT * 16);
	EVE_TAG(BUTTON_RIGHT);
	EVE_BEGIN(EVE_BEGIN_RECTS);
	EVE_VERTEX2F(((EVE_DISP_WIDTH * 3) / 4) * 16, 0);
	EVE_VERTEX2F(EVE_DISP_WIDTH * 16, EVE_DISP_HEIGHT * 16);

	EVE_COLOR_MASK(255, 255, 255, 255);
}

static void render_start_button()
{
#if defined (EVE2_ENABLE) || defined (EVE3_ENABLE) || defined (EVE4_ENABLE)
	EVE_VERTEX_TRANSLATE_X(0 << 4);
	EVE_VERTEX_TRANSLATE_Y(0 << 4);
#else
	EVE_CMD_TRANSLATE(DISPLAY_XOFF << 16, DISPLAY_YOFF << 16);
#endif // defined (EVE2_ENABLE) || defined (EVE3_ENABLE) || defined (EVE4_ENABLE)

	// Add invisible control buttons to display list ...
	EVE_COLOR_MASK(0, 0, 0, 0);

	// Whole screen is for start.
	EVE_CMD_FGCOLOR(0x880000);
	EVE_CMD_GRADCOLOR(0xff0000);
	EVE_TAG(BUTTON_START);
	EVE_BEGIN(EVE_BEGIN_RECTS);
	EVE_VERTEX2F(0, 0);
	EVE_VERTEX2F(EVE_DISP_WIDTH * 16, EVE_DISP_HEIGHT * 16);

	EVE_COLOR_MASK(255, 255, 255, 255);
}

// Get bitmap data and store on EVE GPU RAM_G

static int32_t store_bitmaps(int32_t storeat, const bitmap_header_t *bitmap_info, const uint8_t *bitmap_data)
{
	uint32_t offset = 0;
	uint8_t ramData[512];
	const uint8_t *pm_addr = bitmap_data;
	uint32_t left;
	// calculate storage requirements for bitmap ...
	int32_t bitmap_datasize = bitmap_info->Height * bitmap_info->Stride;

	while (offset < bitmap_datasize)
	{
		memcpy(ramData, pm_addr, 512);

		if (bitmap_datasize - offset < 512)
		{
			left = bitmap_datasize - offset;
		}
		else
		{
			left = 512;
		}
		EVE_LIB_WriteDataToRAMG(ramData, left, storeat);
		offset += left;
		pm_addr += left;
		storeat += left;
	};

	// return address to store following data at...
	return (storeat);
}

// Insert bitmap handles etc into display list

static void setup_bitmaps(uint8_t handle, int32_t storeat, const bitmap_header_t *bitmap_info, uint8_t cells)
{
	// add bitmap handle to display list ...
	EVE_BITMAP_HANDLE(handle);
	EVE_BITMAP_SOURCE(storeat);
	EVE_BITMAP_SIZE(EVE_FILTER_NEAREST, EVE_WRAP_BORDER, EVE_WRAP_BORDER, bitmap_info->Width * ZOOM, bitmap_info->Height * ZOOM);
	EVE_BITMAP_LAYOUT(bitmap_info->Format, bitmap_info->Stride, bitmap_info->Height / cells);
}

static void display_list_start()
{
	// Start of new display list ...
	EVE_LIB_BeginCoProList();
	EVE_CMD_DLSTART();
}

static void display_list_end()
{
	// end of display list ...
	EVE_DISPLAY();
	EVE_CMD_SWAP();
	EVE_LIB_EndCoProList();
	EVE_LIB_AwaitCoProEmpty();
}

static void check_buttons()
{
	if (invaders_TIMER_EXPIRED(REFRESH_BUTTONS))
	{
		uint8_t tagTouch = HAL_MemRead8(EVE_REG_TOUCH_TAG);
		uint8_t randInvader = 0;

		if (fire_button_state() == 0)
		{
			tagTouch = BUTTON_FIRE;
		}
		if (left_button_state() == 0)
		{
			tagTouch = BUTTON_LEFT;
		}
		if (right_button_state() == 0)
		{
			tagTouch = BUTTON_RIGHT;
		}

		switch (tagTouch)
		{
		case BUTTON_LEFT:
			playersMoveX = -PLAYERS_XSPD;
			break;
		case BUTTON_RIGHT:
			playersMoveX = PLAYERS_XSPD;
			break;
		case BUTTON_FIRE:
			// only allow one shot at a time and when player is alive ...
			if ((sigodPlayers.life == LIFE_ALIVE) && (sigodBlaster.life == LIFE_KAPUT))
			{
				// fire blaster ...
				updateTimer[REFRESH_BLASTER] = TIMER_NOW;
				setup_object(&sigodBlaster, sigodPlayers.xcrd + 6, PLAYERS_YPOS, CH_BLASTER, LIFE_ALIVE);

				// saucer score related to number of blaster shots fired ...
				blasterCount = ++blasterCount % 15;

				// saucer direction reverses every 15th shot of blaster ...
				if (blasterCount == 0)
				{
					if (sigodSaucers.life != LIFE_ALIVE)
					{
						saucersMoveX = -saucersMoveX;
					}
				}
			}
			break;
		}
		updateTimer[REFRESH_BUTTONS] = TIMER_BUTTONS;
	}
}

static void wait_ms(uint16_t wait)
{
	updateTimer[REFRESH_GENERAL] = wait;
	while (!updateTimer[REFRESH_GENERAL]) {};
}

static void wait_for_button(uint8_t button)
{
	do
	{
		wait_ms(10);

		uint8_t tagTouch = HAL_MemRead8(EVE_REG_TOUCH_TAG);

		if (fire_button_state() == 0)
		{
			tagTouch = BUTTON_FIRE;
		}
		if (left_button_state() == 0)
		{
			tagTouch = BUTTON_LEFT;
		}
		if (right_button_state() == 0)
		{
			tagTouch = BUTTON_RIGHT;
		}

		if (tagTouch == button)
		{
			break;
		}
	} while (1);
}

static void wait_for_start()
{
	uint8_t index;

	// start new display list ...
	display_list_start();

	// add background and startup text ...
	invaders_render_background();

#if defined (EVE2_ENABLE) || defined (EVE3_ENABLE) || defined (EVE4_ENABLE)
	EVE_CMD_LOADIDENTITY();
	EVE_CMD_SCALE(ZOOM * 65536, ZOOM * 65536);
	EVE_CMD_SETMATRIX();

	EVE_VERTEX_TRANSLATE_X(DISPLAY_XOFF << 4);
	EVE_VERTEX_TRANSLATE_Y(DISPLAY_YOFF << 4);
#endif // defined (EVE2_ENABLE) || defined (EVE3_ENABLE) || defined (EVE4_ENABLE)

	EVE_BEGIN(EVE_BEGIN_BITMAPS);

	render_scoring();
	for (index = 2; index < 10; index++)
	{
		invaders_render_text(siTextList[index].xoff * 8, siTextList[index].yoff * 8, siTextList[index].text);
	}

	// add invader images to score table ...
	bitmap_draw(8 * 8 - 2, 8 * 17, BH_PLAYERSAUCER, CH_SAUCER);
	bitmap_draw(8 * 8, 8 * 19, BH_INVADERSEXPLODE, CH_INVADER1 + 1);
	bitmap_draw(8 * 8, 8 * 21, BH_INVADERSEXPLODE, CH_INVADER3);
	bitmap_draw(8 * 8, 8 * 23, BH_INVADERSEXPLODE, CH_INVADER5 + 1);

	render_start_button();

	// finish display list and send to FT800 ...
	display_list_end();

	// wait until 'START' button is pressed and released ...
	wait_for_button(BUTTON_START);
}

// Main control

static void invaders_main_loop()
{
	// wait for START switch to be pressed ...
	wait_for_start();

	// start game (reset and start first wave) ...
	begin_game(GAME_START_NEW);

	do // play game ...
	{
		do // play wave ...
		{
			check_buttons();
			update_screen();

			if (invaders_TIMER_EXPIRED(REFRESH_DISPLAY))
			{
				// start new display list ...
				display_list_start();

				// add all game objects to display list ...
				invaders_render_background();

				EVE_CMD_LOADIDENTITY();
				EVE_CMD_SCALE(ZOOM * 65536, ZOOM * 65536);
				EVE_CMD_SETMATRIX();

				render_play();

				render_buttons();

				// finish display list and send to FT800 ...
				display_list_end();

				// reset screen display update timer ...
				updateTimer[REFRESH_DISPLAY] = TIMER_DISPLAY;
			}

			// deal with player being hit by an invader missile ...
			if (sigodPlayers.life == LIFE_KAPUT)
			{
				// continue with wave if player has some lives left ...
				if (playersLives > 1)
				{
					playersLives--;
					playersSpawn = 0;
					setup_object(&sigodPlayers, PLAYERS_XPOS, PLAYERS_YPOS, CH_PLAYER, LIFE_SPAWN);
					updateTimer[REFRESH_PLAYERS] = TIMER_NOW;
				}
			}
		} while ((invaderAlive != 0) && (invaderLands == false) && (sigodPlayers.life != LIFE_KAPUT));

		if ((invaderLands == false) && (sigodPlayers.life == LIFE_ALIVE))
		{
			// reset invaders for next wave ...
			begin_game(GAME_NEXT_WAVE);
		}

	} while ((invaderLands == false) && (sigodPlayers.life == LIFE_ALIVE));
}

static bool timer_update_all()
{
	uint8_t timer;

	// refresh all update timers ...
	for (timer = 0; timer < REFRESH_TOTAL; timer++)
	{
		if (updateTimer[timer])
		{
			updateTimer[timer]--;
		}
	}

	return true;
}

// Hooks from main.c program.

void example_setup(void)
{
	// Initialise the display.
	EVE_Init();

	// Calibrate the display.
	eve_ui_calibrate();

	ramInvadersExplode = EVE_RAM_G;
	ramShields = store_bitmaps(ramInvadersExplode, InvadersExplodeImageInfo, InvadersExplodeImageData);
	ramMissile = store_bitmaps(ramShields, ShieldsImageInfo, ShieldsImageData);
	ramPlayerSaucer = store_bitmaps(ramMissile, MissileImageInfo, MissileImageData);
	ramNextBit = store_bitmaps(ramPlayerSaucer, PlayerSaucerImageInfo, PlayerSaucerImageData);

	// start new display list ...
	display_list_start();

	// add bitmap handles to display list ...
	setup_bitmaps(BH_INVADERSEXPLODE, ramInvadersExplode, InvadersExplodeImageInfo, 12);
	setup_bitmaps(BH_SHIELDS, ramShields, ShieldsImageInfo, 20);
	setup_bitmaps(BH_BULLETS, ramMissile, MissileImageInfo, 16);
	setup_bitmaps(BH_PLAYERSAUCER, ramPlayerSaucer, PlayerSaucerImageInfo, 5);

	EVE_BITMAP_HANDLE(FIXED_FONT);
	EVE_BITMAP_SIZE(EVE_FILTER_NEAREST, EVE_WRAP_BORDER, EVE_WRAP_BORDER, 8 * ZOOM, 12 * ZOOM);

	// finish display list and send to FT800 ...
	display_list_end();
}

void example_loop(void)
{
	invaders_main_loop();
}

void example_timer(void *x)
{
	timer_update_all();
}
/* Nothing beyond this */
