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

#include "board_api.h"
#include "tusb.h"
#include <sys/stat.h>   // stat

#include "common.h" //EveApps's common

#include "pico/bootrom.h"
#include "pico/unique_id.h"

#define UF2_FAMILY_ID_EVE3 0x1594d309 // BT815, BT816
#define UF2_FAMILY_ID_EVE4 0x9c217c82 // BT817, BT818

#define BTFLASH_CMD_RUID (0x4B) /* Read Unique ID */
#define BTFLASH_RUID_DUMMY_BYTES (4)
#define BTFLASH_RUID_DATA_BYTES (8)

unsigned char g_UsbProduct[64] = "BT8XX UF2";
uint32_t g_Uf2FamilyId = UF2_FAMILY_ID_EVE3;

extern char infoUf2File[];
extern char infoSPIConfFile[];

unsigned char g_DeviceName[64] = "Bridgetek BT8XX";
#define EFLASH   "e-flash.bin"
#define EFLASHV  "e-flash-v.bin"
#define EFLASHRB "e-flash-readback.bin"

EVE_HalContext g_Host;
extern bool EVE_Hal_NoInit;

unsigned char s_FlashUniqueId[BTFLASH_RUID_DATA_BYTES];
static_assert(sizeof(pico_unique_board_id_t) == BTFLASH_RUID_DATA_BYTES, "");

bool g_HalOk = false;
bool g_HalFlashOk = false;
static bool is_eflash_initialized = false;
static char eflash_file[100];

spi_pins current_spi_pins;

uint32_t bootup_starting = 0;
uint32_t count_bootup_attempt = 0;

bool cbCmdWait_uf2(EVE_HalContext *phost);

static bool file_exists (char *filename) {
  struct stat   buffer;   
  return (stat (filename, &buffer) == 0);
}

static uint32_t eflash_write_with_progress(EVE_HalContext* phost, const char* filePath, const char* fileName, uint32_t address) {
	Ftf_Progress_t* progress = Ftf_Progress_Init(phost, filePath, fileName, address, FTF_PROGESS_WRITE);

	if (!progress) {
		return -1; /// error
	}

	while (1) {
		uint32_t pc = Ftf_Progress_Write_Next(phost, progress);

		Ftf_Progress_Ui(phost, progress);
    printf("\rFlashing " EFLASH " %d %%", pc);

		if (pc >= 100) {
			break;
		}
	}
	Ftf_Progress_Close();

	return progress->fileSize;
}

/**
 * @brief Read SD card to program the flash image to e-flash
 * 
 * @param file File to flash
 * @return size_t Number of bytes flashed
 */
static uint32_t eve_eflash_init_file(const char * file){
  EVE_HalContext *phost = &g_Host;

  // detecting e-flash.bin
  if (file_exists(file) == 0 ){
    printf("%s is not exist, skip auto-flashing\r\n", file);
  }
  printf("%s is detected, flashing it to EVE's connected flash\r\n");

  // Flash EFLASH
  uint32_t sent = eflash_write_with_progress(phost, file, file, 0);
  if (0 >= sent) {
    printf("Error: Flash %s failed\r\n", file);
		is_eflash_initialized = false;
		return 0; // error
	}
  sprintf(eflash_file, "%s", file);
  printf("Flashed %s successful\r\n", file);
  is_eflash_initialized = true;

  return sent;
}

static void eve_eflash_init(){
#if !defined(EVE_FLASH_AVAILABLE)
  return;
#endif
  EVE_HalContext *phost = &g_Host;

  // detecting SD card
  EVE_Util_loadSdCard(phost);
	if (sdhost_card_detect() != SDHOST_CARD_INSERTED) {
		return;
	}
  printf("SD card detected\r\n");

  uint32_t eflashv_size = eve_eflash_init_file("/" EFLASHV);
  if(!eflashv_size)
  {
    eve_eflash_init_file("/" EFLASH);
  }
  else
  {
    // Read the eve-connected flash and write the data into SD card 
    // with the name e-flash-readback.bin if the pre-defined file "e-flash-v.bin" at the root directory.
    Ftf_Read_File_From_Flash(phost, "/" EFLASHRB, 0, eflashv_size);
  }
}

extern char infoSPIConfFile[128];

/**
 * @brief Parse SPI pins text to struct
 * 
 * @param data SPI pins text
 * @return true Parse Ok
 * @return false Parse not ok
 */
