********************************************************************************
*    Demo Ev Charge Point                                                   *
*                                                                              *
********************************************************************************

1. INTRODUCTION
    This is a audio playback example using BT8XX platform and MM2040EV.
    Audio are stored on the SDcard at location:
      /sd/pico-brteve/circuitPython/examples/audio-playback/media
      
    Audio must be converted by EAB with as 8-bit Signed PCM, rate 44100

2. HARDWARE REQUIREMENT
    Below is the list of hardware used in this demo
    - 1 ME817EV
    - 1 MM2040EV
    - 1 LCD LCD 1280x800

3. Flash data
    User have to flash data "BT81X_Flash.bin" into EVE's connected flash, there is 2 ways:
    1. Use EAB tool
    2. - Copy "BT81X_Flash.bin" into a sdcard at folder: 
          /EveApps/DemoEvChargePoint/Test/Flash/BT81X_Flash.bin
       - Plug the sdcard into MM2040EV board
       - Start this demo and select "Yes" when see this question on LCD: 
          "Write BT81X_Flash.bin from sdcard to EVE's connected flash at first?"

4. Copy audio to sdcard
    This application only play audio on a sdcard
    User must convert audio by EAB tool, then copy .raw file to sdcard at folder: 
    /pico-brteve/circuitPython/examples/audio-playback/media
      
5. STARTUP
    - Start MM2040EV with adafruit-circuitpython-raspberry_pi_pico-en_US-6.3.0-rc.0.uf2
    - Copy source code in Python into CircuitPython drive:
        -> lib (which inclide "bteve" folder )
        -> audio_playback folder
        -> code.py
    - Demo should start after that
    
6. Functionalities:
  - Scan .raw files on SDcard and list out on LCD
  - Press Play to start audio
  - Double tap on a file to start audio
  - Press Pause to pause audio
  - Press Stop to stop audio
  - Press Next to play next .raw file
  - Press Previous to play previous .raw file
  - Press Loop to change looping mode: disable/ loop 1 file / loop all files
  - Press Mix to change sequence mode: Mix on/ Mix off
  - File list can scroll up/down
  - Move finger arround volume button to change volume
  
                                   【END】