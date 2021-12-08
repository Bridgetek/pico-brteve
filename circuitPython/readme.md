# CircuitPython support for Raspberry Pi Pico with Eve module from Brdgetek Pte Ltd

This repo is derived from the following repo:
https://github.com/jamesbowman/CircuitPython_bteve


## lib
This folder contains the necessary files to use Eve device by Raspberry Pi Pico.
It provides a python class to control Eve via SPI.

## Examples
The circuitPython code to showcase the usage of graphics features from Eve.
To run an example on CircuitPython:
- Install CircuitPython 7.0.0, follow this instruction:
https://learn.adafruit.com/intro-to-rp2040-pio-with-circuitpython/installing-circuitpython
- Copy source code in Python into CircuitPython drive:
-> lib (which inclide "bteve" folder )
-> Any .py file inside Examples folder

## Telemetrix_examples
The python code to showcase the usage of graphics features from Eve via Telemetrix.
https://github.com/MrYsLab/Telemetrix4Connect2040

To run an Telemetrix example:
```sh
cd telemetrix_examples
py -3 [example].py
```

### Pico modules to work with Eve modules:

- MM2040EV
- ID2040

Both modules defines the same connections with EVE:
```sh
PICO GP2 <--> Eve SCK
PICO GP3 <--> Eve MOSI
PICO GP4 <--> Eve MISO
PICO GP5 <--> Eve CS#
PICO GP7 <--> Eve PDN#
PICO Power <--> Eve Board power
PICO GND <--> Eve Board GND
```
SD card is accessed from Eve SPI but with different CS pin:
```sh
PICO GP13 <---> SD CS#
```

The connection with :
```sh
PICO GP8 <---> D/CX pin of ILI9488
PICO GP9 <---> CSX pin of ILI9488
```

### Eve - Embedded Video Engine
An extreme easy-to-use yet powerful GPU with SPI interface to MCU. See more details:
https://brtchip.com/bt81x/

![image](https://user-images.githubusercontent.com/13127756/110600563-06ef1300-81bf-11eb-87c9-c75d55c7d02a.png)



