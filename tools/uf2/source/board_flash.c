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
#include "tusb.h" // for logging
#include "uf2.h"

#include "EVE_Hal.h"

#define FAILURE_FORCE_RESET 1

extern EVE_HalContext g_Host;
extern bool g_HalFlashOk;

static absolute_time_t s_LastBusyStart;
static bool s_BusyWait;
static bool s_BlockingWait;

bool cbCmdWait(EVE_HalContext* phost)
{
  if (s_BlockingWait)
  {
    int64_t diff = absolute_time_diff_us(s_LastBusyStart, get_absolute_time());
    if (diff > 3 * 1000 * 1000)
    {
      TU_LOG1("Coprocessor timeout\n");
      s_LastBusyStart = get_absolute_time();
      return false;
    }
  }
  return true;
}

//--------------------------------------------------------------------+
//
//--------------------------------------------------------------------+
void board_flash_init(void)
{
  // no-op
}

uint32_t board_flash_size(void)
{
  EVE_HalContext *phost = &g_Host;
  uint32_t sz;
  if (!g_HalFlashOk)
    return 0;

  sz = EVE_Hal_rd32(phost, REG_FLASH_SIZE) * 1024 * 1024;
  TU_LOG1("Flash size: %i\n", sz);
  if (sz > CFG_UF2_FLASH_SIZE)
  {
    TU_LOG1("Flash too large!\n");
  }
  return min(sz, CFG_UF2_FLASH_SIZE);
}

// Upload in 16 chunks of 256 bytes for each 4096 bytes page

#define FLASH_PAGE_SIZE (4096)

#define OFFSET_A (0) /* RAM_G offset to write data (double buffered) */
#define OFFSET_B (4096) /* RAM_G offset to write data (double buffered) */
#define OFFSET_FW (4096 * 2) /* RAM_G offset to write first page */
#define OFFSET_RD (4096 * 3) /* RAM_G offset for reading */

static uint32_t s_RamOffset[4] = { OFFSET_A, OFFSET_B, OFFSET_FW, OFFSET_RD };
static uint16_t s_BlockFlags[3] = { 0, 0 ,0 };
static int s_Cur = 0;
static uint32_t s_CurPage = 0;
static uint32_t s_LastPage = 0;

void board_flash_read(uint32_t addr, void* buffer, uint32_t len)
{
  EVE_HalContext *phost = &g_Host;
  uint32_t res;
  s_BusyWait = false;
  if (!g_HalFlashOk)
  {
    memset(buffer, 0xFF, len);
    return;
  }
  
  s_BlockingWait = true;
  s_LastBusyStart = get_absolute_time();
  if (!EVE_CoCmd_flashRead_flush(phost, OFFSET_RD, addr, len))
  {
    s_LastBusyStart = get_absolute_time();
    EVE_Util_resetCoprocessor(phost);
    
    EVE_CoCmd_flashFast(phost, &res);
    if (res)
    {
      TU_LOG1("Flash error code: %x\n", res);
    }
    if (!EVE_CoCmd_flashRead_flush(phost, OFFSET_RD, addr, len))
    {
      TU_LOG1("Flash read failed at addr 0x%x!\n", addr);
      memset(buffer, 0xFF, len);

#if FAILURE_FORCE_RESET
      // Force reset on failure
      board_dfu_complete();
#endif
      s_BlockingWait = false;
      return;
    }
  }

  s_BlockingWait = false;
  EVE_Hal_rdMem(phost, buffer, OFFSET_RD, len);
}

