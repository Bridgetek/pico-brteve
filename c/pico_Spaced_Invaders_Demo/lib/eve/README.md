# EVE-MCU-Examples-BRT_AN_025

## Ports

### PLATFORM Macro

The PLATFORM macro sets the port files to be used by the compiler. The options are as follows:

<table>
    <tr><th>Port Name</th><th>PLATFORM</th><th>Directory</th></tr>
    <tr><td>Bridgetek FT90x</td><td>PLATFORM_FT9XX</td><td>eve_arch_ft9xx</td></tr>
    <tr><td>Bridgetek FT93x</td><td>PLATFORM_FT9XX</td><td>eve_arch_ft9xx</td></tr>
    <tr><td>Beaglebone Black</td><td>PLATFORM_BEAGLEBONE</td><td>eve_arch_beaglebone</td></tr>
    <tr><td>Expressif ESP32</td><td>PLATFORM_ESP32</td><td>eve_arch_esp32</td></tr>
    <tr><td>TI MSP430</td><td>PLATFORM_MSP430</td><td>eve_arch_msp430</td></tr>
    <tr><td>Microchip PIC18F</td><td>PLATFORM_PIC</td><td>eve_arch_pic</td></tr>
    <tr><td>ST STM32</td><td>PLATFORM_STM32</td><td>eve_arch_stm32</td></tr>
    <tr><td>Raspberry Pi</td><td>PLATFORM_RASPBERRYPI</td><td>eve_arch_rpi</td></tr>
    <tr><td>Raspberry Pi Pico</td><td>PLATFORM_RP2040</td><td>eve_arch_rpi</td></tr>
    <tr><td>Generic using libMPSSE</td><td>USE_MPSSE</td><td>eve_libmpsse</td></tr>
</table>

The C source files in each folder are guarded by the PLATFORM_<i>xxx</i> macro.

The Rasberrry Pi platform is suitable for generic Linux by modifying the GPIO pins and device names to match the CPU and board package.
