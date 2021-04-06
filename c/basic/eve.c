#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "transport.h"
#include "eve.h"

#define EVE_TRANSFER_READ  1
#define EVE_TRANSFER_WRITE 2
#define EXTRACT_CHIPID(romChipId) ((((romChipId) >> 8) & 0xFF) | (((romChipId) & (0xFF)) << 8))
#define EVE_CMD_STRING_MAX 511

static uint8_t cmd_buffer[4];
static uint8_t cmd_buffer_index;
static uint8_t rwstatus;

/**
 * @brief Start data transfer to Coprocessor
 * 
 * @param rw Read or Write
 * @param addr Address to read/write
 */
void eve_transfer_begin(EVE_TRANSFER_T rw, uint32_t addr) {
	if (rw == EVE_TRANSFER_READ) {
		uint8_t spidata[5];
		spidata[0] = (addr >> 16);
		spidata[1] = (addr >> 8);
		spidata[2] = addr & 0xff;
		cs_select();
		spi_write_blocking(SPI_PORT, spidata, 3 + SPI_DUMMY);
		rwstatus = EVE_STATUS_READING;
	} else {
		uint8_t spidata[4];
		spidata[0] = (0x80 | (addr >> 16));
		spidata[1] = (addr >> 8);
		spidata[2] = addr;

		cs_select();
		spi_write_blocking(SPI_PORT, spidata, 3);
		rwstatus = EVE_STATUS_WRITING;
	}
}

/**
 * @brief End data transfer
 * 
 */
void eve_transfer_end() {
	cs_deselect();
	rwstatus = EVE_STATUS_OPENED;
}

/**
 * @brief Write 8 bit to Coprocessor
 * 
 * @param value Value to write
 * @return uint8_t Number of bytes transfered
 */
uint8_t eve_transfer8(uint8_t value) {
	if (rwstatus == EVE_STATUS_READING) {
		spi_read_blocking(SPI_PORT, 0, &value, 1);
		return value;
	} else {
		spi_write_blocking(SPI_PORT, &value, 1);
		return 0;
	}
}

/**
 * @brief Write 2 bytes to Coprocessor
 * 
 * @param value Value to write
 * @return uint16_t Number of bytes transfered
 */
uint16_t eve_transfer16(uint16_t value) {
	uint8_t buffer[2];
	if (rwstatus == EVE_STATUS_READING) {
		spi_read_blocking(SPI_PORT, 0, buffer, 2);
		return (uint16_t) buffer[0] | (uint16_t) buffer[1] << 8;
	} else {
		buffer[0] = value & 0xFF;
		buffer[1] = value >> 8;
		spi_write_blocking(SPI_PORT, buffer, 2);
		return 0;
	}
}

/**
 * @brief Write 4 bytes to Coprocessor
 * 
 * @param value Value to write
 * @return uint32_t Number of bytes transfered
 */
uint32_t eve_transfer32(uint32_t value) {
	uint8_t buffer[4];
	if (rwstatus == EVE_STATUS_READING) {
		spi_read_blocking(SPI_PORT, 0, buffer, 4);
		return (uint32_t) buffer[0] | (uint32_t) buffer[1] << 8
				| (uint32_t) buffer[2] << 16 | (uint32_t) buffer[3] << 24;
	} else {
		buffer[0] = value & 0xFF;
		buffer[1] = (value >> 8) & 0xFF;
		buffer[2] = (value >> 16) & 0xFF;
		buffer[3] = value >> 24;
		spi_write_blocking(SPI_PORT, buffer, 4);
		return 0;
	}
}

/**
 * @brief Transfer a string to Ever platform
 * 
 * @param str String to transfer
 * @param size Size of string
 * @param padMask Padding mask
 * @return uint32_t Numer of bytes transfered
 */
uint32_t eve_transfer_string(const char *str, uint32_t size, uint32_t padMask) {
	if (!size) {
		padMask = 0x3;
		eve_transfer32(0);
		return 4;
	}

	uint32_t transferred = (uint32_t) strnlen(str, size) + 1;
	if (rwstatus == EVE_STATUS_WRITING) {
		spi_write_blocking(SPI_PORT, (uint8_t *) str, transferred);

		if (transferred & padMask) {
			uint32_t pad = 4 - (transferred & padMask);
			uint8_t padding[4] = { 0 };
			spi_write_blocking(SPI_PORT, padding, pad);
			transferred += pad;
		}
	}
	return transferred;
}

/**
 * @brief Transfer (read/write) a block data to Coprocessor
 * 
 * @param result Buffer to get data transfered, NULL when write
 * @param buffer Buffer where data is transfered, NULL when read
 * @param size Size of buffer
 */
