/**
 * @file FileIo.c
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

// POSIX	FatFs                                           | App
// "r"	    FA_READ                                         | FILEIO_E_FOPEN_READ
// "r+"	    FA_READ          | FA_WRITE                     |
// "w"	    FA_CREATE_ALWAYS | FA_WRITE                     | FILEIO_E_FOPEN_WRITE
// "w+"	    FA_CREATE_ALWAYS | FA_WRITE | FA_READ           |
// "a"	    FA_OPEN_APPEND   | FA_WRITE                     | FILEIO_E_FOPEN_APPEND
// "a+"	    FA_OPEN_APPEND   | FA_WRITE | FA_READ           |
// "wx"	    FA_CREATE_NEW    | FA_WRITE                     |
// "w+x"	FA_CREATE_NEW    | FA_WRITE | FA_READ           |

#include "stdio.h"
#include "errno.h"

#include "Platform.h"
#include "FileIo.h"

static long int curPos = 0;
static long int filesz = 0;
static int isFOpenning = 0;

#if defined (FT900_PLATFORM) || defined (FT93X_PLATFORM) || defined (RP2040_PLATFORM)
#include "ff.h"
FIL fp;
#else
FILE *fp;
#endif

#if defined (FT900_PLATFORM) || defined (FT93X_PLATFORM) || defined (RP2040_PLATFORM)

/**
 * @brief Scan files in a folder
 * 
 * @param path Patd to folder
 * @return FRESULT 
 */
static FRESULT scan_files(char* path ) {
	FRESULT res;
	DIR dir;
	unsigned int i;
	static FILINFO fno;

	res = f_opendir(&dir, path); /* Open the directory */
	if (res == FR_OK) {
		for (;;) {
			res = f_readdir(&dir, &fno); /* Read a directory item */
			if (res != FR_OK || fno.fname[0] == 0)
				break; /* Break on error or end of dir */
			if (fno.fattrib & AM_DIR) { /* It is a directory */
				i = strlen(path);
				sprintf(&path[i], "/%s", fno.fname);
				res = scan_files(path); /* Enter the directory */
				if (res != FR_OK)
					break;
				path[i] = 0;
			} else { /* It is a file. */
				printf("%s/%s\n", path, fno.fname);
			}
		}
		f_closedir(&dir);
	}else{
		printf("Error on open %s, res = %d", path, res);
	}

	return res;
}

/**
 * @brief File close
 * 
 * @return int 1 on successful
 */
int FileIO_File_Close() {
	if (isFOpenning) {
		f_close(&fp);
		memset(&fp, 0, sizeof(FIL));
		curPos = 0;
		filesz = 0;
		isFOpenning = 0;
		printf("Closed file\n");
	}
	return 1;
}
/**
 * @brief File seek
 * 
 * @param offset Position to seek
 * @return int 1 on successful
 */
int FileIO_File_Seek(unsigned long offset) {
	FRESULT fResult;
	if (!isFOpenning) {
		printf("File haven't openned\n");
		return 0;
	}
	fResult = f_lseek(&fp, offset);

	if (fResult != FR_OK) {
		printf("Seek error, error: %d\n", fResult);
		return 0;
	}
	return 1;
}

/**
 * @brief Querry file pointer
 * 
 * @return int file pointer possition
 */
int FileIO_File_Tell() {
	if (!isFOpenning) {
		printf("File haven't openned\n");
		return 0;
	}
	return f_tell (&fp);
}

/**
 * @brief File open
 * 
 * @param filePath File to open
 * @param e Open mode: FILEIO_E_FOPEN_READ | FILEIO_E_FOPEN_WRITE | FILEIO_E_FOPEN_APPEND
 * @return int 1 on successful, -1 on error
 */
int FileIO_File_Open(const char *filePath, enum _FILEIO_E_FOPEN e) {
	FRESULT fResult;
	BYTE mode;

	switch (e) {
	case FILEIO_E_FOPEN_READ:
		mode = FA_READ | FA_OPEN_EXISTING;
		break;
	case FILEIO_E_FOPEN_WRITE:
		mode = FA_CREATE_ALWAYS | FA_WRITE;
		break;
	case FILEIO_E_FOPEN_APPEND:
		mode = FA_WRITE;
		break;
	default:
		printf("File open mode is not recognized\n");
		return -1;
	}
	fResult = f_open(&fp, filePath, mode);
	if (fResult != FR_OK) {
		printf("Cannot open %s, please check SD card, error: %d\n", filePath,
				fResult);
		FileIO_File_Close();
		return -1;
	}

	filesz = f_size(&fp);

	isFOpenning = 1;
	curPos = 0;

	if (mode == FILEIO_E_FOPEN_APPEND) {
		FileIO_File_Seek(filesz);
	}

	printf("Opened file %s, file size: %d bytes\n", filePath, filesz);

	return filesz;
}

