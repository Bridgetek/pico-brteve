/**
 * @file FileTransfer.c
 * @brief File transfer interface from host to flash
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

#include "Platform.h"
#include "EVE_CoCmd.h"

#include "FileIo.h"
#include "FileTransfer.h"
#include "Common.h"

 /// Alignment						    
#define __ALIGN_MASK(x,mask)  (((x)+(mask))&~(mask))
#define ALIGN(x,a)            __ALIGN_MASK(x, a - 1)

#define FREAD_BLOCK           (8 * 1024)

#define BLOBSIZE              4096

#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
#define EVE_FLASH_DIR         __FILE__ "\\..\\..\\..\\common\\eve_flash"
#else
#define EVE_FLASH_DIR         "/Test/common/eve_flash"
#endif

#if defined(BT815_ENABLE)
#define FILE_BLOB             (EVE_FLASH_DIR "\\BT815-unified.blob")
#elif defined(BT817_ENABLE)
#define FILE_BLOB             (EVE_FLASH_DIR "\\BT817-unified.blob")
#else
/// Use BT815 blob as default
#define FILE_BLOB             (EVE_FLASH_DIR "\\BT815-unified.blob")
#endif

#if defined(EVE_FLASH_AVAILABLE)
/**
 * @brief Erase flash
 *
 * @return int 1 on successful, 0 on error
 */
uint32_t Ftf_Flash_Erase(EVE_HalContext* phost) {
	// Try switch full mode
	FlashHelper_SwitchState(phost, FLASH_STATUS_FULL);
	if (FLASH_STATUS_FULL != EVE_Hal_rd8(phost, REG_FLASH_STATUS)) {
		// Try update blob and switch full mode again
		Ftf_Write_Blob_Default(phost);
		FlashHelper_SwitchState(phost, FLASH_STATUS_FULL);
		if (FLASH_STATUS_FULL != EVE_Hal_rd8(phost, REG_FLASH_STATUS)) {
			return 0;
		}
	}

	/* Erase the flash */
	EVE_CoCmd_flashErase(phost);
	EVE_Cmd_waitFlush(phost);

	return 1;
}

/**
 * @brief Clear flash cache
 *
 * @return int 1 on successful, 0 on error
 */
uint32_t Ftf_Flash_ClearCache(EVE_HalContext* phost) {
	EVE_CoCmd_dlStart(phost);
	EVE_CoCmd_swap(phost);
	EVE_Cmd_waitFlush(phost);

	EVE_CoCmd_dlStart(phost);
	EVE_CoCmd_swap(phost);
	EVE_Cmd_waitFlush(phost);

	EVE_CoCmd_clearCache(phost);
	EVE_Cmd_waitFlush(phost);
	return 1;
}

/**
 * @brief Write blob file to flash
 * When updating a flash image, the following steps are to be expected:
 *   - Reading the first 4096 bytes in flash image into RAM_G and update the blob first
 *       using cmd_flashupdate in Basic mode.
 *   - Send the command cmd_flashfast to enter into fast mode.
 *   - If success, program the rest data of flash image by cmd_flashupdate
 *   - If not success, write a default blob.
 *
 * @param blob data buffer
 * @return int 1 on successful, 0 on error
 */
uint32_t Ftf_Update_Blob(EVE_HalContext* phost, const char* pbuff) {
	printf("Updating blob\n");

	FlashHelper_SwitchState(phost, FLASH_STATUS_BASIC); // basic mode
	EVE_Hal_wrMem(phost, RAM_G, pbuff, BLOBSIZE);
	EVE_Cmd_waitFlush(phost);
	FlashHelper_Update(phost, 0, RAM_G, BLOBSIZE);
	EVE_Cmd_waitFlush(phost);

	int ret = FlashHelper_SwitchFullMode(phost);

	if (ret == 1) {
		printf("Blob updated successful\n");
		return 1;
	}
	else {
		printf("Failed to update Blob\n");
	}

	return 0;
}

