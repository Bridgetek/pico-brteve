/**
 * @file Gpu_CoCmd.h
 * @brief Co-processor command definitions
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

/*

Gpu_CoCmd_FlashWriteExt
Ft_Gpu_CoCmd_FlashWriteExt

Esd_Calibrate

Ft_QueueCmd                         
Ft_QueueString                      
Ft_FlushCmd                         

Ft_Gpu_HorizontalScanoutFilter      
Ft_Gpu_CoCmd_FlashWriteExt          
Ft_Gpu_CoCmd_FlashHelper_Erase      
Ft_Gpu_CoCmd_FlashHelper_ClearCache 

fadeout                 
fadein                  

Fifo_Init               
Fifo_Update             
Fifo_Write          
Fifo_WriteWait          
Fifo_Write32            
Fifo_GetFreeSpace   

App_WrDl_Buffer         
App_Flush_DL_Buffer     
App_Set_DlBuffer_Index  
GPU_DLSwap              

*/

#ifndef EVE_COMPAT_COCMD__H
#define EVE_COMPAT_COCMD__H
#define Cmd_Getimage EVE_CoCmd_getImage
#define Gpu_CoCmd_Getimage EVE_CoCmd_getImage

#define Gpu_CoCmd_ClearCache EVE_CoCmd_clearCache
#define Gpu_CoCmd_FlashAttach EVE_CoCmd_flashAttach
#define Gpu_CoCmd_FlashDetach EVE_CoCmd_flashDetach
#define Gpu_CoCmd_FlashErase EVE_CoCmd_flashErase
#define Gpu_CoCmd_FlashFast EVE_CoCmd_flashFast
#define Gpu_CoCmd_FlashRead EVE_CoCmd_flashRead
#define Gpu_CoCmd_FlashUpdate EVE_CoCmd_flashUpdate
#define Gpu_CoCmd_Fontcache EVE_CoCmd_fontCache
#define Gpu_CoCmd_Fontcachequery EVE_CoCmd_fontCacheQuery
#define Gpu_CoCmd_VideoStartF EVE_CoCmd_videoStartF
#define Gpu_CoCmd_FlashSource EVE_CoCmd_flashSource
#define Gpu_CoCmd_Nop EVE_CoCmd_nop
#define Gpu_CoCmd_GradientA EVE_CoCmd_gradientA
#define Gpu_CoCmd_RotateAround EVE_CoCmd_rotateAround
#define Gpu_CoCmd_Inflate2 EVE_CoCmd_inflate2
#define Gpu_CoCmd_ResetFonts EVE_CoCmd_resetFonts
#define Gpu_CoCmd_Wait EVE_CoCmd_wait
#define Gpu_CoCmd_RunAnim EVE_CoCmd_runAnim
#define Gpu_CoCmd_AnimFrameRam EVE_CoCmd_animFrameRam
#define Gpu_CoCmd_AnimStartRam EVE_CoCmd_animStartRam
#define Gpu_CoCmd_Calllist EVE_CoCmd_callList
#define Gpu_CoCmd_Newlist EVE_CoCmd_newList
#define Gpu_CoCmd_Endlist EVE_CoCmd_endList

