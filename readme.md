# Pico-BrtEve

Pico-BrtEve is a set of example application to drive Eve with Pico RP2040.

Application can run on a Pico RP2040 mcu as a C or CircuitPython application.
Or run on a PC as a client connect to Telemetrix server on Pico RP2040 mcu.

### Connections

| RP2040 | EVE | UART | SD |
| --- | --- | --- | --- |
| GP0 (TX) | | RX (debug) | |
| GP1 (RX) | | TX (optional) | |
| GP2 (SPI0 SCK) | SCK | | |
| GP3 (SPI0 MOSI) | MOSI | | |
| GP4 (SPI0 MISO) | MISO | | |
| GP5 (GPIO) | CS | | |
| GP6 (GPIO) | INT | | |
| GP7 (GPIO) | PWD | | |
| GP10 (SPI1 SCK) | | |SCK|
| GP11 (SPI1 MOSI) | | |MOSI|
| GP12 (SPI1 MISO) | | |MISO|
| GP13 (GPIO) | | |CS|
| 5V | 5V | | |
| GND | GND | | |

## Embedded World 2022 -- IDM2040-7A demo circuitPython source code 

Please get it from:  https://github.com/BRTSG-FOSS/pico-brteve/tree/EW2022

## Low-level BridgeTek EVE bindings for CircuitPython 
CircuitPython has the built-in low-level BridgeTek EVE bindings, see: 
https://docs.circuitpython.org/en/latest/shared-bindings/_eve/index.html

## Folder introduction

```
📂 Pico-BrtEve
    ├───c            | Example projects in C language
    ├───circuitPython| Example projects in CircuitPython environment
    ├───CPython      | Example projects in CPython environment     
    ├───james-ref    | A reference to https://github.com/jamesbowman/CircuitPython_bteve.git
    ├───lvgl         | TBD
    ├───tools        | UF2 for Eve converter
```
## Licence
[MIT](LICENSE)