/**
 * @brief Write blob file to flash
 *
 * @param blob file Blob file address
 * @return int 1 on successful, 0 on error
 */
uint32_t Ftf_Write_BlobFile(EVE_HalContext* phost, const char* blobfile) {
	char pBuff[BLOBSIZE];

	printf("Writing blob from file %s\n", blobfile);

	int ret = FileIO_File_To_Buffer(blobfile, pBuff, 0, BLOBSIZE, NULL);
	if (!ret) {
		printf("Unable to open file: %s\n", blobfile);
	}

	ret = Ftf_Update_Blob(phost, pBuff);

	/// fail to default Blob
	if (!ret) {
		return Ftf_Write_Blob_Default(phost);
	}

	return 1;
}

/**
 * @brief Write default blob file to flash
 *
 * @return int 1 on successful, 0 on error
 */
uint32_t Ftf_Write_Blob_Default(EVE_HalContext* phost) {
	char pBuff[BLOBSIZE];
	printf("Writing blob default\n");

	int ret = FileIO_File_To_Buffer(FILE_BLOB, pBuff, 0, BLOBSIZE, NULL);
	if (!ret) {
		printf("Unable to open file: %s\n", FILE_BLOB);
	}

	return Ftf_Update_Blob(phost, pBuff);
}

/**
 * @brief Close the file transfer progress
 *
 * @return Ftf_Progress_t*
 */
void Ftf_Progress_Close() {
	FileIO_File_Close();
}

/**
 * @brief File transfer progress bar initialization
 *
 * @param filePath Path to the file
 * @param fileName Filename on Progress bar
 * @param addr Address on flash
 * @param direction FTF_PROGESS_READ or FTF_PROGESS_WRITE
 * @return Ftf_Progress_t*
 */
Ftf_Progress_t* Ftf_Progress_Init(EVE_HalContext* phost, const char* filePath, const char* fileName, uint32_t addr, uint8_t direction) {
	static Ftf_Progress_t progress;
	uint32_t range = 0;
	int32_t fileSize = 0;

	progress.sent = 0;
	progress.addr = addr;
	progress.direction = direction;
#pragma warning(push)
#pragma warning(disable : 4996)
	strcpy(progress.file, filePath);
	strcpy(progress.fileName, fileName);
#pragma warning(pop)

	if (direction == FTF_PROGESS_READ) {
		fileSize = FileIO_File_Open(filePath, FILEIO_E_FOPEN_WRITE);
		if (0 >= fileSize) {
			printf("Unable to open file: %s\n", filePath);
			return 0;
		}
		snprintf(progress.message, MSG_SIZE, "Reading %s from flash", progress.fileName);
	}
	else {
		// update blob from file first
		if (addr == 0) {
			Ftf_Write_BlobFile(phost, filePath);
		}
		else {
			Ftf_Write_Blob_Default(phost);
		}

		fileSize = FileIO_File_Open(filePath, FILEIO_E_FOPEN_READ);

		if (0 >= fileSize) {
			printf("Unable to open file: %s\n", filePath);
			return 0;
		}

		// Jump to real data
		if (addr == 0) {
			progress.sent = progress.addr = BLOBSIZE;
			FileIO_File_Seek(BLOBSIZE);
		}

		snprintf(progress.message, MSG_SIZE, "Writing %s to flash", progress.fileName);
		progress.fileSize = fileSize;

	}

	/// Destination address in flash memory must be 4096-byte aligned
	progress.bytesPerPercent = ALIGN(fileSize / 100, 4096);

	if (progress.bytesPerPercent < FREAD_BLOCK) {
		progress.bytesPerPercent = FREAD_BLOCK;
	}

	return &progress;
}

/**
 * @brief Write a block data of file to flash
 *
 * @param progress Ftf_Progress_t struct
 * @return uint32_t Percent of data transfered, 100 mean file transfer is done
 */
