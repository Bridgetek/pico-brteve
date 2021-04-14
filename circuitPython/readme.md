# circuitPython support for Raspberry Pi Pico with Eve module from Brdgetek Pte Ltd


## lib
This folder contains the necessary files to use Eve device by Raspberry Pi Pico. 
It provides a python class to control Eve via SPI. 

## Examples
The circuitPython code to showcase the usage of graphics features from Eve


### Eve - Embedded Video Engine
An extreme easy-to-use yet powerful GPU with SPI interface to MCU. See more details:
https://brtchip.com/bt81x/


![image](https://user-images.githubusercontent.com/13127756/110600563-06ef1300-81bf-11eb-87c9-c75d55c7d02a.png)


### Eve Modules for Pico 

#### MM2040EV 
#### ID2040

Both modules defines the same connections with EVE:  

PICO GP2 <--> Eve SCK   
PICO GP3 <--> Eve MOSI   
PICO GP4 <--> Eve MISO   
PICO GP5 <--> Eve CS#   
  
PICO Power <--> Eve Board power   
PICO GND <--> Eve Board GND   
