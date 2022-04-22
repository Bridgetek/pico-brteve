/**
 @file MCU.h
 */
/*
 * ============================================================================
 * History
 * =======
 * Nov 2019		Initial beta for FT81x and FT80x
 * Mar 2020		Updated beta - added BT815/6 commands
 * Mar 2021		Beta with BT817/8 support added
 *
 *
 *
 *
 *
 * (C) Copyright,  Bridgetek Pte. Ltd.
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd
 * ("Bridgetek") subject to the licence terms set out
 * http://www.ftdichip.com/FTSourceCodeLicenceTerms.htm ("the Licence Terms").
 * You must read the Licence Terms before downloading or using the Software.
 * By installing or using the Software you agree to the Licence Terms. If you
 * do not agree to the Licence Terms then do not download or use the Software.
 *
 * Without prejudice to the Licence Terms, here is a summary of some of the key
 * terms of the Licence Terms (and in the event of any conflict between this
 * summary and the Licence Terms then the text of the Licence Terms will
 * prevail).
 *
 * The Software is provided "as is".
 * There are no warranties (or similar) in relation to the quality of the
 * Software. You use it at your own risk.
 * The Software should not be used in, or for, any medical device, system or
 * appliance. There are exclusions of Bridgetek liability for certain types of loss
 * such as: special loss or damage; incidental loss or damage; indirect or
 * consequential loss or damage; loss of income; loss of business; loss of
 * profits; loss of revenue; loss of contracts; business interruption; loss of
 * the use of money or anticipated savings; loss of information; loss of
 * opportunity; loss of goodwill or reputation; and/or loss of, damage to or
 * corruption of data.
 * There is a monetary cap on Bridgetek's liability.
 * The Software may have subsequently been amended by another user and then
 * distributed by that other user ("Adapted Software").  If so that user may
 * have additional licence terms that apply to those amendments. However, Bridgetek
 * has no liability in relation to those amendments.
 * ============================================================================
 */

#ifndef MCU_HEADER_H
#define	MCU_HEADER_H

#include <stdint.h> // for Uint8/16/32 and Int8/16/32 data types

/**
 @brief MCU specific initialisation
 @details Must contain any MCU-specific initialisation. This will typically be
 	 setting up the SPI bus, GPIOs and operating environment requirements.
 */
void MCU_Init(void);

/**
 @brief MCU specific setup
 @details Called after the EVE has been power cycled and started. Contains
 	 any MCU-specific configuration options for the EVE.
 */
void MCU_Setup(void);

/**
 @brief MCU specific chip select enable
 @details This function will pull the chip select line to the EVE low to
 	 allow data transmission on the SPI bus.
 	 The EVE requires chip select to toggle frequently.
 */
void MCU_CSlow(void);

/**
 @brief MCU specific chip select deassert
 @details This function will pull the chip select line to the EVE high to
 	 prevent data transmission on the SPI bus.
 	 The EVE requires chip select to toggle frequently.
 */
void MCU_CShigh(void);

/**
 @brief MCU specific power down enable
 @details This function will pull the power down line to the EVE low to
 	 force the device into power down mode.
 	 This will be done during EVE initialisation and can be done to allow
 	 deep power saving.
 */
void MCU_PDlow(void);

/**
 @brief MCU specific power down disable
 @details This function will pull the power down line to the EVE high to
 	 enable normal operation of the EVE.
 	 This will be done during EVE initialisation and can be done to allow
 	 recovery from deep power saving.
 */
void MCU_PDhigh(void);

/**
 @brief MCU specific SPI write
 @details Performs an SPI write of the data block and discards the data
 	 received in response.
 @param DataToWrite - pointer to buffer to write.
 @param length - number of bytes to write.
 */
void MCU_SPIWrite(const uint8_t *DataToWrite, uint32_t length);

/**
 @brief MCU specific SPI 8 bit read
 @details Performs an SPI dummy write and returns the data received in
    	 response.
 @returns Data received from EVE.
 */
uint8_t MCU_SPIRead8(void);

/**
 @brief MCU specific SPI 8 bit write
 @details Performs an SPI write and discards the data received in
    	 response.
 @param Data to write to EVE.
 */
void MCU_SPIWrite8(uint8_t DataToWrite);

/**
 @brief MCU specific SPI 16 bit read
 @details Performs an SPI dummy write and returns the data received in
    	 response.
 @returns Data received from EVE.
 */
uint16_t MCU_SPIRead16(void);

/**
 @brief MCU specific SPI 16 bit write
 @details Performs an SPI write and discards the data received in
    	 response.
 @param Data to write to EVE.
 */
void MCU_SPIWrite16(uint16_t DataToWrite);

/**
 @brief MCU specific SPI 24 bit read
 @details Performs an SPI dummy write and returns the data received in
    	 response.
 @returns Data received from EVE.
 */
uint32_t MCU_SPIRead24(void);

/**
 @brief MCU specific SPI 24 bit write
 @details Performs an SPI write and discards the data received in
    	 response.
 @param Data to write to EVE.
 */
void MCU_SPIWrite24(uint32_t DataToWrite);

/**
 @brief MCU specific SPI 32 bit read
 @details Performs an SPI dummy write and returns the data received in
    	 response.
 @returns Data received from EVE.
 */
uint32_t MCU_SPIRead32(void);

/**
 @brief MCU specific SPI 32 bit write
 @details Performs an SPI write and discards the data received in
    	 response.
 @param Data to write to EVE.
 */
void MCU_SPIWrite32(uint32_t DataToWrite);

/**
 @brief MCU specific 20 ms delay
 @details Cause the MCU to idle or otherwise delay for a minimum of
 	 20 milliseconds. This is used during initialisation to perform a
 	 power down of the EVE for a controlled minimum period of time.
 */
void MCU_Delay_20ms(void);

/**
 @brief MCU specific 500 ms delay
 @details Cause the MCU to idle or otherwise delay for a minimum of
 	 500 milliseconds. This is used during initialisation to perform a
 	 power down of the EVE for a controlled minimum period of time.
 */
void MCU_Delay_500ms(void);

/**
 @brief MCU specific byte swapping routines
 @details EVE addresses from the HAL_SetReadAddress and HAL_SetWriteAddress
 	 are sent in big-endian format. However, data for registers or memory
 	 mapped areas are in little-endian format.
 */
//@{
uint16_t MCU_htobe16(uint16_t h);
uint32_t MCU_htobe32(uint32_t h);
uint16_t MCU_htole16(uint16_t h);
uint32_t MCU_htole32(uint32_t h);
uint16_t MCU_be16toh(uint16_t h);
uint32_t MCU_be32toh(uint32_t h);
uint16_t MCU_le16toh(uint16_t h);
uint32_t MCU_le32toh(uint32_t h);
//@}

#endif	/* MCU_HEADER_H */
