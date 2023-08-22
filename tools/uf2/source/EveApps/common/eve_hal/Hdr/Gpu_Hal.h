/**
 * @file Gpu_Hal.h
 * @brief This file defines the generic APIs of phost access layer for the FT800 or EVE compatible silicon.
 *        Application shall access FT800 or EVE resources over these APIs,regardless of I2C or SPI protocol.
 *        In addition, there are some helper functions defined for FT800 coprocessor engine as well as phost commands.
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

#ifndef GPU_HAL__H
#define GPU_HAL__H

#include "FT_Gpu_Hal.h"
#include "EVE_Util.h"

#include "Gpu_CoCmd.h"

#include <stdlib.h>

#ifdef BT8XXEMU_PLATFORM
void setupComp();
void loop();
#define setup                               \
	EVE_emuMain(int32_t argc, char *argv[]) \
	{                                       \
		setupComp();                        \
		loop();                             \
	}                                       \
	void setupComp
#endif

#ifdef FT4222_PLATFORM
#define MSVC_PLATFORM
#endif

#ifdef MPSSE_PLATFORM
#define MSVC_PLATFORM
#endif

#if defined(BT81XA_ENABLE)
#define BT81X_ENABLE
#endif

#if defined(BT81X_ENABLE)
#define FT81X_ENABLE
#endif

#define PROGMEM eve_progmem
#define PROGMEM_CONST eve_progmem_const

#define FIFO_SIZE_MASK EVE_FIFO_SIZE_MASK
#define FIFO_BYTE_ALIGNMENT_MASK EVE_FIFO_BYTE_ALIGNMENT_MASK
#define CMD_SIZE 4

#define char8_t int8_t
#define bool_t bool
#define uchar8_t uint8_t
typedef float float_t;
typedef double double_t;
//#define float_t float
//#define double_t double

#ifndef FALSE
#define FALSE false
#endif
#ifndef TRUE
#define TRUE true
#endif

#ifndef FT800_PD_N
#define FT800_PD_N 0
#endif
#ifndef FT800_SEL_PIN
#define FT800_SEL_PIN 0
#endif

typedef eve_progmem uint16_t prog_uint16_t;
typedef eve_progmem char prog_uchar8_t;
typedef eve_progmem char prog_char8_t;

#define DispWidth phost->Width
#define DispHeight phost->Height

#define GPU_HAL_MODE_E EVE_MODE_T
#define GPU_I2C_MODE EVE_MODE_I2C
#define GPU_SPI_MODE EVE_MODE_SPI

#define GPU_HAL_STATUS_E EVE_STATUS_T
#define GPU_HAL_CLOSED EVE_STATUS_CLOSED
#define GPU_HAL_OPENED EVE_STATUS_OPENED
#define GPU_HAL_READING EVE_STATUS_READING
#define GPU_HAL_WRITING EVE_STATUS_WRITING
#define GPU_HAL_STATUS_ERROR EVE_STATUS_ERROR

#define GPU_TRANSFERDIR_T EVE_TRANSFER_T
#define GPU_READ EVE_TRANSFER_READ
#define GPU_WRITE EVE_TRANSFER_WRITE

#define Gpu_Hal_Callback_t EVE_Callback
#define Gpu_Hal_Config_t EVE_HalParameters

#define EVE_FIFO_SIZE_MASK EVE_CMD_FIFO_MASK
#define EVE_FIFO_BYTE_ALIGNMENT_MASK EVE_CMD_FIFO_ALIGNMENT_MASK

typedef struct
{
	ft_uint32_t TotalChannelNum; //< Total number channels for libmpsse
} Gpu_HalInit_t;

#define Gpu_Hal_Context_t EVE_HalContext
#define hal_config Parameters
#define spi_clockrate_khz SpiClockrateKHz
#define pdn_pin_no PowerDownPin
#define spi_cs_pin_no SpiCsPin
#define cmd_fifo_wp CmdWp

#ifdef MPSSE_PLATFORM
#define channel_no MpsseChannelNo
#else
#define channel_no SpiCsPin
#endif

#define MSVC_PLATFORM_SPI_LIBFT4222 1
#define MSVC_PLATFORM_SPI_LIBMPSSE 2

#define BootupConfig EVE_Util_bootupConfig

typedef struct Fifo_t
{
	uint32_t fifo_buff; //fifo buffer address
	int32_t fifo_len; //fifo length
	int32_t fifo_wp; //fifo write pointer - maintained by host
	int32_t fifo_rp; //fifo read point - maintained by device
	/* FT800 series specific registers */
	uint32_t HW_Read_Reg; //hardware fifo read register
	uint32_t HW_Write_Reg; //hardware fifo write register
} Fifo_t;