static void board_flash_flush_pending()
{
  EVE_HalContext *phost = &g_Host;
  int cur = s_Cur;
  uint32_t curPage = s_CurPage;
  if (s_LastPage)
  {
    if (s_BlockFlags[2] == 0xFFFF || s_BlockFlags[cur])
    {
      // Flush previous page
      // TU_LOG1("EVE_Cmd_waitFlush\n");
      s_BlockingWait = true;
      s_LastBusyStart = get_absolute_time();
      if (!EVE_Cmd_waitFlush(phost))
      {
        // Retry in case of error
        s_LastBusyStart = get_absolute_time();
        EVE_Util_resetCoprocessor(phost);
        EVE_CoCmd_flashUpdate(phost, s_LastPage << 12, s_RamOffset[cur ? 0 : 1], FLASH_PAGE_SIZE);
        if (!EVE_Cmd_waitFlush(phost))
        {
          TU_LOG1("Flash write failed at addr 0x%x!\n", s_LastPage << 12);
          g_HalFlashOk = false;

#if FAILURE_FORCE_RESET
          // Force reset on failure
          board_dfu_complete();
#endif
        }
      }
      // Last page flushed
      s_LastPage = 0;
      s_BlockingWait = false;
    }
  }
  if (s_BlockFlags[2] == 0xFFFF)
  {
    uint32_t res;

    // Write complete firmware block at once
    s_BlockingWait = true;
    s_LastBusyStart = get_absolute_time();
    EVE_CoCmd_flashUpdate(phost, 0, OFFSET_FW, FLASH_PAGE_SIZE);
    if (!EVE_Cmd_waitFlush(phost))
    {
      // Retry in case of error
      s_LastBusyStart = get_absolute_time();
      EVE_Util_resetCoprocessor(phost);
      EVE_CoCmd_flashUpdate(phost, 0, OFFSET_FW, FLASH_PAGE_SIZE);
      if (!EVE_Cmd_waitFlush(phost))
      {
        TU_LOG1("Flash write failed at addr 0!\n");
        g_HalFlashOk = false;

#if FAILURE_FORCE_RESET
        // Force reset on failure
        board_dfu_complete();
#endif
      }
    }

    // Reset
    TU_LOG1("Flash firmware blob updated\n", res);
    EVE_CoCmd_flashDetach(phost);
    EVE_Util_resetCoprocessor(phost);
    TU_LOG1("Enter fast flash mode\n");
    EVE_CoCmd_flashFast(phost, &res);
    if (res)
    {
      TU_LOG1("Flash error code: %x\n", res);
    }

    // Firmware written
    TU_LOG1("Ready\n");
    s_BlockFlags[2] = 0;
    s_BlockingWait = false;
  }
  if (s_BlockFlags[cur])
  {
    if (s_BlockFlags[cur] != 0xFFFF)
    {
      s_BlockingWait = true;
      s_LastBusyStart = get_absolute_time();
      // Read missing blocks first
      for (int i = 0; i < 16; ++i)
      {
        if ((s_BlockFlags[cur] & (1 << i)) == 0)
        {
          int32_t o = i * 256;
          int32_t src = (curPage << 12) + o;
          int32_t dst = s_RamOffset[cur] + o;
          // TU_LOG1("EVE_CoCmd_flashRead\n");
          EVE_CoCmd_flashRead(phost, dst, src, FLASH_PAGE_SIZE);
          TU_LOG1("Data missing at 0x%x\n", (int)src);
        }
      }
      s_BlockFlags[2] = 0;
      s_BlockingWait = false;
    }

    // Write complete block at once
    // TU_LOG1("EVE_CoCmd_flashUpdate %x %x %i\n", curPage << 12, s_RamOffset[cur], FLASH_PAGE_SIZE);
    EVE_CoCmd_flashUpdate(phost, curPage << 12, s_RamOffset[cur], FLASH_PAGE_SIZE);
    s_LastPage = curPage;
    
    // Page writing, next page
    s_Cur = cur ? 0 : 1;
    s_CurPage = 0;
    s_BlockFlags[s_Cur] = 0;
  }
}

void board_flash_flush()
{
  EVE_HalContext *phost = &g_Host;
  s_BusyWait = false;
  if (!g_HalFlashOk)
    return;

  // Push any pending buffers
  board_flash_flush_pending();

  // Check firmware block
  if (s_BlockFlags[2])
  {
    TU_LOG1("Firmware incomplete, not written!\n");
  }

  // Flush coprocessor
  TU_LOG1("Flash flush\n");
  EVE_Cmd_waitFlush(phost);
}

bool board_flash_busy()
{
  EVE_HalContext *phost = &g_Host;
  if (!g_HalFlashOk)
      return false;

  if (s_BlockFlags[s_Cur] != 0xFFFF)
    return false;

  if (s_BusyWait)
  {
    int64_t diff = absolute_time_diff_us(s_LastBusyStart, get_absolute_time());
    if (diff > 3 * 1000 * 1000)
    {
      TU_LOG1("Timeout while busy\n");
      s_BusyWait = false;
      return false;
    }
  }
  else
  {
    s_BusyWait = true;
    s_LastBusyStart = get_absolute_time();
  }

  uint16_t rp, wp;
  rp = EVE_Cmd_rp(phost);
  wp = EVE_Cmd_wp(phost);
  return !(rp & 1) && (rp != wp);
}

void board_flash_write(uint32_t addr, void const *data, uint32_t len)
{
  EVE_HalContext *phost = &g_Host;
  s_BusyWait = false;
  if (!g_HalFlashOk)
      return;
  if (len != 256)
      return;

  int cur;
  if (addr < FLASH_PAGE_SIZE)
  {
    TU_LOG1("Write firmware addr 0x%x\n", addr);
    cur = 2; /* Special buffer for first page */
  }
  else
  {
    uint32_t page = addr >> 12;
    if (page != s_CurPage)
    {
      board_flash_flush_pending();
    }
    s_CurPage = page;
    cur = s_Cur;
  }
  int block = (addr >> 8) & 0xF; // 0 to 16
  uint32_t dst = s_RamOffset[cur] + (block << 8);
  // TU_LOG1("EVE_Hal_wrMem %x %i\n", dst, len);
  EVE_Hal_wrMem(phost, dst, data, len);
#if MATRIX_RAIN
  // EVE_Hal_wr32(phost, REG_MACRO_0, BITMAP_TRANSFORM_F(-addr));
#endif
  s_BlockFlags[cur] |= (1 << block);
}

/* end of file */