void eve_transfermem(const uint8_t *result, const uint8_t *buffer,
		uint32_t size) {
	if (!size)
		return;

	if (result && buffer) {
		/* invalid input */
	} else if (result) {
		spi_read_blocking(SPI_PORT, 0, (uint8_t *) result, size);
	} else if (buffer) {
		spi_write_blocking(SPI_PORT, (uint8_t *) buffer, size);
	}
}

/**
 * @brief Read 8 bits from Coprocessor's memory
 * 
 * @param addr Address to be read
 * @return uint8_t Data from Coprocessor
 */
uint8_t eve_rd8(uint32_t addr) {
	uint8_t value;
	eve_transfer_begin(EVE_TRANSFER_READ, addr);
	value = eve_transfer8(0);
	eve_transfer_end();
	return value;
}

/**
 * @brief Read 2 bytes from Coprocessor's memory
 * 
 * @param addr Address to be read
 * @return uint16_t Data from Coprocessor
 */
uint16_t eve_rd16(uint32_t addr) {
	uint16_t value;
	eve_transfer_begin(EVE_TRANSFER_READ, addr);
	value = eve_transfer16(0);
	eve_transfer_end();
	return value;
}

/**
 * @brief Read 4 bytes from Coprocessor's memory
 * 
 * @param addr Address to be read
 * @return uint16_t Data from Coprocessor
 */
uint32_t eve_rd32(uint32_t addr) {
	uint32_t value;
	eve_transfer_begin(EVE_TRANSFER_READ, addr);
	value = eve_transfer32(0);
	eve_transfer_end();
	return value;
}

/**
 * @brief Read a block data from Coprocessor's memory
 * 
 * @param result Buffer where data write to
 * @param addr Address to bbe read
 * @param size Size to be read
 */
void eve_rd_mem(const uint8_t *result, uint32_t addr, uint32_t size) {
	eve_transfer_begin(EVE_TRANSFER_READ, addr);
	eve_transfermem(result, NULL, size);
	eve_transfer_end();
}

/**
 * @brief Write 8 bits to Coprocessor's memory
 * 
 * @param addr Address to be write
 * @param v Value to write
 */
void eve_wr8(uint32_t addr, uint8_t v) {
	eve_transfer_begin(EVE_TRANSFER_WRITE, addr);
	eve_transfer8(v);
	eve_transfer_end();
}

/**
 * @brief Write 2 bytes to Coprocessor's memory
 * 
 * @param addr Address to be write
 * @param v Value to write
 */
void eve_wr16(uint32_t addr, uint16_t v) {
	eve_transfer_begin(EVE_TRANSFER_WRITE, addr);
	eve_transfer16(v);
	eve_transfer_end();
}

/**
 * @brief Write 4 bytes to Coprocessor's memory
 * 
 * @param addr Address to be write
 * @param v Value to write
 */
void eve_wr32(uint32_t addr, uint32_t v) {
	eve_transfer_begin(EVE_TRANSFER_WRITE, addr);
	eve_transfer32(v);
	eve_transfer_end();
}

/**
 * @brief Write a buffer to Coprocessor's memory
 * 
 * @param addr Address to be write
 * @param buffer Data to be write
 * @param size Size of buffer
 */
void eve_wr_mem(uint32_t addr, const uint8_t *buffer, uint32_t size) {
	eve_transfer_begin(EVE_TRANSFER_WRITE, addr);
	eve_transfermem(NULL, buffer, size);
	eve_transfer_end();
}

/**
 * @brief Write a string to Coprocessor's memory
 * 
 * @param addr Address to be write
 * @param str String to be write
 * @param index Start postion in the string
 * @param size Size of the string
 * @param padMask Padding mask
 */
void eve_wr_string(uint32_t addr, const char *str, uint32_t size,
		uint32_t padMask) {
	eve_transfer_begin(EVE_TRANSFER_WRITE, addr);
	eve_transfer_string(str, size, padMask);
	eve_transfer_end();
}

//---------------------------------------------------------------------

/**
 * @brief Write buffer to Coprocessor's comand fifo
 *
 * @param buffer Data pointer
 * @param size Size to write
 * @return true Write ok
 * @return false Write error
 */
bool eve_cmd_mem(const uint8_t *buffer, uint32_t size) {
	cmd_buffer_index = 0;
	eve_transfermem(NULL, buffer, size);
	return true;
}

/**
 * @brief Write a byte to Coprocessor's command fifo
 * 
 * @param value Byte to write
 * @return true True if ok
 * @return false False if error
 */
bool eve_cmd_8(uint8_t value) {
	if (cmd_buffer_index < 4) {
		cmd_buffer[cmd_buffer_index++] = value;
	}

	if (cmd_buffer_index == 4) {
		cmd_buffer_index = 0;
		return eve_cmd_mem(cmd_buffer, 4);
	}

	return true;
}

/**
 * @brief Write 2 bytes to Coprocessor's command fifo
 * 
 * @param value Data to write
 * @return true True if ok
 * @return false False if error
 */