/**
 * @brief File read
 * 
 * @param buffer Buffer to get data
 * @param bytes Number of bytes to read
 * @return int 0 on reach EoF or failure
 */
int FileIO_File_Read(char* buffer, long bytes) {
	unsigned int bytesread = 0;
	unsigned int bytescount = 0;
	unsigned int bytescounts = 0;
	int chunk_size = 1024;
	FRESULT fResult;

	if (!isFOpenning) {
		printf("File haven't opened\n");
		return 0;
	}
	if (curPos >= filesz) { // reached EOF, close the file
		printf("Reached EOF\n");
		return 0;
	}

	while (bytescounts < bytes) {
		chunk_size = bytes > chunk_size ? chunk_size : bytes;
		bytesread = (filesz - curPos) > chunk_size ? chunk_size : (filesz - curPos);

		fResult = f_read(&fp, &buffer[bytescounts], bytesread, &bytescount);
		if (fResult != FR_OK) {
			printf("Error on f_read\n");
			return 0;
		}
		bytescounts += bytescount;

		if (bytesread != bytescount) {
			printf("Reached EOF, stop\n");
			break;
		}

		curPos += bytescount;
		if (curPos >= filesz) { // reached EOF, break and return bytescounts
			printf("Reached EOF\n");
			FileIO_File_Close();
			return bytescounts;
		}
	}

	return bytescounts;
}

/**
 * @brief File write
 * 
 * @param buffer Data buffer to write
 * @param buffersize Size of the buffer
 * @return int 1 on successful
 */
int FileIO_File_Write(const char* buffer, long buffersize) {
	FRESULT fResult;

	unsigned int written = 0;

	if (!isFOpenning) {
		printf("File haven't openned\n");
		return 0;
	}

	fResult = f_write(&fp, buffer, buffersize, &written);

	if (fResult != FR_OK) {
		printf("Error on f_write\n");
		return 0;
	}
	if (buffersize != written) {
		printf("Writting error\n");
		FileIO_File_Close();
		return 0;
	}

	return buffersize;
}

/**
 * @brief List file in a folder
 * 
 * @param path Path to folder
 * @return int 1 on successful
 */
int FileIO_File_List(char* path) {
	scan_files(path);
	return 1;
}

/**
 * @brief Read a line
 * 
 * @return char* on successful, 0 on failure
 */
char *FileIO_Read_Line(){
    static char buffer[1000];

    if(buffer == f_gets(buffer, 255, &fp)){ // When the function succeeded, buff will be returuned.
        return buffer;
    }else{
        printf("Reached Eof\n");
        FileIO_File_Close();
        return 0;
    }
}
#elif defined (MSVC_PLATFORM) || defined (BT8XXEMU_PLATFORM)

/**
 * @brief File close
 * 
 * @return int 1 on successful
 */
int FileIO_File_Close() {
	if (isFOpenning) {
		fclose(fp);
		fp = NULL;
		curPos = 0;
		filesz = 0;
		isFOpenning = 0;
	}
	return 0;
}

/**
 * @brief File seek
 * 
 * @param offset Position to seek
 * @return int 1 on successful
 */
int FileIO_File_Seek(unsigned long offset) {
	int ret = 0;

	printf("Seeking to %ld\n", offset);

	if (!isFOpenning) {
		printf("File haven't openned\n");
		return 0;
	}

	ret = fseek(fp, offset, SEEK_SET);

	if (ret) {
		printf("Seek to %lu error, error: %d\n", offset, ret);
		return 0;
	}
	return 1;
}

/**
 * @brief Querry file pointer
 * 
 * @return int file pointer possition
 */
int FileIO_File_Tell() {
	if (!isFOpenning) {
		printf("File haven't openned\n");
		return 0;
	}
	return ftell (fp);
}

/**
 * @brief File open
 * Please call EVE_Util_loadSdCard to use on FT9XX platform
 * 
 * @param filePath File to open
 * @param e Open mode: FILEIO_E_FOPEN_READ | FILEIO_E_FOPEN_WRITE | FILEIO_E_FOPEN_APPEND
 * @return int File size (read mode), 1 (write mode), 0 on error
 */
int FileIO_File_Open(const char *filePath, enum _FILEIO_E_FOPEN e){
	char mode[3];
	mode[1] = 'b';
	mode[2] = 0;
	
	FileIO_File_Close();

	switch (e) {
	case FILEIO_E_FOPEN_READ:
		mode[0] = 'r';
		break;
	case FILEIO_E_FOPEN_WRITE:
		mode[0] = 'w';
		break;
	case FILEIO_E_FOPEN_APPEND:
		mode[0] = 'a';
		break;
	default:
		printf("File open mode is not recognized\n");
		return 0;
	}

#pragma warning(push)
#pragma warning(disable : 4996)
	fp = fopen(filePath, mode);
#pragma warning(pop)
	if (!fp) {
		printf("Cannot open %s, please check SD card, error: %d\n", filePath, errno);
		return 0;
	}

	fseek(fp, 0, SEEK_END);
	filesz = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	isFOpenning = 1;

	printf("Opened %s, file size: %ld\n", filePath, filesz);

	if (FILEIO_E_FOPEN_READ == e) {
		return filesz;
	}
	else {
		return 1;
	}
}

