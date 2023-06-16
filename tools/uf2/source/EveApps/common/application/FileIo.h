/**
 * @file FileIo.h
 * @brief File read-write library for Eve application, support a unique interface for every platform
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

#ifndef FILEIO_H_
#define FILEIO_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum _FILEIO_E_FOPEN {
	FILEIO_E_FOPEN_READ, FILEIO_E_FOPEN_WRITE, FILEIO_E_FOPEN_APPEND
};
enum _FILEIO_E_FRESULT{
	FILEIO_E_FRESULT_OK, FILEIO_E_FRESULT_FAIL, FILEIO_E_FRESULT_EOF
};
#define FileIO_Append(M, ...)                       \
{                                                   \
	int num = sprintf(msg, M, ##__VA_ARGS__);       \
	FileIO_File_Write(msg, num);                    \
}

int FileIO_File_Close();
int FileIO_File_Seek(unsigned long offset);
int FileIO_File_Tell();
int FileIO_File_Open(const char *filePath, enum _FILEIO_E_FOPEN e);
int FileIO_File_Read(char* buffer, long bytes);
int FileIO_File_Write(const char* buffer, long buffersize);
int FileIO_File_List(char* path);

int FileIO_File_To_Buffer(const char *file, char *buff, long offset, int size,
		int *ByteCount);
void FileIO_Buffer_To_File(const char *txt, const char *buffer, unsigned long buffersize);
char *FileIO_Read_Line();

#endif /* FILEIO_H_ */