bool eve_cmd_16(uint16_t value) {
	if (cmd_buffer_index < 3) {
		cmd_buffer[cmd_buffer_index++] = value & 0xFF;
		cmd_buffer[cmd_buffer_index++] = value >> 8;
	}

	if (cmd_buffer_index == 4) {
		cmd_buffer_index = 0;
		return eve_cmd_mem(cmd_buffer, 4);
	}

	return true;
}

/**
 * @brief Write 4 bytes to Coprocessor's command fifo
 * 
 * @param value Data to write
 * @return true True if ok
 * @return false False if error
 */
bool eve_cmd_32(uint32_t value) {
	eve_transfer32(value);
	return true;
}

/**
 * @brief Write a string into Coprocessor's command fifo
 * 
 * @param str String to write
 * @param maxLength Length to write, up to EVE_CMD_STRING_MAX
 * @return uint32_t Number of bytes transfered
 */
uint32_t eve_cmd_string(const char *str, uint32_t maxLength) {
	eve_transfer_string((const char *) str, maxLength, 0x3);

	return maxLength;
}

/**
 * @brief Read from Coprocessor
 *
 * @return uint16_t Read pointer
 */
uint16_t eve_cmd_rp() {
	return eve_rd16(REG_CMD_READ) & EVE_CMD_FIFO_MASK;
}

/**
 * @brief Write to Coprocessor
 *
 * @return uint16_t Write pointer
 */
uint16_t eve_cmd_wp() {
	return eve_rd16(REG_CMD_WRITE) & EVE_CMD_FIFO_MASK;
}

/**
 * @brief Move the write pointer forward by the specified number of bytes. Returns the previous write pointer
 *
 * @param bytes Number of bytes to move
 * @return uint16_t Previous write pointer
 */
uint16_t eve_move_wp(uint16_t bytes) {
	uint16_t wp;
	uint16_t prev_wp;

	prev_wp = eve_cmd_wp();
	wp = (prev_wp + bytes) & EVE_CMD_FIFO_MASK;
	eve_wr16(REG_CMD_WRITE, wp);

	return prev_wp;
}

/**
 * @brief Wait till Command FIFO buffer empty
 * 
 * @return true True if ok
 * @return false False if error
 */
bool eve_wait_flush() {
	uint16_t rp = eve_cmd_rp();
	uint16_t wp = eve_cmd_wp();

	while (rp != wp) {
		rp = eve_cmd_rp();
		wp = eve_cmd_wp();
		sleep_ms(10);
	}
	return true;
}

/**
 * @brief Send a host command to Coprocessor
 * 
 * @param cmd Command to send
 */
void eve_host_command(uint8_t cmd) {
	uint8_t hcmd[3] = { 0 };
	hcmd[0] = cmd;
	hcmd[1] = 0;
	hcmd[2] = 0;

	cs_select();
	spi_write_blocking(SPI_PORT, hcmd, 3);
	cs_deselect();
}

/**
 * @brief This API sends a 3byte command to the eve
 * 
 * @param cmd Command to send
 */
void eve_host_command_ext3(uint32_t cmd) {
	uint8_t hcmd[3] = { 0 };
	hcmd[0] = cmd & 0xff;
	hcmd[1] = (cmd >> 8) & 0xff;
	hcmd[2] = (cmd >> 16) & 0xff;

	cs_select();
	spi_write_blocking(SPI_PORT, hcmd, 3);
	cs_deselect();
}

/**
 * @brief Toggle PD_N pin of FT800 board for a power cycle
 * 
 * @param up Up or Down
 */
bool eve_powercycle(bool up) {
	if (up) {
		pdn_down();
		sleep_ms(20);
		pdn_up();
		sleep_ms(20);
	} else {
		pdn_up();
		sleep_ms(20);
		pdn_down();
		sleep_ms(20);
	}
	return true;
}

/**
 * @brief Write 1 command into Coprocessor's command fifo
 * 
 * @param cmd Command to send
 */
void eve_cocmd_d(uint32_t cmd) {
	eve_cmd_32(cmd);
}

/**
 * @brief Write 1 command with 1 param into Coprocessor's command fifo
 * 
 * @param cmd Command to send
 * @param d0 1st param
 */
void eve_cocmd_dd(uint32_t cmd, uint32_t d0) {
	eve_cmd_32(cmd);
	eve_cmd_32(d0);
}

/**
 * @brief Write 1 command with 2 param into Coprocessor's command fifo
 * 
 * @param cmd Command to send
 * @param d0 1st param
 * @param d1 2nd param
 */
void eve_cocmd_ddd(uint32_t cmd, uint32_t d0, uint32_t d1) {
	eve_cmd_32(cmd);
	eve_cmd_32(d0);
	eve_cmd_32(d1);
}

