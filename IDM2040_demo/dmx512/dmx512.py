import board
import busio
import digitalio
import time
from array import array

UART1_POL_PIN =board.GP16

class dmx512():
    def __init__(self, max_channels,tx_pin):
        # First byte is always 0, 512 after that is the 512 channels
        self.dmx_message = array('B', [0] * (max_channels+1))
        self.dmx_uart = digitalio.DigitalInOut(tx_pin)
        self.dmx_uart.direction = digitalio.Direction.OUTPUT
        self.dmx_uart.value = 1
        self.tx_pin=tx_pin
        
        self.de_pin = digitalio.DigitalInOut(board.GP27)
        self.de_pin.direction = digitalio.Direction.OUTPUT
        self.de_pin.value = 1

        self.uart1_pol_pin = digitalio.DigitalInOut(board.GP16)
        self.uart1_pol_pin.direction = digitalio.Direction.OUTPUT
        self.uart1_pol_pin.value = 1

    #the special method __del__ is not implemented for user-defined classes.
    def __del__(self):
        self.dmx_uart.deinit()
        self.de_pin.deinit()
        self.uart1_pol_pin.deinit()
    def deinit(self):   
        self.dmx_uart.deinit()
        self.de_pin.deinit()
        self.uart1_pol_pin.deinit()
        
    def set_messages(self, v):
        self.dmx_message=v   
        
    def set_channels(self, message):
        """
        a dict and writes them to the array
        format {channel:value}
        """
        for ch in message:
            self.dmx_message[ch] = message[ch]
            #print("set_channels",ch,message[ch])


    def write_frame(self):
        """
        Send a DMX frame
        """
        self.dmx_uart.value = 0
        #time.sleep(88E-6) #0.000088
        time.sleep(120E-6) #0.000120 #worked  
        #expected 120 us
        self.dmx_uart.value = 1
        self.dmx_uart.deinit()
        time.sleep(8E-6)  # 0.000008
        #expected 4us
        #self.dmx_uart = busio.UART(tx=self.tx_pin,rx=None, baudrate=250000, bits=8, parity=None, stop=2)
        self.dmx_uart = busio.UART(tx=self.tx_pin,rx=board.GP9, baudrate=250000, bits=8, parity=None, stop=2)
        #print("w",self.dmx_message)
        self.dmx_uart.write(self.dmx_message)
        self.dmx_uart.deinit()
        self.dmx_uart = digitalio.DigitalInOut(self.tx_pin)
        self.dmx_uart.direction = digitalio.Direction.OUTPUT
        self.dmx_uart.value = 1

    def set4ch(self,m1,m2,m3,m4):
        self.set_channels(m1)
        self.set_channels(m2)
        self.set_channels(m3)
        self.set_channels(m4)
        self.write_frame()
        
    def setStart4ch(self,start,m1,m2,m3,m4):
        self.set_channels({start:m1})
        self.set_channels({start+1:m2})
        self.set_channels({start+2:m3})
        self.set_channels({start+3:m4})
        self.write_frame()
        
    def off(self,start=1):
        self.set_channels({start:0})
        self.set_channels({start+1:0})
        self.set_channels({start+2:0})
        self.set_channels({start+3:0})
        self.write_frame()

import board
import digitalio
from digitalio import DigitalInOut, Direction
def setPin(pin,):
    with digitalio.DigitalInOut(pin) as Pin:
     Pin.direction = Direction.INPUT
     Pin.pull=digitalio.Pull.UP
     #print(pin,Pin, Pin.value)
def initIO():      
    print("initIO")   
    circuitpython_gpio_pins= [
        board.GP0,
        board.GP1,
        #board.GP2, 
        board.GP3, 
        board.GP4, 
        board.GP5, 
        board.GP6, 
        board.GP7, 
        board.GP8, 
        board.GP9,
        board.GP10, 
        board.GP11, 
        board.GP12, 
        board.GP13,
        board.GP14, 
        board.GP15, 
        board.GP16, 
        board.GP17, 
        board.GP18, 
        board.GP19, 
        board.GP20, 
        board.GP21,
        board.GP22, 
        board.GP23, 
        board.GP24, 
        board.GP25, 
        board.GP26, 
        board.GP27, 
        board.GP28,
    ]
    for pin in circuitpython_gpio_pins:
        setPin(pin)
    PIN_22 = DigitalInOut(board.GP22)  # digital write to ESP_D_R_PIN
    PIN_22.direction = Direction.INPUT

    
if __name__ == "__main__":
    initIO()
    channel = 512
    addrDMX=4
    led_pin = digitalio.DigitalInOut(board.GP25)
    led_pin.direction = digitalio.Direction.OUTPUT
    led_pin.value = 1
    #print("DMX test addrDMX",addrDMX)
    dmx = dmx512(channel,board.GP8)
     
    for i in range(1,6):
        print(i)
       
        if (i==2):
            dmx.setStart4ch(addrDMX,0,255,0,0)
        elif (i==3):
            dmx.setStart4ch(addrDMX,0,0,255,0)
        elif (i==4):
            dmx.setStart4ch(addrDMX,0,0,0,i*32)
        elif  (i==1):
            dmx.setStart4ch(addrDMX,255,0,0,0)
        else:
            dmx.setStart4ch(addrDMX,32,64,64,0)
            
        dmx.write_frame() # re-write once
        time.sleep(1)
        dmx.off(addrDMX)
        dmx.off(addrDMX)
        time.sleep(1)
        
    dmx.off(addrDMX)
    dmx.deinit()
    

