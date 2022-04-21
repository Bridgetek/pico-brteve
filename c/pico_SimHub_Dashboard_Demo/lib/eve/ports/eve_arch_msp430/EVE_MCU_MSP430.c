/**
 @file EVE_MCU_MSP430.c
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

// Guard against being used for incorrect CPU type.
#if defined(PLATFORM_MSP430)

#pragma message "Compiling " __FILE__ " for TI MSP430"

/* Replace with header file for target MCU */
#include <msp430g2553.h>

#include "..\include\EVE.h"
#include "..\include\EVE_config.h"
#include "..\include\FT8xx.h"
#include "..\include\HAL.h"
#include "..\include\MCU.h"
#include <stdint.h> // for Uint8/16/32 and Int8/16/32 data types


#define Nop() __no_operation()
#define MISO            BIT1                // P1.1 is SPI MISO
#define MOSI            BIT2                // P1.2 is SPI MOSI
#define PD              BIT3                // P1.3 is Power Down
#define SCLK            BIT4                // P1.4 is SPI clock
#define CS              BIT5                // P1.5 is Chip Select
#define CPU_FREQ        1000                // for use in delay function

void initClock()
{
    DCOCTL = 0;                                 // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_1MHZ;                      // Set DCO for 1MHz (1/8/12/16 available)
    DCOCTL = CALDCO_1MHZ;
}

void initSPI()
{
    /* set pins to be used for CS# and PD# */
    P1OUT |= CS + PD;                       //p1.5 - #CS, p1.3 - #PD
    P1DIR |= CS + PD;

    /* set SPI pins in both P1SEL and P1SEL2 as to work with Universal Serial Communication Interface*/
    P1SEL = MISO | MOSI | SCLK;             // p1.1 MISO, p1.2 MOSI, P1.4 SCLK
    P1SEL2 = MISO | MOSI | SCLK;

    /* configure UCA0 for SPI */
    UCA0CTL1 = UCSWRST;
    UCA0CTL0 |= UCCKPH + UCMSB + UCMST + UCSYNC;     // 3-pin, 8-bit SPI master (mode 0)
    UCA0CTL1 |= UCSSEL_2;                   // USCI clock source = SMCLK
    UCA0BR0 |= 0x02;                        // divide SMCLK by 2 in baud rate control register
    UCA0BR1 = 0;                            // = 0.5 mhz spi
    UCA0MCTL = 0;                           // No modulation
    UCA0CTL1 &= ~UCSWRST;                   // **Initialise USCI state machine** - restart module

    IE2 |= UCA0RXIE;                        // Enable USCI0 RX interrupt
}

/* configure MCU, SPI and PD pins */
void MCU_Init(void){

    WDTCTL = WDTPW | WDTHOLD;               // stop watch dog timer

    initClock();                            // set Clocks
    initSPI();                              // configure SPI

    __enable_interrupt();                   // enable interrupts

}

void MCU_Setup(void)
{
/* function to configure QSPI*/
//#ifdef FT81X_ENABLE

//#endif // FT81X_ENABLE
}


// ########################### SPI Send and Receive ####################################
// --------------------- Global variables for SPI data ----------------------------------
volatile uint8_t DataRead;      // Stores slave data

// --------------------- SPI RX ISR ----------------------------------
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI_SPIRX_ISR(void)
{
    if (IFG2 & UCA0RXIFG){      // check if SPI receiver interrupt flag is set
      DataRead = UCA0RXBUF;     //Store value clocked in from FT8xx from SPI Rx buffer (auto resets UCA0RxIFG)
    }
}
// --------------------- SPI Read/Write 8 bits ----------------------------------
uint8_t MCU_SPIReadWrite8(uint8_t DataToWrite)
{

    while (!(IFG2 & UCA0TXIFG));            // USCI_A0 TX (SPI) buffer ready?
    UCA0TXBUF = DataToWrite;                // Send data to FT8xx by writing data to SPI buffer
    IFG2 = UCA0RXIFG;                       // trigger UCA0RXIFG to receive SPI data

    return DataRead;
}