/**
 * @brief Write 1 command with 3 param into Coprocessor's command fifo
 * 
 * @param cmd Command to send
 * @param d0 1st param
 * @param d1 2nd param
 * @param d2 3rd param
 */
void eve_cocmd_dddd(uint32_t cmd, uint32_t d0, uint32_t d1, uint32_t d2) {
	eve_cmd_32(cmd);
	eve_cmd_32(d0);
	eve_cmd_32(d1);
	eve_cmd_32(d2);
}

/**
 * @brief Write 1 command with 4 param into Coprocessor's command fifo
 * 
 * @param cmd Command to send
 * @param d0 1st param
 * @param d1 2nd param
 * @param d2 3rd param
 * @param d3 4th param
 */
void eve_cocmd_ddddd(uint32_t cmd, uint32_t d0, uint32_t d1, uint32_t d2,
		uint32_t d3) {
	eve_cmd_32(cmd);
	eve_cmd_32(d0);
	eve_cmd_32(d1);
	eve_cmd_32(d2);
	eve_cmd_32(d3);
}

/**
 * @brief Write 1 command with 3 param into Coprocessor's command fifo
 * 
 * @param cmd Command to send
 * @param d0 1st param
 * @param w1 2nd param
 * @param w2 3rd param
 */
void eve_cocmd_ddww(uint32_t cmd, uint32_t d0, uint16_t w1, uint16_t w2) {
	eve_cmd_32(cmd);
	eve_cmd_32(d0);
	eve_cmd_16(w1);
	eve_cmd_16(w2);
}

/**
 * @brief Write 1 command with 4 param into Coprocessor's command fifo
 * 
 * @param cmd Command to send
 * @param d0 1st param
 * @param w1 2nd param
 * @param w2 3rd param
 * @param w3 4th param
 */
void eve_cocmd_ddwww(uint32_t cmd, uint32_t d0, uint16_t w1, uint16_t w2,
		uint16_t w3) {
	eve_cmd_32(cmd);
	eve_cmd_32(d0);
	eve_cmd_16(w1);
	eve_cmd_16(w2);
	eve_cmd_16(w3);
	eve_cmd_16(0);
}

/**
 * @brief Write 1 command with 6 param into Coprocessor's command fifo
 * 
 * @param cmd Command to send
 * @param d0 1st param
 * @param d1 2nd param
 * @param w2 3rd param
 * @param w3 4th param
 * @param w4 5th param
 * @param w5 6th param
 */
void eve_cocmd_dddwwww(uint32_t cmd, uint32_t d0, uint32_t d1, uint16_t w2,
		uint16_t w3, uint16_t w4, uint16_t w5) {
	eve_cmd_32(cmd);
	eve_cmd_32(d0);
	eve_cmd_32(d1);
	eve_cmd_16(w2);
	eve_cmd_16(w3);
	eve_cmd_16(w4);
	eve_cmd_16(w5);
}

/**
 * @brief Write 1 command with 4 param into Coprocessor's command fifo
 * 
 * @param cmd Command to send
 * @param w0 1st param
 * @param w1 2nd param
 * @param w2 3rd param
 * @param w3 4th param
 */
void eve_cocmd_dwwdd(uint32_t cmd, uint16_t w0, uint16_t w1, uint32_t d2,
		uint32_t d3) {
	eve_cmd_32(cmd);
	eve_cmd_16(w0);
	eve_cmd_16(w1);
	eve_cmd_32(d2);
	eve_cmd_32(d3);
}

/**
 * @brief Write 1 command with 6 param into Coprocessor's command fifo
 * 
 * @param cmd Command to send
 * @param w0 1st param
 * @param w1 2nd param
 * @param d2 3rd param
 * @param w3 4th param
 * @param w4 5th param
 * @param w5 6th param
 */
void eve_cocmd_dwwdwwd(uint32_t cmd, uint16_t w0, uint16_t w1, uint32_t d2,
		uint16_t w3, uint16_t w4, uint32_t d5) {
	eve_cmd_32(cmd);
	eve_cmd_16(w0);
	eve_cmd_16(w1);
	eve_cmd_32(d2);
	eve_cmd_16(w3);
	eve_cmd_16(w4);
	eve_cmd_32(d5);
}

/**
 * @brief Write 1 command with 6 param into Coprocessor's command fifo
 * 
 * @param cmd Command to send
 * @param w0 1st param
 * @param w1 2nd param
 * @param w2 3rd param
 * @param w3 4th param
 * @param d4 5th param
 * @param w5 6th param
 */
void eve_cocmd_dwwwwdw(uint32_t cmd, uint16_t w0, uint16_t w1, uint16_t w2,
		uint16_t w3, uint32_t d4, uint16_t w5) {
	eve_cmd_32(cmd);
	eve_cmd_16(w0);
	eve_cmd_16(w1);
	eve_cmd_16(w2);
	eve_cmd_16(w3);
	eve_cmd_32(d4);
	eve_cmd_16(w5);
	eve_cmd_16(0);
}