/* Type of file to load from SDCard or Windows file system */
#define LOADIMAGE 1 //loadimage command takes destination address and options before the actual bitmap data
#define INFLATE 2 //inflate command takes destination address before the actual bitmap
#define LOAD 3 //load bitmaps directly
#define INFLATE2 4

/*******************************************************************************/
/*******************************************************************************/
/* The basic APIs Level 1 */

static inline eve_deprecated("Use `EVE_Hal_initialize`") bool Gpu_Hal_Init(Gpu_HalInit_t *halinit)
{
	EVE_HalPlatform *platform = EVE_Hal_initialize();
	halinit->TotalChannelNum = (uint32_t)EVE_Hal_list();
	return !!platform;
}

static inline eve_deprecated("Use `EVE_Hal_open`") bool Gpu_Hal_Open(EVE_HalContext *phost)
{
	EVE_HalParameters parameters;
	EVE_Hal_defaults(&parameters);
	return EVE_Hal_open(phost, &parameters);
}

#define Gpu_Hal_Close EVE_Hal_close
#define Gpu_Hal_DeInit EVE_Hal_release

#define Gpu_Hal_ESD_Idle EVE_Hal_idle

#define Gpu_Hal_StartTransfer EVE_Hal_startTransfer
#define Gpu_Hal_Transfer8 EVE_Hal_transfer8
#define Gpu_Hal_Transfer16 EVE_Hal_transfer16
#define Gpu_Hal_Transfer32 EVE_Hal_transfer32
#define Gpu_Hal_EndTransfer EVE_Hal_endTransfer

#define Gpu_Hal_Rd8 EVE_Hal_rd8
#define Gpu_Hal_Rd16 EVE_Hal_rd16
#define Gpu_Hal_Rd32 EVE_Hal_rd32
#define Gpu_Hal_Wr8 EVE_Hal_wr8
#define Gpu_Hal_Wr16 EVE_Hal_wr16
#define Gpu_Hal_Wr32 EVE_Hal_wr32

#define Gpu_Hal_WrMem EVE_Hal_wrMem
#define Gpu_Hal_WrMem_ProgMem EVE_Hal_wrProgMem
#define Gpu_Hal_WrMemFromFlash EVE_Hal_wrProgMem

static inline eve_deprecated("Use `EVE_Hal_rdMem` (note: buffer and addr are swapped)") ft_void_t Gpu_Hal_RdMem(EVE_HalContext *phost, ft_uint32_t addr, ft_uint8_t *buffer, ft_uint32_t length)
{
	EVE_Hal_rdMem(phost, buffer, addr, length);
}

/*******************************************************************************/
/*******************************************************************************/
/* APIs for coprocessor Fifo read/write and space management */
#define Gpu_Hal_WrCmd32 EVE_Cmd_wr32

/// Write a buffer to the command buffer. Waits if there is not enough space in the command buffer. Returns FALSE in case a coprocessor fault occurred
#define Gpu_Hal_WrCmdBuf EVE_Cmd_wrMem
#define Gpu_Hal_WrCmdBuf_nowait EVE_Cmd_wrMem

#define Gpu_Hal_WrCmdBuf_ProgMem EVE_Cmd_wrProgMem
#define Gpu_Hal_WrCmdBufFromFlash EVE_Cmd_wrProgMem

