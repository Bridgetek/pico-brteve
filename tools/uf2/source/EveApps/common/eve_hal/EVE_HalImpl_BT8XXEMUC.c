/**
 * @file EVE_HalImpl_BT8XXEMUC.c
 * @brief BT8XX Emulator Library
 *
 * @author Jan Boon <jan@no-break.space>
 *
 * @date 2017
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

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 26812) // Unscoped enum
#pragma warning(disable : 6262) // Large stack
#endif

#ifdef BT8XXEMU_REMOTE

#include <bt8xxemu.h>
#include <stdlib.h>

#include <stdio.h>
#include <assert.h>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#if !defined(NTDDI_VERSION) && !defined(_WIN32_WINNT) && !defined(WINVER)
#define NTDDI_VERSION 0x05010000 /* NTDDI_WINXP */
#define _WIN32_WINNT 0x0501 /* _WIN32_WINNT_WINXP */
#define WINVER 0x0501 /* _WIN32_WINNT_WINXP */
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif

#define BUFSIZE 64 * 1024

#define BT8XXEMU_PIPE_OPEN 0xFF02
#define BT8XXEMU_PIPE_CLOSE 0xFF04
#define BT8XXEMU_PIPE_ECHO 0xFF10

#define BT8XXEMU_CALL_VERSION 0x00
#define BT8XXEMU_CALL_DEFAULTS 0x01
#define BT8XXEMU_CALL_RUN 0x02
#define BT8XXEMU_CALL_STOP 0x03
#define BT8XXEMU_CALL_DESTROY 0x04
#define BT8XXEMU_CALL_IS_RUNNING 0x05
#define BT8XXEMU_CALL_TRANSFER 0x06
#define BT8XXEMU_CALL_CHIP_SELECT 0x07
#define BT8XXEMU_CALL_HAS_INTERRUPT 0x08

#define BT8XXEMU_CALL_FLASH_DEFAULTS 0x0101
#define BT8XXEMU_CALL_FLASH_CREATE 0x0102
#define BT8XXEMU_CALL_FLASH_DESTROY 0x0104
#define BT8XXEMU_CALL_FLASH_TRANSFER_SPI4 0x0109

typedef struct BT8XXEMUC_Remote BT8XXEMUC_Remote;
struct BT8XXEMUC_Remote
{
	LONG atomicLock;
	HANDLE pipe;
	union
	{
		uint32_t flash;
		uint32_t emulator;
	};
};

#pragma pack(push, 1)
typedef struct
{
	int32_t flags;
	BT8XXEMU_EmulatorMode mode;
	uint32_t mousePressure;
	uint32_t externalFrequency;
	uint32_t reduceGraphicsThreads;
	wchar_t romFilePath[260];
	wchar_t otpFilePath[260];
	wchar_t coprocessorRomFilePath[260];
	int64_t userContext;
	int32_t flash;
} BT8XXEMUC_RemoteEmulatorParameters;
typedef struct
{
	wchar_t deviceType[26];
	uint64_t sizeBytes;
	wchar_t dataFilePath[260];
	wchar_t statusFilePath[260];
	int persistent;
	int stdOut;
	int64_t userContext;
} BT8XXEMUC_RemoteFlashParameters;
typedef union
{
	char buffer[BUFSIZE];
	struct
	{
		uint32_t messageType;
		union
		{
			uint32_t versionApi;
			uint32_t emulator;
			uint32_t flash;
		};
		union
		{
			struct
			{
				BT8XXEMUC_RemoteEmulatorParameters remoteParams;
				BT8XXEMU_EmulatorMode mode;
			};
			// BT8XXEMU_FlashParameters flashParams;
			BT8XXEMUC_RemoteFlashParameters remoteFlashParams;
			char str[1024];
			uint8_t data;
			uint8_t signal;
			uint8_t chipSelect;
			uint8_t isRunning;
			uint8_t hasInterrupt;
		};
	};
} BT8XXEMUC_Data;
#pragma pack(pop)

