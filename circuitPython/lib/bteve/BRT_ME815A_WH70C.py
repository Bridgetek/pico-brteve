import os
import board
import busio
import digitalio
import sdcardio
import storage

from .gameduino import Gameduino

def spilock(f):
    def wrapper(*args):
        spi = args[0].sp
        while not spi.try_lock():
            pass
        r = f(*args)
        spi.unlock()
        return r
    return wrapper

class ME815A_WH70C_Pico(Gameduino):
    def __init__(self):
        mach = os.uname().machine
        if mach == 'Raspberry Pi Pico with rp2040':
            self.sp = busio.SPI(board.GP2, MOSI=board.GP3, MISO=board.GP4)

        def pin(p):
            r = digitalio.DigitalInOut(p)
            r.direction = digitalio.Direction.OUTPUT
            r.value = True
            return r
			
        self.cs = pin(board.GP5)
        self.setup_spi()


    @spilock
    def setup_spi(self):
        self.sp.configure(baudrate=1000000, phase=0, polarity=0)

    
    @spilock
    def transfer(self, wr, rd = 0):
        self.cs.value = False
        self.sp.write(wr)
        r = None
        if rd != 0:
            r = bytearray(rd)
            self.sp.readinto(r)
        self.cs.value = True
        return r
