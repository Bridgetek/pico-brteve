/**
 * @file FlashHelper.c
 * @brief Eve's connected flash helper functions
 *
 * @author Tuan Nguyen <tuan.nguyen@brtchip.com>
 *
 * @date 2019
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

#include "FlashHelper.h"
#include "Gpu_Hal.h"

/* Below are helper functions, not listed in cocmd */

/**
 * @brief Do calibration
 *
 * @return ft_bool_t True on successfull or otherwise
 */
ft_bool_t Esd_Calibrate(EVE_HalContext* phost)
{
	ft_uint32_t result;
	ft_uint32_t transMatrix[6];
#if defined(EVE_SUPPORT_CAPACITIVE)
	EVE_Hal_wr8(phost, REG_CTOUCH_EXTENDED, CTOUCH_MODE_COMPATIBILITY);
#else
	EVE_Hal_wr8(phost, REG_TOUCH_ADC_MODE, ADC_DIFFERENTIAL);
#endif

	eve_printf_debug("App_CoPro_Widget_Calibrate: Start\n");

	EVE_CoCmd_dlStart(phost);
	EVE_Cmd_wr32(phost, CLEAR_COLOR_RGB(64, 64, 64));
	EVE_Cmd_wr32(phost, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(phost, COLOR_RGB(0xff, 0xff, 0xff));

	EVE_CoCmd_text(phost, (uint16_t)(phost->Width / 2), (uint16_t)(phost->Height / 2), 27, OPT_CENTER, "Please Tap on the dot");

	result = EVE_CoCmd_calibrate(phost);
	EVE_Cmd_waitFlush(phost);

	eve_printf_debug("App_CoPro_Widget_Calibrate: End\n");

	// Print the configured values
	EVE_Hal_rdMem(phost, (ft_uint8_t*)transMatrix, REG_TOUCH_TRANSFORM_A, 4 * 6); //read all the 6 coefficients
	eve_printf_debug("Touch screen transform values are A 0x%x,B 0x%x,C 0x%x,D 0x%x,E 0x%x, F 0x%x\n",
		transMatrix[0], transMatrix[1], transMatrix[2], transMatrix[3], transMatrix[4], transMatrix[5]);

	return result != 0;
}

#ifdef EVE_FLASH_AVAILABLE
/**
 * @brief Writes the given data to flash.
 * If the data matches the existing contents of flash, nothing is done.
 * Otherwise the flash is erased in 4K units, and the data is written.
 *
 * dest_flash: destination address in flash memory. Must be 4096-byte aligned
 * src_ram: source data in main memory. Must be 4-byte aligned
 * num: number of bytes to write, should be multiple of 4096, otherwise, dummy data will be padded
 *
 * @param phost Pointer to Hal context
 * @param dest_flash Destination on flash
 * @param src_ram Source in RAM_G
 * @param num Numbber of byte to write
 * @return Flash_Cmd_Status_t
 */
Flash_Cmd_Status_t FlashHelper_Update(EVE_HalContext* phost, uint32_t dest_flash, uint32_t src_ram, uint32_t num)
{
	uint32_t last_chunk = (num % 4096); /* must be multiple of 4096. Cut off the extended data */

	if ((dest_flash % FLASH_UPDATE_ALIGN_BYTE != 0) || ((src_ram % 4) != 0)) /* Check aligned address */
	{
		return FLASH_CMD_ALIGNED_ERR;
	}

	if (num < FLASH_UPDATE_ALIGN_BYTE)
	{
		EVE_CoCmd_flashUpdate(phost, dest_flash, src_ram, FLASH_UPDATE_ALIGN_BYTE);
		EVE_Cmd_waitFlush(phost);
	}
	else if (last_chunk == 0) /* num is multiple of 4k */
	{
		EVE_CoCmd_flashUpdate(phost, dest_flash, src_ram, num);
		EVE_Cmd_waitFlush(phost);
	}
	else /* num is not fit in multiple of 4k */
	{
		EVE_CoCmd_flashUpdate(phost, dest_flash, src_ram, num - last_chunk);
		EVE_Cmd_waitFlush(phost);

		/* 4k is quite big for allocating new stack/heap data. So reuse the pointer and write dummy data to flash */
		EVE_CoCmd_flashUpdate(phost, dest_flash + num - last_chunk, src_ram + num - last_chunk, FLASH_UPDATE_ALIGN_BYTE);
		EVE_Cmd_waitFlush(phost);
	}
	return FLASH_CMD_SUCCESS;
}
#endif

#ifdef EVE_FLASH_AVAILABLE
/**
 * @brief Read data from flash to array
 * dest_ram: address in ram where the flash copy data to
 * src_flash: source address in flash memory. Must be 64-byte aligned. From 0 to 64*1024 for 64MB flash
 * num: number of bytes would be read
 * read_data: pointer to user read data
 *
 * @param phost Pointer to Hal context
 * @param dest_ram Destination on RAM_G
 * @param src_flash Source in flash
 * @param num Numbber of byte to read
 * @param read_data Buffer to get data
 * @return Flash_Cmd_Status_t
 */
Flash_Cmd_Status_t FlashHelper_Read(EVE_HalContext* phost, uint32_t dest_ram, uint32_t src_flash, uint32_t num, uint8_t *read_data)
{
	num = num - (num % 4); /* Only read lesser or equal aligned bytes */

	if ((src_flash % FLASH_READ_ALIGN_BYTE != 0) || ((dest_ram % 4) != 0)) /* Check aligned address */
	{
		return FLASH_CMD_ALIGNED_ERR;
	}

	EVE_CoCmd_flashRead(phost, dest_ram, src_flash, num);
	EVE_Cmd_waitFlush(phost);

	EVE_Hal_rdMem(phost, read_data, dest_ram, num);
	return FLASH_CMD_SUCCESS;
}
#endif

#ifdef EVE_FLASH_AVAILABLE
/**
 * @brief Erase entire flash
 *
 * @param phost Pointer to Hal context
 * @return ft_void_t
 */
ft_void_t FlashHelper_Erase(EVE_HalContext* phost)
{
	EVE_CoCmd_flashErase(phost);
	EVE_Cmd_waitFlush(phost);
}
#endif

#ifdef EVE_FLASH_AVAILABLE
/**
 * @brief Flash state/status
 *
 * @param phost Pointer to Hal context
 * @return uint8_t
 */
uint8_t FlashHelper_GetState(EVE_HalContext* phost)
{
	return EVE_Hal_rd8(phost, REG_FLASH_STATUS);
}
#endif

#ifdef EVE_FLASH_AVAILABLE
/**
 * @brief Clears the graphics system's flash cache. It should be executed after modifying graphics data in flash, otherwise bitmaps from flash may render "stale" data.
 * This command must be executed when the display list is in use, immediately after a CMD SWAP command.
 *
 * @param phost Pointer to Hal context
 * @return ft_void_t
 */
ft_void_t FlashHelper_ClearCache(EVE_HalContext* phost)
{
	EVE_CoCmd_clearCache(phost);
	EVE_Cmd_waitFlush(phost);
}
#endif

/*

*/
#ifdef EVE_FLASH_AVAILABLE
/**
 * @brief Write data to flash, and align byte if needed.
 * Note:
 * - Destination flash address must be virgin (not used before). Otherwise, users have to perform flash erase before using.
 * - Destination address must be 256-byte aligned.
 * - Automatically padding 0xFF to non-aligned num.
 *
 * @param phost Pointer to Hal context
 * @param dest_flash Destination on flash
 * @param num Number of bytes to write
 * @param write_data Data buffer to write
 * @return Flash_Cmd_Status_t
 */
Flash_Cmd_Status_t FlashHelper_Write(EVE_HalContext* phost, uint32_t dest_flash, uint32_t num, const uint8_t* write_data)
{
	uint8_t padding_arr[FLASH_WRITE_ALIGN_BYTE]; /* write_data must be 256-byte aligned */
	uint32_t aligned_length = num % FLASH_WRITE_ALIGN_BYTE;

	if (dest_flash % FLASH_WRITE_ALIGN_BYTE != 0) /* Check aligned address */
	{
		return FLASH_CMD_ALIGNED_ERR;
	}

	if (aligned_length == 0) /* write_data is already aligned */
	{
		FlashHelper_flashWriteExt(phost, dest_flash, num, write_data);
		EVE_Cmd_waitFlush(phost);
	}
	else
	{
		/* Write first aligned chunks of write_data */
		if (num - aligned_length > 0)
		{
			FlashHelper_flashWriteExt(phost, dest_flash, num - aligned_length, write_data);
			EVE_Cmd_waitFlush(phost);
		}
		/* Write the rest write_data */
		write_data = write_data + num - aligned_length;
		for (uint32_t i = 0; i < FLASH_WRITE_ALIGN_BYTE; i++)
		{
			if (i < aligned_length)
			{
				padding_arr[i] = *write_data++;
			}
			else
			{
				padding_arr[i] = 0xFF; /* Should use 0xFF instead of 0x00 to avoid writing overhead */
			}
		}
		FlashHelper_flashWriteExt(phost, dest_flash + num - aligned_length, FLASH_WRITE_ALIGN_BYTE, padding_arr);
		EVE_Cmd_waitFlush(phost);
	}
	return FLASH_CMD_SUCCESS;
}
#endif

/*

*/
#ifdef EVE_FLASH_AVAILABLE
/**
 * @brief Switch to other flash state
 * Error code:
 * - 0x0    command succeeds
 * - 0xffff command fails (invalid transition state)
 * - 0xe001 flash is not attached
 * - 0xe002 no header detected in sector 0 - is flash blank?
 * - 0xe003 sector 0 data failed integrity check
 * - 0xe004 device/blob mismatch - was correct blob loaded?
 * - 0xe005 failed full-speed test - check board wiring
 *
 * @param phost Pointer to Hal context
 * @param nextState New state to switch to
 * @return uint32_t
 */
uint32_t FlashHelper_SwitchState(EVE_HalContext* phost, uint8_t nextState)
{
	uint32_t ret = 0;
	//uint8_t read_data[CMD_FIFO_SIZE]; Debug only
	uint8_t curr_flash_state = EVE_Hal_rd8(phost, REG_FLASH_STATUS);
	uint16_t ret_addr = 0;
	if (curr_flash_state == nextState) {
		return ret;
	}

	//Only handle if nextState is diff
	if (FLASH_STATUS_DETACHED == nextState)
	{
		EVE_CoCmd_flashDetach(phost);
		EVE_Cmd_waitFlush(phost);
	}
	else if (FLASH_STATUS_BASIC == nextState)
	{
		if (FLASH_STATUS_FULL == curr_flash_state)
		{
			do
			{
				EVE_CoCmd_flashDetach(phost);
				EVE_Cmd_waitFlush(phost);
			} while (FLASH_STATUS_DETACHED != EVE_Hal_rd8(phost, REG_FLASH_STATUS));
		}
		EVE_CoCmd_flashAttach(phost);
		EVE_Cmd_waitFlush(phost);
	}
	else if (FLASH_STATUS_FULL == nextState)
	{
		if (FLASH_STATUS_BASIC != curr_flash_state)
		{
			do
			{
				EVE_CoCmd_flashAttach(phost);
				EVE_Cmd_waitFlush(phost);
			} while (FLASH_STATUS_BASIC != EVE_Hal_rd8(phost, REG_FLASH_STATUS));
		}
		EVE_CoCmd_flashFast(phost, 0);
		EVE_Cmd_waitFlush(phost);

		/* Read the return code in CMD_BUFFER */
		ret_addr = (EVE_Cmd_wp(phost) - 4) & FIFO_SIZE_MASK;
		ret_addr = (ret_addr + 3) & FIFO_BYTE_ALIGNMENT_MASK; //4 byte alignment

		ret = EVE_Hal_rd32(phost, RAM_CMD + ret_addr);
	}
	else
	{
		ret = 0xffff;
	}

	return ret;
}
#endif

#ifdef EVE_FLASH_AVAILABLE
/**
 * @brief Switch flash into full mode
 *
 * @param phost Pointer to Hal context
 * @return uint32_t
 */
uint32_t FlashHelper_SwitchFullMode(EVE_HalContext* phost)
{
	uint8_t val;
	/* Try detaching and attaching the flash followed by fast mdoe */
	EVE_CoCmd_flashDetach(phost);
	EVE_Cmd_waitFlush(phost);
	val = EVE_Hal_rd8(phost, REG_FLASH_STATUS);

	if (FLASH_STATUS_DETACHED != val)
	{
		printf("Error, Flash is not able to detatch %d\n", val);
		return 0;
	}

	EVE_CoCmd_flashAttach(phost);
	EVE_Cmd_waitFlush(phost);
	val = EVE_Hal_rd8(phost, REG_FLASH_STATUS);

	if (FLASH_STATUS_BASIC != val)
	{
		printf("Error, Flash is not able to attach %d\n", val);
		return 0;
	}

	EVE_CoCmd_flashFast(phost, 0);
	EVE_Cmd_waitFlush(phost);
	val = EVE_Hal_rd8(phost, REG_FLASH_STATUS);

	if (FLASH_STATUS_FULL != val)
	{
		printf("Error, Flash is not able to get into full mode, status: %d\n", val);
		return 0;
	}
	return 1;
}
#endif

#if (EVE_SUPPORT_CHIPID >= EVE_BT817)
/**
 * @brief Adjust for non-sqare pixel panel
 *
 * @param physical_W Panel width in inches
 * @param physical_H Panel height in inches
 * @return ft_void_t
 */
ft_void_t Ft_Gpu_HorizontalScanoutFilter(EVE_HalContext* phost, uint32_t physical_W, uint32_t physical_H) {
	uint32_t eve_W = phost->Width;
	uint32_t eve_H = phost->Height;
	uint32_t logical_W = eve_H * physical_W / physical_H;

	// Configure panel
	EVE_Hal_wr32(phost, REG_HSIZE, logical_W);
	EVE_CoCmd_hsf(phost, eve_W);
}
#endif /* EVE_SUPPORT_CHIPID >= EVE_BT817 */

#ifdef EVE_FLASH_AVAILABLE
/**
 * @brief Write data to flash. Constraints:
 * - Destination flash address must be virgin (not used before)
 * - data array must be aligned 256-bit
 *
 * @param phost Pointer to Hal context
 * @param dest Destination on flash
 * @param num Number of byte to write
 * @param data Data buffer to write
 * @return ft_void_t
 */
ft_void_t FlashHelper_flashWriteExt(EVE_HalContext* phost, uint32_t dest, uint32_t num, const uint8_t* data)
{
	uint32_t send_data32 = 0;

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_FLASHWRITE);
	EVE_Cmd_wr32(phost, dest);
	EVE_Cmd_wr32(phost, num);
	for (uint32_t i = 0; i < num; i = i + 4)
	{
		/* Pack 4 bytes into a 32-bit data each sending package */
		send_data32 = *data++;
		send_data32 |= (*data++) << 8;
		send_data32 |= (*data++) << 16;
		send_data32 |= (*data++) << 24;
		EVE_Cmd_wr32(phost, send_data32);
	}
	EVE_Cmd_endFunc(phost);
}
#endif