#define Ft_Gpu_CoCmd_AnimDraw EVE_CoCmd_animDraw
#define Ft_Gpu_CoCmd_AnimFrame EVE_CoCmd_animFrame
#define Ft_Gpu_CoCmd_AnimFrameRam EVE_CoCmd_animFrameRam
#define Ft_Gpu_CoCmd_AnimStart EVE_CoCmd_animStart
#define Ft_Gpu_CoCmd_AnimStop EVE_CoCmd_animStop
#define Ft_Gpu_CoCmd_AnimStartRam EVE_CoCmd_animStartRam
#define Ft_Gpu_CoCmd_AnimXY EVE_CoCmd_animXY
#define Ft_Gpu_CoCmd_Apilevel EVE_CoCmd_apiLevel
#define Ft_Gpu_CoCmd_Append EVE_CoCmd_append
#define Ft_Gpu_CoCmd_AppendF EVE_CoCmd_appendF
#define Ft_Gpu_CoCmd_BgColor EVE_CoCmd_bgColor
#define Ft_Gpu_CoCmd_Bitmap_Transform EVE_CoCmd_bitmapTransform
#define Ft_Gpu_CoCmd_BitmapTransform EVE_CoCmd_bitmapTransform
#define Ft_Gpu_CoCmd_Button EVE_CoCmd_button
#define Ft_Gpu_CoCmd_Calibrate EVE_CoCmd_calibrate
#define Ft_Gpu_CoCmd_Calibratesub EVE_CoCmd_calibrateSub
#define Ft_Gpu_CoCmd_Calllist EVE_CoCmd_callList
#define Ft_Gpu_CoCmd_ClearCache EVE_CoCmd_clearCache
#define Ft_Gpu_CoCmd_Clock EVE_CoCmd_clock
#define Ft_Gpu_CoCmd_ColdStart EVE_CoCmd_coldStart
// #define Ft_Gpu_CoCmd_Crc EVE_CoCmd_crc
#define Ft_Gpu_CoCmd_CSketch EVE_CoCmd_cSketch
#define Ft_Gpu_CoCmd_Dial EVE_CoCmd_dial
#define Ft_Gpu_CoCmd_DlStart EVE_CoCmd_dlStart
#define Ft_Gpu_CoCmd_Endlist EVE_CoCmd_endList
#define Ft_Gpu_CoCmd_FgColor EVE_CoCmd_fgColor
#define Ft_Gpu_CoCmd_FillWidth EVE_CoCmd_fillWidth
#define Ft_Gpu_CoCmd_FlashAttach EVE_CoCmd_flashAttach
#define Ft_Gpu_CoCmd_FlashDetach EVE_CoCmd_flashDetach
#define Ft_Gpu_CoCmd_FlashErase EVE_CoCmd_flashErase
#define Ft_Gpu_CoCmd_FlashFast EVE_CoCmd_flashFast
// #define Ft_Gpu_CoCmd_FlashRead EVE_CoCmd_flashRead
static inline bool Ft_Gpu_CoCmd_FlashRead(EVE_HalContext *phost, uint32_t dest, uint32_t src, uint32_t num)
{
	EVE_CoCmd_flashRead(phost, dest, src, num);
	return EVE_Cmd_waitFlush(phost);
}
#define Ft_Gpu_CoCmd_FlashSource EVE_CoCmd_flashSource
#define Ft_Gpu_CoCmd_FlashSpiDesel EVE_CoCmd_flashSpiDesel
#define Ft_Gpu_CoCmd_FlashSpiRx EVE_CoCmd_flashSpiRx
#define Ft_Gpu_CoCmd_FlashSpiTx EVE_CoCmd_flashSpiTx
#define Ft_Gpu_CoCmd_FlashUpdate EVE_CoCmd_flashUpdate
#define Ft_Gpu_CoCmd_FlashWrite EVE_CoCmd_flashWrite
#define Ft_Gpu_CoCmd_Fontcache EVE_CoCmd_fontCache
#define Ft_Gpu_CoCmd_Fontcachequery EVE_CoCmd_fontCacheQuery
#define Ft_Gpu_CoCmd_Gauge EVE_CoCmd_gauge
#define Ft_Gpu_CoCmd_GetMatrix EVE_CoCmd_getMatrix
#define Ft_Gpu_CoCmd_Getpoint EVE_CoCmd_getpoint
#define Ft_Gpu_CoCmd_GetPtr EVE_CoCmd_getPtr
#define Ft_Gpu_CoCmd_GradColor EVE_CoCmd_gradColor
#define Ft_Gpu_CoCmd_Gradient EVE_CoCmd_gradient
#define Ft_Gpu_CoCmd_GradientA EVE_CoCmd_gradientA
#define Ft_Gpu_CoCmd_Hsf EVE_CoCmd_hsf
#define Ft_Gpu_CoCmd_Inflate EVE_CoCmd_inflate
#define Ft_Gpu_CoCmd_Inflate2 EVE_CoCmd_inflate2
#define Ft_Gpu_CoCmd_Interrupt EVE_CoCmd_interrupt
#define Ft_Gpu_CoCmd_Keys EVE_CoCmd_keys
#define Ft_Gpu_CoCmd_LoadIdentity EVE_CoCmd_loadIdentity
#define Ft_Gpu_CoCmd_LoadImage EVE_CoCmd_loadImage
#define Ft_Gpu_CoCmd_Logo EVE_CoCmd_logo
#define Ft_Gpu_CoCmd_MediaFifo EVE_CoCmd_mediaFifo
#define Ft_Gpu_CoCmd_MemCpy EVE_CoCmd_memCpy
#define Ft_Gpu_CoCmd_MemCrc EVE_CoCmd_memCrc
#define Ft_Gpu_CoCmd_MemSet EVE_CoCmd_memSet
#define Ft_Gpu_CoCmd_MemWrite EVE_CoCmd_memWrite
#define Ft_Gpu_CoCmd_MemZero EVE_CoCmd_memZero
#define Ft_Gpu_CoCmd_Newlist EVE_CoCmd_newList
#define Ft_Gpu_CoCmd_Nop EVE_CoCmd_nop
#define Ft_Gpu_CoCmd_Number EVE_CoCmd_number
#define Ft_Gpu_CoCmd_PlayVideo EVE_CoCmd_playVideo
#define Ft_Gpu_CoCmd_Progress EVE_CoCmd_progress
#define Ft_Gpu_CoCmd_RegRead EVE_CoCmd_regRead
#define Ft_Gpu_CoCmd_ResetFonts EVE_CoCmd_resetFonts
#define Ft_Gpu_CoCmd_RomFont EVE_CoCmd_romFont
#define Ft_Gpu_CoCmd_Rotate EVE_CoCmd_rotate
#define Ft_Gpu_CoCmd_RotateAround EVE_CoCmd_rotateAround
#define Ft_Gpu_CoCmd_RunAnim EVE_CoCmd_runAnim
#define Ft_Gpu_CoCmd_Scale EVE_CoCmd_scale
#define Ft_Gpu_CoCmd_ScreenSaver EVE_CoCmd_screenSaver
#define Ft_Gpu_CoCmd_Scrollbar EVE_CoCmd_scrollbar
#define Ft_Gpu_CoCmd_SetBase EVE_CoCmd_setBase
#define Ft_Gpu_CoCmd_SetBitmap EVE_CoCmd_setBitmap
#define Ft_Gpu_CoCmd_SetFont EVE_CoCmd_setFont
#define Ft_Gpu_CoCmd_SetFont2 EVE_CoCmd_setFont2
#define Ft_Gpu_CoCmd_SetMatrix EVE_CoCmd_setMatrix
#define Ft_Gpu_CoCmd_SetRotate EVE_CoCmd_setRotate
#define Ft_Gpu_CoCmd_SetScratch EVE_CoCmd_setScratch
#define Ft_Gpu_CoCmd_Sketch EVE_CoCmd_sketch
#define Ft_Gpu_CoCmd_Slider EVE_CoCmd_slider
#define Ft_Gpu_CoCmd_Snapshot EVE_CoCmd_snapshot
#define Ft_Gpu_CoCmd_Snapshot2 EVE_CoCmd_snapshot2
#define Ft_Gpu_CoCmd_Spinner EVE_CoCmd_spinner
#define Ft_Gpu_CoCmd_Stop EVE_CoCmd_stop
#define Ft_Gpu_CoCmd_Swap EVE_CoCmd_swap
#define Ft_Gpu_CoCmd_Sync EVE_CoCmd_sync
#define Ft_Gpu_CoCmd_Pclkfreq EVE_CoCmd_pclkFreq
#define Ft_Gpu_CoCmd_Testcard EVE_CoCmd_testCard
#define Ft_Gpu_CoCmd_Text EVE_CoCmd_text
#define Ft_Gpu_CoCmd_Toggle EVE_CoCmd_toggle
#define Ft_Gpu_CoCmd_Touch_Transform EVE_CoCmd_touchTransform
#define Ft_Gpu_CoCmd_Track EVE_CoCmd_track
#define Ft_Gpu_CoCmd_Translate EVE_CoCmd_translate
#define Ft_Gpu_CoCmd_VideoFrame EVE_CoCmd_videoFrame
#define Ft_Gpu_CoCmd_VideoStart EVE_CoCmd_videoStart
#define Ft_Gpu_CoCmd_VideoStartF EVE_CoCmd_videoStartF
#define Ft_Gpu_CoCmd_Wait EVE_CoCmd_wait
#define Ft_Gpu_CoCmd_Text_S EVE_CoCmd_text_s
#define Ft_Gpu_CoCmd_Text_Ex EVE_CoCmd_text_ex
#define Ft_Gpu_CoCmd_LoadImage_Flash EVE_CoCmd_loadImage_flash
#define Ft_Gpu_CoCmd_Inflate_Flash EVE_CoCmd_inflate_flash
#define Ft_Gpu_CoCmd_LoadImage_ProgMem EVE_CoCmd_loadImage_progMem
#define Ft_Gpu_CoCmd_Inflate_ProgMem EVE_CoCmd_inflate_progMem