bool board_spi_onChanged(char *data){
  int spi_arr[8];
  int id = 0;
  char *data_next;
  char *data_root = data;
  
  while (*data) { // While there are more characters to process...
    while(*data && data[0] != ':') { // Move pointer to ':'
        data++;
    }
    while(*data && data[0] != ' ') { // Skip all space
        data++;
    }
    
    long val = strtol(data, &data_next, 10); // Read number
    if (data_next != data)
    {   // Found a number   
        spi_arr[id++] = val;
    }
    else if (!(*data)){
      goto end; // End of text
    } 
    else{
      printf("Error: SPI pins cannot be parsed\r\n");
      return false;
    }
    data = data_next+1;   
  }

end:
  id=0;
  current_spi_pins.MISO  = spi_arr[id++];
  current_spi_pins.CS    = spi_arr[id++];
  current_spi_pins.SCK   = spi_arr[id++];
  current_spi_pins.MOSI  = spi_arr[id++];
  current_spi_pins.INT   = spi_arr[id++];
  current_spi_pins.PWD   = spi_arr[id++];
  current_spi_pins.IO2   = spi_arr[id++];
  current_spi_pins.IO3   = spi_arr[id++];
  return true;
}

/**
 * @brief Initialize the EVE
 * 
 * @return true EVE is initialized successully 
 * @return false EVE is initialized unsuccessully 
 */