#define MESSAGE_SIZE(param) (DWORD)((ptrdiff_t)(void *)(&(data.param)) + (ptrdiff_t)sizeof(data.param) - (ptrdiff_t)(void *)(&data.buffer[0]))
#define STRING_MESSAGE_SIZE() (DWORD)((ptrdiff_t)(void *)(&data.str[0]) + (ptrdiff_t)strlen(&data.str[0]) + 1 - (ptrdiff_t)(void *)(&data.buffer[0]))

static void BT8XXEMUC_readParams(BT8XXEMU_EmulatorParameters *params, const BT8XXEMUC_RemoteEmulatorParameters *remoteParams)
{
	memset(params, 0, sizeof(BT8XXEMU_EmulatorParameters));
	params->Flags = remoteParams->flags;
	params->Mode = remoteParams->mode;
	params->MousePressure = remoteParams->mousePressure;
	params->ReduceGraphicsThreads = remoteParams->reduceGraphicsThreads;
	wcscpy(params->RomFilePath, remoteParams->romFilePath);
	wcscpy(params->OtpFilePath, remoteParams->otpFilePath);
	wcscpy(params->CoprocessorRomFilePath, remoteParams->coprocessorRomFilePath);
	params->UserContext = (void *)(intptr_t)remoteParams->userContext;
	assert(remoteParams->flash == 0);
	// params->Flash = 0;
}

static void BT8XXEMUC_writeParams(BT8XXEMUC_RemoteEmulatorParameters *remoteParams, const BT8XXEMU_EmulatorParameters *params)
{
	remoteParams->flags = params->Flags;
	remoteParams->mode = params->Mode;
	remoteParams->mousePressure = params->MousePressure;
	remoteParams->reduceGraphicsThreads = params->ReduceGraphicsThreads;
	wcscpy(remoteParams->romFilePath, params->RomFilePath);
	wcscpy(remoteParams->otpFilePath, params->OtpFilePath);
	wcscpy(remoteParams->coprocessorRomFilePath, params->CoprocessorRomFilePath);
	remoteParams->userContext = (int64_t)(intptr_t)params->UserContext;
	remoteParams->flash = (params->Flash) ? params->Flash->flash : 0;
}

static void BT8XXEMUC_readFlashParams(BT8XXEMU_FlashParameters *flashParams, const BT8XXEMUC_RemoteFlashParameters *remoteFlashParams)
{
	wcscpy(flashParams->DeviceType, remoteFlashParams->deviceType);
	flashParams->SizeBytes = remoteFlashParams->sizeBytes;
	wcscpy(flashParams->DataFilePath, remoteFlashParams->dataFilePath);
	wcscpy(flashParams->StatusFilePath, remoteFlashParams->statusFilePath);
	flashParams->Persistent = remoteFlashParams->persistent;
	flashParams->StdOut = remoteFlashParams->stdOut;
}

static void BT8XXEMUC_writeFlashParams(BT8XXEMUC_RemoteFlashParameters *remoteFlashParams, const BT8XXEMU_FlashParameters *flashParams)
{
	wcscpy(remoteFlashParams->deviceType, flashParams->DeviceType);
	remoteFlashParams->sizeBytes = flashParams->SizeBytes;
	wcscpy(remoteFlashParams->dataFilePath, flashParams->DataFilePath);
	wcscpy(remoteFlashParams->statusFilePath, flashParams->StatusFilePath);
	remoteFlashParams->persistent = flashParams->Persistent;
	remoteFlashParams->stdOut = flashParams->StdOut;
	remoteFlashParams->userContext = (int64_t)(intptr_t)flashParams->UserContext;
}

static LONG s_AtomicLock = 0;
static LONG s_RefCount = 0;
static HANDLE s_Process = INVALID_HANDLE_VALUE;
static HANDLE s_ProcessThread = INVALID_HANDLE_VALUE;
static HANDLE s_Pipe = INVALID_HANDLE_VALUE;
static BT8XXEMUC_Data s_VersionData;
static int s_PipeNb = 0;

