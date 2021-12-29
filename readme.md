# Pico-BrtEve

Pico-BrtEve is a set of example application to drive Eve with Pico RP2040.

Application can run on a Pico RP2040 mcu as a C or CircuitPython application.
Or run on a PC as a client connect to Telemetrix server on Pico RP2040 mcu.

#### Connections

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
| 5V | 5V | | |
| GND | GND | | |


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
