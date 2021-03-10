#ifndef EVE__H
#define EVE__H

#define EVE_DL_SIZE          (8 * 1024UL) /* 8kB Display List buffer size */
#define EVE_CMD_FIFO_SIZE    ((4) * 1024UL) /* 4kB coprocessor FIFO size */
#define EVE_CMD_FIFO_MASK    (EVE_CMD_FIFO_SIZE - 1)

#define ROM_CHIPID           0xC0000
#define RAM_DL               0x300000
#define REG_ID               0x302000
#define REG_FREQUENCY        (REG_ID + 12UL)
#define REG_CPURESET         0x302020
#define REG_HCYCLE           (REG_CPURESET + 12UL)
#define REG_HOFFSET          (REG_CPURESET + 16UL)
#define REG_HSIZE            (REG_CPURESET + 20UL)
#define REG_HSYNC0           (REG_CPURESET + 24UL)
#define REG_HSYNC1           (REG_CPURESET + 28UL)
#define REG_VCYCLE           (REG_CPURESET + 32UL)
#define REG_VOFFSET          (REG_CPURESET + 36UL)
#define REG_VSIZE            (REG_CPURESET + 40UL)
#define REG_VSYNC0           (REG_CPURESET + 44UL)
#define REG_VSYNC1           (REG_CPURESET + 48UL)
#define REG_DLSWAP           (REG_CPURESET + 52UL)
#define REG_ROTATE           (REG_CPURESET + 56UL)
#define REG_OUTBITS          (REG_CPURESET + 60UL)
#define REG_DITHER           (REG_CPURESET + 64UL)
#define REG_SWIZZLE          (REG_CPURESET + 68UL)
#define REG_CSPREAD          (REG_CPURESET + 72UL)
#define REG_PCLK_POL         (REG_CPURESET + 76UL)
#define REG_PCLK             (REG_CPURESET + 80UL)
#define REG_GPIOX_DIR        (REG_CPURESET + 120UL)
#define REG_GPIOX            (REG_CPURESET + 124UL)
#define REG_CMD_READ         (0x3020F8)
#define REG_CMD_WRITE        (REG_CMD_READ + 4UL)
#define REG_CTOUCH_EXTENDED  (REG_CMD_READ + 16UL)
#define REG_CMDB_SPACE       (0x302574)
#define REG_CMDB_WRITE       (REG_CMDB_SPACE + 4UL)
#define REG_PCLK_FREQ        (REG_CMDB_SPACE + 160UL)