/*************************************************************************************************
*                      DL buffer functions
**************************************************************************************************/
uint32_t DlBuffer_Index;

#ifdef BUFFER_OPTIMIZATION  
uint8_t  DlBuffer[EVE_DL_SIZE];
#endif

/**
 * @brief Write DL command to buffer
 *
 * @param phost Pointer to Hal context
 * @param cmd DL command
 */
void App_WrDl_Buffer(Gpu_Hal_Context_t* phost, uint32_t cmd)
{
#ifdef BUFFER_OPTIMIZATION  
	/* Copy the command instruction into buffer */
	uint32_t* pBuffcmd;
	/* Prevent buffer overflow */
	if (DlBuffer_Index < DL_SIZE)
	{
		pBuffcmd = (uint32_t*)&DlBuffer[DlBuffer_Index];
		*pBuffcmd = cmd;
	}
	else
	{
		printf("DlBuffer overflow\n");
	}
#else
	EVE_Hal_wr32(phost, (RAM_DL + DlBuffer_Index), cmd);
#endif
	/* Increment the command index */
	DlBuffer_Index += CMD_SIZE;
}

/**
 * @brief Flush DL buffer to Coprocessor
 *
 * @param phost Pointer to Hal context
 */
void App_Flush_DL_Buffer(const Gpu_Hal_Context_t* phost)
{
#ifdef  BUFFER_OPTIMIZATION
	if (DlBuffer_Index > 0)
		EVE_Hal_wrMem(phost, RAM_DL, DlBuffer, DlBuffer_Index); /* Not legal on big endian CPU */
#endif
	DlBuffer_Index = 0;
}

