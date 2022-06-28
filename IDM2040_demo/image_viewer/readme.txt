********************************************************************************
*    Demo Image Slide Viewer                                                   *
*                                                                              *
*                                                                              *
********************************************************************************
    This demo load an image from SDcard and show on the screen.
    User can swipe image via gesture sensor (APDS-9660), or IR receiver, or swipe on LCD.

1. Hardware Requirement
    Below is the list of hardware used in this demo
    - 1 ME817EV
    - 1 Micro SDcard
    - 1 MM2040EV
    - 1 LCD LCD 1024x600
    - 1 IR remote + receiver (https://www.adafruit.com/product/157)
    - 1 APDS9960 Sensor (https://www.adafruit.com/product/3595)

2. Software
    CircuitPython 6.3.0 (https://github.com/adafruit/circuitpython/releases)
    * Router setup: Encryption type must be TKIP

3. Wiring
   - MM2040EV and APDS9960:
        Connect APDS9960 STEMMA connector into MM2040EV Cn5 (I2C0)

   - MM2040EV and IR receiver:

        |------------------------|
        | MM2040EV| IR           |
        |---------|--------------|
        | 5v      | 5v   (pin 1) |
        | GND     | GND  (pin 2) |
        | GP1     | V.out(pin 3) |
        |------------------------|

        See IR receiver pin diagram at: https://learn.adafruit.com/ir-sensor/testing-an-ir-sensor

4. Startup
    - Copy folder "test" into a micro sdcard and plugin the sdcard
    - Start MM2040EV with adafruit-circuitpython-raspberry_pi_pico-en_US-7.0.0-alpha.3.uf2
    - Copy source code in Python into CircuitPython drive:
        -> lib (which inclide "bteve" folder )
        -> lib (which include "adafruit"folder)
        -> apps
        -> Copy and rename code_ir_sensors_and_gestures.py to code.py
    - Demo should start after that

5. Demo Functionality:
    - Wave hand left/right to swipe image left/right
      *Gesture detection range is only up to a few centimeters

    - Push button left/right on IR controller to swipe image
    - Swipe on LCD left/right to swipe image




