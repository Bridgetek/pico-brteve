/**
 @file EVE_Linux_RPi.c
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
#if defined(PLATFORM_RASPBERRYPI)

#pragma message "Compiling " __FILE__ " for Raspberry Pi"

#include <string.h>
#include <stdio.h>
#include <stdint.h> // for Uint8/16/32 and Int8/16/32 data types
#include <stdlib.h>
#include <unistd.h>
#include <endian.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

// Powerdown pin
/*
	printf ("J8 Pin 19 - MOSI (SPI0_D1)\n");
	printf ("J8 Pin 21 - MISO (SPI0_D0)\n");
	printf ("J8 Pin 23 - SCLK (SPI0_SCLK)\n");
	printf ("J8 Pin 22 - CS (GPIO25) - Note this is not the SPI0_CS0 pin\n");
	printf ("J8 Pin 18 - PD# (GPIO24) - Powerdown pin\n");
*/
#define PIN_NUM_PD   24
#define PIN_NUM_CS   25

#define str(s) xstr(s)
#define xstr(s) #s

// This is the Linux Platform specific section and contains the functions which
// enable the GPIO and SPI interfaces.

// ------------------ Platform specific initialisation  ------------------------

static int spiHandle = 0;

const char *SPI_device = "/dev/spidev0.0";
const char *GPIO_export="/sys/class/gpio/export";
const char *GPIO_unexport="/sys/class/gpio/unexport";
const char *GPIO_pd_dir="/sys/class/gpio/gpio" str(PIN_NUM_PD) "/direction";
const char *GPIO_cs_dir="/sys/class/gpio/gpio" str(PIN_NUM_CS) "/direction";
const char *GPIO_pd_val="/sys/class/gpio/gpio" str(PIN_NUM_PD) "/value";
const char *GPIO_cs_val="/sys/class/gpio/gpio" str(PIN_NUM_CS) "/value";

void Platform_Init(void)
{
    FILE *hGPIO = NULL;

    spiHandle = open(SPI_device, O_RDWR);
    if (spiHandle == -1)
    {
        printf("Please make sure /dev/spidev1.0 is enabled. The raspi-config\n");
        printf("configuration tool can enable \"SPI\" under the section\n");
        printf("\"Interfacing Options\". \n");
        exit(-1);
    }
    // Initialize SPIM HW
    uint8_t lsb = 0;
    ioctl(spiHandle, SPI_IOC_WR_LSB_FIRST, &lsb );
    uint32_t speed = 1000000;
    ioctl(spiHandle, SPI_IOC_WR_MAX_SPEED_HZ, &speed );
    uint32_t mode = SPI_MODE_0;
    ioctl(spiHandle, SPI_IOC_WR_MODE, &mode );

    if ((hGPIO = fopen(GPIO_unexport, "w")) != NULL)
    {
        fwrite(str(PIN_NUM_PD), sizeof(char), strlen(str(PIN_NUM_PD)), hGPIO);
        fwrite(str(PIN_NUM_CS), sizeof(char), strlen(str(PIN_NUM_CS)), hGPIO);
        fclose(hGPIO);
        hGPIO = NULL;
    }

    if ((hGPIO = fopen(GPIO_export, "w")) != NULL)
    {
        fwrite(str(PIN_NUM_PD), sizeof(char), strlen(str(PIN_NUM_PD)), hGPIO);
        fclose(hGPIO);
    }
    if ((hGPIO = fopen(GPIO_export, "w")) != NULL)
    {
        fwrite(str(PIN_NUM_CS), sizeof(char), strlen(str(PIN_NUM_CS)), hGPIO);
        fclose(hGPIO);
    }
    hGPIO = NULL;

    if ((hGPIO = fopen(GPIO_pd_dir, "r+")) != NULL)
    {
        fwrite("out", sizeof(char), 3, hGPIO);
        fclose(hGPIO);
        hGPIO = NULL;

        if ((hGPIO = fopen(GPIO_pd_val, "r+")) != NULL)
        {
            // All good.
            fclose(hGPIO);
        }
    }
    else
    {
        printf("failed to set GPIO direction %d!\n", PIN_NUM_PD);
    }

    if ((hGPIO = fopen(GPIO_cs_dir, "r+")) != NULL)
    {
        fwrite("out", sizeof(char), 3, hGPIO);
        fclose(hGPIO);
        hGPIO = NULL;

        if ((hGPIO = fopen(GPIO_cs_val, "r+")) != NULL)
        {
            // All good.
            fclose(hGPIO);
        }
    }
    else
    {
        printf("failed to set GPIO chip select %d!\n", PIN_NUM_CS);
    }

    if (hGPIO == NULL)
    {
        printf("failed to export GPIO %d. Make sure you run this with \"sudo\"\n", PIN_NUM_PD);
        exit(-1);
    }
}

void Platform_Setup(void)
{
}

int Platform_SPI_transfer(struct spi_ioc_transfer *xfer, int count)
{
    return (ioctl(spiHandle, SPI_IOC_MESSAGE(count), xfer));
}

// ########################### GPIO CONTROL ####################################

// --------------------- Chip Select line low ----------------------------------
void Platform_CSlow(void)
{
    FILE *h1;

    if ((h1 = fopen(GPIO_cs_val, "r+")) != NULL)
    {
        fwrite("0", sizeof(char), 1, h1);
        fclose(h1);
    }
}

// --------------------- Chip Select line high ---------------------------------
void Platform_CShigh(void)
{
    FILE *h1;

    if ((h1 = fopen(GPIO_cs_val, "r+")) != NULL)
    {
        fwrite("1", sizeof(char), 1, h1);
        fclose(h1);
    }
}

// -------------------------- PD line low --------------------------------------
void Platform_PDlow(void)
{
    FILE *h1;

    if ((h1 = fopen(GPIO_pd_val, "r+")) != NULL)
    {
        fwrite("0", sizeof(char), 1, h1);
        fclose(h1);
    }
}

// ------------------------- PD line high --------------------------------------
void Platform_PDhigh(void)
{
    FILE *h1;

    if ((h1 = fopen(GPIO_pd_val, "r+")) != NULL)
    {
        fwrite("1", sizeof(char), 1, h1);
        fclose(h1);
    }
}

// ------------------------- Delay functions -----------------------------------

void Platform_Delay_20ms(void)
{
    usleep(20 * 1000);
}

void Platform_Delay_500ms(void)
{
    usleep(500 * 1000);
}

// Beaglebone is Little Endian.
// Use toolchain defined functions.
uint16_t Platform_htobe16(uint16_t h)
{
    return htobe16(h);
}

uint32_t Platform_htobe32(uint32_t h)
{
    return htobe32(h);
}

uint16_t Platform_htole16(uint16_t h)
{
    return htole16(h);
}

uint32_t Platform_htole32(uint32_t h)
{
    return htole32(h);
}

uint16_t Platform_be16toh(uint16_t h)
{
    return be16toh(h);
}

uint32_t Platform_be32toh(uint32_t h)
{
    return be32toh(h);
}

uint16_t Platform_le16toh(uint16_t h)
{
    return le16toh(h);
}

uint32_t Platform_le32toh(uint32_t h)
{
    return le32toh(h);
}

#endif /* (PLATFORM_RASPBERRYPI) */