/**
 * @brief Set DL buffer index
 *
 * @param index
 */
void App_Set_DlBuffer_Index(uint32_t index)
{
	DlBuffer_Index = index;
}

/**
 * @brief API to check the status of previous DLSWAP and perform DLSWAP of new DL
 * Check for the status of previous DLSWAP and if still not done wait for few ms and check again
 *
 * @param phost Pointer to Hal context
 * @param DL_Swap_Type DL list swap type
 */
void GPU_DLSwap(Gpu_Hal_Context_t* phost, uint8_t DL_Swap_Type)
{
	uint8_t Swap_Type = DLSWAP_FRAME;
	uint8_t Swap_Done = DLSWAP_FRAME;

	if (DL_Swap_Type == DLSWAP_LINE)
	{
		Swap_Type = DLSWAP_LINE;
	}

	/* Perform a new DL swap */
	EVE_Hal_wr8(phost, REG_DLSWAP, Swap_Type);

	/* Wait till the swap is done */
	while (Swap_Done)
	{
		Swap_Done = EVE_Hal_rd8(phost, REG_DLSWAP);

		if (DLSWAP_DONE != Swap_Done)
		{
			EVE_sleep(10);//wait for 10ms
		}
	}
}

/**
 * @brief Fadeout animation
 *
 * @param phost Pointer to Hal context
 */