/// Wait for the command buffer to fully empty. Returns FALSE in case a coprocessor fault occurred
#define Gpu_Hal_WaitCmdFifoEmpty EVE_Cmd_waitFlush
#define Gpu_Hal_WaitCmdfifo_empty EVE_Cmd_waitFlush

/// Wait for the command buffer to have at least the requested amount of free space
#define Gpu_Hal_WaitCmdFreespace EVE_Cmd_waitSpace

/*
// Use the provided wait functions!
static inline ft_void_t Gpu_Hal_RdCmdRpWp(EVE_HalContext *phost, ft_uint16_t *rp, ft_uint16_t *wp)
{
	*rp = EVE_Cmd_rp(phost);
	*wp = EVE_Cmd_wp(phost);
}
*/

/*******************************************************************************/
/*******************************************************************************/

#ifdef _MSC_VER
#pragma deprecated(Gpu_CoCmd_SendCmd) /* Use EVE_Cmd_wr32 */
#pragma deprecated(Gpu_Copro_SendCmd) /* Use EVE_Cmd_wr32 */
#pragma deprecated(Gpu_CoCmd_StartFrame) /* Remove */
#pragma deprecated(Gpu_CoCmd_EndFrame) /* Remove */
#pragma deprecated(Gpu_CoCmd_StartFunc) /* Use EVE_Cmd_startFunc */
#pragma deprecated(Gpu_CoCmd_EndFunc) /* Use EVE_Cmd_endFunc */
#endif

#define Gpu_CoCmd_SendCmd EVE_Cmd_wr32
inline static eve_deprecated("Use `EVE_Cmd_startFunc`, `EVE_Cmd_wr32`, and `EVE_Cmd_endFunc`") ft_void_t Gpu_CoCmd_SendCmdArr(EVE_HalContext *phost, ft_uint32_t *cmd, ft_size_t nb)
{
	/* This is not valid behaviour on big endian CPU */
	EVE_Cmd_wrMem(phost, (uint8_t *)cmd, (uint32_t)nb * 4);
}
#define Gpu_CoCmd_SendStr(phost, str) EVE_Cmd_wrString(phost, str, EVE_CMD_STRING_MAX)
#define Gpu_CoCmd_SendStr_S EVE_Cmd_wrString
#define Gpu_CoCmd_StartFrame(phost) eve_noop()
#define Gpu_CoCmd_EndFrame(phost) eve_noop()

#define Gpu_Copro_SendCmd Gpu_CoCmd_SendCmd

#define Gpu_CoCmd_StartFunc(phost, size) EVE_Cmd_startFunc(phost)
#define Gpu_CoCmd_EndFunc(phost, size) EVE_Cmd_endFunc(phost)

#define Gpu_Fonts EVE_Gpu_Fonts
#define Gpu_Fonts_t EVE_Gpu_Fonts
#define Gpu_FontsExt EVE_Gpu_FontsExt
#define Gpu_FontsExt_t EVE_Gpu_FontsExt
#define GPU_NUMCHAR_PERFONT EVE_GPU_NUMCHAR_PERFONT
#define GPU_FONT_TABLE_SIZE EVE_GPU_FONT_TABLE_SIZE

/*******************************************************************************/
/*******************************************************************************/
/* APIs for Host Commands */
#define GPU_INTERNAL_OSC EVE_INTERNAL_OSC
#define GPU_EXTERNAL_OSC EVE_EXTERNAL_OSC
#define GPU_PLL_SOURCE_T EVE_PLL_SOURCE_T

#define GPU_PLL_48M EVE_PLL_48M
#define GPU_PLL_36M EVE_PLL_36M
#define GPU_PLL_24M EVE_PLL_24M
#define GPU_PLL_FREQ_T EVE_PLL_FREQ_T

#define GPU_ACTIVE_M EVE_ACTIVE_M
#define GPU_STANDBY_M EVE_STANDBY_M
#define GPU_SLEEP_M EVE_SLEEP_M
#define GPU_POWERDOWN_M EVE_POWERDOWN_M
#define GPU_POWER_MODE_T EVE_POWER_MODE_T