/**
 * @brief Write 1 command with 7 param into Coprocessor's command fifo
 * 
 * @param cmd Command to send
 * @param w0 1st param
 * @param w1 2nd param
 * @param w2 3rd param
 * @param w3 4th param
 * @param d4 5th param
 * @param w5 6th param
 * @param w6 7th param
 */
void eve_cocmd_dwwwwdww(uint32_t cmd, uint16_t w0, uint16_t w1, uint16_t w2,
		uint16_t w3, uint32_t d4, uint16_t w5, uint16_t w6) {
	eve_cmd_32(cmd);
	eve_cmd_16(w0);
	eve_cmd_16(w1);
	eve_cmd_16(w2);
	eve_cmd_16(w3);
	eve_cmd_32(d4);
	eve_cmd_16(w5);
	eve_cmd_16(w6);
}

/**
 * @brief Write 1 command with 4 param into Coprocessor's command fifo
 * 
 * @param cmd Command to send
 * @param w0 1st param
 * @param w1 2nd param
 * @param w2 3rd param
 * @param w3 4th param
 */
void eve_cocmd_dwwww(uint32_t cmd, uint16_t w0, uint16_t w1, uint16_t w2,
		uint16_t w3) {
	eve_cmd_32(cmd);
	eve_cmd_16(w0);
	eve_cmd_16(w1);
	eve_cmd_16(w2);
	eve_cmd_16(w3);
}

/**
 * @brief Write 1 command with 5 param into Coprocessor's command fifo
 * 
 * @param cmd Command to send
 * @param w0 1st param
 * @param w1 2nd param
 * @param w2 3rd param
 * @param w3 4th param
 * @param w4 5th param
 */
void eve_cocmd_dwwwww(uint32_t cmd, uint16_t w0, uint16_t w1, uint16_t w2,
		uint16_t w3, uint16_t w4) {
	eve_cmd_32(cmd);
	eve_cmd_16(w0);
	eve_cmd_16(w1);
	eve_cmd_16(w2);
	eve_cmd_16(w3);
	eve_cmd_16(w4);
	eve_cmd_16(0);
}

/**
 * @brief Write 1 command with 5 param into Coprocessor's command fifo
 * 
 * @param cmd Command to send
 * @param w0 1st param
 * @param w1 2nd param
 * @param w2 3rd param
 * @param w3 4th param
 * @param d4 5th param
 */
void eve_cocmd_dwwwwd(uint32_t cmd, uint16_t w0, uint16_t w1, uint16_t w2,
		uint16_t w3, uint32_t d4) {
	eve_cmd_32(cmd);
	eve_cmd_16(w0);
	eve_cmd_16(w1);
	eve_cmd_16(w2);
	eve_cmd_16(w3);
	eve_cmd_32(d4);
}

/**
 * @brief Write 1 command with 7 param into Coprocessor's command fifo
 * 
 * @param cmd Command to send
 * @param w0 1st param
 * @param w1 2nd param
 * @param w2 3rd param
 * @param w3 4th param
 * @param w4 5th param
 * @param w5 6th param
 * @param w6 7th param
 */
void eve_cocmd_dwwwwwww(uint32_t cmd, uint16_t w0, uint16_t w1, uint16_t w2,
		uint16_t w3, uint16_t w4, uint16_t w5, uint16_t w6) {
	eve_cmd_32(cmd);
	eve_cmd_16(w0);
	eve_cmd_16(w1);
	eve_cmd_16(w2);
	eve_cmd_16(w3);
	eve_cmd_16(w4);
	eve_cmd_16(w5);
	eve_cmd_16(w6);
	eve_cmd_16(0);
}

/**
 * @brief Write 1 command with 8 param into Coprocessor's command fifo
 * 
 * @param cmd Command to send
 * @param w0 1st param
 * @param w1 2nd param
 * @param w2 3rd param
 * @param w3 4th param
 * @param w4 5th param
 * @param w5 6th param
 * @param w6 7th param
 * @param w7 8th param
 */
void eve_cocmd_dwwwwwwww(uint32_t cmd, uint16_t w0, uint16_t w1, uint16_t w2,
		uint16_t w3, uint16_t w4, uint16_t w5, uint16_t w6, uint16_t w7) {
	eve_cmd_32(cmd);
	eve_cmd_16(w0);
	eve_cmd_16(w1);
	eve_cmd_16(w2);
	eve_cmd_16(w3);
	eve_cmd_16(w4);
	eve_cmd_16(w5);
	eve_cmd_16(w6);
	eve_cmd_16(w7);
}