void fadeout(EVE_HalContext* phost)
{
	for (int8_t i = 100; i >= 0; i -= 3)
	{
		EVE_Hal_wr8(phost, REG_PWM_DUTY, i);
		EVE_sleep(2);//sleep for 2 ms
	}
}

/**
 * @brief Fadein animation
 * perform display fadein effect by changing the display PWM from 0 till 100 and finally 128
 *
 * @param phost Pointer to Hal context
 */
void fadein(EVE_HalContext* phost)
{
	for (int8_t i = 0; i <= 100; i += 3)
	{
		EVE_Hal_wr8(phost, REG_PWM_DUTY, i);
		EVE_sleep(2);//sleep for 2 ms
	}
	/* Finally make the PWM 100% */
	uint8_t i = 128;
	EVE_Hal_wr8(phost, REG_PWM_DUTY, i);
}

/**
 * @brief Init mediafifo
 *
 * @param pFifo Fifo pointer
 * @param StartAddress Address on RAM_G
 * @param Length Fifo length
 * @param HWReadRegAddress REG_MEDIAFIFO_READ
 * @param HWWriteRegAddress REG_MEDIAFIFO_WRITE
 */
void Fifo_Init(Fifo_t* pFifo, uint32_t StartAddress, uint32_t Length, uint32_t HWReadRegAddress, uint32_t HWWriteRegAddress)
{
	/* update the context parameters */
	pFifo->fifo_buff = StartAddress;
	pFifo->fifo_len = Length;
	pFifo->fifo_rp = pFifo->fifo_wp = 0;

	/* update the hardware register addresses - specific to FT800 series chips */
	pFifo->HW_Read_Reg = HWReadRegAddress;
	pFifo->HW_Write_Reg = HWWriteRegAddress;
}