bool board_dfu_init(void)
{
  EVE_HalContext *phost = &g_Host;
  EVE_HalParameters params;

  if(!board_spi_onChanged(infoSPIConfFile)) return false;

  // Initialize HAL
  EVE_Hal_NoInit = true;
  EVE_Hal_initialize();
  EVE_Hal_defaults(&params);
  params.SpiSckPin = current_spi_pins.SCK;
	params.SpiMosiPin = current_spi_pins.MOSI;
	params.SpiMisoPin = current_spi_pins.MISO;
	params.PowerDownPin = current_spi_pins.PWD;
	params.SpiCsPin = current_spi_pins.CS;
  params.CbCmdWait = cbCmdWait_uf2;

  bootup_starting = 1;
  count_bootup_attempt = 0;
  if (!EVE_Hal_open(phost, &params))
  {
    eve_printf_debug("Failed to open device!\n");
    return false;
  }

  if (!EVE_Util_bootupConfig(phost))
  {
    bootup_starting = 0;
    eve_printf_debug("Failed to bootup device!\n");
    EVE_Hal_close(phost);
    return false;
  }
  bootup_starting = 0;
  eve_eflash_init();

  g_HalOk = true;

  int chipId = EVE_CHIPID;
  char *chipPrefix = (EVE_GEN >= EVE3) ? "BT" : "FT";
  char *chipBranding = (EVE_GEN >= EVE3) ? "Bridgetek" : "FTDI";

  if (EVE_GEN >= EVE4)
  {
    g_Uf2FamilyId = UF2_FAMILY_ID_EVE4;
  }

  eve_printf_debug("%s %s%lx\n", chipBranding, chipPrefix, chipId);
  eve_printf_debug("RP2040 Flash Tool\n");

  if (EVE_Hal_supportFlash(phost) 
      && (EVE_CoCmd_flashAttach(phost) > FLASH_STATUS_DETACHED))
  {
    /* Read Unique ID from EVE flash */
    EVE_CoCmd_flashDetach(phost);
    EVE_CoCmd_flashSpiDesel(phost);
    EVE_CoCmd_flashSpiTx(phost, 1);
    EVE_Cmd_wr32(phost, BTFLASH_CMD_RUID);
    EVE_CoCmd_flashSpiRx(phost, 0, BTFLASH_RUID_DUMMY_BYTES + BTFLASH_RUID_DATA_BYTES);
    EVE_CoCmd_flashSpiDesel(phost);
    EVE_Cmd_waitFlush(phost);
    EVE_Hal_rdMem(phost, s_FlashUniqueId, BTFLASH_RUID_DUMMY_BYTES, BTFLASH_RUID_DATA_BYTES);
    g_HalFlashOk = true;
  }
  else
  {
    /* Read Unique ID from Pico flash */
    eve_printf_debug("This EVE board does not appear to have any flash!\n");
    pico_get_unique_board_id((pico_unique_board_id_t *)&s_FlashUniqueId[0]);
  }

  uint32_t flashCode;
  EVE_CoCmd_flashAttach(phost);
  uint32_t flashStatus = EVE_CoCmd_flashFast(phost, &flashCode);
  if (flashCode)
  {
      eve_printf_debug("Flash error code: %x\n", flashCode);
      eve_printf_debug("Please upload the EVE firmware blob UF2\n");
  }
  else
  {
      eve_printf_debug("Flash firmware blob OK\n");
  }

  const char *flashStatusStr;
  switch (flashStatus)
  {
  case FLASH_STATUS_DETACHED:
      flashStatusStr = "DETACHED";
      break;
  case FLASH_STATUS_BASIC:
      flashStatusStr = "BASIC";
      break;
  case FLASH_STATUS_FULL:
      flashStatusStr = "FULL";
      break;
  default:
      flashStatusStr = "INIT";
      break;
  }

  sprintf(infoUf2File,
    "TinyUF2 Bootloader " UF2_VERSION "\r\n"
    "Model: %s %s%lx\r\n"
    "Board-ID: %s%lx-RP2040-v1.0\r\n"
    "Date: " __DATE__ "\r\n"
    "X-Eve-Flash-Status: %s\r\n"
    "X-Eve-Flash-Code: %x\r\n"
    "X-Eve-Flash-Serial: %02x%02x%02x%02x%02x%02x%02x%02x\r\n"
    "X-Eve-Flash-image: %s\r\n",
    chipBranding, chipPrefix, chipId,
    chipPrefix, chipId,
    flashStatusStr, flashCode,
    (int)s_FlashUniqueId[0], (int)s_FlashUniqueId[1], (int)s_FlashUniqueId[2], (int)s_FlashUniqueId[3],
    (int)s_FlashUniqueId[4], (int)s_FlashUniqueId[5], (int)s_FlashUniqueId[6], (int)s_FlashUniqueId[7],
    is_eflash_initialized == true? eflash_file:"NONE");

  sprintf(g_UsbProduct, "%s%lx UF2", chipPrefix, chipId);
  sprintf(g_DeviceName, "%s %s%lx", chipBranding, chipPrefix, chipId);

  eve_printf_debug("Flash serial: %02x%02x%02x%02x%02x%02x%02x%02x\n",
    (int)s_FlashUniqueId[0], (int)s_FlashUniqueId[1], (int)s_FlashUniqueId[2], (int)s_FlashUniqueId[3],
    (int)s_FlashUniqueId[4], (int)s_FlashUniqueId[5], (int)s_FlashUniqueId[6], (int)s_FlashUniqueId[7]);

  // EVE_Hal_wr32(phost, REG_MACRO_0, COLOR_RGB(128, 128, 128));
#if MATRIX_RAIN
  // EVE_Hal_wr32(phost, REG_MACRO_0, BITMAP_TRANSFORM_F(256));
  EVE_CoCmd_memZero(phost, 0, 4096 * 2);
#endif

  EVE_CoCmd_dlStart(phost);
  EVE_CoDl_clearColorRgb(phost, 0, 0, 0);
  EVE_CoDl_clear(phost, 1, 1, 1);
#if MATRIX_RAIN
  EVE_CoDl_blendFunc(phost, SRC_ALPHA, ONE);
  EVE_CoDl_colorRgb(phost, 64, 192, 64);
  EVE_CoDl_bitmapHandle(phost, 0);
  EVE_CoCmd_setBitmap(phost, 0, TEXT8X8, 512, 512);
  EVE_CoCmd_dl(phost, BITMAP_SIZE_H(2, 4));
  EVE_CoCmd_dl(phost, BITMAP_SIZE(NEAREST, REPEAT, REPEAT, 0, 0));
  EVE_CoDl_bitmapHandle(phost, 1);
  EVE_CoCmd_setBitmap(phost, 4096, TEXT8X8, 512, 512);
  EVE_CoCmd_dl(phost, BITMAP_SIZE_H(2, 4));
  EVE_CoCmd_dl(phost, BITMAP_SIZE(NEAREST, REPEAT, REPEAT, 0, 0));
  EVE_CoDl_begin(phost, BITMAPS);
  // EVE_CoDl_macro(phost, 0);
  EVE_CoDl_vertex2ii(phost, 0, 0, 0, 0);
  EVE_CoDl_vertex2ii(phost, 0, 0, 1, 0);
  EVE_CoDl_end(phost);
  EVE_CoDl_blendFunc_default(phost);
  // EVE_CoCmd_dl(phost, BITMAP_TRANSFORM_F(0));
#endif
  EVE_CoDl_colorRgb(phost, 255, 255, 255);
#if TINYUF2_DISPLAY
  EVE_CoDl_bitmapHandle(phost, 0);
  EVE_CoCmd_memSet(phost, 0, 0xAA, DISPLAY_WIDTH * DISPLAY_HEIGHT * 2);
  EVE_CoCmd_setBitmap(phost, 0, RGB565, DISPLAY_WIDTH, DISPLAY_HEIGHT);
  EVE_CoDl_bitmapSize(phost, NEAREST, BORDER, BORDER, DISPLAY_HEIGHT, DISPLAY_WIDTH);
  // EVE_CoDl_bitmapSwizzle(phost, BLUE, GREEN, RED, ALPHA);
  EVE_CoDl_bitmapTransformA(phost, 0);
  EVE_CoDl_bitmapTransformB(phost, 256);
  EVE_CoDl_bitmapTransformD(phost, 256);
  EVE_CoDl_bitmapTransformE(phost, 0);
  EVE_CoDl_begin(phost, BITMAPS);
  EVE_CoDl_vertex2ii(phost, 0, 0, 0, 0);
  EVE_CoDl_end(phost);
#else
  EVE_CoCmd_text(phost, 5, 5, 30, 0, g_DeviceName);
  EVE_CoCmd_text(phost, 5, 45, 28, 0, "RP2040 Flash Tool");
  // EVE_CoDl_macro(phost, 0);
  // EVE_CoDl_begin(phost, POINTS);
  // EVE_CoDl_pointSize(phost, 180);
  // EVE_CoDl_vertexFormat(phost, 1);
  // EVE_CoDl_vertex2f(phost, 45, 45);
  // EVE_CoDl_end(phost);
#endif
  EVE_CoDl_display(phost);
  EVE_CoCmd_swap(phost);
  EVE_Hal_flush(phost);
  return true;
}

