********************************************************************************
*    Demo Camera live stream                                                   *
*                                                                              *
*                                                                              *
********************************************************************************
    This demo show an live stream video on LCD.
    Video is captured by an TTL camera module.

1. Hardware Requirement
    Below is the list of hardware used in this demo
    - 1 ME817EV
    - 1 MM2040EV
    - 1 LCD LCD 1024x600
    - 1 TTL camera (adafru.it/397)

2. Software
    CircuitPython 7.0.0 (https://github.com/adafruit/circuitpython/releases)

3. Wiring

        |------------------------|
        | MM2040EV| Camera       |
        |---------|--------------|
        | 5v      | 5v           |
        | GND     | GND          |
        | GP1     | RX           |
        | GP0     | TX           |
        |------------------------|

4. Startup
    - Copy folder "test" into sdcard
    - Start MM2040EV with adafruit-circuitpython-raspberry_pi_pico-en_US-7.0.0-alpha.3.uf2
    - Copy source code in Python into CircuitPython drive:
        -> lib (which inclide "bteve" folder )
        -> lib (which include "adafruit"folder)
        -> code.py
    - Demo should start after that