/**
 * @brief update both the read and write pointers
 *
 * @param host Pointer to Hal context
 * @param pFifo Fifo poiner
 */
void Fifo_Update(Gpu_Hal_Context_t* host, Fifo_t* pFifo)
{
	pFifo->fifo_rp = EVE_Hal_rd32(host, pFifo->HW_Read_Reg);
}

/**
 * @brief write and update the write register
 *
 * @param host Pointer to Hal context
 * @param pFifo Fifo pointer
 * @param buffer
 * @param NumbytetoWrite
 * @return uint32_t
 */
uint32_t Fifo_Write(Gpu_Hal_Context_t* host, Fifo_t* pFifo, const uint8_t* buffer, uint32_t NumbytetoWrite)
{
	uint32_t FreeSpace = Fifo_GetFreeSpace(host, pFifo);
	uint32_t TotalBytes = NumbytetoWrite;

	if (NumbytetoWrite > FreeSpace)
	{
		/* update the read pointer and get the free space */
		Fifo_Update(host, pFifo);
		FreeSpace = Fifo_GetFreeSpace(host, pFifo);

		if (NumbytetoWrite > FreeSpace)
		{
			TotalBytes = FreeSpace;
		}
	}

	/* sanity check */
	if (TotalBytes <= 0)
	{
		return 0;//error condition
	}
	/* check for the loopback conditions */
	if ((pFifo->fifo_wp + (int32_t)TotalBytes) >= pFifo->fifo_len)
	{
		uint32_t partialchunk = pFifo->fifo_len - pFifo->fifo_wp;
		uint32_t secpartialchunk = TotalBytes - partialchunk;

		EVE_Hal_wrMem(host, pFifo->fifo_buff + pFifo->fifo_wp, buffer, partialchunk);
		if (secpartialchunk > 0)
		{
			EVE_Hal_wrMem(host, pFifo->fifo_buff, buffer + partialchunk, secpartialchunk);
		}
		pFifo->fifo_wp = secpartialchunk;

	}
	else
	{
		EVE_Hal_wrMem(host, pFifo->fifo_buff + pFifo->fifo_wp, buffer, TotalBytes);
		pFifo->fifo_wp += TotalBytes;
	}

	/* update the write pointer address in write register */
	EVE_Hal_wr32(host, pFifo->HW_Write_Reg, pFifo->fifo_wp);

	return TotalBytes;
}
/***************************************************************************
* Interface Description    : FIFO related apis
*                            write and wait for the fifo to be empty. handle cases even if
*                            the Numbytes are more than freespace
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/

/**
 * @brief write and wait for the fifo to be empty. handle cases even if the Numbytes are more than freespace
 *
 * @param host Pointer to Hal context
 * @param pFifo Fifo pointer
 * @param buffer
 * @param Numbyte
 */