#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
#define GPU_SYSCLK_DEFAULT EVE_SYSCLK_DEFAULT
#define GPU_SYSCLK_72M EVE_SYSCLK_72M
#define GPU_SYSCLK_60M EVE_SYSCLK_60M
#define GPU_SYSCLK_48M EVE_SYSCLK_48M
#define GPU_SYSCLK_36M EVE_SYSCLK_36M
#define GPU_SYSCLK_24M EVE_SYSCLK_24M
#define GPU_81X_PLL_FREQ_T EVE_81X_PLL_FREQ_T

#define GPU_5MA EVE_5MA
#define GPU_10MA EVE_10MA
#define GPU_15MA EVE_15MA
#define GPU_20MA EVE_20MA
#define GPU_81X_GPIO_DRIVE_STRENGTH_T EVE_81X_GPIO_DRIVE_STRENGTH_T

#define GPU_GPIO0 EVE_GPIO0
#define GPU_GPIO1 EVE_GPIO1
#define GPU_GPIO2 EVE_GPIO2
#define GPU_GPIO3 EVE_GPIO3
#define GPU_GPIO4 EVE_GPIO4
#define GPU_DISP EVE_DISP
#define GPU_DE EVE_DE
#define GPU_VSYNC_HSYNC EVE_VSYNC_HSYNC
#define GPU_PCLK EVE_PCLK
#define GPU_BACKLIGHT EVE_BACKLIGHT
#define GPU_R_G_B EVE_R_G_B
#define GPU_AUDIO_L EVE_AUDIO_L
#define GPU_INT_N EVE_INT_N
#define GPU_TOUCHWAKE EVE_TOUCHWAKE
#define GPU_SCL EVE_SCL
#define GPU_SDAEVE_SDA
#define GPU_SPI_MISO_MOSI_IO2_IO3 EVE_SPI_MISO_MOSI_IO2_IO3
#define GPU_81X_GPIO_GROUP_T EVE_81X_GPIO_GROUP_T

#define GPU_81X_RESET_ACTIVE EVE_81X_RESET_ACTIVE
#define GPU_81X_RESET_REMOVAL EVE_81X_RESET_REMOVAL
#endif

#define GPU_CORE_RESET EVE_CORE_RESET

#define COCMD_FAULT(rp) EVE_CMD_FAULT(rp)

#define GPU_SPI_NUMCHANNELS_T EVE_SPI_CHANNELS_T
#define GPU_SPI_SINGLE_CHANNEL EVE_SPI_SINGLE_CHANNEL
#define GPU_SPI_DUAL_CHANNEL EVE_SPI_DUAL_CHANNEL
#define GPU_SPI_QUAD_CHANNEL EVE_SPI_QUAD_CHANNEL

#define GPU_SPI_NUMDUMMYBYTES uint8_t
#define GPU_SPI_ONEDUMMY 1
#define GPU_SPI_TWODUMMY 2

#define SPI_SINGLE_CHANNEL EVE_SPI_SINGLE_CHANNEL
#define SPI_DUAL_CHANNEL EVE_SPI_DUAL_CHANNEL
#define SPI_QUAD_CHANNEL EVE_SPI_QUAD_CHANNEL

#define SPI_ONE_DUMMY_BYTE EVE_SPI_ONE_DUMMY_BYTE
#define SPI_TWO_DUMMY_BYTE EVE_SPI_TWO_DUMMY_BYTES

#define ft_delay EVE_sleep

#define Gpu_Hal_WaitLogo_Finish EVE_Cmd_waitLogo

inline static ft_int16_t Gpu_Hal_TransferString(EVE_HalContext *phost, const ft_char8_t *str)
{
	return EVE_Hal_transferString(phost, str, 0, EVE_CMD_STRING_MAX, 0) - 1;
}

