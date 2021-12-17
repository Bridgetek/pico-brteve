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

## License
Copyright (c) 2012-2021 Scott Chacon and others

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
