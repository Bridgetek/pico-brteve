********************************************************************************
*    Demo Ev Charge Point                                                   *
*                                                                              *
********************************************************************************

1. INTRODUCTION
    This is a electric charging station demo application using BT8XX platform.

2. HARDWARE REQUIREMENT
    Below is the list of hardware used in this demo
    - 1 ME817EV
    - 1 MM2040EV
    - 1 LCD LCD 1280x800

3. Flash data
    User have to flash data "BT81X_Flash.bin" into EVE's connected flash, there is 2 way:
    1. Use EAB tool
    2. - Copy "BT81X_Flash.bin" into a sdcard at folder: /EveApps/DemoEvChargePoint/Test/Flash/BT81X_Flash.bin
       - Plug the sdcard into MM2040EV board
       - Start this demo and select "Yes" when see this question on LCD: ""Write BT81X_Flash.bin from sdcard to EVE's connected flash at first?""

4. STARTUP
    - Start MM2040EV with adafruit-circuitpython-raspberry_pi_pico-en_US-6.3.0-rc.0.uf2
    - Copy source code in Python into CircuitPython drive:
        -> lib (which inclide "bteve" folder )
        -> every .py file in this folder
    - Demo should start after that
    
    
                                   【END】