inline static ft_int16_t Gpu_Hal_TransferString_S(EVE_HalContext *phost, const ft_char8_t *str, int length)
{
	return EVE_Hal_transferString(phost, str, 0, length, 0) - 1;
}
#define Gpu_Hal_Sleep EVE_sleep

#define Gpu_HostCommand EVE_Hal_hostCommand
#define Gpu_HostCommand_Ext3 EVE_Hal_hostCommandExt3
#define Gpu_Hal_Powercycle EVE_Hal_powerCycle
#define Gpu_Hal_SetSPI EVE_Hal_setSPI
#define Gpu_CurrentFrequency EVE_Hal_currentFrequency

#define Gpu_ClockTrimming EVE_Hal_clockTrimming

#define Gpu_ClockSelect EVE_Host_clockSelect
#define Gpu_PLL_FreqSelect EVE_Host_pllFreqSelect
#define Gpu_PowerModeSwitch EVE_Host_powerModeSwitch
#define Gpu_CoreReset EVE_Host_coreReset

#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
#define Gpu_81X_SelectSysCLK EVE_Host_selectSysClk
#define GPU_81X_PowerOffComponents EVE_Host_powerOffComponents
#define GPU_81X_PadDriveStrength EVE_Host_padDriveStrength
#define Gpu_81X_ResetActive EVE_Host_resetActive
#define Gpu_81X_ResetRemoval EVE_Host_resetRemoval
#endif

#define Hal_LoadSDCard() EVE_Util_loadSdCard(NULL)

#define Gpu_ClearScreen EVE_Util_clearScreen

typedef enum
{
	FLASH_CMD_SUCCESS = 0,
	FLASH_CMD_ALIGNED_ERR
} Flash_Cmd_Status_t;

#define FLASH_WRITE_ALIGN_BYTE (256)
#define FLASH_UPDATE_ALIGN_BYTE (4096)
#define FLASH_READ_ALIGN_BYTE (64)

/* Flash section */
void Gpu_CoCmd_FlashHelper_Init(Gpu_Hal_Context_t *phost);
uint32_t Gpu_CoCmd_FlashHelper_SwitchState(Gpu_Hal_Context_t *phost, uint8_t nextState);
uint32_t Gpu_CoCmd_FlashHelper_SwitchFullMode(Gpu_Hal_Context_t *phost);
Flash_Cmd_Status_t Gpu_CoCmd_FlashHelper_Write(Gpu_Hal_Context_t *phost, uint32_t dest_flash, uint32_t num, const uint8_t *write_data);
Flash_Cmd_Status_t Gpu_CoCmd_FlashHelper_Update(Gpu_Hal_Context_t *phost, uint32_t dest_flash, uint32_t src_ram, uint32_t num);
Flash_Cmd_Status_t Gpu_CoCmd_FlashHelper_Read(Gpu_Hal_Context_t *phost, uint32_t dest_ram, uint32_t src_flash, uint32_t num, const uint8_t *read_data);
void Gpu_CoCmd_FlashHelper_Erase(Gpu_Hal_Context_t *phost);
void Gpu_CoCmd_FlashHelper_ClearCache(Gpu_Hal_Context_t *phost);
uint8_t Gpu_CoCmd_FlashHelper_GetState(Gpu_Hal_Context_t *phost);

/* Definitions to ensure these don't get redefined */
#define App_WrCoCmd_Buffer(phost, cmd) EVE_Cmd_wr32((phost), (cmd))
#define App_WrCoStr_Buffer(phost, s) EVE_Cmd_wrString((phost), (s), EVE_CMD_STRING_MAX)
#define App_Flush_Co_Buffer(phost) EVE_Cmd_waitFlush((phost))
#define App_Flush_Co_Buffer_nowait(phost) EVE_Cmd_waitFlush((phost))
#define App_Set_CmdBuffer_Index(idx) eve_noop()

#define POLAR_UTIL /* Enable sin()/cos() calculator utilities */