/**
 * @brief Write 1 command with 7 param into Coprocessor's command fifo
 * 
 * @param cmd Command to send
 * @param w0 1st param
 * @param w1 2nd param
 * @param w2 3rd param
 * @param w3 4th param
 * @param w4 5th param
 * @param w5 6th param
 * @param s  7th param
 */
void eve_cocmd_dwwwwwwz(uint32_t cmd, uint16_t w0, uint16_t w1, uint16_t w2,
		uint16_t w3, uint16_t w4, uint16_t w5, const char *s) {
	eve_cmd_32(cmd);
	eve_cmd_16(w0);
	eve_cmd_16(w1);
	eve_cmd_16(w2);
	eve_cmd_16(w3);
	eve_cmd_16(w4);
	eve_cmd_16(w5);
	eve_cmd_string(s, EVE_CMD_STRING_MAX);
}

/**
 * @brief Write 1 command with 8 param into Coprocessor's command fifo
 * 
 * @param cmd Command to send
 * @param w0 1st param
 * @param w1 2nd param
 * @param w2 3rd param
 * @param w3 4th param
 * @param w4 5th param
 * @param w5 6th param
 * @param s  7th param
 * @param len 8th param
 */
void eve_cocmd_dwwwwwwz_s(uint32_t cmd, uint16_t w0, uint16_t w1, uint16_t w2,
		uint16_t w3, uint16_t w4, uint16_t w5, const char *s, uint32_t len) {
	eve_cmd_32(cmd);
	eve_cmd_16(w0);
	eve_cmd_16(w1);
	eve_cmd_16(w2);
	eve_cmd_16(w3);
	eve_cmd_16(w4);
	eve_cmd_16(w5);
	eve_cmd_string(s, len);
}

/**
 * @brief Count number of format argument passed into a string
 * 
 * @param str String input
 * @return uint8_t Number of format argument
 */
static uint8_t count_args(const char *str) {
	uint8_t count = 0;
	const char *tmp = str;

	while ((tmp = strstr(tmp, "%"))) {
		if (*(tmp + 1) == '%') {
			tmp += 2;
		} else {
			count++;
			tmp++;
		}
	}
	return count;
}

/**
 * @brief Send cmd_text into Coprocessor's command fifo
 * 
 * @param x Command parameter
 * @param y Command parameter
 * @param font Command parameter
 * @param options Command parameter
 * @param s String to draw
 */
void eve_cocmd_text(int16_t x, int16_t y, int16_t font, uint16_t options,
		const char *s, ...) {
	va_list args;
	uint8_t num;

	if (font >= 32) {
		if (font != 63)
			printf("Invalid font handle specified: %i\r\n", (int) font);
		return;
	}
	va_start(args, s);
	num = (options & OPT_FORMAT) ? (count_args(s)) : (0); /* Only check % characters if option OPT_FORMAT is set */

	eve_cmd_32(CMD_TEXT);
	eve_cmd_16(x);
	eve_cmd_16(y);
	eve_cmd_16(font);
	eve_cmd_16(options);
	eve_cmd_string(s, EVE_CMD_STRING_MAX);
	for (uint8_t i = 0; i < num; i++)
		eve_cmd_32((uint32_t) va_arg(args, uint32_t));

	va_end(args);
}

/**
 * @brief Send cmd_gradient into Coprocessor's command fifo
 * 
 * @param x0 Command parameter
 * @param y0 Command parameter
 * @param rgb0 Command parameter
 * @param x1 Command parameter
 * @param y1 Command parameter
 * @param rgb1 Command parameter
 */
void eve_cocmd_gradient(int16_t x0, int16_t y0, uint32_t rgb0, int16_t x1,
		int16_t y1, uint32_t rgb1) {
	eve_cocmd_dwwdwwd(CMD_GRADIENT, x0, y0, rgb0, x1, y1, rgb1);
}

/**
 * @brief Send cmd_slider into Coprocessor's command fifo
 * 
 * @param x Command parameter
 * @param y Command parameter
 * @param w Command parameter
 * @param h Command parameter
 * @param options Command parameter
 * @param val Command parameter
 * @param range Command parameter
 */
void eve_cocmd_slider(int16_t x, int16_t y, int16_t w, int16_t h,
		uint16_t options, uint16_t val, uint16_t range) {
	eve_cocmd_dwwwwwww(CMD_SLIDER, x, y, w, h, options, val, range);
}

/**
 * @brief Send cmd_progress into Coprocessor's command fifo
 * 
 * @param x Command parameter
 * @param y Command parameter
 * @param w Command parameter
 * @param h Command parameter
 * @param options Command parameter
 * @param val Command parameter
 * @param range Command parameter
 */
void eve_cocmd_progress(int16_t x, int16_t y, int16_t w, int16_t h,
		uint16_t options, uint16_t val, uint16_t range) {
	eve_cocmd_dwwwwwww(CMD_PROGRESS, x, y, w, h, options, val, range);
}

