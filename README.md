# IDM2040-7A-EW2022
The demo applications for Embedded World 2022
1. LDS SDK firmware: It is a customized circuitPython 7.1.0 run-time built with LDS driver, which shall be the base to run any circuitPython code. It is here:  
      https://github.com/BRTSG-FOSS/pico-brteve/tree/EW2022/LDS_Demo/IDM2040_LDS_SDK_Firmware   
3. User shall program flash image ["EVE_Flash/BT81X_Flash.bin"](https://github.com/BRTSG-FOSS/pico-brteve/tree/EW2022/EVE_Flash/EVE_Flash)  into EVE's connected flash with [Eve Asset Builder (EAB)](https://brtchip.com/ic-module/toolchains/).   
4. These example projects depend on the pico-brtEve library   
   https://github.com/BRTSG-FOSS/pico-brteve/tree/main/circuitPython/lib/brteve.   
   User shall download it to the folder "lib\brtEve" of ciruitPython drive on your PC when IDM2040 is connected. 