#ifdef RGB //Undefine RGB from wingdi.h in Visual Studio
#undef RGB
#endif
#define RGB(r, g, b) ((((vc_int32_t)(r)) << 16) | (((vc_int32_t)(g)) << 8) | (b))
#define SQ(v) ((v) * (v))
#ifndef MIN
#define MIN(x, y) ((x) > (y) ? (y) : (x))
#endif
#ifndef MAX
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#endif
#define PLAYCOLOR 0x00A0A080
#define NOTE(n, sharp) (((n) - 'C') + ((sharp)*128))
#define F16(s) ((int32_t)((s)*65536))
#define INVALID_TOUCH_XY 0x8000
#define ABS(x) ((x) > (0) ? (x) : (-x))
#define ALIGN_TWO_POWER_N(Value, alignval) (((Value) + (alignval - 1)) & (~(alignval - 1)))

#if !defined(ARDUINO_PLATFORM) //Arduino support these keyword
#define pgm_read_byte_near(x) (*(x))
#define pgm_read_byte(x) (*(x))
#define pgm_read_word(addr) (*((prog_uint16_t *)(addr)))
#define delay(x) EVE_sleep(x)
#define random(x) (rand() % (x))
#define DBGPRINT(x) printf(x)
#endif

#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
/* strcpy_P and strlen_P only support Arduino */
#define strcpy_P strcpy
#define strlen_P strlen
#endif

void GPU_DLSwap(Gpu_Hal_Context_t *phost, uint8_t DL_Swap_Type);

#if defined(FT9XX_PLATFORM)
#define GET_CURR_MILLIS() EVE_millis()
#elif defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
#define GET_CURR_MILLIS() time(NULL)
#elif defined(ARDUINO_PLATFORM)
#define GET_CURR_MILLIS() millis()
#else
#define GET_CURR_MILLIS() 0
#endif

uint32_t GET_ASTC_FORMAT(uint16_t w, uint16_t h);
void astc_tile2(uint8_t *iData, uint16_t bw, uint16_t bh, uint32_t size, const uint8_t *oData);
#ifdef FT81X_ENABLE
void Set_GpuClock(Gpu_Hal_Context_t *phost);
uint32_t Get_GpuClock(Gpu_Hal_Context_t *phost);
#endif

static int32_t Gpu_Hal_Dec2Ascii(char8_t *pSrc, int32_t value)
{
	int16_t Length;
	char8_t *pdst;
	char8_t charval;
	int32_t CurrVal = value;
	int32_t tmpval;
	int32_t i;
	char8_t tmparray[16];
	char8_t idx = 0;

	Length = (int16_t)strlen((char *)pSrc);
	pdst = pSrc + Length;

	if (0 == value)
	{
		*pdst++ = '0';
		*pdst++ = '\0';
		return 0;
	}

	if (CurrVal < 0)
	{
		*pdst++ = '-';
		CurrVal = -CurrVal;
	}
	/* insert the value */
	while (CurrVal > 0)
	{
		tmpval = CurrVal;
		CurrVal /= 10;
		tmpval = tmpval - CurrVal * 10;
		charval = '0' + tmpval;
		tmparray[idx++] = charval;
	}

	for (i = 0; i < idx; i++)
	{
		*pdst++ = tmparray[idx - i - 1];
	}
	*pdst++ = '\0';

	return 0;
}

static void Gpu_Hal_LoadImageToMemory(Gpu_Hal_Context_t *phost, const char *fileName, int32_t destination, uint8_t type)
{
	if (type == LOADIMAGE)
	{
		EVE_Util_loadImageFile(phost, destination, fileName, NULL);
	}
	else if (type == INFLATE)
	{
		EVE_Util_loadInflateFile(phost, destination, fileName);
	}
	else if (type == LOAD)
	{
		EVE_Util_loadRawFile(phost, destination, fileName);
	}
}

static void Gpu_Hal_ResetCmdFifo(Gpu_Hal_Context_t *phost)
{
	EVE_Cmd_restore(phost);
}

#endif /* GPU_HAL__H */
