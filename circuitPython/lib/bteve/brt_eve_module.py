import sys
import time
import struct
from collections import namedtuple

if sys.implementation.name == 'circuitpython':
    from _eve import _EVE
else:
    from ._eve import _EVE
from .eve import EVE
from .registers import *

if sys.implementation.name != "circuitpython":
    def const(x): return x

FIFO_MAX = const(0xffc)    # Maximum reported free space in the EVE command FIFO

class CoprocessorException(Exception):
    pass

"""
Adapted from https://github.com/jfurcean/CircuitPython_WiiChuck.git
where this class ClassicController appears. It is covered by this license

The MIT License (MIT)
  
Copyright (c) 2021 John Furcean

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
"""

class Brt_Eve_Module(_EVE, EVE):
    def init(self):
        self.register(self)

        self.coldstart()

        # self.bringup()

        t0 = time.monotonic()
        while self.rd32(REG_ID) != 0x7c:
            assert (time.monotonic() - t0) < 1.0, "No response - is device attached?"

        self.getspace()

        print("ID %x  %x %x %x %x" % (
            self.rd32(REG_ID),
            self.rd32(0xc0000),
            self.rd32(REG_HSIZE),
            self.rd32(REG_VSIZE),
            self.rd32(REG_CMDB_SPACE)))

        self.standard_startup()

    def coldstart(self):
        self.host_cmd(0x61, 0x46)   # 72 MHz
        self.host_cmd(0x44)         # int clock
        self.host_cmd(0x00)         # Wake up
        self.host_cmd(0x68)         # Core reset

    def bringup(self):
        time.sleep(.4)

        while 1:
            print()
            self.d.sel()
            for c in (0x30, 0x20, 0x00, 0xff, 0xff, 0xff):
                r = self.d.writeread(bytes([c]))[0]
                print("Sent %02x recv %02x" % (c, r))
            self.d.unsel()
            time.sleep(2)

    def host_cmd(self, a, b = 0, c = 0):
        self.transfer(bytes([a, b, c]))

    def standard_startup(self):
        self.Clear(1,1,1)
        self.swap()
        self.cmd_flashread(0, 0x1000, 0x1000)
        self.finish()
        # print('*** Done flash ***')
        time.sleep(.1)
        if self.rd32(0xffc) == 0x7C6A0100:
            self.cc(self.rd(0, 512))
        else:
            print('*** Did not find flash config ***')
        self.finish()
        self.w = self.rd32(REG_HSIZE)
        self.h = self.rd32(REG_VSIZE)
        # self.wr32(REG_GPIO, 0x83)
        # print(self.w, self.h, self.rd32(REG_PCLK), hex(self.rd32(REG_GPIO)))
        self.cmd_regwrite(REG_GPIO, 0x83)
        time.sleep(.1)

    def _addr(self, a):
        return struct.pack(">I", a)[1:]

    def rd(self, a, n):
        return self.transfer(self._addr(a), 1 + n)[1:]

    def wr(self, a, v):
        self.transfer(self._addr(0x800000 | a) + v)

    def rd32(self, a):
        return struct.unpack("<I", self.rd(a, 4))[0]

    def wr32(self, a, v):
        self.wr(a, struct.pack("I", v))

    def getspace(self):
        self.space = self.rd32(REG_CMDB_SPACE)
        if self.space & 1:
            raise CoprocessorException

    def reserve(self, n):
        while self.space < n:
            self.getspace()

    def is_finished(self):
        self.getspace()
        return self.space == FIFO_MAX
            
    def write(self, ss):
        self.reserve(len(ss))
        self.wr(REG_CMDB_WRITE, ss)
        self.space -= len(ss)
        return


        # Write ss to the command FIFO
        for i in range(0, len(ss), 64):
            s = ss[i:i + 64]
            self.reserve(len(s))
            self.wr(REG_CMDB_WRITE, s)
            self.space -= len(s)

    def finish(self):
        self.flush()
        self.reserve(FIFO_MAX)

    def is_idle(self):
        self.getspace()
        return self.space == FIFO_MAX


    def result(self, n=1):
        # Return the result field of the preceding command
        self.finish()
        wp = self.rd32(REG_CMD_READ)
        return self.rd32(RAM_CMD + (4095 & (wp - 4 * n)))
		

    def video_signal(self, h_Active, h_Front, h_Sync, h_Back, h_Total, v_Active, v_Front, v_Sync, v_Back, v_Total):
        assert((h_Active + h_Front + h_Sync + h_Back) == h_Total)
        assert((v_Active + v_Front + v_Sync + v_Back) == v_Total)

        # self.cmd_regwrite(REG_ADAPTIVE_FRAMERATE, 0)

        self.cmd_regwrite(REG_HCYCLE, h_Total)
        self.cmd_regwrite(REG_HOFFSET, h_Sync + h_Back)
        self.cmd_regwrite(REG_HSIZE, h_Active)

        self.cmd_regwrite(REG_VCYCLE, v_Total)
        self.cmd_regwrite(REG_VOFFSET, v_Sync + v_Back)
        self.cmd_regwrite(REG_VSIZE, v_Active)

        # See CEA-861 p.21
        self.cmd_regwrite(REG_HSYNC1, 0)
        self.cmd_regwrite(REG_HSYNC0, h_Sync)

        self.cmd_regwrite(REG_VSYNC1, 0)
        self.cmd_regwrite(REG_VSYNC0, v_Sync)

        if 0:
            self.cmd_regwrite(REG_TRIM, 23)
            self.cmd_regwrite(0x302614, 0x8c1)

        self.cmd_regwrite(REG_PCLK, 1)
        self.cmd_regwrite(REG_PCLK_FREQ, 0x8B1)
                                            

    #Good for LVDS interface 1280x720
    #This panel shall work with BT817/8 board only
    def setup_1280x720(self):
        self.Clear()
        self.swap()
        setup = [
            (REG_OUTBITS, 0),
            (REG_DITHER, 0),
            (REG_GPIO, 0x83),
            (REG_CSPREAD, 0),
            (REG_PCLK_POL, 0),
            (REG_ADAPTIVE_FRAMERATE, 0),
        ]
        for (a, v) in setup:
            self.cmd_regwrite(a, v)

        self.video_signal(
            h_Active = 1280,
            h_Front = 110,
            h_Sync = 40,
            h_Back = 220,
            h_Total = 1650,
            v_Active = 800,
            v_Front = 5,
            v_Sync = 5,
            v_Back = 20,
            v_Total = 830)
        self.w = 1280
        self.h = 800
        return

    #For XXXX board QVGA LCD panel
    def setup_320x240(self):
        self.Clear()
        self.swap()
        setup = [
            # (REG_OUTBITS, 0),
            (REG_DITHER, 1),
            (REG_GPIO, 0x83),
            (REG_CSPREAD, 1),
            (REG_PCLK_POL, 0),
            (REG_SWIZZLE, 2),
            #(REG_ADAPTIVE_FRAMERATE, 0),
        
            (REG_HCYCLE, 408),
            (REG_HOFFSET, 70),
            (REG_HSIZE, 320),
        
            (REG_HSYNC1, 10),
            (REG_HSYNC0, 0),
        
            (REG_VCYCLE, 263),
            (REG_VOFFSET, 13),
            (REG_VSIZE, 240),
        
            (REG_VSYNC1, 2),
            (REG_VSYNC0, 0),
        ]
        for (a, v) in setup:
            self.cmd_regwrite(a, v)
        

        self.cmd_regwrite(REG_PCLK, 8)
        self.w = 320
        self.h = 240


    #For ME810A-HV35R board ILI9488 LCD panel
    def setup_320x480(self):
        self.Clear()
        self.swap()
        setup = [
            # (REG_OUTBITS, 0),
            (REG_DITHER, 1),
            (REG_GPIO, 0x83),
            (REG_CSPREAD, 1),
            (REG_PCLK_POL, 1),
            (REG_SWIZZLE, 2),
            #(REG_ADAPTIVE_FRAMERATE, 0),
        
            (REG_HCYCLE, 400),
            (REG_HOFFSET, 40),
            (REG_HSIZE, 320),
        
            (REG_HSYNC1, 10),
            (REG_HSYNC0, 0),
        
            (REG_VCYCLE, 500),
            (REG_VOFFSET, 10),
            (REG_VSIZE, 480),
        
            (REG_VSYNC1, 5),
            (REG_VSYNC0, 0),
        ]
        for (a, v) in setup:
            self.cmd_regwrite(a, v)
        

        self.cmd_regwrite(REG_PCLK, 5)
        self.w = 320
        self.h = 480

    def setup_640x480(self):
        self.Clear()
        self.swap()
        setup = [
            # (REG_OUTBITS, 0),
            (REG_DITHER, 0),
            (REG_GPIO, 0x83),
            (REG_CSPREAD, 0),
            (REG_PCLK_POL, 0),
            (REG_ADAPTIVE_FRAMERATE, 0),
        
            (REG_HCYCLE, 800),
            (REG_HOFFSET, 16 + 96),
            (REG_HSIZE, 640),
        
            (REG_HSYNC1, 0),
            (REG_HSYNC0, 96),
        
            (REG_VCYCLE, 525),
            (REG_VOFFSET, 12),
            (REG_VSIZE, 480),
        
            (REG_VSYNC1, 0),
            (REG_VSYNC0, 10),
        ]
        for (a, v) in setup:
            self.cmd_regwrite(a, v)
        

        self.cmd_regwrite(REG_PCLK, 3)
        self.w = 640
        self.h = 480


    #Good for Riverdi 800x480 panel
    def setup_800x480(self):
        self.Clear()
        self.swap()
        setup = [
            # (REG_OUTBITS, 0),
            (REG_DITHER, 1),
            (REG_GPIO_DIR, 0xFF),
            (REG_GPIO, 0xFF),
            (REG_CSPREAD, 0),
            (REG_PCLK_POL, 1),
            (REG_ADAPTIVE_FRAMERATE, 0),

            (REG_HCYCLE, 928),
            (REG_HOFFSET, 88),
            (REG_HSIZE, 800),

            (REG_HSYNC1, 48),
            (REG_HSYNC0, 0),

            (REG_VCYCLE, 525),
            (REG_VOFFSET, 32),
            (REG_VSIZE, 480),

            (REG_VSYNC1, 3),
            (REG_VSYNC0, 0),
        ]
        for (a, v) in setup:
            self.cmd_regwrite(a, v)


        self.cmd_regwrite(REG_PCLK, 2)
        self.finish()

        self.w = 800
        self.h = 480

    #Good for Riverdi 1024x600 panel (RGB40pin)
    def setup_1024x600(self):
        self.Clear()
        self.swap()
        setup = [
            # (REG_OUTBITS, 0),
            (REG_DITHER, 1),
            (REG_GPIO_DIR, 0xFF),
            (REG_GPIO, 0xFF),
            (REG_CSPREAD, 0),
            (REG_PCLK_POL, 1),
            (REG_ADAPTIVE_FRAMERATE, 0),

            (REG_HCYCLE, 1344),
            (REG_HOFFSET, 160),
            (REG_HSIZE, 1024),

            (REG_HSYNC1, 100),
            (REG_HSYNC0, 0),

            (REG_VCYCLE, 635),
            (REG_VOFFSET, 23),
            (REG_VSIZE, 600),

            (REG_VSYNC1, 10),
            (REG_VSYNC0, 0),
        ]
        for (a, v) in setup:
            self.cmd_regwrite(a, v)


        self.cmd_regwrite(REG_PCLK, 1)
        self.cmd_regwrite(REG_PCLK_FREQ, 0xD12)
        self.finish()

        self.w = 1024
        self.h = 600