#define Gpu_CoCmd_SetBitmap Ft_Gpu_CoCmd_SetBitmap
#define Gpu_CoCmd_SetScratch Ft_Gpu_CoCmd_SetScratch
#define Gpu_CoCmd_RomFont Ft_Gpu_CoCmd_RomFont
#define Gpu_CoCmd_Text Ft_Gpu_CoCmd_Text
#define Gpu_CoCmd_Number Ft_Gpu_CoCmd_Number
#define Gpu_CoCmd_LoadIdentity Ft_Gpu_CoCmd_LoadIdentity
#define Gpu_CoCmd_Toggle Ft_Gpu_CoCmd_Toggle
#define Gpu_CoCmd_Gauge Ft_Gpu_CoCmd_Gauge
#define Gpu_CoCmd_FillWidth Ft_Gpu_CoCmd_FillWidth
#define Gpu_CoCmd_RegRead Ft_Gpu_CoCmd_RegRead
#define Gpu_CoCmd_VideoStart Ft_Gpu_CoCmd_VideoStart
#define Gpu_CoCmd_GetProps EVE_CoCmd_getProps
#define Gpu_CoCmd_Memcpy Ft_Gpu_CoCmd_MemCpy
#define Gpu_CoCmd_Spinner Ft_Gpu_CoCmd_Spinner
#define Gpu_CoCmd_BgColor Ft_Gpu_CoCmd_BgColor
#define Gpu_CoCmd_Swap Ft_Gpu_CoCmd_Swap
#define Gpu_CoCmd_Inflate Ft_Gpu_CoCmd_Inflate
#define Gpu_CoCmd_Translate Ft_Gpu_CoCmd_Translate
#define Gpu_CoCmd_Stop Ft_Gpu_CoCmd_Stop
#define Gpu_CoCmd_SetBase Ft_Gpu_CoCmd_SetBase
#define Gpu_CoCmd_Slider Ft_Gpu_CoCmd_Slider
#define Gpu_CoCmd_VideoFrame Ft_Gpu_CoCmd_VideoFrame
#define Gpu_CoCmd_Interrupt Ft_Gpu_CoCmd_Interrupt
#define Gpu_CoCmd_FgColor Ft_Gpu_CoCmd_FgColor
#define Gpu_CoCmd_Rotate Ft_Gpu_CoCmd_Rotate
#define Gpu_CoCmd_Button Ft_Gpu_CoCmd_Button
#define Gpu_CoCmd_MemWrite Ft_Gpu_CoCmd_MemWrite
#define Gpu_CoCmd_Scrollbar Ft_Gpu_CoCmd_Scrollbar
#define Gpu_CoCmd_GetMatrix Ft_Gpu_CoCmd_GetMatrix
#define Gpu_CoCmd_Sketch Ft_Gpu_CoCmd_Sketch
#define Gpu_CoCmd_CSketch Ft_Gpu_CoCmd_CSketch
#define Gpu_CoCmd_PlayVideo Ft_Gpu_CoCmd_PlayVideo
#define Gpu_CoCmd_MemSet Ft_Gpu_CoCmd_MemSet
#define Gpu_CoCmd_Calibrate(phost, r) Ft_Gpu_CoCmd_Calibrate((phost))
#define Gpu_CoCmd_SetFont Ft_Gpu_CoCmd_SetFont
#define Gpu_CoCmd_Bitmap_Transform Ft_Gpu_CoCmd_Bitmap_Transform
#define Gpu_CoCmd_BitmapTransform Ft_Gpu_CoCmd_Bitmap_Transform
#define Gpu_CoCmd_GradColor Ft_Gpu_CoCmd_GradColor
#define Gpu_CoCmd_Append Ft_Gpu_CoCmd_Append
#define Gpu_CoCmd_MemZero Ft_Gpu_CoCmd_MemZero
#define Gpu_CoCmd_Scale Ft_Gpu_CoCmd_Scale
#define Gpu_CoCmd_Clock Ft_Gpu_CoCmd_Clock
#define Gpu_CoCmd_Gradient Ft_Gpu_CoCmd_Gradient
#define Gpu_CoCmd_SetMatrix Ft_Gpu_CoCmd_SetMatrix
#define Gpu_CoCmd_Track Ft_Gpu_CoCmd_Track
#define Gpu_CoCmd_GetPtr Ft_Gpu_CoCmd_GetPtr
#define Gpu_CoCmd_Progress Ft_Gpu_CoCmd_Progress
#define Gpu_CoCmd_ColdStart Ft_Gpu_CoCmd_ColdStart
#define Gpu_CoCmd_MediaFifo Ft_Gpu_CoCmd_MediaFifo
#define Gpu_CoCmd_Keys Ft_Gpu_CoCmd_Keys
#define Gpu_CoCmd_Dial Ft_Gpu_CoCmd_Dial
#define Gpu_CoCmd_Snapshot2 Ft_Gpu_CoCmd_Snapshot2
#define Gpu_CoCmd_LoadImage Ft_Gpu_CoCmd_LoadImage
#define Gpu_CoCmd_SetFont2 Ft_Gpu_CoCmd_SetFont2
#define Gpu_CoCmd_SetRotate Ft_Gpu_CoCmd_SetRotate
#define Gpu_CoCmd_Dlstart Ft_Gpu_CoCmd_DlStart
#define Gpu_CoCmd_Snapshot Ft_Gpu_CoCmd_Snapshot
#define Gpu_CoCmd_ScreenSaver Ft_Gpu_CoCmd_ScreenSaver
#define Gpu_CoCmd_MemCrc Ft_Gpu_CoCmd_MemCrc
#define Gpu_CoCmd_Logo Ft_Gpu_CoCmd_Logo
#define Gpu_CoCmd_Sync Ft_Gpu_CoCmd_Sync
#define Gpu_CoCmd_Pclkfreq Ft_Gpu_CoCmd_Pclkfreq

#define Gpu_CoCmd_AnimStart Ft_Gpu_CoCmd_AnimStart
#define Gpu_CoCmd_AnimStop Ft_Gpu_CoCmd_AnimStop
#define Gpu_CoCmd_AnimXY Ft_Gpu_CoCmd_AnimXY
#define Gpu_CoCmd_AnimDraw Ft_Gpu_CoCmd_AnimDraw
#define Gpu_CoCmd_AnimFrame Ft_Gpu_CoCmd_AnimFrame

#define Gpu_CoCmd_FlashWrite EVE_CoCmd_flashWrite
#define Gpu_CoCmd_FlashSpiTx EVE_CoCmd_flashSpiTx
#define Gpu_CoCmd_FlashSpiRx EVE_CoCmd_flashSpiRx
#define Gpu_CoCmd_FlashSpiDesel EVE_CoCmd_flashSpiDesel

#define Gpu_CoCmd_AppendF EVE_CoCmd_appendF
// #define Gpu_CoCmd_GetPoint EVE_CoCmd_getPoint

#endif /* EVE_COMPAT_COCMD__H */
/* end of file */