// --------------------- SPI Read/Write 16 bits ----------------------------------
uint16_t MCU_SPIReadWrite16(uint16_t DataToWrite)
{
    uint16_t DataRead = 0;
    DataRead = MCU_SPIReadWrite8((DataToWrite) >> 8) << 8;
    DataRead |= MCU_SPIReadWrite8((DataToWrite) & 0xff);

    return DataRead;
}

// --------------------- SPI Read/Write 24 bits ----------------------------------
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

    return DataRead;
}

// --------------------- SPI Read/Write 32 bits ----------------------------------
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

    return DataRead;
}

// --------------------- SPI Read 8 bits ----------------------------------
uint8_t MCU_SPIRead8(void)
{
    uint8_t DataRead = 0;

    DataRead = MCU_SPIReadWrite8(0);

    return DataRead;
}

// --------------------- SPI Write 8 bits ----------------------------------
void MCU_SPIWrite8(uint8_t DataToWrite)
{
    MCU_SPIReadWrite8(DataToWrite);
}

// --------------------- SPI Read 16 bits ----------------------------------
uint16_t MCU_SPIRead16(void)
{
    uint16_t DataRead = 0;

    DataRead = MCU_SPIReadWrite16(0);

    return DataRead;
}

// --------------------- SPI Write 16 bits ----------------------------------
void MCU_SPIWrite16(uint16_t DataToWrite)
{
    MCU_SPIReadWrite16(DataToWrite);
}

// --------------------- SPI Read 24 bits ----------------------------------
uint32_t MCU_SPIRead24(void)
{
    uint32_t DataRead = 0;

    DataRead = MCU_SPIReadWrite24(0);

    return DataRead;
}

// --------------------- SPI Write 24 bits ----------------------------------
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

// --------------------- SPI Read/Write 32 bits ----------------------------------
void MCU_SPIWrite32(uint32_t DataToWrite)
{
    MCU_SPIReadWrite32(DataToWrite);
}

// --------------------- SPI burst write ----------------------------------
void MCU_SPIWrite(const uint8_t *DataToWrite, uint32_t length)
{
    uint16_t DataPointer = 0;
    DataPointer = 0;

    while(DataPointer < length)
    {
        MCU_SPIWrite8(DataToWrite[DataPointer]);  // Send data byte-by-byte from array
        DataPointer ++;
    }
}

// ########################### GPIO CONTROL ####################################

// --------------------- Chip Select line low ----------------------------------
inline void MCU_CSlow(void)
{
    P1OUT &= (~CS);                       // CS# line low
    __delay_cycles(10);
}

// --------------------- Chip Select line high ---------------------------------
inline void MCU_CShigh(void)
{
    __delay_cycles(10);
    P1OUT |= (CS);                        // CS# line high
}

// -------------------------- PD line low --------------------------------------
inline void MCU_PDlow(void)
{
    P1OUT &= (~PD);                       // PD# line low
}

// ------------------------- PD line high --------------------------------------
inline void MCU_PDhigh(void)
{
    P1OUT |= (PD);                        // PD# line high
}

// ------------------- msec delay based on MCLK (CPU_FREQ) ----------------------
void delay(int msec){
    //could use a timer here
    while(msec)
    {
        __delay_cycles(CPU_FREQ);
        msec--;
    }
}
// ----------------------------- 20ms delay --------------------------------------
void MCU_Delay_20ms(void)
{
    delay(20);
}

// ----------------------------- 500ms delay -------------------------------------
void MCU_Delay_500ms(void)
{
    delay(500);
}

// ########################### ENDIAN CONVERSION ####################################

uint32_t bswap32(uint32_t x)
{
    uint32_t s;
    s = ((x) >> 24);
    s |= (((x) & 0x00FF0000) >> 8);
    s |= (((x) & 0x0000FF00) << 8);
    s |= ((x) << 24);

    return s;
}

uint16_t bswap16(uint16_t x)
{
    uint16_t s;
    s = ((x) >> 8);
    s |= ((x) << 8);

    return s;
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

#endif /* defined(PLATFORM_MSP430) */
