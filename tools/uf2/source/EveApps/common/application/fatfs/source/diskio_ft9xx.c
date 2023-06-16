/**
 * @file diskio_ft9xx.c
 * @brief FT9XX implementation of the diskIO
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

/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#if defined(__FT900__) || defined(__FT930__)

#include "ff.h"		/* FatFs lower layer API */
#include "diskio.h"		/* FatFs lower layer API */
#include "ft900_sdhost.h"
#include <string.h>

#define _USE_WRITE	1	/* 1: Enable disk_write function */
#define _USE_IOCTL	1	/* 1: Enable disk_ioctl fucntion */


// ============================================================================================== //
//                     				G L O B A L   V A R I A B L E S		   			  			  //
// ============================================================================================== //
static int sd_init = 1; // Already initialized externally, only reinitialize when lost
static int sd_ready = 0;

// A 32 bit aligned buffer of 512 bytes to copy non-aligned sectors to/from FatFs
// It's OK to use the same buffer for read/write as FatFS will never read and write at the same time.
// Even when accessed from multiple threads (FS_REENTRANCY == 1)
static unsigned long __attribute__ ((aligned (32))) temp[128];

/* FatFS Functions ******************/

/** Initialise a drive
 *  @param pdrv Physical Drive number
 *  @return Disk Status */
DSTATUS disk_initialize(BYTE pdrv) {
	DSTATUS stat = 0;

	if (sd_ready || !sd_init) {
		// sdhost_sys_init();
		sdhost_init();
		sd_init = 1;
		sd_ready = 0;
	}

	if (sdhost_card_detect() != SDHOST_CARD_INSERTED) {
		return STA_NOINIT | STA_NODISK;
	}

	if (sdhost_card_init() != SDHOST_OK) {
		stat = STA_NOINIT;
		sd_init = 0;
	} else {
		sd_ready = 1;
	}

	return stat;
}

/** Disk Status
 *  @param pdrv Physical Drive number
 *  @return Disk Status */
DSTATUS disk_status(BYTE pdrv) {
	DSTATUS stat = 0;
	SDHOST_STATUS sdHostStatus;

	if (!sd_init) {
		return STA_NOINIT;
	}

	sdHostStatus = sdhost_card_detect();

	if (sdHostStatus != SDHOST_CARD_INSERTED) {
		if (sd_ready) {
			sd_ready = 0;
			sdhost_init();
		}
	}

	if (!sd_ready) {
		stat |= STA_NOINIT;
	}

	if (sdHostStatus == SDHOST_CARD_REMOVED) {
		stat |= STA_NODISK;
	}

	return stat;
}

/** Read sector(s) from disk
 *  @param pdrv Physical Drive number
 *  @param buff Data buffer to store into
 *  @param sector The logical sector address
 *  @param count The number of sectors to read
 *  @return Disk Status */
DRESULT disk_read(BYTE pdrv, BYTE* buff, DWORD sector, UINT count) {
	DRESULT res = RES_OK;
	SDHOST_STATUS sdHostStatus = SDHOST_OK;
	int i;

	if (((unsigned int) buff & 3) != 0) {

		//print("%%%%%%%%%% READ - DISKIO - buffer NOT 32 bit aligned %%%%%%%%%%%%%%%");
		for (i = 0; i < count; i++) {
			sdHostStatus = sdhost_transfer_data(SDHOST_READ, (void*) temp,
			SDHOST_BLK_SIZE, sector);
			memmove(buff, temp, sizeof(temp));
			sector++;
			buff += sizeof(temp);
		}
	} else {
		sdHostStatus = sdhost_transfer_data(SDHOST_READ, (void*) buff,
		SDHOST_BLK_SIZE * count, sector);
	}

	if (sdHostStatus != SDHOST_OK) {
		res = RES_ERROR;
	}

	return res;
}

#if _USE_WRITE
/** Write sector(s) to the disk
 *  @param pdrv Physical Drive number
 *  @param buff Data buffer to write to the disk
 *  @param sector The logical sector address
 *  @param count The number of sectors to write
 *  @return Disk Status */
DRESULT disk_write(BYTE pdrv, const BYTE* buff, DWORD sector, UINT count) {
	DRESULT res = RES_OK;
	// Non-aligned writes are inefficient because of this additional move to temp buffer. But thankfully they are rate in FatFS
	SDHOST_STATUS sdHostStatus = SDHOST_OK;
	int i;

	if (((unsigned int) buff & 3) != 0) {

		//	print("%%%%%%%%%% WRITE - DISKIO - buffer NOT 32 bit aligned %%%%%%%%%%%%%%%");

		for (i = 0; i < count; i++) {
			memmove(temp, buff, sizeof(temp));
			sdHostStatus = sdhost_transfer_data(SDHOST_WRITE, (void*) temp,
			SDHOST_BLK_SIZE, sector);
			sector++;
			buff += sizeof(temp);
		}
	} else {
		sdHostStatus = sdhost_transfer_data(SDHOST_WRITE, (void*) buff,
		SDHOST_BLK_SIZE * count, sector);
	}

	if (sdHostStatus != SDHOST_OK) {
		res = RES_ERROR;
	}

	return res;
}
#endif

