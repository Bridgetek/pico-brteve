# EVE Hal Library

The EVE Hardware Abstraction Layer included with EVE Screen Designer consists of the following modules:
* **EVE_Hal**: This is the lowest level layer, which implements writing to and reading from EVE memory space, as well as sending host commands.
* **EVE_Host**: Wraps host commands.
* **EVE_Cmd**: Handles writing to the coprocessor command queue, also ensuring coprocessor fault reports are detected.
* **EVE_Util**: Utilities for bootup, reset, and other common management.

# Usage

## Include

```
#include <EVE_Hal.h>
```

## Initialization

```
/* Initialize HAL */
EVE_Hal_initialize();

/* List the devices */
size_t deviceCount = EVE_Hal_list();
for (size_t i = 0; i < deviceCount; ++i)
{
	EVE_DeviceInfo info;
	EVE_Hal_info(&info, i);
}

/* Fetch the default parameters for a device.
Pass the device index, or -1 to select the first device */
EVE_HalParameters params = { 0 };
EVE_Hal_defaultsEx(&params, EVE_BT816, -1);

/* Open the specified device */
EVE_HalContext host = { 0 };
EVE_HalContext *phost = &host;
if (!EVE_Hal_open(phost, &params))
	return false;

/* Get the default bootup parameters for the device.
Change display resolution in the parameters if needed. */
EVE_BootupParameters bootupParams;
EVE_Util_bootupDefaults(phost, &bootupParams);

/* Boot up the device */
if (!EVE_Util_bootup(phost, &bootupParams))
	return false;

/* Show a blank screen */
EVE_Util_clearScreen(phost);
```

## Cleanup

```
EVE_Util_shutdown(phost);
EVE_Hal_close(phost);
EVE_Hal_release();
```

#  EVE Hal

## Initialization functions

* EVE_Hal_initialize
* EVE_Hal_release
* EVE_Hal_defaultsEx
* EVE_Hal_open
* EVE_Hal_close
* EVE_Hal_idle

## Transfer functions

### EVE_Hal_startTransfer

Starts a read or write transfer, using the `EVE_TRANSFER_READ`or`EVE_TRANSFER_WRITE` option respectively. From hardware point of view, this keeps the SPI cable select flagged until `EVE_Hal_endTransfer` is called. Write transfers may be buffered in software, depending on the implementation.

### EVE_Hal_transfer8/16/32

Transfers an integer value using the currently open transfer, using the appropriate byte ordering.

### EVE_Hal_transferMem

Transfers a chunk of memory using the currently open transfer.

### EVE_Hal_transferProgmem

Transfers a chunk of data from program memory using the currently open transfer. It is recommended to keep static data in program memory, as microcontroller memory may be limited.

### EVE_Hal_transferString

Transfers a string using the currently open transfer. The string will be limited to whichever is shorter of the specified maximum length, or the actual string length, but will always be null-terminated, which may add another byte to the specified maximum length. Additionally the string may be padded to the specified padding mask, which may also add additional bytes to the written length. The maximum amount of transferred data shall be no more than `padMask + 1`.

### EVE_Hal_endTransfer

Ends the transfer started by `EVE_Hal_startTransfer`. From a hardware point of view, this turns off the SPI cable select.

Write transfers may be buffered in software, depending on the implementation. In case a write transfer is buffered, it may be transferred during the this function call, or anytime as is most efficient for the underlying implementation.

## Transfer helper functions

* EVE_Hal_rd8/16/32
* EVE_Hal_rdMem
* EVE_Hal_wr8/16/32
* EVE_Hal_wrMem
* EVE_Hal_wrProgmem
* EVE_Hal_wrString

## Hardware functions

* EVE_Hal_hostCommand
* EVE_Hal_hostCommandExt3
* EVE_Hal_powerCycle
* EVE_Hal_setSPI
* EVE_Hal_currentFrequency
* EVE_Hal_clockTrimming

## Misc

* EVE_millis
* EVE_sleep

# EVE Host

## Host commands

* EVE_Host_clockSelect
* EVE_Host_pllFreqSelect
* EVE_Host_powerModeSwitch
* EVE_Host_coreReset
* EVE_Host_selectSysClk
* EVE_Host_powerOffComponents
* EVE_Host_padDriveStrength
* EVE_Host_resetActive
* EVE_Host_resetRemoval

# EVE Cmd

## Status utility

### EVE_Cmd_rp

Get the current read pointer. Safe to use during ongoing command transaction.

### EVE_Cmd_wp

Get the current write pointer. Updates the cached write pointer when CMDB is not supported. Safe to use during ongoing command transaction.

### EVE_Cmd_space

Get the currently available space. Updates cached available space. Safe to use during ongoing command transaction.

## Command writing

### EVE_Cmd_startFunc

Begin writing a function, keeps the transfer open. The transfer may be closed using `EVE_Cmd_endFunc`.

Functions must be ended, even if a coprocessor fault occurs while writing them. The coprocessor may be reset only after ending the function.

It is safe to call any of the command writing functions while the coprocessor is in fault mode. The coprocessor fault may be checked at the end of transfer.

While a command transaction is ongoing, HAL functions outside of `EVE_Cmd_*` must not be used.

### EVE_Cmd_endFunc

End writing a function, closes the transfer.

### EVE_Cmd_wrMem

Write a memory buffer to the command buffer. Waits if there is not enough space in the command buffer. Returns false in case a co processor fault occurred.

### EVE_Cmd_wrProgmem

Write a program memory buffer to the command buffer. Waits if there is not enough space in the command buffer. Returns false in case a co processor fault occurred.

### EVE_Cmd_wrString

Write a string to the command buffer, null-terminated, padded to 4 bytes. Waits if there is not enough space in the command buffer. Returns `false` in case a coprocessor fault occured.

The value for `maxLength`may be no more than `EVE_CMD_STRING_MAX`, which should also be used as it's default value in case the string length is not known.

### EVE_Cmd_wr8/16/32

Write a value to the command buffer. Uses a cache to write 4 bytes at once to the command buffer for multiple 8-bit and 16-bit writes. Wire endianness is handled by the transfer. Waits if there is not enough space in the command buffer. Returns `false` in case a coprocessor fault occurred.

### EVE_Cmd_moveWp

Move the write pointer forward by the specified number of bytes. Returns the previous write pointer. This is used to reserve space in the command buffer for reading output from a command. Returns an uneven value in case a coprocessor fault occurred. The return value may be more conveniently validated using `EVE_CMD_FAULT` to check whether a coprocessor fault occurred.

## Queue management

### EVE_Cmd_waitFlush

Wait for the command buffer to fully empty. Returns `false` in case a coprocessor fault occured.

### EVE_Cmd_waitSpace

Wait for the command buffer to have at least the requested amount of free space. Returns `false` in case a coprocessor fault occurred.

### EVE_Cmd_waitLogo

Wait for logo to finish displaying. Waits for both the read and write pointer to go to 0. Returns `false` in case a coprocessor fault occurred.

# EVE Util

* EVE_Util_bootup
* EVE_Util_shutdown
* EVE_Util_clearScreen
* EVE_Util_resetCoprocessor