#define CMD_DLSTART          0xFFFFFF00
#define CMD_SWAP             0xFFFFFF01
#define CMD_INTERRUPT        0xFFFFFF02
#define CMD_BGCOLOR          0xFFFFFF09
#define CMD_FGCOLOR          0xFFFFFF0A
#define CMD_GRADIENT         0xFFFFFF0B
#define CMD_TEXT             0xFFFFFF0C
#define CMD_BUTTON           0xFFFFFF0D
#define CMD_KEYS             0xFFFFFF0E
#define CMD_PROGRESS         0xFFFFFF0F
#define CMD_SLIDER           0xFFFFFF10
#define CMD_SCROLLBAR        0xFFFFFF11
#define CMD_TOGGLE           0xFFFFFF12
#define CMD_GAUGE            0xFFFFFF13
#define CMD_CLOCK            0xFFFFFF14
#define CMD_CALIBRATE        0xFFFFFF15
#define CMD_SPINNER          0xFFFFFF16
#define CMD_STOP             0xFFFFFF17
#define CMD_MEMCRC           0xFFFFFF18
#define CMD_REGREAD          0xFFFFFF19
#define CMD_MEMWRITE         0xFFFFFF1A
#define CMD_MEMSET           0xFFFFFF1B
#define CMD_MEMZERO          0xFFFFFF1C
#define CMD_MEMCPY           0xFFFFFF1D
#define CMD_APPEND           0xFFFFFF1E
#define CMD_SNAPSHOT         0xFFFFFF1F
#define CMD_BITMAP_TRANSFORM 0xFFFFFF21
#define CMD_INFLATE          0xFFFFFF22
#define CMD_GETPTR           0xFFFFFF23
#define CMD_LOADIMAGE        0xFFFFFF24
#define CMD_GETPROPS         0xFFFFFF25
#define CMD_LOADIDENTITY     0xFFFFFF26
#define CMD_TRANSLATE        0xFFFFFF27
#define CMD_SCALE            0xFFFFFF28
#define CMD_ROTATE           0xFFFFFF29
#define CMD_SETMATRIX        0xFFFFFF2A
#define CMD_SETFONT          0xFFFFFF2B
#define CMD_TRACK            0xFFFFFF2C
#define CMD_DIAL             0xFFFFFF2D
#define CMD_NUMBER           0xFFFFFF2E
#define CMD_SCREENSAVER      0xFFFFFF2F
#define CMD_SKETCH           0xFFFFFF30
#define CMD_LOGO             0xFFFFFF31
#define CMD_COLDSTART        0xFFFFFF32
#define CMD_GETMATRIX        0xFFFFFF33
#define CMD_GRADCOLOR        0xFFFFFF34
#define CMD_SETROTATE        0xFFFFFF36
#define CMD_SNAPSHOT2        0xFFFFFF37
#define CMD_SETBASE          0xFFFFFF38
#define CMD_MEDIAFIFO        0xFFFFFF39
#define CMD_PLAYVIDEO        0xFFFFFF3A
#define CMD_SETFONT2         0xFFFFFF3B
#define CMD_SETSCRATCH       0xFFFFFF3C
#define CMD_ROMFONT          0xFFFFFF3F
#define CMD_VIDEOSTART       0xFFFFFF40
#define CMD_VIDEOFRAME       0xFFFFFF41
#define CMD_SYNC             0xFFFFFF42
#define CMD_SETBITMAP        0xFFFFFF43
#define CMD_FLASHERASE       0xFFFFFF44
#define CMD_FLASHWRITE       0xFFFFFF45
#define CMD_FLASHREAD        0xFFFFFF46
#define CMD_FLASHUPDATE      0xFFFFFF47
#define CMD_FLASHDETACH      0xFFFFFF48
#define CMD_FLASHATTACH      0xFFFFFF49
#define CMD_FLASHFAST        0xFFFFFF4A
#define CMD_FLASHSPIDESEL    0xFFFFFF4B
#define CMD_FLASHSPITX       0xFFFFFF4C
#define CMD_FLASHSPIRX       0xFFFFFF4D
#define CMD_FLASHSOURCE      0xFFFFFF4E
#define CMD_CLEARCACHE       0xFFFFFF4F
#define CMD_INFLATE2         0xFFFFFF50
#define CMD_ROTATEAROUND     0xFFFFFF51
#define CMD_RESETFONTS       0xFFFFFF52
#define CMD_ANIMSTART        0xFFFFFF53
#define CMD_ANIMSTOP         0xFFFFFF54
#define CMD_ANIMXY           0xFFFFFF55
#define CMD_ANIMDRAW         0xFFFFFF56
#define CMD_GRADIENTA        0xFFFFFF57
#define CMD_FILLWIDTH        0xFFFFFF58
#define CMD_APPENDF          0xFFFFFF59
#define CMD_ANIMFRAME        0xFFFFFF5A
#define CMD_NOP              0xFFFFFF5B
#define CMD_VIDEOSTARTF      0xFFFFFF5F
#define CMD_CALIBRATESUB     0xFFFFFF60
#define CMD_TESTCARD         0xFFFFFF61
#define CMD_HSF              0xFFFFFF62
#define CMD_APILEVEL         0xFFFFFF63
#define CMD_GETIMAGE         0xFFFFFF64
#define CMD_WAIT             0xFFFFFF65
#define CMD_RETURN           0xFFFFFF66
#define CMD_CALLLIST         0xFFFFFF67
#define CMD_NEWLIST          0xFFFFFF68
#define CMD_ENDLIST          0xFFFFFF69
#define CMD_PCLKFREQ         0xFFFFFF6A
#define CMD_FONTCACHE        0xFFFFFF6B
#define CMD_FONTCACHEQUERY   0xFFFFFF6C
#define CMD_ANIMFRAMERAM     0xFFFFFF6D
#define CMD_ANIMSTARTRAM     0xFFFFFF6E
#define CMD_RUNANIM          0xFFFFFF6F
#define CMD_FLASHPROGRAM     0xFFFFFF70

#define VERTEX2F(x, y)                    ((1UL << 30) | (((x)&32767UL) << 15) | (((y)&32767UL) << 0))
#define COLOR_RGB(red, green, blue)       ((4UL << 24) | (((red)&255UL) << 16) | (((green)&255UL) << 8) | (((blue)&255UL) << 0))
#define POINT_SIZE(size)                  ((13UL << 24) | (((size)&8191UL) << 0))
#define CLEAR_COLOR_RGB(red, green, blue) ((2UL << 24) | (((red)&255UL) << 16) | (((green)&255UL) << 8) | (((blue)&255UL) << 0))
#define CLEAR_COLOR_A(alpha)              ((15UL << 24) | (((alpha)&255UL) << 0))
#define BEGIN(prim)                       ((31UL << 24) | (((prim)&15UL) << 0))
#define CLEAR(c, s, t)                    ((38UL << 24) | (((c)&1UL) << 2) | (((s)&1UL) << 1) | (((t)&1UL) << 0))
#define DISPLAY()                         ((0UL << 24))