#if _USE_IOCTL
/** Disk IO Control
 *  @param pdrv Physical Drive Number
 *  @param cmd Control Code
 *  @param buff Buffer to send/receive control data
 *  @return Disk Status */
DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void* buff) {

	DRESULT res = RES_PARERR;
	sdhost_context_t* sd_context = sdhost_get_context();
	BYTE* csd;
	DWORD csize;
	BYTE n;

	switch (cmd) {
	case CTRL_SYNC:
		res = RES_OK;
		break;

	case GET_SECTOR_COUNT: /* Get number of sectors on the disk (WORD) */
		csd = (BYTE*) &sd_context->CSD[0];
		if ((csd[14] >> 6) == 1) // The last byte in the structure indicated the table version number
				{ /* SDv2? */
			// For a CSDv2 Table, the size of user memory is defined as (C_SIZE + 1) * 512 * 1024
			// So to number of (512b) sectors = (C_SIZE + 1) * 1024
			// See SD Physical Layer specification v4.10 page 123
			csize = csd[5] + ((WORD) csd[6] << 8) + 1;
			*(DWORD*) buff = (DWORD) csize << 10;

		} else { /* SDv1 or MMCv3 */
			// For a CSDv1 Table, the size of user memory is defined as Capacity = (C_SIZE + 1) * MULT * BLOCK_LEN
			// MULT = 0x0001 << (C_SIZE_MULT + 2) and BLOCK_LEN = 0x0001 << READ_BL_LEN
			// So to number of (512b) sectors = Capacity >> 9
			// See SD Physical Layer specification v4.10 page 118

			n = (csd[9] & 15) + ((csd[4] & 128) >> 7) + ((csd[5] & 3) << 1) + 2;
			csize = (csd[6] >> 6) + ((WORD) csd[7] << 2)
					+ ((WORD) (csd[8] & 3) << 10) + 1;
			*(DWORD*) buff = (DWORD) csize << (n - 9);
		}

		res = RES_OK;

		break;

	case GET_SECTOR_SIZE: /* Get sectors on the disk (WORD) */
		*(WORD*) buff = 512;
		res = RES_OK;
		break;

	case GET_BLOCK_SIZE: /* Get erase block size in unit of sectors (DWORD) */
		csd = (BYTE*) &sd_context->CSD[0];
		if (sd_context->isSDSCCard == false) { /* SDv2? */
			unsigned long __attribute__ ((aligned (32))) data[64 / 4];
			sdhost_get_card_status_reg(data);

			*(DWORD*) buff = 16UL << (*((uint8_t*) data + 10) >> 4);
			res = RES_OK;

		} else { /* SDv1 */

			WORD sec_size = (((csd[4] & 63) << 1) + ((WORD) (csd[3] & 128) >> 7)
					+ 1);
			// Find WRITE_BL_LEN in units of 512 Bytes
			uint8_t wbl_len_sec = ((csd[1] >> 6) | ((csd[2] & 0x3) << 2)) - 9;
			*(DWORD*) buff = sec_size << wbl_len_sec;

			res = RES_OK;

		}
		break;

	case MMC_GET_TYPE: /* Get card type flags (1 byte) */
		buff = &sd_context->isSDSCCard;
		res = RES_OK;
		break;

	case MMC_GET_CSD: /* Receive CSD as a data block (16 bytes) */
		csd = (BYTE*) &sd_context->CSD[0];
		for (int i = 0; i < 16; i++) {
			*((BYTE*) buff + i) = *(csd + i);
		}
		res = RES_OK;
		break;

	case MMC_GET_CID: /* Receive CID as a data block (16 bytes) */
		csd = (BYTE*) &sd_context->CID[0];
		for (int i = 0; i < 16; i++) {
			*((BYTE*) buff + i) = *(csd + i);
		}
		res = RES_OK;
		break;

	case MMC_GET_OCR: /* Receive OCR as an R3 resp (4 bytes) */
		csd = (BYTE*) &sd_context->OCR;
		for (int i = 0; i < 4; i++) {
			*((BYTE*) buff + i) = *(csd + i);
		}
		res = RES_OK;
		break;

	case MMC_GET_SDSTAT: /* Receive SD status as a data block (64 bytes) */
		if (sd_context->isSDSCCard == false) { /* SDv2? */
			uint32_t __attribute__ ((aligned (32))) data[64 / 4];
			sdhost_get_card_status_reg(data);
			for (int i = 0; i < 64; i++) {
				*((BYTE*) buff + i) = *((BYTE*) data + i);
			}
			res = RES_OK;
		}
		break;
	default:
		res = RES_PARERR;
	}

	return res;

}
#endif

#if _FS_READONLY == 0
/** Get the current time
 *  @return The time in the following format:
 *          bit[31:25] = Year from 1980 (0..127),
 *          bit[24:21] = Month (1..12),
 *          bit[20:16] = Day of the Month (1..31),
 *          bit[15:11] = Hour (0..23),
 *          bit[10:5]  = Minute (0..59),
 *          bit[4..0]  = Second / 2 (0..29) */
//DWORD get_fattime(void) {
//	return 0; /* Invalid timestamp */
//}
#endif
#endif //deffined(__FT900__) || defined(__FT930__)

