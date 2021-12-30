# eve_flash_pico

## Introduction
This program is used to program Eve-connected flash via Raspberry Pi Pico.  It is based on https://github.com/adafruit/tinyuf2. 

## Build 
Need prepare pico-SDK from https://github.com/raspberrypi/pico-sdk version 1.3.0
EveApps from this version: https://github.com/BRTSG-FOSS/EveApps/commit/5c6975de0709826cab0286ec52c06ef76f56faf7

Open Developer command promt for VS2019
```
cd pico-brteve\tools\uf2\source
git clone https://github.com/BRTSG-FOSS/EveApps
git clone https://github.com/hathach/tinyusb
mkdir build
cd build
cmake -G "NMake Makefiles" -DEVE_APPS_PLATFORM=RP2040  ../
nmake
```


