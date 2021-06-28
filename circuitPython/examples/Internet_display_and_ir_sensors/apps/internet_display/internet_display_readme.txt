********************************************************************************
*    Internet data display Demo                                                *
*                                                                              *
*                                                                              *
********************************************************************************

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
    Before wiring, please note that: Sensor module designed to work with power 3.3v, while MM2040EV I2c output 5v as default.
    -> So need to break Qwiic Vin and connect it to 3.3v output on MM2040 (CN11 and CN12 pin 3rd)
       Sensor module can work with 5v as my testing but im not sure its safety
    
    After that:
    - Connect biometric sensor to CN4 (I2C1)
    - Connect Air quality sensor to CN5 (I2C0)
    
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
    | GP01 - CN11 pin 4             | RST          |
    | GP01 - CN11 pin 5             | BUSY         |
    |----------------------------------------------|
    
    See board_mapping_2.jpg for Pin mapping on MM2040EV
        
4. Startup
    - Add wifi SSID and wifi password into apps\internet_display\secrets.py
    - Copy folder "test" into sdcard
    - Start MM2040EV with CircuitPython 6.3.0
    - In the code.py, disable this line: app=ir_sensors_and_gestures(eve)
    - Copy source code in Python into CircuitPython drive:
        -> lib (which inclide "bteve" folder )
        -> lib (which include "adafruit"folder)
        -> apps
        -> Copy and rename code_internet_display.py to code.py
    - Demo should start after that
    
    * If stuck in "Connecting to wifi" screen, try enable TKIP Security Mode in the Wifi Router,
        and restart demo
        
5. Demo Functional:
    - At startup, air quality and biametric data is show on screen
      Data also synchonize in realtime into Adafruit IO
      https://io.adafruit.com/tuan_nguyen_brtchip/dashboards/test1?kiosk=true
      
    - Touch on screen to show an image on internet
      Images are download from this hosting: http://ancuviet.com:
         + http://ancuviet.com/image0006.jpg
         + http://ancuviet.com/image0005.jpg
         + http://ancuviet.com/image0004.jpg
         + http://ancuviet.com/image0003.jpg
         + http://ancuviet.com/image0002.jpg
         + http://ancuviet.com/image0001.jpg
     - Touch on screen again to switch back to sensor data page

    
    
                                [End]