/**
 * @brief File read
 * 
 * @param buffer Buffer to get data
 * @param size Number of bytes to read
 * @return int Number of bytes read
 */
int FileIO_File_Read(char* buffer, long size) {
	const int chunkSize = 1024;
	int blockSize = 0;
	int byteRead = 0;
	int offset = 0;

	if (!fp || !isFOpenning) {
		printf("File is not opening\n");
		return 0;
	}

	if (curPos >= filesz) { // reached EOF, close the file
		printf("Reached EOF\n");
		return 0;
	}
	
	while (size > 0) {
		blockSize = chunkSize > size ? size : chunkSize;

		byteRead = (int)fread(&buffer[offset], 1, blockSize, fp);
		curPos += byteRead;
		offset += byteRead;
		size -= byteRead;

		if (!feof(fp) && blockSize != byteRead) {
			printf("Error on f_read\n");
			return 0;
		}

		/// Reached EOF, stop
		if (feof(fp) || curPos >= filesz) {
			return offset;
		}
	}
	return offset;
}

/**
 * @brief File write
 * 
 * @param buffer Data buffer to write
 * @param buffersize Size of the buffer
 * @return int 1 on successful
 */
int FileIO_File_Write(const char* buffer, long buffersize) {
	int written = 0;

	if (!isFOpenning) {
		printf("File haven't openned\n");
		return 0;
	}

	written = (int)fwrite(buffer, 1, buffersize, fp);

	if (written != buffersize) {
		printf("Writting error: written=%u, Expected=%ld, errno=%d\n", written, buffersize, errno); 
		FileIO_File_Close();
		return 0;
	}

	return written;
}

int FileIO_File_List(char* path) {
	return 1;
}

/**
 * @brief Read a line
 * 
 * @return char* on successful, 0 on failure
 */
char *FileIO_Read_Line(){
    static char buffer[1000];

    if(fgets(buffer, 255, fp)){
        return buffer;
    }else{
        printf("Reached Eof\n");
        FileIO_File_Close();
        return 0;
    }
}
#else // defined (MSVC_PLATFORM) || defined (BT8XXEMU_PLATFORM)
int FileIO_File_Close() { return 0; };
int FileIO_File_Seek(unsigned long offset) { return 0; };
int FileIO_File_Tell() { return 0; };
int FileIO_File_Open(const char* filePath, enum _FILEIO_E_FOPEN e) { return 0; };
int FileIO_File_Read(char* buffer, long bytes) { return 0; };
int FileIO_File_Write(const char* buffer, long buffersize) { return 0; };
int FileIO_File_List(char* path) { return 0; };
char* FileIO_Read_Line() { return 0; };
#endif // No platform supported

/**
 * @brief Read a whole file to a buffer
 * 
 * @param txt File to read
 * @param buff Result buffer
 * @param offset Offset where to read
 * @param size Number of byte to read
 * @param byteCount Result of number of byte read
 * @return int 1 on successful
 */
int FileIO_File_To_Buffer(const char *file, char *buff, long offset, int size, int *byteCount) {
	int fileSize = 0;
	int numRead = 0;

	fileSize = FileIO_File_Open(file, FILEIO_E_FOPEN_READ);
	numRead = fileSize - offset;

	if (offset >= fileSize) {
		return 0;
	}

	if (0 >= fileSize) {
		return 0;
	}

	if (size > 0 && size < numRead) {
		numRead = size;
	}

	// seek file pointer to offset
	FileIO_File_Seek(offset);

	FileIO_File_Read(buff, numRead);

	FileIO_File_Close();

	if (byteCount) {
		*byteCount = numRead;
	}

	return 1;
}

/**
 * @brief Write a buffer to file
 * 
 * @param txt File to write
 * @param buffer Buffer to write
 * @param buffersize Buffer size
 * @param mode File open mode: FILEIO_E_FOPEN_READ | FILEIO_E_FOPEN_WRITE | FILEIO_E_FOPEN_APPEND
 */
void FileIO_Buffer_To_File(const char *txt, const char *buffer, unsigned long buffersize) {
	if (-1 == FileIO_File_Open(txt, FILEIO_E_FOPEN_WRITE)) {
		printf("Error open file\n");
		return;
	}
	FileIO_File_Write(buffer, buffersize);
	FileIO_File_Close();
}