uint32_t Ftf_Progress_Write_Next(EVE_HalContext* phost, Ftf_Progress_t* progress) {
	uint8_t pbuff[FREAD_BLOCK];
	uint32_t bytes;
	uint32_t sent = 0;
	uint32_t ramGSent = 0;
	uint32_t blockSize = 0;

	// Tranfer 1 percent of file
	while (progress->sent < progress->fileSize && sent < progress->bytesPerPercent) {
		blockSize = FREAD_BLOCK > progress->bytesPerPercent ? progress->bytesPerPercent : FREAD_BLOCK;

		// Tranfer to ram_g
		while (ramGSent < RAM_G_SIZE && progress->sent < progress->fileSize && sent < progress->bytesPerPercent) {
			bytes = FileIO_File_Read(pbuff, blockSize);

			if (0 == bytes) {
				printf("Error on reading file: %s\n", progress->file);
				return 0;
			}
			EVE_Hal_wrMem(phost, ramGSent, pbuff, bytes);
			EVE_Cmd_waitFlush(phost);
			ramGSent += bytes;
			sent += bytes;
			progress->sent += bytes;
		}

		// Update flash from ram_g
		ramGSent = (ramGSent + 4095) & (~4095);//to ensure 4KB alignment
		FlashHelper_Update(phost, progress->addr, 0, ramGSent);
		progress->addr += ramGSent;
	}

	return progress->sent * 100 / progress->fileSize; /* Percent */
}

/**
 * @brief Read a block data from flash to file
 *
 * @param progress Ftf_Progress_t struct
 * @return uint32_t Percent of data received, 100 mean data transfer is done
 */
uint32_t Ftf_Progress_Read_Next(EVE_HalContext* phost, Ftf_Progress_t* progress) {
	const uint32_t bufferSize = FREAD_BLOCK;
	uint8_t pbuff[FREAD_BLOCK];
	uint32_t sent = 0;
	uint32_t blockSize = 0;
	uint32_t gramaddr = RAM_G;
	uint32_t fileWriteBlock = 0;

	while (progress->sent < progress->fileSize && sent < progress->bytesPerPercent) {
		fileWriteBlock = bufferSize > progress->bytesPerPercent ? progress->bytesPerPercent : bufferSize;

		if ((progress->fileSize - progress->sent) < fileWriteBlock) {
			fileWriteBlock = progress->fileSize - progress->sent;
		}

		// source address in flash memory Must be 64-byte aligned. 
		blockSize = ALIGN(fileWriteBlock, 64);

		uint32_t fret = FlashHelper_Read(phost, gramaddr, progress->addr, blockSize, pbuff);

		if (FLASH_CMD_SUCCESS != fret) {
			printf("Error when reading flash\n");
			return 0;
		}

		sent += blockSize;
		progress->sent += blockSize;
		progress->addr += blockSize;
		if (0 == FileIO_File_Write(pbuff, blockSize)) {
			printf("Unable to write file: %s\n", progress->file);
			return 0;
		}
	}
	return progress->sent * 100 / progress->fileSize; /* Percent */
}

/**
 * @brief Default UI for the progress bar
 * User may construct their own UI for the progress bar with Ftf_Progress_Init and Ftf_Progress_Write_Next/Ftf_Progress_Read_Next
 *
 * @param progress Ftf_Progress_t struct
 * @return uint32_t 1 on successful, 0 on error
 */