static void BT8XXEMUC_lockProcessPipe()
{
	while (InterlockedExchange(&s_AtomicLock, 1))
		SwitchToThread();
}

static void BT8XXEMUC_unlockProcessPipe()
{
	InterlockedExchange(&s_AtomicLock, 0);
}

// Open a new process if it has not been opened yet, uses reference counting
static bool BT8XXEMUC_openProcess()
{
	BT8XXEMUC_lockProcessPipe();

	if (!s_RefCount)
	{
		char pipeHandle[MAX_PATH];
		sprintf(pipeHandle, "\\\\.\\pipe\\bt8xxemus_%i", (int)GetCurrentProcessId());

		s_Pipe = CreateNamedPipeA(pipeHandle, PIPE_ACCESS_DUPLEX,
		    PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		    1, BUFSIZE, BUFSIZE, 1000, NULL);

		if (s_Pipe == INVALID_HANDLE_VALUE)
		{
			BT8XXEMUC_unlockProcessPipe();
			return false;
		}

		STARTUPINFO si = { 0 };
		PROCESS_INFORMATION pi = { 0 };
		char processCommand[MAX_PATH];
		sprintf(processCommand, "bt8xxemus.exe %s", pipeHandle);
		if (!CreateProcessA(NULL, processCommand, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
		{
			CloseHandle(s_Pipe);
			s_Pipe = INVALID_HANDLE_VALUE;
			BT8XXEMUC_unlockProcessPipe();
			return false;
		}

		s_Process = pi.hProcess;
		s_ProcessThread = pi.hThread;

		if (!ConnectNamedPipe(s_Pipe, NULL))
		{
			CloseHandle(s_Pipe);
			s_Pipe = INVALID_HANDLE_VALUE;
			TerminateProcess(s_Process, EXIT_FAILURE);
			CloseHandle(s_Process);
			s_Process = INVALID_HANDLE_VALUE;
			CloseHandle(s_ProcessThread);
			s_ProcessThread = INVALID_HANDLE_VALUE;
			BT8XXEMUC_unlockProcessPipe();
			return false;
		}
	}
	++s_RefCount;

	BT8XXEMUC_unlockProcessPipe();
	return true;
}

// Reduce the reference count of the process, and closes it when done
static void BT8XXEMUC_closeProcess()
{
	BT8XXEMUC_lockProcessPipe();

	--s_RefCount;
	if (!s_RefCount)
	{
		DWORD nb;
		DWORD len;
		BT8XXEMUC_Data data;

		data.messageType = BT8XXEMU_PIPE_CLOSE;
		len = MESSAGE_SIZE(messageType);

		if (!WriteFile(s_Pipe, data.buffer, len, &nb, NULL) || len != nb)
		{
			CloseHandle(s_Pipe);
			s_Pipe = INVALID_HANDLE_VALUE;
			TerminateProcess(s_Process, EXIT_FAILURE);
			CloseHandle(s_Process);
			s_Process = INVALID_HANDLE_VALUE;
			CloseHandle(s_ProcessThread);
			s_ProcessThread = INVALID_HANDLE_VALUE;
			BT8XXEMUC_unlockProcessPipe();
			return;
		}

		CloseHandle(s_Pipe);
		s_Pipe = INVALID_HANDLE_VALUE;

		WaitForSingleObject(s_Process, INFINITE);
		CloseHandle(s_Process);
		s_Process = INVALID_HANDLE_VALUE;
		CloseHandle(s_ProcessThread);
		s_ProcessThread = INVALID_HANDLE_VALUE;
	}

	BT8XXEMUC_unlockProcessPipe();
}

// Open an additional pipe on the open process. Must be closed
// before closing the process
static HANDLE BT8XXEMUC_openPipe()
{
	BT8XXEMUC_lockProcessPipe();

	++s_PipeNb;

	DWORD nb;
	DWORD len;
	BT8XXEMUC_Data data;

	data.messageType = BT8XXEMU_PIPE_OPEN;
	data.versionApi = BT8XXEMU_VERSION_API;
	sprintf(data.str, "\\\\.\\pipe\\bt8xxemus_%i_%i", (int)GetCurrentProcessId(), s_PipeNb);
	len = STRING_MESSAGE_SIZE();

	HANDLE pipe = CreateNamedPipeA(data.str, PIPE_ACCESS_DUPLEX,
	    PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
	    1, BUFSIZE, BUFSIZE, 1000, NULL);

	if (pipe == INVALID_HANDLE_VALUE
	    || !WriteFile(s_Pipe, data.buffer, len, &nb, NULL) || len != nb
	    || !ReadFile(s_Pipe, data.buffer, BUFSIZE, &nb, NULL)
	    || !ConnectNamedPipe(pipe, NULL))
	{
		BT8XXEMUC_unlockProcessPipe();
		return INVALID_HANDLE_VALUE;
	}

	BT8XXEMUC_unlockProcessPipe();
	return pipe;
}

static void BT8XXEMUC_lockPipe(BT8XXEMUC_Remote *emulator)
{
	while (InterlockedExchange(&emulator->atomicLock, 1))
		SwitchToThread();
}

static void BT8XXEMUC_unlockPipe(BT8XXEMUC_Remote *emulator)
{
	InterlockedExchange(&emulator->atomicLock, 0);
}

// Close a pipe
static void BT8XXEMUC_closePipe(BT8XXEMUC_Remote *emulator)
{
	BT8XXEMUC_lockPipe(emulator);

	DWORD nb;
	DWORD len;
	BT8XXEMUC_Data data;

	data.messageType = BT8XXEMU_PIPE_CLOSE;
	len = MESSAGE_SIZE(messageType);
	WriteFile(emulator->pipe, data.buffer, len, &nb, NULL);
	CloseHandle(emulator->pipe);
	emulator->pipe = INVALID_HANDLE_VALUE;

	BT8XXEMUC_unlockPipe(emulator);
}

const char *BT8XXEMU_version()
{
	if (BT8XXEMUC_openProcess())
	{
		BT8XXEMUC_lockProcessPipe();

		DWORD nb;
		DWORD len;

		s_VersionData.messageType = BT8XXEMU_CALL_VERSION;
		s_VersionData.versionApi = BT8XXEMU_VERSION_API;

		;
		{
			BT8XXEMUC_Data data;
			len = MESSAGE_SIZE(versionApi);
		}

		if (!WriteFile(s_Pipe, s_VersionData.buffer, len, &nb, NULL) || len != nb
		    || !ReadFile(s_Pipe, s_VersionData.buffer, BUFSIZE, &nb, NULL))
		{
			BT8XXEMUC_unlockProcessPipe();
			BT8XXEMUC_closeProcess();
			return "BT8XX Emulator Library\nNot Responding";
		}

		BT8XXEMUC_unlockProcessPipe();
		BT8XXEMUC_closeProcess();

		return s_VersionData.str;
	}

	return "BT8XX Emulator Library\nNot Installed";
}

void BT8XXEMU_defaults(uint32_t versionApi, BT8XXEMU_EmulatorParameters *params, BT8XXEMU_EmulatorMode mode)
{
	if (BT8XXEMUC_openProcess())
	{
		BT8XXEMUC_lockProcessPipe();

		DWORD nb;
		DWORD len;
		BT8XXEMUC_Data data;

		data.messageType = BT8XXEMU_CALL_DEFAULTS;
		data.versionApi = BT8XXEMU_VERSION_API;
		// BT8XXEMUC_writeParams(&data.remoteParams, params);
		memset(&data.remoteParams, 0, sizeof(BT8XXEMUC_RemoteEmulatorParameters));
		data.mode = mode;
		len = MESSAGE_SIZE(mode);

		if (!WriteFile(s_Pipe, data.buffer, len, &nb, NULL) || len != nb
		    || !ReadFile(s_Pipe, data.buffer, BUFSIZE, &nb, NULL))
		{
			BT8XXEMUC_unlockProcessPipe();
			BT8XXEMUC_closeProcess();
			memset(params, 0, sizeof(BT8XXEMU_EmulatorParameters));
			return;
		}

		BT8XXEMUC_unlockProcessPipe();
		BT8XXEMUC_closeProcess();

		BT8XXEMUC_readParams(params, &data.remoteParams);
		return;
	}

	memset(params, 0, sizeof(BT8XXEMU_EmulatorParameters));
}

void BT8XXEMU_run(uint32_t versionApi, BT8XXEMU_Emulator **emulator, const BT8XXEMU_EmulatorParameters *params)
{
	/*
	if (params->Close) printf("Close callback is not permitted in service mode\n");
	if (params->Graphics) printf("Graphics callback is not permitted in service mode\n");
	if (params->Log) printf("Log callback is not permitted in service mode\n");
	// if (params.Main) printf("Main callback is not permitted in service mode\n");
	if (params->MCUSleep) printf("MCUSleep callback is not permitted in service mode\n");
	*/

	if (params->Close
	    || params->Graphics
	    || params->Log
	    || params->MCUSleep)
	{
		// TODO: Callback handling
		*emulator = NULL;
		return;
	}

	if (BT8XXEMUC_openProcess())
	{
		*emulator = malloc(sizeof(BT8XXEMU_Emulator));
		if (!*emulator)
		{
			BT8XXEMUC_closeProcess();
			return;
		}
		memset(*emulator, 0, sizeof(BT8XXEMU_Emulator));
		(*emulator)->pipe = BT8XXEMUC_openPipe(); // Create a separate pipe for each emulator instance

		if ((*emulator)->pipe == INVALID_HANDLE_VALUE)
		{
			BT8XXEMUC_closeProcess();
			free(*emulator);
			*emulator = NULL;
			return;
		}

		DWORD nb;
		DWORD len;
		BT8XXEMUC_Data data;

		data.messageType = BT8XXEMU_CALL_RUN;
		data.versionApi = BT8XXEMU_VERSION_API;
		BT8XXEMUC_writeParams(&data.remoteParams, params);
		len = MESSAGE_SIZE(remoteParams);

		// data.params.Main = NULL;
		// data.params.Close = NULL; // Temporary

		if (!WriteFile((*emulator)->pipe, data.buffer, len, &nb, NULL) || len != nb
		    || !ReadFile((*emulator)->pipe, data.buffer, BUFSIZE, &nb, NULL))
		{
			BT8XXEMUC_closePipe(*emulator);
			BT8XXEMUC_closeProcess();
			free(*emulator);
			*emulator = NULL;
			return;
		}

		(*emulator)->emulator = data.emulator;

		if (params->Main)
		{
			params->Main((*emulator), params->UserContext);
		}

		return;
	}

	*emulator = NULL;
}

void BT8XXEMU_stop(BT8XXEMU_Emulator *emulator)
{
	BT8XXEMUC_lockPipe(emulator);

	DWORD nb;
	DWORD len;
	BT8XXEMUC_Data data;

	data.messageType = BT8XXEMU_CALL_STOP;
	data.emulator = emulator->emulator;
	len = MESSAGE_SIZE(emulator);

	if (!WriteFile(emulator->pipe, data.buffer, len, &nb, NULL) || len != nb
	    || !ReadFile(emulator->pipe, data.buffer, BUFSIZE, &nb, NULL))
	{
		// ...
	}

	BT8XXEMUC_unlockPipe(emulator);
}

void BT8XXEMU_destroy(BT8XXEMU_Emulator *emulator)
{
	BT8XXEMUC_lockPipe(emulator);

	DWORD nb;
	DWORD len;
	BT8XXEMUC_Data data;

	data.messageType = BT8XXEMU_CALL_DESTROY;
	data.emulator = emulator->emulator;
	len = MESSAGE_SIZE(emulator);

	if (!WriteFile(emulator->pipe, data.buffer, len, &nb, NULL) || len != nb
	    || !ReadFile(emulator->pipe, data.buffer, BUFSIZE, &nb, NULL))
	{
		// ...
	}

	BT8XXEMUC_unlockPipe(emulator);

	BT8XXEMUC_closePipe(emulator);
	BT8XXEMUC_closeProcess();
	free(emulator);
}

int BT8XXEMU_isRunning(BT8XXEMU_Emulator *emulator)
{
	BT8XXEMUC_lockPipe(emulator);

	DWORD nb;
	DWORD len;
	BT8XXEMUC_Data data;

	data.messageType = BT8XXEMU_CALL_IS_RUNNING;
	data.emulator = emulator->emulator;
	len = MESSAGE_SIZE(emulator);

	if (!WriteFile(emulator->pipe, data.buffer, len, &nb, NULL) || len != nb
	    || !ReadFile(emulator->pipe, data.buffer, BUFSIZE, &nb, NULL))
	{
		BT8XXEMUC_unlockPipe(emulator);
		return 0;
	}

	BT8XXEMUC_unlockPipe(emulator);
	return data.isRunning;
}

uint8_t BT8XXEMU_transfer(BT8XXEMU_Emulator *emulator, uint8_t value)
{
	BT8XXEMUC_lockPipe(emulator);

	DWORD nb;
	DWORD len;
	BT8XXEMUC_Data data;

	data.messageType = BT8XXEMU_CALL_TRANSFER;
	data.emulator = emulator->emulator;
	data.data = value;
	len = MESSAGE_SIZE(data);

	if (!WriteFile(emulator->pipe, data.buffer, len, &nb, NULL) || len != nb
	    || !ReadFile(emulator->pipe, data.buffer, BUFSIZE, &nb, NULL))
	{
		BT8XXEMUC_unlockPipe(emulator);
		return 0;
	}

	BT8XXEMUC_unlockPipe(emulator);
	return data.data;
}

void BT8XXEMU_chipSelect(BT8XXEMU_Emulator *emulator, int cs)
{
	BT8XXEMUC_lockPipe(emulator);

	DWORD nb;
	DWORD len;
	BT8XXEMUC_Data data;

	data.messageType = BT8XXEMU_CALL_CHIP_SELECT;
	data.emulator = emulator->emulator;
	data.chipSelect = cs;
	len = MESSAGE_SIZE(chipSelect);

	if (!WriteFile(emulator->pipe, data.buffer, len, &nb, NULL) || len != nb
	    || !ReadFile(emulator->pipe, data.buffer, BUFSIZE, &nb, NULL))
	{
		// ...
	}

	BT8XXEMUC_unlockPipe(emulator);
}

int BT8XXEMU_hasInterrupt(BT8XXEMU_Emulator *emulator)
{
	BT8XXEMUC_lockPipe(emulator);

	DWORD nb;
	DWORD len;
	BT8XXEMUC_Data data;

	data.messageType = BT8XXEMU_CALL_HAS_INTERRUPT;
	data.emulator = emulator->emulator;
	len = MESSAGE_SIZE(emulator);

	if (!WriteFile(emulator->pipe, data.buffer, len, &nb, NULL) || len != nb
	    || !ReadFile(emulator->pipe, data.buffer, BUFSIZE, &nb, NULL))
	{
		BT8XXEMUC_unlockPipe(emulator);
		return 0;
	}

	BT8XXEMUC_unlockPipe(emulator);
	return data.hasInterrupt;
}

void BT8XXEMU_Flash_defaults(uint32_t versionApi, BT8XXEMU_FlashParameters *params)
{
	if (BT8XXEMUC_openProcess())
	{
		BT8XXEMUC_lockProcessPipe();

		DWORD nb;
		DWORD len;
		BT8XXEMUC_Data data;

		data.messageType = BT8XXEMU_CALL_FLASH_DEFAULTS;
		data.versionApi = BT8XXEMU_VERSION_API;
		// BT8XXEMUC_writeFlashParams(&data.remoteFlashParams, params);
		memset(&data.remoteFlashParams, 0, sizeof(BT8XXEMUC_RemoteFlashParameters));
		len = MESSAGE_SIZE(remoteFlashParams);

		if (!WriteFile(s_Pipe, data.buffer, len, &nb, NULL) || len != nb
		    || !ReadFile(s_Pipe, data.buffer, BUFSIZE, &nb, NULL))
		{
			BT8XXEMUC_unlockProcessPipe();
			BT8XXEMUC_closeProcess();
			memset(params, 0, sizeof(BT8XXEMU_FlashParameters));
			return;
		}

		BT8XXEMUC_unlockProcessPipe();
		BT8XXEMUC_closeProcess();

		BT8XXEMUC_readFlashParams(params, &data.remoteFlashParams);
		return;
	}

	memset(params, 0, sizeof(BT8XXEMU_FlashParameters));
}

BT8XXEMU_Flash *BT8XXEMU_Flash_create(uint32_t versionApi, const BT8XXEMU_FlashParameters *params)
{
	if (BT8XXEMUC_openProcess())
	{
		BT8XXEMU_Flash *flash = malloc(sizeof(BT8XXEMU_Flash));
		if (!flash)
			return NULL;
		memset(flash, 0, sizeof(BT8XXEMU_Flash));
		flash->pipe = BT8XXEMUC_openPipe(); // Create a separate pipe for each emulator instance

		if (flash->pipe == INVALID_HANDLE_VALUE)
		{
			BT8XXEMUC_closeProcess();
			free(flash);
			return NULL;
		}

		DWORD nb;
		DWORD len;
		BT8XXEMUC_Data data;

		data.messageType = BT8XXEMU_CALL_FLASH_CREATE;
		data.versionApi = BT8XXEMU_VERSION_API;
		BT8XXEMUC_writeFlashParams(&data.remoteFlashParams, params);
		len = MESSAGE_SIZE(remoteFlashParams);

		if (!WriteFile(flash->pipe, data.buffer, len, &nb, NULL) || len != nb
		    || !ReadFile(flash->pipe, data.buffer, BUFSIZE, &nb, NULL))
		{
			BT8XXEMUC_closePipe(flash);
			BT8XXEMUC_closeProcess();
			free(flash);
			return NULL;
		}

		flash->flash = data.flash;

		return flash;
	}

	return NULL;
}

void BT8XXEMU_Flash_destroy(BT8XXEMU_Flash *flash)
{
	BT8XXEMUC_lockPipe(flash);

	DWORD nb;
	DWORD len;
	BT8XXEMUC_Data data;

	data.messageType = BT8XXEMU_CALL_FLASH_DESTROY;
	data.flash = flash->flash;
	len = MESSAGE_SIZE(emulator);

	if (!WriteFile(flash->pipe, data.buffer, len, &nb, NULL) || len != nb
	    || !ReadFile(flash->pipe, data.buffer, BUFSIZE, &nb, NULL))
	{
		// ...
	}

	BT8XXEMUC_unlockPipe(flash);

	BT8XXEMUC_closePipe(flash);
	BT8XXEMUC_closeProcess();
	free(flash);
}

uint8_t BT8XXEMU_Flash_transferSpi4(BT8XXEMU_Flash *flash, uint8_t signal)
{
	BT8XXEMUC_lockPipe(flash);

	DWORD nb;
	DWORD len;
	BT8XXEMUC_Data data;

	data.messageType = BT8XXEMU_CALL_FLASH_TRANSFER_SPI4;
	data.flash = flash->flash;
	data.signal = signal;
	len = MESSAGE_SIZE(signal);

	if (!WriteFile(flash->pipe, data.buffer, len, &nb, NULL) || len != nb
	    || !ReadFile(flash->pipe, data.buffer, BUFSIZE, &nb, NULL))
	{
		BT8XXEMUC_unlockPipe(flash);
		return 0;
	}

	BT8XXEMUC_unlockPipe(flash);
	return data.data;
}

#endif

#ifdef _MSC_VER
#pragma warning(pop)
#endif

/* end of file */