/**
 * @brief Send cmd_calibrate into Coprocessor's command fifo
 * 
 */
uint32_t eve_cocmd_calibrate() {
	uint16_t resAddr;

	eve_cmd_32(CMD_CALIBRATE);
	resAddr = eve_move_wp(4);
	return 0;
}

/**
 * @brief Send cmd_button into Coprocessor's command fifo
 * 
 * @param x Command parameter
 * @param y Command parameter
 * @param w Command parameter
 * @param h Command parameter
 * @param font Command parameter
 * @param options Command parameter
 * @param s Command parameter
 */
void eve_cocmd_button(int16_t x, int16_t y, int16_t w, int16_t h, int16_t font,
		uint16_t options, const char *s, ...) {
	va_list args;
	uint8_t num;

	if (font >= 32) {
		if (font != 63)
			printf("Invalid font handle specified: %i\r\n", (int) font);
		return;
	}

	va_start(args, s);
	num = (options & OPT_FORMAT) ? (count_args(s)) : (0); /* Only check % characters if option OPT_FORMAT is set */

	eve_cmd_32(CMD_BUTTON);
	eve_cmd_32((((uint32_t) y << 16) | (x & 0xffff)));
	eve_cmd_32((((uint32_t) h << 16) | (w & 0xffff)));
	eve_cmd_32((((uint32_t) options << 16) | (font & 0xffff)));
	eve_cmd_string(s, EVE_CMD_STRING_MAX);
	for (uint8_t i = 0; i < num; i++)
		eve_cmd_32((uint32_t) va_arg(args, uint32_t));

	va_end(args);
}

/**
 * @brief Send cmd_toggle into Coprocessor's command fifo
 * 
 * @param x Command parameter
 * @param y Command parameter
 * @param w Command parameter
 * @param font Command parameter
 * @param options Command parameter
 * @param state Command parameter
 * @param s Command parameter
 */
void eve_cocmd_toggle(int16_t x, int16_t y, int16_t w, int16_t font,
		uint16_t options, uint16_t state, const char *s, ...) {
	va_list args;
	uint8_t num;

	if (font >= 32) {
		if (font != 63)
			printf("Invalid font handle specified: %i\r\n", (int) font);
		return;
	}

	va_start(args, s);
	num = (options & OPT_FORMAT) ? (count_args(s)) : (0); //Only check % characters if option OPT_FORMAT is set

	eve_cmd_32(CMD_TOGGLE);
	eve_cmd_16(x);
	eve_cmd_16(y);
	eve_cmd_16(w);
	eve_cmd_16(font);
	eve_cmd_16(options);
	eve_cmd_16(state);
	eve_cmd_string(s, EVE_CMD_STRING_MAX);
	for (uint8_t i = 0; i < num; i++)
		eve_cmd_32((uint32_t) va_arg(args, uint32_t));

	va_end(args);
}

void eve_cocmd_dlStart() {
	eve_cocmd_d(CMD_DLSTART);
}
void eve_cocmd_swap() {
	eve_cocmd_d(CMD_SWAP);
}
void eve_cocmd_interrupt(uint32_t ms) {
	eve_cocmd_dd(CMD_INTERRUPT, ms);
}
void eve_cocmd_coldStart() {
	eve_cocmd_d(CMD_COLDSTART);
}

/**
 * @brief Do calibration on LCD
 * 
 * @param w Screen w
 * @param h Screen h
 */
uint8_t eve_calibrate(uint32_t w, uint32_t h) {
	printf("App_CoPro_Widget_Calibrate: Start\n");
	eve_wr8(REG_CTOUCH_EXTENDED, CTOUCH_MODE_COMPATIBILITY);

	eve_transfer_begin(EVE_TRANSFER_WRITE, REG_CMDB_WRITE);

	eve_cocmd_dlStart();
	eve_cmd_32(CLEAR_COLOR_RGB(64, 64, 64));
	eve_cmd_32(CLEAR(1, 1, 1));
	eve_cmd_32(COLOR_RGB(0, 0xff, 0xff));
	eve_cocmd_text((uint16_t)(w / 2), (uint16_t)(h / 2), 27, OPT_CENTER,
			"Please Tap on the dot");
	eve_cocmd_calibrate();

	eve_transfer_end();

	eve_wait_flush();
	printf("App_CoPro_Widget_Calibrate: End\n");
	return true;
}

static uint8_t clearScreen_command[12] = {
	0, 0, 0, 2, // GPU instruction CLEAR_COLOR_RGB
	7, 0, 0, 38, // GPU instruction CLEAR
	0, 0, 0, 0, // GPU instruction DISPLAY
};

/**
 * @brief Do clear the screen
 * 
 */
