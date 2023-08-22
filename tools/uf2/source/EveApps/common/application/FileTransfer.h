/**
 * @file FileTransfer.h
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

#ifndef FILETRANSFER_H_
#define FILETRANSFER_H_
#include <stdint.h>

#define FTF_PROGESS_READ  1
#define FTF_PROGESS_WRITE 2
#define MSG_SIZE          200

typedef struct Ftf_Progress {
#if defined(BT8XXEMU_PLATFORM) || defined(MSVC_PLATFORM)
	char file[200];
	char fileName[200];
	char message[MSG_SIZE];
#else
	char file[300];
	char fileName[100];
	char message[200];
#endif
	uint32_t fileSize;
	uint32_t sent;
	uint32_t bytesPerPercent;
	uint32_t addr;
	uint8_t direction;
}Ftf_Progress_t;

#if defined(EVE_FLASH_AVAILABLE)
uint32_t Ftf_Flash_Erase(EVE_HalContext *phost);
uint32_t Ftf_Flash_ClearCache(EVE_HalContext* phost);
uint32_t Ftf_Write_BlobFile(EVE_HalContext *phost, const char* blobfile);
uint32_t Ftf_Write_Blob_Default(EVE_HalContext* phost);

void Ftf_Progress_Close();
Ftf_Progress_t* Ftf_Progress_Init(EVE_HalContext *phost, const char *filePath, const char *fileName, uint32_t addr, uint8_t direction);
uint32_t Ftf_Progress_Write_Next(EVE_HalContext *phost, Ftf_Progress_t *progress);
uint32_t Ftf_Progress_Read_Next(EVE_HalContext *phost, Ftf_Progress_t *progress);
uint32_t Ftf_Progress_Ui(EVE_HalContext *phost, const Ftf_Progress_t *progress);
uint32_t Ftf_Write_File_To_Flash_With_Progressbar(EVE_HalContext *phost, const char *filePath, const char *fileName, uint32_t address);
uint32_t Ftf_Read_File_From_Flash_With_Progressbar(EVE_HalContext *phost, uint8_t *filePath, const char *fileName,
		uint32_t address, uint32_t size);

uint32_t Ftf_Write_File_To_Flash_By_Cmd_Fifo(EVE_HalContext* phost, const char* fileName, uint32_t addr, int isErase);
uint32_t Ftf_Write_FileArr_To_Flash_By_Cmd_Fifo(EVE_HalContext* phost, const char* file[], uint32_t addr);

uint32_t Ftf_Write_File_To_Flash_By_RAM_G(EVE_HalContext *phost, const char *fileName, uint32_t addr);
uint32_t Ftf_Write_FileArr_To_Flash_By_RAM_G(EVE_HalContext *phost, const char *file[], uint32_t addr);

uint32_t Ftf_Read_File_From_Flash(EVE_HalContext *phost, const uint8_t *output, uint32_t address, uint32_t size);

uint32_t Ftf_Flash_Get_Size(EVE_HalContext *phost);
#endif

uint32_t Ftf_Write_File_nBytes_To_RAM_G(EVE_HalContext *phost, const char *file, uint32_t addr, int nbytes, int offset);
uint32_t Ftf_Write_File_To_RAM_G(EVE_HalContext *phost, const char *file, uint32_t addr);
uint32_t Ftf_Write_FileArr_To_RAM_G(EVE_HalContext *phost, char *file[], uint32_t addr);
uint32_t Ftf_Read_File_From_RAM_G(EVE_HalContext *phost, const uint8_t *output, uint32_t startAddress, uint32_t size);

#endif /* FILETRANSFER_H_ */

