/**
 @file EVE_MCU.c
 */
/*
 * ============================================================================
 * History
 * =======
 * Nov 2019		Initial version
 *
 *
 *
 *
 *
 *
 *
 * (C) Copyright Bridgetek Pte Ltd
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

// Guard against being used for incorrect CPU type. PIC is compiled with Microchip XC compiler.
#if defined(PLATFORM_PIC)

#pragma message "Compiling " __FILE__ " for Microchip PIC"

#define bswap16(x) (((x) >> 8) | ((x) << 8))
#define bswap32(x) (((x) >> 24) | (((x) & 0x00FF0000) >> 8) \
                  | (((x) & 0x0000FF00) << 8) | ((x) << 24))


#include <xc.h>

#include <EVE.h>
#include <EVE_config.h>
#include <FT8xx.h>
#include <HAL.h>
#include <MCU.h>
#include <string.h>
#include <stdint.h> // for Uint8/16/32 and Int8/16/32 data types

#define _XTAL_FREQ 12000000      // Required for _delay() function, internal OSC Max

// PIC configuration definitions
#pragma config WDTEN = OFF                                                      // Watchdog Timer Enable bit
#pragma config PWRTEN = OFF                                                     // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = EXTMCLR                                                  // MCLR Pin Function Select bit (MCLR pin function is MCLR)
#pragma config CP0 = OFF                                                        // Code Protection Block 0
#pragma config CP1 = OFF                                                        // Code Protection Block 1
#pragma config CP2 = OFF                                                        // Code Protection Block 2
#pragma config CP3 = OFF                                                        // Code Protection Block 3
#pragma config CPB = OFF                                                        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF                                                        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)
#pragma config BOREN = ON                                                       // Brown-out Reset Selection bits (BOR enabled)

#pragma config IESO = ON       // Internal External Switchover enable bit 
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit 
#pragma config PRICLKEN = ON       // 
#pragma config PLLCFG = ON       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)
#pragma config FOSC = 0x03       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)


// This is the MCU specific section and contains the functions which talk to the
// PIC registers. If porting the code to a different PIC or to another MCU, these
// should be modified to suit the registers of the selected MCU.

// ------------------- MCU specific initialisation  ----------------------------
void MCU_Init(void)
{
    ANSELA = 0x00;
    ANSELB = 0x00;
    ANSELC = 0x00;
    ANSELD = 0x00;

    // Port pin set-up
    TRISCbits.TRISC7 = 0;                                                       // CS
    TRISCbits.TRISC6 = 0;                                                       // PD pin
    TRISCbits.TRISC3 = 0;                                                       // SCK
    TRISCbits.TRISC4 = 1;                                                       // SDI (MISO)
    TRISCbits.TRISC5 = 0;                                                       // SDO (MOSI)

    TRISDbits.TRISD7 = 1;                                                       // RXD INPUT
    TRISDbits.TRISD6 = 0;                                                       // TXD OUTPUT
    TRISDbits.TRISD5 = 1;                                                       // RTS INPUT (FROM FT232 RTS))
    TRISDbits.TRISD4 = 0;                                                       // CTS OUTPUT (TO FT232 CTS)

    LATBbits.LATB0 = 1;

    // SPI 1 set-up
    SSP1CON1bits.SSPEN  = 0;                                                    //Disable SPI1

    SSP1CON1bits.WCOL   = 0;
    SSP1CON1bits.SSPOV  = 0;
    SSP1CON1bits.CKP    = 0;
    SSP1CON1bits.SSPM3 = 0;
    SSP1CON1bits.SSPM2 = 0;
    SSP1CON1bits.SSPM1 = 0;
    SSP1CON1bits.SSPM0 = 1;

    SSP1STATbits.SMP = 0;
    SSP1STATbits.CKE = 1;
    SSP1STATbits.BF  = 0;

    SSP1CON1bits.SSPEN  = 1;                                                    // Enable SPI1 after configuration
}

void MCU_Setup(void)
{
//#ifdef FT81X_ENABLE
// Turn on EVE quad-SPI for FT81x devices. PIC18F does not support QSPI
//	MCU_CSlow();
//	MCU_SPIWrite24(MCU_htobe32((REG_SPI_WIDTH << 8) | (1 << 31)));
//	MCU_SPIWrite8(2);
//	MCU_CShigh();

// Turn on FT9xx quad-SPI.
//	spi_option(SPIM, spi_option_bus_width, 4);

//#endif // FT81X_ENABLE

// Turn off SPI buffering. Timing of chip select is critical.
//	spi_option(SPIM, spi_option_fifo, 0);
}


// ########################### GPIO CONTROL ####################################

// --------------------- Chip Select line low ----------------------------------
void MCU_CSlow(void)
{
    LATCbits.LATC7 = 0;                                                         // CS# line low
    Nop();
}  

// --------------------- Chip Select line high ---------------------------------
void MCU_CShigh(void)
{
    Nop();
    LATCbits.LATC7 = 1;                                                         // CS# line high
}

// -------------------------- PD line low --------------------------------------
void MCU_PDlow(void)
{
    LATCbits.LATC6 = 0;                                                         // PD# line low
}

// ------------------------- PD line high --------------------------------------
void MCU_PDhigh(void)
{
    LATCbits.LATC6 = 1;                                                         // PD# line high
}

// --------------------- SPI Send and Receive ----------------------------------
uint8_t MCU_SPIReadWrite8(uint8_t DataToWrite)
{
    uint8_t DataRead = 0;
    
    DataRead = SSP1BUF;                                                         // Dummy read, ensure BF clearedDataRead = SSP1BUF;
    SSP1BUF = (DataToWrite);                                                    // Write data to SPI data register
    while(!SSP1STATbits.BF);                                                    // Wait for completion of the SPI transfer
    DataRead = SSP1BUF;                                                         // Get the value clocked in from the FT8xx

    return DataRead;
}

uint16_t MCU_SPIReadWrite16(uint16_t DataToWrite)
{
    uint16_t DataRead = 0;
    DataRead = MCU_SPIReadWrite8((DataToWrite) >> 8) << 8;
    DataRead |= MCU_SPIReadWrite8((DataToWrite) & 0xff);

    return MCU_be16toh(DataRead);
}

uint32_t MCU_SPIReadWrite24(uint32_t DataToWrite)
{
    uint32_t DataRead = 0;
    uint32_t temp;
    
    temp = (MCU_SPIReadWrite8((DataToWrite) >> 24)); 
    DataRead |= (temp<<24);
    temp = (MCU_SPIReadWrite8((DataToWrite) >> 16));
    DataRead |= (temp<<16);
    temp = (MCU_SPIReadWrite8((DataToWrite) >> 8));
    DataRead |= (temp<<8);

    return MCU_be32toh(DataRead);
}

uint32_t MCU_SPIReadWrite32(uint32_t DataToWrite)
{
    uint32_t DataRead = 0;
    uint32_t temp;
          
    temp = (MCU_SPIReadWrite8((DataToWrite) >> 24));    
    DataRead |= (temp << 24);
    temp = (MCU_SPIReadWrite8((DataToWrite) >> 16));
    DataRead |= (temp << 16);      
    DataRead |= (MCU_SPIReadWrite8((DataToWrite) >> 8) << 8);       
    DataRead |= (MCU_SPIReadWrite8(DataToWrite) & 0xff); 
    
    return MCU_be32toh(DataRead);
}

void MCU_Delay_20ms(void)
{
    __delay_ms(20);                                                             // gives 20ms due to x4 PLL
    __delay_ms(20);                                                             // gives 20ms due to x4 PLL
    __delay_ms(20);                                                             // gives 20ms due to x4 PLL
    __delay_ms(20);                                                             // gives 20ms due to x4 PLL
}

void MCU_Delay_500ms(void)
{
    uint8_t dly = 0;

    for(dly =0; dly < 100; dly++)
    {
        __delay_ms(20);
    }
}

// --------------------- SPI Send and Receive ----------------------------------

uint8_t MCU_SPIRead8(void)
{
	uint8_t DataRead = 0;

	DataRead = MCU_SPIReadWrite8(0);
    
	return DataRead;
}

void MCU_SPIWrite8(uint8_t DataToWrite)
{
	MCU_SPIReadWrite8(DataToWrite);
}

uint16_t MCU_SPIRead16(void)
{
	uint16_t DataRead = 0;

	DataRead = MCU_SPIReadWrite16(0);

	return DataRead;
}

void MCU_SPIWrite16(uint16_t DataToWrite)
{
	MCU_SPIReadWrite16(DataToWrite);
}

uint32_t MCU_SPIRead24(void)
{
	uint32_t DataRead = 0;

	DataRead = MCU_SPIReadWrite24(0);

	return DataRead;
}

void MCU_SPIWrite24(uint32_t DataToWrite)
{
	MCU_SPIReadWrite24(DataToWrite);
}

uint32_t MCU_SPIRead32(void)
{
	uint32_t DataRead = 0;

	DataRead = MCU_SPIReadWrite32(0);

	return DataRead;
}

void MCU_SPIWrite32(uint32_t DataToWrite)
{
	MCU_SPIReadWrite32(DataToWrite);
}

void MCU_SPIWrite(const uint8_t *DataToWrite, uint32_t length)
{
	//spi_writen(SPIM, DataToWrite, length);

    uint16_t DataPointer = 0;   
    DataPointer = 0;

    while(DataPointer < length)
    {
        MCU_SPIWrite8(DataToWrite[DataPointer]);                                       // Send data byte-by-byte from array
        DataPointer ++;
    }
}

uint16_t MCU_htobe16 (uint16_t h)
{
    return h;
}

uint32_t MCU_htobe32 (uint32_t h)
{
    return h;
}

uint16_t MCU_htole16 (uint16_t h)
{
        return bswap16(h); 
}

uint32_t MCU_htole32 (uint32_t h)
{
        return bswap32(h);
}

uint16_t MCU_be16toh (uint16_t h)
{
    return h;
}
uint32_t MCU_be32toh (uint32_t h)
{
     return h;
}

uint16_t MCU_le16toh (uint16_t h)
{
        return bswap16(h); 
}

uint32_t MCU_le32toh (uint32_t h)
{
        return bswap32(h);
}

#endif /* defined(PLATFORM_PIC) */