void eve_clear_screen() {
	eve_wr_mem(RAM_DL, clearScreen_command, sizeof(clearScreen_command));
	eve_wr8(REG_DLSWAP, DLSWAP_FRAME);
}

/**
 * @brief Initialize EVE and turn on LCD
 * 
 * @param bootup Bootup parameters
 * @param config Configuration parameters
 */
bool eve_init(eve_bootup *bootup, eve_config *config) {
	uint8_t engines_tatus;
	uint32_t chip_id;
	uint8_t id;

	do {
		//PD_N low and high
		eve_powercycle(1);

		eve_host_command(EVE_EXTERNAL_OSC);
		sleep_ms(10);

		//EVE_SYSCLK_72M
		eve_host_command_ext3((uint32_t) 0x61 | (0x40 << 8) | (0x06 << 8));

		//send host command "ACTIVE" to wake up
		eve_host_command(EVE_ACTIVE_M);
		sleep_ms(300);

		/* Wait for valid chip ID */
		chip_id = eve_rd32(ROM_CHIPID);
		while (EXTRACT_CHIPID(chip_id) < EVE_FT800
				|| EXTRACT_CHIPID(chip_id) > EVE_BT818) {
			sleep_ms(20);
			chip_id = eve_rd32(ROM_CHIPID);
		}
	} while (!chip_id);

	/* ROM_CHIPID is valid across all EVE devices */
	printf("EVE chip id %lx %lx.%lx\r\n", EXTRACT_CHIPID(chip_id),
			((chip_id >> 16) & 0xFF), ((chip_id >> 24) & 0xFF));

	/* Read Register ID to check if EVE is ready. */
	while ((id = eve_rd8(REG_ID)) != 0x7C) {
		sleep_ms(20);
	}
	printf("EVE register ID after wake up %x\r\n", (unsigned int) id);
	(chip_id == eve_rd32(ROM_CHIPID));

	/* Update REG_FREQUENCY as per user selected */
	if (bootup->SystemClock != EVE_SYSCLK_DEFAULT) {
		uint32_t clockMHz = bootup->SystemClock * 12;
		eve_wr32(REG_FREQUENCY, clockMHz * 1000 * 1000);
		printf("EVE clock frequency set to %d MHz\n", (unsigned int) clockMHz);
	}

	/* Read REG_CPURESET to check if engines are ready.
	 Bit 0 for coprocessor engine,
	 Bit 1 for touch engine,
	 Bit 2 for audio engine.
	 */
	while ((engines_tatus = eve_rd8(REG_CPURESET)) != 0x00) {
		if (engines_tatus & 0x01) {
			printf("Coprocessor engine is not ready\r\n");
		}
		if (engines_tatus & 0x02) {
			printf("Touch engine is not ready\r\n");
		}
		if (engines_tatus & 0x04) {
			printf("Audio engine is not ready\r\n");
		}
		sleep_ms(20);
	}
	printf("All engines are ready\r\n");

	/* Turn off display output clock */
	eve_wr8(REG_PCLK, 0);

	eve_wr16(REG_HCYCLE, config->HCycle);
	eve_wr16(REG_HOFFSET, config->HOffset);
	eve_wr16(REG_HSYNC0, config->HSync0);
	eve_wr16(REG_HSYNC1, config->HSync1);
	eve_wr16(REG_VCYCLE, config->VCycle);
	eve_wr16(REG_VOFFSET, config->VOffset);
	eve_wr16(REG_VSYNC0, config->VSync0);
	eve_wr16(REG_VSYNC1, config->VSync1);
	eve_wr8(REG_SWIZZLE, config->Swizzle);
	eve_wr8(REG_PCLK_POL, config->PCLKPol);
	eve_wr16(REG_HSIZE, config->Width);
	eve_wr16(REG_VSIZE, config->Height);
	eve_wr16(REG_CSPREAD, config->CSpread);
	eve_wr16(REG_DITHER, config->Dither);
	eve_wr16(REG_OUTBITS,
			(uint16_t)(
					((config->OutBitsR & 0x7) << 6)
							| ((config->OutBitsG & 0x7) << 3)
							| (config->OutBitsB & 0x7)));

	eve_wr16(REG_PCLK_FREQ, config->PCLKFreq);

	// Drive strenght setting
	eve_wr16(REG_GPIOX_DIR, 0xffff);
	eve_wr16(REG_GPIOX, 0xffff);

	eve_clear_screen();

	/* Setup coprocessor defaults */
	printf("Configure coprocessor defaults\n");
	eve_wr32(REG_CMDB_WRITE, CMD_DLSTART);
	eve_wr32(REG_CMDB_WRITE, CMD_COLDSTART);

	if (!eve_wait_flush())
		return false;

	eve_wr8(REG_PCLK, config->PCLK); /* After this display is visible on the LCD */

	return true;
}

/* end of file */
