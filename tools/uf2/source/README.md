
# eve_flash_pico

## Introduction
This program is used to program Eve-connected flash via Raspberry Pi Pico.  
It is based on https://github.com/adafruit/tinyuf2. 

## Build 

  1. Use Window 10 OS.
  2. Install cmake 3.19.x, python, Visual Studio 2022 community (must select C++), GNU Arm Embedded Toolchain for window.
  3. Clone pico-sdk:
     ```
      $ git clone --branch 1.3.0 https://github.com/raspberrypi/pico-sdk.git
      $ cd pico-sdk
      $ git submodule update --init
     ```
     
  5. Set environment variable:
      ```
      $ set PICO_SDK_PATH=[PATH-TO pico-sdk]
      $ set PICO_TOOLCHAIN_PATH=C:\\Program Files (x86)\\GNU Arm Embedded Toolchain\\10 2020-q4-major\\bin
      ```
      
  7. Open developer command promt for Visual Studio and run:

   ```
   $ cd pico-brteve\tools\uf2
   $ mkdir build
   $ cd build
   $ set PICO_SDK_PATH=path-to--pico-sdk
   $ set PICO_TOOLCHAIN_PATH=path-to--GNU Arm Embedded Toolchain\\10 2020-q4-major\\bin
   $ "C:\Program Files\CMake\bin\cmake.exe" -G "NMake Makefiles"  -DEVE_APPS_GRAPHICS=BT817 -DEVE_APPS_DISPLAY=WVGA  ../source
   $ nmake eve_flash_pico
   ```

   ```
   Debug mode:
   $ cmake -G "NMake Makefiles" -DEVE_APPS_GRAPHICS=BT817 -DEVE_APPS_DISPLAY=WXGA -DCMAKE_BUILD_TYPE=DEBUG ../
   ```

## Debugging

Every printf message is driven to UART0 - TX (GP1 - CN11 pin 5th)

Use an USB to serial UART device such as FT232 to see printf message


