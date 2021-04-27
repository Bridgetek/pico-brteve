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
        self.cs = self.pin(board.GP5)
        self.pdn = self.pin(board.GP7)
        self.cs_ili9488 = self.pin(board.GP9)
        self.dcx_ili9488 = self.pin(board.GP8)      
        
        self.sdcs = board.GP13
        if not self.setup_sd(self.sdcs):
            self.pin(self.sdcs)
        self.setup_spi()

    def pin(self,p):
        r = digitalio.DigitalInOut(p)
        r.direction = digitalio.Direction.OUTPUT
        r.value = True
        return r
        
    def init(self):
        print("BRT Eve Module init\n")
        Brt_Eve_Module.init(self)
        # self.setup_1280x720()
        self.setup_800x480()
        # self.setup_1024x600()
        # self.init_ili9488()
        # self.setup_320x480()

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
        
    def write_ili9488(self,cmd,data):
        self.write_ili9488_cmd(cmd)
        self.write_ili9488_data(data)

    @spilock
    def write_ili9488_cmd(self,cmd):
        self.cs_ili9488.value = False           
        self.dcx_ili9488.value = False              
      
        self.sp.write(cmd)        
        self.cs_ili9488.value = True

    @spilock
    def write_ili9488_data(self,data):
        self.cs_ili9488.value = False           
        self.dcx_ili9488.value = True              
      
        self.sp.write(data)        
        self.cs_ili9488.value = True
        
    def init_ili9488(self):
        self.wr32(REG_GPIO, 0x83)   

        ILI9488_CMD_SOFTWARE_RESET = b'\x01'
        ILI9488_CMD_COLOMNADDR = b'\x2A'
        ILI9488_CMD_ROWADDR = b'\x2B'
        
        ILI9488_CMD_INTERFACE_MODE_CONTROL = b'\xB0'
        ILI9488_CMD_FRAME_RATE_CONTROL = b'\xB1'
        ILI9488_CMD_INTERFACE_PIXEL_FORMAT = b'\x3A'
        ILI9488_INTERFACE_PIXEL_FORMAT_18BIT_DPI = b'\x66'
        ILI9488_CMD_IMAGEFUNCTION = b'\xE9'
        ILI9488_CMD_WRITE_CONTROL_DISPLAY = b'\x53'
        ILI9488_CMD_MADCTRL = b'\x36'
        
        ILI9488_CMD_DISPLAY_FUNCTION_CONTROL = b'\xB6'
        ILI9488_CMD_SLEEP_OUT = b'\x11'
        ILI9488_CMD_DISPLAYON = b'\x29'
        
               
        self.write_ili9488_cmd(ILI9488_CMD_SOFTWARE_RESET)
        time.sleep(0.00012)
        
        #colomn address set - 0 to 319
        self.write_ili9488(ILI9488_CMD_COLOMNADDR,bytes([0x00,0x00,0x01, 0x3f]))
        #row address set - 0 to 479
        self.write_ili9488(ILI9488_CMD_ROWADDR,bytes([0x00,0x00,0x01, 0xdf]))
        
        #Frame rate 70HZ
        self.write_ili9488(ILI9488_CMD_FRAME_RATE_CONTROL,b'\xB0')
        
        #adjust control 3
        self.write_ili9488(b'\xF7', bytes([0xA9,0x51,0x2C,0x82]))
        
        self.write_ili9488(ILI9488_CMD_INTERFACE_MODE_CONTROL, b'\x02')
        self.write_ili9488(ILI9488_CMD_INTERFACE_PIXEL_FORMAT, ILI9488_INTERFACE_PIXEL_FORMAT_18BIT_DPI)
        self.write_ili9488(ILI9488_CMD_IMAGEFUNCTION, b'\x00')
        self.write_ili9488(ILI9488_CMD_WRITE_CONTROL_DISPLAY, b'\x2C')
        
        #bgr connection and colomn address order
        self.write_ili9488(ILI9488_CMD_MADCTRL,b'\x48')
        
        self.write_ili9488(ILI9488_CMD_DISPLAY_FUNCTION_CONTROL, bytes([0x30,0x02,0x3B]))
        
        self.write_ili9488_cmd(ILI9488_CMD_SLEEP_OUT)
        time.sleep(0.02)
        
        self.write_ili9488_cmd(ILI9488_CMD_DISPLAYON)
        write_ili9488(b'\x2C',b'\xB0')
        
        pin(cs_ili9488)
        pin(dcx_ili9488)
        
        
        
        
        
        
        
