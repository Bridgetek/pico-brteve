********************************************************************************
*    Internet data display Demo                                                *
*                                                                              *
*                                                                              *
********************************************************************************
    This demo display data from sensor into LCD and sync with cloud in realtime.
    It also download data (an image) from internet and show on LCD.

1. Hardware Requirement
    Below is the list of hardware used in this demo
    - 1 ME817EV
    - 1 Micro SDcard (to store image downloaded from internet)
    - 1 MM2040EV
    - 1 LCD 1024x600
    - 1 SparkFun Pulse Oximeter and Heart Rate Sensor - MAX30101 & MAX32664 (Qwiic)
        https://www.sparkfun.com/products/15219
    - 1 SparkFun Environmental Combo Breakout - CCS811/BME280 (Qwiic)
        https://www.sparkfun.com/products/14348
    - 1 Adafruit AirLift – ESP32 WiFi Co-Processor Breakout Board
        adafru.it/4201

2. Software
    CircuitPython 6.3.0 (https://github.com/adafruit/circuitpython/releases)

3. Wiring
    - Connect biometric sensor to CN5 (I2C0 - GP18, GP19)
    - Connect Air quality sensor to CN4 (I2C1 - GP20, GP21)

   - MM2040EV and ESP32 WiFi module:
    |----------------------------------------------|
    | MM2040EV                      | WiFi module  |
    |-------------------------------|--------------|
    | 5v                            | VIN          |
    | GND                           | GND          |
    | GP10 - CN13 pin 4 - SPI1 SCK  | SCK          |
    | GP11 - CN13 pin 6 - SPI1 MOSI | MOSI         |
    | GP12 - CN13 pin 5 - SPI1 MISO | MISO         |
    | GP22 - CN13 pin 7 - SPI1 CS   | CS           |
    | GP00 - CN11 pin 4             | RST          |
    | GP01 - CN11 pin 5             | BUSY         |
    |----------------------------------------------|

4. Startup
    - Build a dashboard on io.adafruit.com, with below feeds name:
        * CO2
        * heartrate
        * Humidity
        * oxygen
        * Pressure
        * temp
        * TVOC
        See an example at https://io.adafruit.com/tuan_nguyen_brtchip/dashboards/test1?kiosk=true

    - Add your wifi SSID, wifi password and AIO keys into secrets.py

    - Start MM2040EV with adafruit-circuitpython-raspberry_pi_pico-en_US-6.3.0-rc.0.uf2
    - Copy source code in Python into CircuitPython drive:
        -> lib (which inclide "bteve" folder )
        -> examples\internet_data_display\lib (which include "adafruit"folder)
        -> sensors
        -> code.py, internet_data_display.py, page1_ui.py, page2_ui.py, secrets.py, __init__.py
    - Demo should start after that

    * If stucking in "Connecting to wifi" screen, try to enable TKIP Security Mode in your Wifi Router,
        and restart demo

5. Demo Functionality:
    - At startup, air quality and biametric data is show on screen
      Data also synchonize in realtime into Adafruit IO:
      https://io.adafruit.com/tuan_nguyen_brtchip/dashboards/test1?kiosk=true

    - Touch on screen to show an image on internet
      Images are downloaded from this folder: https://github.com/BRTSG-FOSS/pico-brteve/tree/main/circuitPython/examples/internet-data-display/images/
         + image0006.jpg
         + image0005.jpg
         + image0004.jpg
         + image0003.jpg
         + image0002.jpg
         + image0001.jpg
     - Touch "Back" button to switch back to sensor data page



                                [End]