#define DLSWAP_DONE 0UL
#define DLSWAP_LINE 1UL
#define DLSWAP_FRAME 2UL

#define OPT_CENTER 1536UL
#define OPT_FORMAT 4096UL

#define POINTS 2UL
#define LINES 3UL
#define RECTS 9UL

#define CTOUCH_MODE_COMPATIBILITY 1UL
#define CTOUCH_MODE_EXTENDED 0UL

typedef enum EVE_STATUS_T
{
	EVE_STATUS_CLOSED = 0,
	EVE_STATUS_OPENED,
	EVE_STATUS_READING,
	EVE_STATUS_WRITING,
	EVE_STATUS_ERROR,
} EVE_STATUS_T;

typedef enum EVE_81X_PLL_FREQ_T
{
	EVE_SYSCLK_DEFAULT = 0x00, // default 60mhz on ft810, 48mhz on ft800
	EVE_SYSCLK_84M = 0x07,
	EVE_SYSCLK_72M = 0x06,
	EVE_SYSCLK_60M = 0x05,
	EVE_SYSCLK_48M = 0x04,
	EVE_SYSCLK_36M = 0x03,
	EVE_SYSCLK_24M = 0x02,
} EVE_81X_PLL_FREQ_T;


typedef struct eve_bootup
{
	/* Clock PLL multiplier (ft81x: 5, 60MHz, bt81x: 6, 72MHz) */
	EVE_81X_PLL_FREQ_T SystemClock;

	/* External oscillator (default: false) */
	bool ExternalOsc;
} eve_bootup;


typedef enum EVE_POWER_MODE_T
{
	EVE_ACTIVE_M = 0x00,
	EVE_STANDBY_M = 0x41, // default
	EVE_SLEEP_M = 0x42,
	EVE_POWERDOWN_M = 0x50,
} EVE_POWER_MODE_T;

/* APIs for Host Commands */
typedef enum EVE_PLL_SOURCE_T
{
	EVE_INTERNAL_OSC = 0x48, // default
	EVE_EXTERNAL_OSC = 0x44,
} EVE_PLL_SOURCE_T;

#define EVE_FT800 0x0800
#define EVE_FT801 0x0801
#define EVE_FT810 0x0810
#define EVE_FT811 0x0811
#define EVE_FT812 0x0812
#define EVE_FT813 0x0813
#define EVE_BT815 0x0815
#define EVE_BT816 0x0816
#define EVE_BT817 0x0817
#define EVE_BT818 0x0818

typedef enum EVE_TRANSFER_T
{
	EVE_TRANSFER_NONE = 0,
	EVE_TRANSFER_READ,
	EVE_TRANSFER_WRITE,
} EVE_TRANSFER_T;

typedef struct eve_config
{
	int16_t Width; /* Line buffer width (pixels) */
	int16_t Height; /* Screen and render height (lines) */
	int16_t HCycle;
	int16_t HOffset;
	int16_t HSync0;
	int16_t HSync1;
	int16_t VCycle;
	int16_t VOffset;
	int16_t VSync0;
	int16_t VSync1;
	uint8_t PCLK;
	int8_t Swizzle;
	int8_t PCLKPol;
	int8_t CSpread;
	uint8_t OutBitsR;
	uint8_t OutBitsG;
	uint8_t OutBitsB;
	uint16_t PCLKFreq;
	bool Dither;
	int16_t HsfWidth;
} eve_config;

void eve_cocmd_text(int16_t x, int16_t y, int16_t font, uint16_t options,
    const char *s, ...);
void eve_cocmd_gradient(int16_t x0, int16_t y0, uint32_t rgb0, int16_t x1,
    int16_t y1, uint32_t rgb1);
void eve_cocmd_slider(int16_t x, int16_t y, int16_t w, int16_t h,
    uint16_t options, uint16_t val, uint16_t range);
void eve_cocmd_progress(int16_t x, int16_t y, int16_t w, int16_t h,
    uint16_t options, uint16_t val, uint16_t range);
void eve_cocmd_button(int16_t x, int16_t y, int16_t w, int16_t h, int16_t font,
    uint16_t options, const char *s, ...);
void eve_cocmd_toggle(int16_t x, int16_t y, int16_t w, int16_t font,
    uint16_t options, uint16_t state, const char *s, ...);
void eve_cocmd_dlStart();
void eve_cocmd_swap();
void eve_cocmd_interrupt(uint32_t ms);
void eve_cocmd_coldStart();

void eve_transfer_begin(EVE_TRANSFER_T rw, uint32_t addr);
void eve_transfer_end();
bool eve_cmd_32(uint32_t value);
uint8_t eve_calibrate(uint32_t w, uint32_t h);
bool eve_init(eve_bootup *bootup, eve_config *config);

#endif /* 
/* end of file */
