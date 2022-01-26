# eve_flash_pico

## Introduction
This program is used to program Eve-connected flash via Raspberry Pi Pico.  
It is based on https://github.com/adafruit/tinyuf2. 

## Build 

Need prepare pico-SDK from https://github.com/raspberrypi/pico-sdk version 1.3.0

EveApps from this version: https://github.com/BRTSG-FOSS/EveApps/commit/5c6975de0709826cab0286ec52c06ef76f56faf7

Open Developer command promt for VS2019

Build release:

```
cd pico-brteve\tools\uf2\source
git clone https://github.com/BRTSG-FOSS/EveApps
cd EveApps && git checkout 5c6975de0709826cab0286ec52c06ef76f56faf7 && cd ../
mkdir build
cd build
cmake -G "NMake Makefiles" -DEVE_APPS_PLATFORM=RP2040 -DEVE_APPS_GRAPHICS=BT817 -DEVE_APPS_DISPLAY=WXGA  ../
nmake eve_flash_pico
```

Build with debug flag:

```
cmake -G "NMake Makefiles" -DEVE_APPS_PLATFORM=RP2040 -DEVE_APPS_GRAPHICS=BT817 -DEVE_APPS_DISPLAY=WXGA -DCMAKE_BUILD_TYPE=DEBUG ../
```

## Debugging

Every printf message is driven to UART0 - TX (GP1 - CN11 pin 5th)

Use an USB to serial UART device such as FT232 to see printf message