#if TINYUF2_DISPLAY

void board_display_init()
{
  EVE_HalContext *phost = &g_Host;
  if (!g_HalOk)
    return;

  EVE_CoCmd_memSet(phost, 0, 0x55, DISPLAY_WIDTH * DISPLAY_HEIGHT * 2);
}

void board_display_draw_line(int y, uint16_t *pixel_color, uint32_t pixel_num)
{
  EVE_HalContext *phost = &g_Host;
  if (!g_HalOk)
    return;

  EVE_Hal_wrMem(phost, y * DISPLAY_WIDTH * 2, (uint8_t *)pixel_color, pixel_num * 2);
}

#endif

void board_dfu_complete(void)
{
  EVE_HalContext *phost = &g_Host;

  // Mostly reset
  if (g_HalOk)
  {
    g_HalFlashOk = false;
    g_HalOk = false;
    EVE_Cmd_waitFlush(phost);
    EVE_Hal_close(phost);
  }
  EVE_Hal_release();

  // Return to RP2040 boot loader
  board_app_jump();
}

bool board_app_valid(void)
{
  // Always return false, otherwise tinyuf2 immediately exits
  return false;
}

void board_app_jump(void)
{
  // Return to RP2040 boot loader
  reset_usb_boot(0, 0);
}

uint8_t board_usb_get_serial(uint8_t serial_id[16])
{
  memcpy(serial_id, s_FlashUniqueId, BTFLASH_RUID_DATA_BYTES);
  return BTFLASH_RUID_DATA_BYTES;
}

//--------------------------------------------------------------------+
// LED pattern
//--------------------------------------------------------------------+

void board_rgb_write(uint8_t const rgb[])
{
  /*
  EVE_HalContext *phost = &g_Host;
  if (!g_HalOk)
    return;

  // This does not work, since this write comes from an interrupt
  EVE_Hal_wr32(phost, REG_MACRO_0, COLOR_RGB(rgb[0], rgb[1], rgb[2]));
  */
}

//--------------------------------------------------------------------+
// Timer
//--------------------------------------------------------------------+

static struct repeating_timer s_BoardTimer;

static bool board_repeating_timer_callback(struct repeating_timer *t)
{
  board_timer_handler();
  return true;
}

void board_timer_start(uint32_t ms)
{
  s_BoardTimer.alarm_id = 99;
  add_repeating_timer_ms(-((int32_t)ms), board_repeating_timer_callback, &s_BoardTimer, &s_BoardTimer);
}

void board_timer_stop(void)
{
  cancel_repeating_timer(&s_BoardTimer);
}

/* end of file */
