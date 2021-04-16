import os
import board
import busio
import digitalio
import sdcardio
import storage
import sys

if sys.implementation.name == 'circuitpython':
    from _eve import _EVE
else:
    from ._eve import _EVE
from .eve import EVE
from .registers import *

from .brt_eve_module import Brt_Eve_Module


def spilock(f):
    def wrapper(*args):
        spi = args[0].sp
        while not spi.try_lock():
            pass
        r = f(*args)
        spi.unlock()
        return r
    return wrapper

class Brt_PicoEve_Module(Brt_Eve_Module):
    def __init__(self):
        mach = os.uname().machine
        if mach == 'Raspberry Pi Pico with rp2040':
            self.sp = busio.SPI(board.GP2, MOSI=board.GP3, MISO=board.GP4)
            self.sd_spi = busio.SPI(board.GP10, MOSI=board.GP11, MISO=board.GP12)
        def pin(p):
            r = digitalio.DigitalInOut(p)
            r.direction = digitalio.Direction.OUTPUT
            r.value = True
            return r

        self.cs = pin(board.GP5)
        self.sdcs = board.GP13
        if not self.setup_sd(self.sdcs):
            pin(self.sdcs)
        self.setup_spi()


    def init(self):
        print("BRT Eve Module init\n")
        Brt_Eve_Module.init(self)
        #self.setup_1280x720()
        #self.setup_800x480()
        self.setup_1024x600()

    def setup_sd(self, sdcs):
        try:
            self.sdcard = sdcardio.SDCard(self.sd_spi, sdcs)
        except OSError:
            return False
        self.vfs = storage.VfsFat(self.sdcard)
        storage.mount(self.vfs, "/sd")
        return True

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