void Fifo_WriteWait(Gpu_Hal_Context_t* host, Fifo_t* pFifo, const uint8_t* buffer, uint32_t Numbyte)
{
	uint32_t TotalBytes = Numbyte;
	uint32_t currchunk = 0;
	uint32_t FreeSpace;
	const uint8_t* pbuff = buffer;
	/* blocking call, manage to check for the error case and break in case of error */
	while (TotalBytes > 0)
	{
		currchunk = TotalBytes;
		FreeSpace = Fifo_GetFreeSpace(host, pFifo);
		if (currchunk > FreeSpace)
		{
			currchunk = FreeSpace;
		}

		Fifo_Write(host, pFifo, pbuff, currchunk);
		pbuff += currchunk;
		TotalBytes -= currchunk;

	}
}

/**
 * @brief write one word and update the write register
 *
 * @param host Pointer to Hal context
 * @param pFifo Fifo pointer
 * @param WriteWord Word to write
 */
void Fifo_Write32(Gpu_Hal_Context_t* host, Fifo_t* pFifo, uint32_t WriteWord)
{
	Fifo_WriteWait(host, pFifo, (uint8_t*)&WriteWord, 4);
}

/**
 * @brief get the free space in the fifo
 *
 * @param host Pointer to Hal context
 * @param pFifo Fifo pointer
 * @return uint32_t
 */
uint32_t Fifo_GetFreeSpace(Gpu_Hal_Context_t* host, Fifo_t* pFifo)
{
	uint32_t FreeSpace = 0;

	Fifo_Update(host, pFifo);

	if (pFifo->fifo_wp >= pFifo->fifo_rp)
	{
		FreeSpace = pFifo->fifo_len - pFifo->fifo_wp + pFifo->fifo_rp;
	}
	else
	{
		FreeSpace = pFifo->fifo_rp - pFifo->fifo_wp;
	}

	if (FreeSpace >= 4)
	{
		FreeSpace -= 4;//make sure 1 word space is maintained between rd and wr pointers
	}
	return FreeSpace;
}

int32_t FlashHelper_GetSizeMB(Gpu_Hal_Context_t* phost)
{
	uint8_t status = Gpu_Hal_Rd8(phost, REG_FLASH_STATUS);

	if (status == FLASH_STATUS_DETACHED)
	{
		Gpu_CoCmd_FlashAttach(phost);
		App_Flush_Co_Buffer(phost);
		Gpu_Hal_WaitCmdfifo_empty(phost);
		status = Gpu_Hal_Rd8(phost, REG_FLASH_STATUS);

		if (FLASH_STATUS_BASIC != status)
		{
			printf("Error, Flash is not able to attach\n");
			return -1;
		}
	}
	int32_t size = Gpu_Hal_Rd32(phost, REG_FLASH_SIZE);

	return size;
}
/* Nothing beyond this */
