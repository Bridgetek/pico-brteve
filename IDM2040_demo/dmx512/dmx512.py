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
        time.sleep(120E-6) #0.000120 #worked   ,120 us
        self.dmx_uart.value = 1
        self.dmx_uart.deinit()
        time.sleep(8E-6)  # 0.000008
        self.dmx_uart = busio.UART(tx=self.tx_pin,rx=board.GP9, baudrate=250000, bits=8, parity=None, stop=2)
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

 

