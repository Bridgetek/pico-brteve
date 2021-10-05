
## INTRODUCTION
This is a demo for a movie player from flash and SD card

Video is stored in the sdcard, user can choose to play video directly on SD card, or on EVE's connected flash

Video path on SD card is /pico-brteve/circuitPython/examples/video2/trailer_400p_20s.avi

On EVE's connected flash, video is stored at address 4096


## HARDWARE 
- 1 ME817EV
- 1 MM2040EV
- 1 LCD 1280x800 or LCD 800x480

## STARTUP
- Start MM2040EV with adafruit-circuitpython-raspberry_pi_pico-en_US-7.0.0-alpha.3.uf2
- Copy video trailer_400p_20s.avi to sdcard at '/pico-brteve/circuitPython/examples/video2/trailer_400p_20s.avi'
- Copy source code in Python into CircuitPython drive:
    + lib (which include "bteve" folder )
    + code.py
    + folder video2
- Demo should start after that    

## Screenshot:

Select video source:

![image](https://user-images.githubusercontent.com/48436368/135993746-da2f6769-c85a-4f84-bd28-54d92a33734b.png)

Choose to flash video at first:

![image](https://user-images.githubusercontent.com/48436368/135993883-20c45a44-cfd6-4a2b-a58e-4fcc081d3585.png)

Video:

![image](https://user-images.githubusercontent.com/48436368/135993941-1a574c59-d246-46eb-baa3-c7b82cd61665.png)