uint32_t Ftf_Progress_Ui(EVE_HalContext* phost, const Ftf_Progress_t* progress) {
	char s[100];
	uint16_t x;
	uint16_t y;
	uint16_t w;
	uint16_t h;
	uint16_t opt;
	uint16_t font = 29;
	uint16_t val;
	const uint32_t range = 1000;
	uint64_t sent64 = progress->sent;

	if (progress->fileSize == 0) {
		return 1;
	}
	opt = 0;
	val = (uint16_t)(sent64 * 1000 / progress->fileSize);

	w = (uint16_t)(DispWidth * 8 / 10);
	h = (uint16_t)(DispHeight * 1 / 10);
	x = (uint16_t)((DispWidth - w) / 2);
	y = (uint16_t)((DispHeight - h) / 2);

	EVE_CoCmd_dlStart(phost);
	EVE_Cmd_wr32(phost, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(phost, CLEAR_COLOR_RGB(0, 0, 0));

	EVE_CoCmd_text(phost, x, y - 50, font, opt, progress->message);
	EVE_CoCmd_progress(phost, x, y, w, h, opt, val, range);

	snprintf(s, 100, "%u %%", val * 100 / range);
	EVE_Cmd_wr32(phost, COLOR_RGB(0, 200, 0));
	EVE_CoCmd_text(phost, x + w / 2, y + 5, font, opt, s);
	EVE_Cmd_wr32(phost, COLOR_RGB(255, 255, 255));

	EVE_Cmd_wr32(phost, DISPLAY());
	EVE_CoCmd_swap(phost);
	EVE_Cmd_waitFlush(phost);

	return 1;
}

/**
 * @brief Write file to flash and show default progress bar on LCD
 *
 * @param filePath File to transfer
 * @param fileName File name on the progress bar
 * @param address Address on flash
 * @return uint32_t Number of bytes transfered
 */
uint32_t Ftf_Write_File_To_Flash_With_Progressbar(EVE_HalContext* phost, const char* filePath, const char* fileName, uint32_t address) {
	Ftf_Progress_t* progress = Ftf_Progress_Init(phost, filePath, fileName, address, FTF_PROGESS_WRITE);

	if (!progress) {
		return -1; /// error
	}
	while (1) {
		uint32_t pc = Ftf_Progress_Write_Next(phost, progress);
		Ftf_Progress_Ui(phost, progress);

		if (pc >= 100) {
			break;
		}
	}
	Ftf_Progress_Close();

	return progress->fileSize;
}

/**
 * @brief Read file from flash and show default progress bar on LCD
 *
 * @param filePath File to write
 * @param fileName File name on the progress bar
 * @param address Address on flash
 * @return uint32_t Number of bytes received
 */
uint32_t Ftf_Read_File_From_Flash_With_Progressbar(EVE_HalContext* phost, uint8_t* filePath, const char* fileName,
	uint32_t address, uint32_t size) {
	Ftf_Progress_t* progress = Ftf_Progress_Init(phost, filePath, fileName, address, FTF_PROGESS_READ);
	progress->fileSize = size;
	while (1) {
		uint32_t pc = Ftf_Progress_Read_Next(phost, progress);
		Ftf_Progress_Ui(phost, progress);

		if (pc >= 100) {
			break;
		}
	}
	Ftf_Progress_Close();
	return progress->fileSize;
}

/**
 * @brief Write file to flash via CMD_FLASHWRITE
 *
 * @param fileName File to write
 * @param addr Address on flash
 * @param isErase false if Flash is already erased previously with cmd_flasherase
 * @return int Number of bytes transfered on successful, 0 on error
 */
uint32_t Ftf_Write_File_To_Flash_By_Cmd_Fifo(EVE_HalContext* phost, const char* fileName, uint32_t addr, int isErase) {
	char pBuff[EVE_CMD_FIFO_SIZE];
	int32_t fileSize = 0;
	int32_t blocklen = 0;

	//Erase Flash
	if (isErase && !Ftf_Flash_Erase(phost)) {
		return 0;
	}

	// Check flash status
	FlashHelper_SwitchState(phost, FLASH_STATUS_FULL);
	if (FLASH_STATUS_FULL != EVE_Hal_rd8(phost, REG_FLASH_STATUS)) {
		return 0;
	}

	fileSize = FileIO_File_Open(fileName, FILEIO_E_FOPEN_READ);

	if (0 >= fileSize) {
		printf("Unable to open file: %s\n", fileName);
		return 0;
	}

	EVE_Cmd_wr32(phost, CMD_FLASHWRITE);
	EVE_Cmd_wr32(phost, addr);
	EVE_Cmd_wr32(phost, ALIGN(fileSize, FLASH_WRITE_ALIGN_BYTE));

	/// Download data to Coprocessor
	blocklen = FileIO_File_Read(pBuff, EVE_CMD_FIFO_SIZE);
	while (blocklen > 0) {
		int bytes = ALIGN(blocklen, FLASH_WRITE_ALIGN_BYTE);
		EVE_Cmd_wrMem(phost, pBuff, bytes);
		blocklen = FileIO_File_Read(pBuff, EVE_CMD_FIFO_SIZE);
	}
	FileIO_File_Close();

	return fileSize; /* File size */
}

/**
 * @brief Erase the flash and transfer a list of files into flash with CMD_FLASHWRITE
 *
 * @param file File name array
 * @param addr Address on flash
 * @return int Number of bytes transfered, 0 on error
 */
uint32_t Ftf_Write_FileArr_To_Flash_By_Cmd_Fifo(EVE_HalContext* phost, const char* file[], uint32_t addr) {
	int i = 0;
	uint32_t allSize = 0;
	uint32_t sent = 0;
	char pBuff[EVE_CMD_FIFO_SIZE];
	int32_t fileSize = 0;
	uint32_t blocklen = 0;

	// Erase Flash
	if (!Ftf_Flash_Erase(phost)) {
		exit(0);
	}

	// Check flash status
	FlashHelper_SwitchState(phost, FLASH_STATUS_FULL);
	if (FLASH_STATUS_FULL != EVE_Hal_rd8(phost, REG_FLASH_STATUS)) {
		return 0;
	}

	i = 0;
	while (file[i] != NULL) {
		allSize += FileIO_File_Open(file[i], FILEIO_E_FOPEN_READ);
		FileIO_File_Close();
		i++;
	}

	EVE_Cmd_wr32(phost, CMD_FLASHWRITE);
	EVE_Cmd_wr32(phost, addr);
	EVE_Cmd_wr32(phost, ALIGN(allSize, FLASH_WRITE_ALIGN_BYTE));

	i = 0;
	while (file[i] != NULL) {
		fileSize = FileIO_File_Open(file[i], FILEIO_E_FOPEN_READ);

		if (0 >= fileSize) {
			printf("Unable to open file: %s\n", file[i]);
			return sent;
		}

		/// Download data to Coprocessor
		do {
			blocklen = FileIO_File_Read(pBuff, EVE_CMD_FIFO_SIZE);
			EVE_Cmd_wrMem(phost, pBuff, blocklen);
			sent += blocklen;
		} while (blocklen > 0);

		FileIO_File_Close();

		i++;
	}

	int remain = ALIGN(allSize, FLASH_WRITE_ALIGN_BYTE) - sent;
	if (remain) {
		memset(pBuff, 0, sizeof(pBuff));
		EVE_Cmd_wrMem(phost, pBuff, remain);
	}

	return allSize;
}

/**
 * @brief Write file to flash via RAM_G
 *
 * @param fileName File to write
 * @param addr Address on flash
 * @return int Number of bytes transfered on successful, 0 on error
 */
uint32_t Ftf_Write_File_To_Flash_By_RAM_G(EVE_HalContext* phost, const char* fileName, uint32_t addr) {
	const uint32_t bufferSize = FREAD_BLOCK;
	uint8_t pbuff[FREAD_BLOCK];
	uint32_t bytes;
	uint32_t sent = 0;
	uint32_t ramGSent = 0;

	// update blob from file first
	if (addr < BLOBSIZE) {
		Ftf_Write_BlobFile(phost, fileName);
	}
	else {
		/// check and write blob
		int ret = FlashHelper_SwitchState(phost, FLASH_STATUS_FULL); // full mode
		if (ret != 0) {
			Ftf_Write_Blob_Default(phost);

			ret = FlashHelper_SwitchState(phost, FLASH_STATUS_FULL); // full mode
			if (ret != 0) {
				printf("Cannot switch flash to fullmode\n");
				return 0;
			}
		}
	}

	uint32_t fileSize = FileIO_File_Open(fileName, FILEIO_E_FOPEN_READ);

	if (0 >= fileSize) {
		printf("Unable to open file: %s\n", fileName);
		return 0;
	}

	// Ignore Blob data part of file
	if (addr < BLOBSIZE) {
		sent = addr = BLOBSIZE;
		FileIO_File_Seek(addr);
	}

	/// Transfer rest of file to EVE (in fast mode)
	while (sent < fileSize) {
		// Fill up RAM_G
		while (ramGSent < RAM_G_SIZE && sent < fileSize) {
			bytes = FileIO_File_Read(pbuff, bufferSize);
			if (0 == bytes) {
				printf("Error on reading file: %s\n", fileName);
				return 0;
			}
			EVE_Hal_wrMem(phost, ramGSent, pbuff, bytes);

			ramGSent += bytes;
			sent += bytes;
		}

		// Send RAM_G to Flash
		ramGSent = (ramGSent + 4095) & (~4095);//to ensure 4KB alignment
		FlashHelper_Update(phost, addr, 0, ramGSent);
		addr += ramGSent;
	}
	FileIO_File_Close();

	return sent; /* File size */
}

/**
 * @brief Transfer a list of files into flash with CMD_UPDATE
 *
 * @param file File name array
 * @param addr Address on flash
 * @return int Number of bytes transfered, 0 on error
 */
uint32_t Ftf_Write_FileArr_To_Flash_By_RAM_G(EVE_HalContext* phost, const char* file[], uint32_t addr) {
	int i = 0;
	uint32_t bytes = 0;
	uint32_t sent = 0;
	while (file[i] != NULL) {
		bytes = Ftf_Write_File_To_Flash_By_RAM_G(phost, file[i], addr);
		if (!bytes) {
			printf("Error when write file %s to RAM_G", file[i]);
			return 0;
		}
		addr += bytes;
		sent += bytes;
		i++;
	}

	return sent;
}

/**
 * @brief Read data on RAM_G into a file
 *
 * @param output Filename output
 * @param startAddress Address on RAM_G
 * @param size Size to read
 * @return uint32_t 1 on successful, 0 on error
 */
uint32_t Ftf_Read_File_From_Flash(EVE_HalContext* phost, const uint8_t* output, uint32_t address, uint32_t size) {
	const uint32_t gramaddr = 0;
	const uint32_t blockSize = 4 * 1024; /// blockSize must be less than RAM_G size
	uint8_t* buff = (char*)malloc(4 * 1024);
	address = ALIGN(address, 64);
	size = ALIGN(size, 4);
	if (!buff) {
		printf("Unable to malloc\n");
		return 0;
	}

	if (-1 == FileIO_File_Open(output, FILEIO_E_FOPEN_WRITE)) {
		printf("Unable to open file: %s\n", output);
		free(buff);
		return 0;
	}

	uint32_t sent = 0;
	while (size > 0) {
		memset(buff, 0, 4 * 1024);
		uint32_t bytes = blockSize > size ? size : blockSize;
		FlashHelper_Read(phost, gramaddr, address, bytes, buff);
		size -= bytes;
		sent += bytes;
		address += bytes;

		if (0 == FileIO_File_Write(buff, bytes)) {
			printf("Unable to write file: %s\n", output);
			free(buff);
			return 0;
		}
	}

	FileIO_File_Close();
	free(buff);

	return 1;
}

/**
 * @brief Get flash size
 *
 * @return uint32_t Flash size in Mb
 */
uint32_t Ftf_Flash_Get_Size(EVE_HalContext* phost) {
	FlashHelper_SwitchState(phost, FLASH_STATUS_BASIC);
	int32_t size = EVE_Hal_rd32(phost, REG_FLASH_SIZE);

	return size;
}

#endif
/**
 * @brief Transfer a file to RAM_G
 *
 * @param file File to transfer
 * @param addr Address on RAM_G
 * @return uint32_t Number of bytes transfered on successful, 0 on error
 */
uint32_t Ftf_Write_File_nBytes_To_RAM_G(EVE_HalContext* phost, const char* file, uint32_t addr, int nbytes, int offset) {
	const uint32_t BufferSize = FREAD_BLOCK;
	int32_t bytes;
	int32_t sent = 0;
	int32_t fileSize = 0;
	uint8_t pbuff[FREAD_BLOCK];

	fileSize = FileIO_File_Open(file, FILEIO_E_FOPEN_READ);
	if (nbytes == 0){
		nbytes = fileSize;
	}

	if (offset) {
		FileIO_File_Seek(offset);
	}

	if (0 >= fileSize) {
		printf("Unable to open file: %s\n", file);
		return 0;
	}
	while (fileSize > 0 && sent < nbytes) {
		bytes = FileIO_File_Read(pbuff, BufferSize);
		if (bytes == 0) {
			printf("Error on f_read\n");
			break;
		}
		if ((sent + bytes) > nbytes) {
			bytes = nbytes - sent;
		}
		EVE_Hal_wrMem(phost, addr, pbuff, bytes);
		fileSize -= bytes;
		sent += bytes;
		addr += bytes;
	}

	FileIO_File_Close();

	return sent;
}

/**
 * @brief Transfer a file to RAM_G
 *
 * @param file File to transfer
 * @param addr Address on RAM_G
 * @return uint32_t Number of bytes transfered on successful, 0 on error
 */
uint32_t Ftf_Write_File_To_RAM_G(EVE_HalContext* phost, const char* file, uint32_t addr) {
	return Ftf_Write_File_nBytes_To_RAM_G(phost, file, addr, 0, 0);
}

/**
 * @brief Transfer a file list into RAM_G
 *
 * @param file File name array
 * @param addr Address on RAM_G
 * @return uint32_t Number of bytes transfered on successful, 0 on error
 */
uint32_t Ftf_Write_FileArr_To_RAM_G(EVE_HalContext* phost, char* file[], uint32_t addr) {
	int i = 0;
	uint32_t bytes;
	uint32_t sent = 0;
	while (file[i] != NULL) {
		bytes = Ftf_Write_File_To_RAM_G(phost, file[i], addr);
		if (0 == bytes) {
			printf("Error when write file: %s\n", file[i]);
			return 0;
		}

		sent += bytes;
		i++;
	}

	return sent;
}

/**
 * @brief Read data on RAM_G into a file
 *
 * @param output Filename output
 * @param startAddress Address on RAM_G
 * @param size Size to read
 * @return uint32_t 1 on successful, 0 on error
 */
uint32_t Ftf_Read_File_From_RAM_G(EVE_HalContext* phost, const uint8_t* output, uint32_t startAddress, uint32_t size) {
	if (-1 == FileIO_File_Open(output, FILEIO_E_FOPEN_WRITE)) {
		printf("Unable to open file: %s\n", output);
		return 0;
	}

	uint8_t pbuff[FREAD_BLOCK];
	uint32_t block = 0;
	uint32_t offset = 0;

	while (size > 0) {
		block = size > FREAD_BLOCK ? FREAD_BLOCK : size;
		EVE_Hal_rdMem(phost, pbuff, startAddress + offset, block);
		offset += block;
		size -= block;

		if (0 == FileIO_File_Write(pbuff, block)) {
			printf("Unable to write file: %s\n", output);
			return 0;
		}

	}
	FileIO_File_Close();

	return 1;
}
