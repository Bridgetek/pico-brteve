# eve_flash_pico

## Introduction
This program is used to program Eve-connected flash via Raspberry Pi Pico.  It is based on https://github.com/adafruit/tinyuf2. 

## Build 
Need prepare pico-SDK from https://github.com/raspberrypi/pico-sdk

Open Developer command promt for VS2019
```
$ cd pico-brteve\tools\uf2\source
$ git clone https://github.com/BRTSG-FOSS/EveApps
$ git clone https://github.com/hathach/tinyusb
$ mkdir build
$ cd build
$ cmake -G "NMake Makefiles" -DEVE_APPS_PLATFORM=RP2040  ../
$ nmake
```


