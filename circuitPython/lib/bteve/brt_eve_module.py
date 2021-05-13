import sys
import time
import struct
from collections import namedtuple
from .mcu_bt817_8 import *
from .mcu_ft80x import *

if sys.implementation.name == 'circuitpython':
    from _eve import _EVE
else:
    from ._eve import _EVE
from .eve import EVE

# Order matches the register layout, so can fill with a single block read
_Touch = namedtuple(
    "TouchInputs",
    (
    "rawy",
    "rawx",
    "rz",
    "y",
    "x",
    "tag_y",
    "tag_x",
    "tag",
    ))
_State = namedtuple(
    "State",
    (
    "touching",
    "press",
    "release"
    ))
_Tracker = namedtuple(
    "Tracker",
    (
    "tag",
    "val"
    ))
_Inputs = namedtuple(
    "Inputs",
    (
    "touch",
    "tracker",
    "state",
    ))

EVE_DL_SIZE  =(8 * 1024) # 8kB Display List buffer size 
EVE_DL_COUNT = (2 * 1024)
EVE_CMD_FIFO_SIZE =((4) * 1024) # 4kB coprocessor FIFO size 
EVE_CMD_FIFO_COUNT =(1024)
EVE_CMD_FIFO_MASK =(EVE_CMD_FIFO_SIZE - 1)
EVE_CMD_FIFO_ALIGNMENT_MASK =(EVE_CMD_FIFO_SIZE - ((4) - 1))

def EVE_CMD_FAULT(rp): return (rp & 0x3)

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
    def __init__(self):
        EVE.__init__(self)

        self.FIFO_MAX = (0xffc)    # Maximum reported free space in the EVE command FIFO
        self.Mcu_FT80X  = Mcu_FT80X()
        self.Mcu_BT817_8=Mcu_BT817_8()

        # Default attributes
        self.module    = self.MODULE_ME817EV()
        self.generation= self.GENERATION_EVE4()
        self.ic        = self.IC_EVE_BT817()
        self.lcd       = self.LCD_WXGA_1280X800()
        self.touch     = self.TOUCH_GOODIX()
        
        self._all_settings=self._get_all_settings()
        self.all_modules=self._get_module_info()

        self.w=1280
        self.h=800
        self.mcu=self.Mcu_BT817_8

        self.CmdWp = 0 # internal CmdWp position

    def init(self):
        print("Initialing for MCU " + self.ic)
        funcdict = {
            self.IC_EVE_FT800():self.Mcu_FT80X,
            self.IC_EVE_BT817():self.Mcu_BT817_8,
        }
        if self.ic in self._all_settings:
            self.mcu=funcdict[self.ic]

            #Copy attribute
            dic=self.mcu.__dict__
            for k in dic:
                if not k.startswith('__'):
                    v=self.mcu.__dict__[k]
                    setattr( self, k, v )

        else:
            print("Error: EVE IC is not set")

        self.register(self)
        self.coldstart()

        t0 = time.monotonic()
        while self.rd32(self.mcu.REG_ID) != 0x7c:
            assert (time.monotonic() - t0) < 1.0, "No response - is device attached?"

        self.getspace()

        print("ID %x  %x %x %x" % (
            self.rd32(self.mcu.REG_ID),
            self.rd32(0xc0000),
            self.rd32(self.mcu.REG_HSIZE),
            self.rd32(self.mcu.REG_VSIZE)))

        self.standard_startup()

        funcdict = {
            self.LCD_WXGA_1280X800       () : self.setup_1280x800,
            self.LCD_WXGA_1280X720       () : self.setup_1280x720,
            self.LCD_WSVGA_1024X600      () : self.setup_1024x600,
            self.LCD_WVGA_800X480        () : self.setup_800x480,
            self.LCD_VGA_600X480         () : self.setup_640x480,
            self.LCD_WQVGA_480X272       () : self.setup_480x272,
            self.LCD_HVGA_320X480_ILI9488() : self.setup_320x480,
            self.LCD_QVGA_320X240        () : self.setup_320x240,
        }
        if self.lcd in self._all_settings:
            print("LCD size:", self.lcd)
            funcdict[self.lcd]()
            if self.lcd == self.LCD_HVGA_320X480_ILI9488():         
                self.init_ili9488()
        else:
            print("Error: LCD size is not set")

        if self.touch in self._all_settings:
            print("Touch type:", self.touch)
            if self.touch ==  self.TOUCH_GOODIX():
                    print("Setup touch for Goodix\n")
                    self.wr8(self.mcu.REG_CPURESET, 2)
                    self.wr16(self.mcu.REG_TOUCH_CONFIG, 0x05d0)
                    self.wr8(self.mcu.REG_CPURESET, 0)
        else:
            print("Warning: Touch type is not set")

    def coldstart(self):
        self.host_cmd(0x61, 0x46)   # 72 MHz
        self.host_cmd(0x44)         # Select PLL input from external clock source
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
        '''
        self.cmd_flashread(0, 0x1000, 0x1000)
        self.finish()
        # print('*** Done flash ***')
        time.sleep(.1)
        if self.rd32(0xffc) == 0x7C6A0100:
            self.cc(self.rd(0, 512))
        else:
            print('*** Did not find flash config ***')
        '''
        self.finish()
        self.w = self.rd32(self.mcu.REG_HSIZE)
        self.h = self.rd32(self.mcu.REG_VSIZE)
        # self.wr32(self.mcu.REG_GPIO, 0x83)
        # print(self.w, self.h, self.rd32(self.mcu.REG_PCLK), hex(self.rd32(self.mcu.REG_GPIO)))
        self.cmd_regwrite(self.mcu.REG_GPIO, 0x83)
        time.sleep(.1)

    def _addr(self, a):
        return struct.pack(">I", a)[1:]

    def rd(self, a, n):
        return self.transfer(self._addr(a), 1 + n)[1:]

    def wr(self, a, v):
        self.transfer(self._addr(0x800000 | a) + v)

    def rd8(self, a):
        return struct.unpack("<B", self.rd(a, 1))[0]

    def rd16(self, a):
        return struct.unpack("<H", self.rd(a, 2))[0]

    def rd32(self, a):
        return struct.unpack("<I", self.rd(a, 4))[0]

    def wr8(self, a, v):
        self.CmdWp += 1
        self.CmdWp &= EVE_CMD_FIFO_MASK;
        self.wr(a, struct.pack("B", v))

    def wr16(self, a, v):
        self.CmdWp += 2
        self.CmdWp &= EVE_CMD_FIFO_MASK;
        self.wr(a, struct.pack("H", v))

    def wr32(self, a, v):
        self.CmdWp += 4
        self.CmdWp &= EVE_CMD_FIFO_MASK;
        self.wr(a, struct.pack("I", v))

    def EVE_Cmd_wp(self):
        wp=self.rd32(self.mcu.REG_CMD_WRITE)
        return wp & EVE_CMD_FIFO_MASK

    def EVE_Cmd_rp(self):
        return self.rd16(self.mcu.REG_CMD_READ) & EVE_CMD_FIFO_MASK


    def getspace(self):
        if ( self.generation == self.GENERATION_EVE3() or 
             self.generation == self.GENERATION_EVE4() ):
            self.space = self.rd16(self.mcu.REG_CMDB_SPACE) & EVE_CMD_FIFO_MASK;
            if (EVE_CMD_FAULT(self.space)):
                print("Co-processor faulty")
                raise CoprocessorException
        else:
            wp = self.EVE_Cmd_wp()
            rp = self.EVE_Cmd_rp()
            self.space = (rp - wp - 4) & EVE_CMD_FIFO_MASK;

        if self.space & 1:
            print("Co-processor faulty")
            raise CoprocessorException

    def reserve(self, n):
        while self.space < n:
            self.getspace()

    def is_finished(self):
        self.getspace()
        return self.space == self.FIFO_MAX
            
    def write(self, ss):
        self.reserve(len(ss))
        # print("Writing buffer len:", len(ss), "data:", ''.join('{:02x}'.format(x) for x in ss))
        if ( self.generation == self.GENERATION_EVE3() or 
             self.generation == self.GENERATION_EVE4() ):
            self.wr(self.mcu.REG_CMDB_WRITE, ss)
        else:
            self.wr(self.mcu.RAM_CMD + self.CmdWp, ss)
            self.CmdWp += len(ss)
            self.CmdWp &= EVE_CMD_FIFO_MASK;
            self.wr32(self.mcu.REG_CMD_WRITE, self.CmdWp)

        self.getspace()
        
    def finish(self):
        self.flush()
        self.reserve(self.FIFO_MAX)

    def is_idle(self):
        self.getspace()
        return self.space == self.FIFO_MAX

    def VertexFormat(self, f): # Fix built-in mismatch with EVE1
        if ( self.generation == self.GENERATION_EVE1()):
            pass
        else:
            super().VertexFormat(f)

    def result(self, n=1):
        # Return the result field of the preceding command
        self.finish()
        wp = self.rd32(self.mcu.REG_CMD_READ)
        return self.rd32(RAM_CMD + (4095 & (wp - 4 * n)))
        
    def video_signal(self, h_Active, h_Front, h_Sync, h_Back, h_Total, v_Active, v_Front, v_Sync, v_Back, v_Total):
        assert((h_Active + h_Front + h_Sync + h_Back) == h_Total)
        assert((v_Active + v_Front + v_Sync + v_Back) == v_Total)

        # self.cmd_regwrite(self.mcu.REG_ADAPTIVE_FRAMERATE, 0)

        self.cmd_regwrite(self.mcu.REG_HCYCLE, h_Total)
        self.cmd_regwrite(self.mcu.REG_HOFFSET, h_Sync + h_Back)
        self.cmd_regwrite(self.mcu.REG_HSIZE, h_Active)

        self.cmd_regwrite(self.mcu.REG_VCYCLE, v_Total)
        self.cmd_regwrite(self.mcu.REG_VOFFSET, v_Sync + v_Back)
        self.cmd_regwrite(self.mcu.REG_VSIZE, v_Active)

        # See CEA-861 p.21
        self.cmd_regwrite(self.mcu.REG_HSYNC1, 0)
        self.cmd_regwrite(self.mcu.REG_HSYNC0, h_Sync)

        self.cmd_regwrite(self.mcu.REG_VSYNC1, 0)
        self.cmd_regwrite(self.mcu.REG_VSYNC0, v_Sync)

        if 0:
            self.cmd_regwrite(self.mcu.REG_TRIM, 23)
            self.cmd_regwrite(0x302614, 0x8c1)

        self.cmd_regwrite(self.mcu.REG_PCLK, 1)
		
    def set_module(self, module):
        if module in self.all_modules:
            self.module = module;
            self.set_ic(self.all_modules[module]['ic'])
            self.set_lcd(self.all_modules[module]['lcd'])
            self.set_touch(self.all_modules[module]['touch'])
            self.set_gen(self.all_modules[module]['generation'])
        else:
            print('Invalid module')

    def set_gen(self, gen):
        if gen in self._all_settings:
            self.generation = gen;
        else:
            print('Invalid ic generation')
    
    def set_ic(self, ic):
        if ic in self._all_settings:
            self.ic = ic;
        else:
            print('Invalid ic')

    def set_lcd(self, lcd):
        if lcd in self._all_settings:
            self.lcd = lcd;
        else:
            print('Invalid lcd')

    def set_touch(self, touch):
        if touch in self._all_settings:
            self.touch = touch;
        else:
            print('Invalid touch')
        
    def MODULE_VM800P35A_BK       (self): return 'module_vm800p35a_bk'
    def MODULE_VM800P43A_BK       (self): return 'module_vm800p43a_bk'
    def MODULE_VM800P50A_BK       (self): return 'module_vm800p50a_bk'
    def MODULE_VM801P43A_BK       (self): return 'module_vm801p43a_bk'
    def MODULE_VM801P50A_BK       (self): return 'module_vm801p50a_bk'
    def MODULE_VM800B35A_BK       (self): return 'module_vm800b35a_bk'
    def MODULE_VM800B43A_BK       (self): return 'module_vm800b43a_bk'
    def MODULE_VM800B50A_BK       (self): return 'module_vm800b50a_bk'
    def MODULE_VM801B43A_BK       (self): return 'module_vm801b43a_bk'
    def MODULE_VM801B50A_BK       (self): return 'module_vm801b50a_bk'
    def MODULE_VM800C35A_N        (self): return 'module_vm800c35a_n'
    def MODULE_VM800C43A_N        (self): return 'module_vm800c43a_n'
    def MODULE_VM800C35A_D        (self): return 'module_vm800c35a_d'
    def MODULE_VM800C43A_D        (self): return 'module_vm800c43a_d'
    def MODULE_VM800C50A_D        (self): return 'module_vm800c50a_d'
    def MODULE_ME812A_WH50R       (self): return 'module_me812a_wh50r'
    def MODULE_ME812AU_WH50R      (self): return 'module_me812au_wh50r'
    def MODULE_ME813A_WH50C       (self): return 'module_me813a_wh50c'
    def MODULE_ME813AU_WH50C      (self): return 'module_me813au_wh50c'
    def MODULE_VM810C50A_D        (self): return 'module_vm810c50a_d'
    def MODULE_VM810C50A_N        (self): return 'module_vm810c50a_n'
    def MODULE_ME810A_HV35R       (self): return 'module_me810a_hv35r'
    def MODULE_VM816C50A_D        (self): return 'module_vm816c50a_d'
    def MODULE_VM816C50A_N        (self): return 'module_vm816c50a_n'
    def MODULE_VM816CU50A_D       (self): return 'module_vm816cu50a_d'
    def MODULE_VM816CU50A_N       (self): return 'module_vm816cu50a_n'
    def MODULE_ME817EV            (self): return 'module_me817ev'
    def GENERATION_EVE1           (self): return 'generation_eve1'
    def GENERATION_EVE2           (self): return 'generation_eve2'
    def GENERATION_EVE3           (self): return 'generation_eve3'
    def GENERATION_EVE4           (self): return 'generation_eve4'
    def IC_EVE_FT800              (self): return 'ic_eve_ft800'
    def IC_EVE_FT801              (self): return 'ic_eve_ft801'
    def IC_EVE_FT810              (self): return 'ic_eve_ft810'
    def IC_EVE_FT811              (self): return 'ic_eve_ft811'
    def IC_EVE_FT812              (self): return 'ic_eve_ft812'
    def IC_EVE_FT813              (self): return 'ic_eve_ft813'
    def IC_EVE_BT815              (self): return 'ic_eve_bt815'
    def IC_EVE_BT816              (self): return 'ic_eve_bt816'
    def IC_EVE_BT817              (self): return 'ic_eve_bt817'
    def IC_EVE_BT818              (self): return 'ic_eve_bt818'
    def CLASS_VM800P              (self): return 'class_vm800p'
    def CLASS_VM801P              (self): return 'class_vm801p'
    def CLASS_VM800B              (self): return 'class_vm800b'
    def CLASS_VM801B              (self): return 'class_vm801b'
    def CLASS_VM800C              (self): return 'class_vm800c'
    def CLASS_VM810C              (self): return 'class_vm810c'
    def CLASS_VM816C              (self): return 'class_vm816c'
    def CLASS_817_8               (self): return 'class_817/8'
    def LCD_QVGA_320X240          (self): return 'lcd_qvga_320x240'
    def LCD_WQVGA_480X272         (self): return 'lcd_wqvga_480x272'
    def LCD_VGA_600X480           (self): return 'lcd_vga_600x480'
    def LCD_WVGA_800X480          (self): return 'lcd_wvga_800x480'
    def LCD_WSVGA_1024X600        (self): return 'lcd_wsvga_1024x600'
    def LCD_WXGA_1280X720         (self): return 'lcd_wxga_1280x720'
    def LCD_WXGA_1280X800         (self): return 'lcd_wxga_1280x800'
    def LCD_HVGA_320X480_ILI9488  (self): return 'lcd_hvga_320x480_ili9488'
    def TOUCH_FOCALTECH           (self): return 'touch_focaltech' 
    def TOUCH_GOODIX              (self): return 'touch_goodix   ' 
    def TOUCH_RESISTIVE           (self): return 'touch_resistive' 
    def TOUCH_CAPACITIVE          (self): return 'touch_capacitive'

    def _get_module_info(self):
        return {
            self.MODULE_VM800P35A_BK     ():{
                'lcd'       :self.LCD_QVGA_320X240(),
                'ic'        :self.IC_EVE_FT800(),
                'touch'     :self.TOUCH_RESISTIVE(),
                'generation':self.GENERATION_EVE1(),
                'class'     :self.CLASS_VM800P(),

            },
            self.MODULE_VM800P43A_BK     ():{
                'lcd'       :self.LCD_WQVGA_480X272(),
                'ic'        :self.IC_EVE_FT800(),
                'touch'     :self.TOUCH_RESISTIVE(),
                'generation':self.GENERATION_EVE1(),
                'class'     :self.CLASS_VM800P(),
            },
            self.MODULE_VM800P50A_BK     ():{
                'lcd'       :self.LCD_WQVGA_480X272(),
                'ic'        :self.IC_EVE_FT800(),
                'touch'     :self.TOUCH_RESISTIVE(),
                'generation':self.GENERATION_EVE1(),
                'class'     :self.CLASS_VM800P(),
            },
            self.MODULE_VM801P43A_BK     ():{
                'lcd'       :self.LCD_WQVGA_480X272(),
                'ic'        :self.IC_EVE_FT801(),
                'touch'     :self.TOUCH_CAPACITIVE(),
                'generation':self.GENERATION_EVE1(),
                'class'     :self.CLASS_VM801P()
            },
            self.MODULE_VM801P50A_BK     ():{
                'lcd'       :self.LCD_WQVGA_480X272(),
                'ic'        :self.IC_EVE_FT801(),
                'touch'     :self.TOUCH_CAPACITIVE(),
                'generation':self.GENERATION_EVE1(),
                'class'     :self.CLASS_VM801P()
            },
            self.MODULE_VM800B35A_BK     ():{
                'lcd'       :self.LCD_QVGA_320X240(),
                'ic'        :self.IC_EVE_FT800(),
                'touch'     :self.TOUCH_RESISTIVE(),
                'generation':self.GENERATION_EVE1(),
                'class'     :self.CLASS_VM800B()
            },
            self.MODULE_VM800B43A_BK     ():{
                'lcd'       :self.LCD_WQVGA_480X272(),
                'ic'        :self.IC_EVE_FT800(),
                'touch'     :self.TOUCH_RESISTIVE(),
                'generation':self.GENERATION_EVE1(),
                'class'     :self.CLASS_VM800B()
            },
            self.MODULE_VM800B50A_BK     ():{
                'lcd'       :self.LCD_WQVGA_480X272(),
                'ic'        :self.IC_EVE_FT800(),
                'touch'     :self.TOUCH_RESISTIVE(),
                'generation':self.GENERATION_EVE1(),
                'class'     :self.CLASS_VM800B()
            },
            self.MODULE_VM801B43A_BK     ():{
                'lcd'       :self.LCD_WQVGA_480X272(),
                'ic'        :self.IC_EVE_FT801(),
                'touch'     :self.TOUCH_CAPACITIVE(),
                'generation':self.GENERATION_EVE1(),
                'class'     :self.CLASS_VM801B()
            },
            self.MODULE_VM801B50A_BK     ():{
                'lcd'       :self.LCD_WQVGA_480X272(),
                'ic'        :self.IC_EVE_FT801(),
                'touch'     :self.TOUCH_CAPACITIVE(),
                'generation':self.GENERATION_EVE1(),
                'class'     :self.CLASS_VM801B()
            },
            self.MODULE_VM800C35A_N      ():{
                'lcd'       :'ADAPTER BOARD for 3.5in TFT WITHOUT DISPLAY',
                'ic'        :self.IC_EVE_FT800(),
                'touch'     :self.TOUCH_RESISTIVE(),
                'generation':self.GENERATION_EVE1(),
                'class'     :self.CLASS_VM800C()
            },
            self.MODULE_VM800C43A_N      ():{
                'lcd'       :'ADAPTER BOARD FOR 4.3IN TFT WITHOUT DISPLAY',
                'ic'        :self.IC_EVE_FT800(),
                'touch'     :self.TOUCH_RESISTIVE(),
                'generation':self.GENERATION_EVE1(),
                'class'     :self.CLASS_VM800C()
            },
            self.MODULE_VM800C35A_D      ():{
                'lcd'       :self.LCD_QVGA_320X240(),
                'ic'        :self.IC_EVE_FT800(),
                'touch'     :self.TOUCH_RESISTIVE(),
                'generation':self.GENERATION_EVE1(),
                'class'     :self.CLASS_VM800C()
            },
            self.MODULE_VM800C43A_D      ():{
                'lcd'       :self.LCD_WQVGA_480X272(),
                'ic'        :self.IC_EVE_FT800(),
                'touch'     :self.TOUCH_RESISTIVE(),
                'generation':self.GENERATION_EVE1(),
                'class'     :self.CLASS_VM800C()
            },
            self.MODULE_VM800C50A_D      ():{
                'lcd'       :self.LCD_WQVGA_480X272(),
                'ic'        :self.IC_EVE_FT800(),
                'touch'     :self.TOUCH_RESISTIVE(),
                'generation':self.GENERATION_EVE1(),
                'class'     :self.CLASS_VM800C()
            },
            self.MODULE_ME812A_WH50R     ():{
                'lcd'       :self.LCD_WVGA_800X480(),
                'ic'        :self.IC_EVE_FT812(),
                'touch'     :self.TOUCH_RESISTIVE(),
                'generation':self.GENERATION_EVE2(),
                'class'     :self.CLASS_VM810C()
            },
            self.MODULE_ME812AU_WH50R    ():{
                'lcd'       :self.LCD_WVGA_800X480(),
                'ic'        :self.IC_EVE_FT812(),
                'touch'     :self.TOUCH_RESISTIVE(),
                'generation':self.GENERATION_EVE2(),
                'class'     :self.CLASS_VM810C()
            },
            self.MODULE_ME813A_WH50C     ():{
                'lcd'       :self.LCD_WVGA_800X480(),
                'ic'        :self.IC_EVE_FT813(),
                'touch'     :self.TOUCH_CAPACITIVE(),
                'generation':self.GENERATION_EVE2(),
                'class'     :self.CLASS_VM810C()
            },
            self.MODULE_ME813AU_WH50C    ():{
                'lcd'       :self.LCD_WVGA_800X480(),
                'ic'        :self.IC_EVE_FT813(),
                'touch'     :self.TOUCH_CAPACITIVE(),
                'generation':self.GENERATION_EVE2(),
                'class'     :self.CLASS_VM810C()
            },
            self.MODULE_VM810C50A_D      ():{
                'lcd'       :self.LCD_WVGA_800X480(),
                'ic'        :self.IC_EVE_FT810(),
                'touch'     :self.TOUCH_RESISTIVE(),
                'generation':self.GENERATION_EVE2(),
                'class'     :self.CLASS_VM810C()
            },
            self.MODULE_VM810C50A_N      ():{
                'lcd'       :'ADAPTER BOARD FOR 4.3/5.0IN TFT WITHOUT DISPLAY',
                'ic'        :self.IC_EVE_FT810(),
                'touch'     :self.TOUCH_RESISTIVE(),
                'generation':self.GENERATION_EVE2(),
                'class'     :self.CLASS_VM810C()
            },
            self.MODULE_ME810A_HV35R     ():{
                'lcd'       :self.LCD_HVGA_320X480_ILI9488(),
                'ic'        :self.IC_EVE_FT810(),
                'touch'     :self.TOUCH_RESISTIVE(),
                'generation':self.GENERATION_EVE2(),
                'class'     :self.CLASS_VM810C()
            },
            self.MODULE_VM816C50A_D      ():{
                'lcd'       :self.LCD_WVGA_800X480(),
                'ic'        :self.IC_EVE_BT816(),
                'touch'     :self.TOUCH_RESISTIVE(),
                'generation':self.GENERATION_EVE3(),
                'class'     :self.CLASS_VM816C()
            },
            self.MODULE_VM816C50A_N      ():{
                'lcd'       :'ADAPTER BOARD FOR 5.0IN TFT WITHOUT DISPLAY',
                'ic'        :self.IC_EVE_BT816(),
                'touch'     :self.TOUCH_RESISTIVE(),
                'generation':self.GENERATION_EVE3(),
                'class'     :self.CLASS_VM816C()
            },
            self.MODULE_VM816CU50A_D     ():{
                'lcd'       :self.LCD_WVGA_800X480(),
                'ic'        :self.IC_EVE_BT816(),
                'touch'     :self.TOUCH_RESISTIVE(),
                'generation':self.GENERATION_EVE3(),
                'class'     :self.CLASS_VM816C()
            },
            self.MODULE_VM816CU50A_N     ():{
                'lcd'       :'ADAPTER BOARD FOR 5.0IN TFT WITHOUT DISPLAY',
                'ic'        :self.IC_EVE_BT816(),
                'touch'     :self.TOUCH_RESISTIVE(),
                'generation':self.GENERATION_EVE3(),
                'class'     :self.CLASS_VM816C()
            },
            self.MODULE_ME817EV          ():{
                'lcd'       :'ADAPTER BOARD FOR 5.0IN TFT WITHOUT DISPLAY',
                'ic'        :self.IC_EVE_BT817(),
                'touch'     :self.TOUCH_CAPACITIVE(),
                'generation':self.GENERATION_EVE4(),
                'class'     :self.CLASS_VM816C()
            },
        }

    def _get_all_settings(self):
        return {
            self.MODULE_VM800P35A_BK       (),
            self.MODULE_VM800P43A_BK       (),
            self.MODULE_VM800P50A_BK       (),
            self.MODULE_VM801P43A_BK       (),
            self.MODULE_VM801P50A_BK       (),
            self.MODULE_VM800B35A_BK       (),
            self.MODULE_VM800B43A_BK       (),
            self.MODULE_VM800B50A_BK       (),
            self.MODULE_VM801B43A_BK       (),
            self.MODULE_VM801B50A_BK       (),
            self.MODULE_VM800C35A_N        (),
            self.MODULE_VM800C43A_N        (),
            self.MODULE_VM800C35A_D        (),
            self.MODULE_VM800C43A_D        (),
            self.MODULE_VM800C50A_D        (),
            self.MODULE_ME812A_WH50R       (),
            self.MODULE_ME812AU_WH50R      (),
            self.MODULE_ME813A_WH50C       (),
            self.MODULE_ME813AU_WH50C      (),
            self.MODULE_VM810C50A_D        (),
            self.MODULE_VM810C50A_N        (),
            self.MODULE_ME810A_HV35R       (),
            self.MODULE_VM816C50A_D        (),
            self.MODULE_VM816C50A_N        (),
            self.MODULE_VM816CU50A_D       (),
            self.MODULE_VM816CU50A_N       (),
            self.MODULE_ME817EV            (),
            self.GENERATION_EVE1           (),
            self.GENERATION_EVE2           (),
            self.GENERATION_EVE3           (),
            self.GENERATION_EVE4           (),
            self.IC_EVE_FT800              (),
            self.IC_EVE_FT801              (),
            self.IC_EVE_FT810              (),
            self.IC_EVE_FT811              (),
            self.IC_EVE_FT812              (),
            self.IC_EVE_FT813              (),
            self.IC_EVE_BT815              (),
            self.IC_EVE_BT816              (),
            self.IC_EVE_BT817              (),
            self.IC_EVE_BT818              (),
            self.LCD_QVGA_320X240          (),
            self.LCD_WQVGA_480X272         (),
            self.LCD_WVGA_800X480          (),
            self.LCD_WSVGA_1024X600        (),
            self.LCD_WXGA_1280X800         (),
            self.LCD_HVGA_320X480_ILI9488 (),
            self.TOUCH_FOCALTECH           (),
            self.TOUCH_GOODIX              (),
            self.TOUCH_RESISTIVE           (),
            self.TOUCH_CAPACITIVE          (),
        }

    #For Gameduino 3x dazzler (HDMI output) , Eve Type = BT815/6
    def setup_1280x720(self):
        self.Clear()
        self.swap()
        setup = [
            (self.mcu.REG_OUTBITS, 0),
            (self.mcu.REG_DITHER, 0),
            (self.mcu.REG_GPIO, 0x83),
            (self.mcu.REG_CSPREAD, 0),
            (self.mcu.REG_PCLK_POL, 0),
            (self.mcu.REG_ADAPTIVE_FRAMERATE, 0),
        ]
        for (a, v) in setup:
            self.cmd_regwrite(a, v)

        self.video_signal(
            h_Active = 1280,
            h_Front = 110,
            h_Sync = 40,
            h_Back = 220,
            h_Total = 1650,
            v_Active = 720,
            v_Front = 5,
            v_Sync = 5,
            v_Back = 20,
            v_Total = 750)
        self.w = 1280
        self.h = 720
        return

    #For XXXX board QVGA LCD panel
    def setup_320x240(self):
        self.Clear()
        self.swap()
        setup = [
            # (self.mcu.REG_OUTBITS, 0),
            (self.mcu.REG_DITHER, 1),
            (self.mcu.REG_GPIO, 0x83),
            (self.mcu.REG_CSPREAD, 1),
            (self.mcu.REG_PCLK_POL, 0),
            (self.mcu.REG_SWIZZLE, 2),
        
            (self.mcu.REG_HCYCLE, 408),
            (self.mcu.REG_HOFFSET, 70),
            (self.mcu.REG_HSIZE, 320),
        
            (self.mcu.REG_HSYNC1, 10),
            (self.mcu.REG_HSYNC0, 0),
        
            (self.mcu.REG_VCYCLE, 263),
            (self.mcu.REG_VOFFSET, 13),
            (self.mcu.REG_VSIZE, 240),
        
            (self.mcu.REG_VSYNC1, 2),
            (self.mcu.REG_VSYNC0, 0),
        ]
        for (a, v) in setup:
            self.cmd_regwrite(a, v)
        

        self.cmd_regwrite(self.mcu.REG_PCLK, 8)
        self.w = 320
        self.h = 240

    #For ME810A-HV35R board, Eve Type >= FT180. 
    def setup_320x480(self):
        self.Clear()
        self.swap()
        setup = [
            # (self.mcu.REG_OUTBITS, 0),
            (self.mcu.REG_DITHER, 1),
            (self.mcu.REG_GPIO, 0x83),
            (self.mcu.REG_CSPREAD, 1),
            (self.mcu.REG_PCLK_POL, 1),
            (self.mcu.REG_SWIZZLE, 2),
        
            (self.mcu.REG_HCYCLE, 400),
            (self.mcu.REG_HOFFSET, 40),
            (self.mcu.REG_HSIZE, 320),
        
            (self.mcu.REG_HSYNC1, 10),
            (self.mcu.REG_HSYNC0, 0),
        
            (self.mcu.REG_VCYCLE, 500),
            (self.mcu.REG_VOFFSET, 10),
            (self.mcu.REG_VSIZE, 480),
        
            (self.mcu.REG_VSYNC1, 5),
            (self.mcu.REG_VSYNC0, 0),
        ]
        for (a, v) in setup:
            self.cmd_regwrite(a, v)
        

        self.cmd_regwrite(self.mcu.REG_PCLK, 5)
        self.w = 320
        self.h = 480

    def setup_640x480(self):
        self.Clear()
        self.swap()
        setup = [
            # (self.mcu.REG_OUTBITS, 0),
            (self.mcu.REG_DITHER, 0),
            (self.mcu.REG_GPIO, 0x83),
            (self.mcu.REG_CSPREAD, 0),
            (self.mcu.REG_PCLK_POL, 0),
            (self.mcu.REG_ADAPTIVE_FRAMERATE, 0),
        
            (self.mcu.REG_HCYCLE, 800),
            (self.mcu.REG_HOFFSET, 16 + 96),
            (self.mcu.REG_HSIZE, 640),
        
            (self.mcu.REG_HSYNC1, 0),
            (self.mcu.REG_HSYNC0, 96),
        
            (self.mcu.REG_VCYCLE, 525),
            (self.mcu.REG_VOFFSET, 12),
            (self.mcu.REG_VSIZE, 480),
        
            (self.mcu.REG_VSYNC1, 0),
            (self.mcu.REG_VSYNC0, 10),
        ]
        for (a, v) in setup:
            self.cmd_regwrite(a, v)
        

        self.cmd_regwrite(self.mcu.REG_PCLK, 3)
        self.w = 640
        self.h = 480

    #Good for Riverdi 800x480 panel, Eve Type >= FT810
    def setup_800x480(self):
        self.Clear()
        self.swap()
        setup = [
            # (self.mcu.REG_OUTBITS, 0),
            (self.mcu.REG_DITHER, 1),
            (self.mcu.REG_GPIO_DIR, 0xFF),
            (self.mcu.REG_GPIO, 0xFF),
            (self.mcu.REG_CSPREAD, 0),
            (self.mcu.REG_PCLK_POL, 1),
            (self.mcu.REG_ADAPTIVE_FRAMERATE, 0),

            (self.mcu.REG_HCYCLE, 928),
            (self.mcu.REG_HOFFSET, 88),
            (self.mcu.REG_HSIZE, 800),

            (self.mcu.REG_HSYNC1, 48),
            (self.mcu.REG_HSYNC0, 0),

            (self.mcu.REG_VCYCLE, 525),
            (self.mcu.REG_VOFFSET, 32),
            (self.mcu.REG_VSIZE, 480),

            (self.mcu.REG_VSYNC1, 3),
            (self.mcu.REG_VSYNC0, 0),
        ]
        for (a, v) in setup:
            self.cmd_regwrite(a, v)


        self.cmd_regwrite(self.mcu.REG_PCLK, 2)
        self.finish()

        self.w = 800
        self.h = 480

    #Good for Riverdi 1024x600 panel (RGB40pin) , Eve Type = BT817/8
    def setup_1024x600(self):
        self.Clear()
        self.swap()
        setup = [
            # (self.mcu.REG_OUTBITS, 0),
            (self.mcu.REG_DITHER, 1),
            (self.mcu.REG_GPIO_DIR, 0xFF),
            (self.mcu.REG_GPIO, 0xFF),
            (self.mcu.REG_CSPREAD, 0),
            (self.mcu.REG_PCLK_POL, 1),
            (self.mcu.REG_ADAPTIVE_FRAMERATE, 0),

            (self.mcu.REG_HCYCLE, 1344),
            (self.mcu.REG_HOFFSET, 160),
            (self.mcu.REG_HSIZE, 1024),

            (self.mcu.REG_HSYNC1, 100),
            (self.mcu.REG_HSYNC0, 0),

            (self.mcu.REG_VCYCLE, 635),
            (self.mcu.REG_VOFFSET, 23),
            (self.mcu.REG_VSIZE, 600),

            (self.mcu.REG_VSYNC1, 10),
            (self.mcu.REG_VSYNC0, 0),
        ]
        for (a, v) in setup:
            self.cmd_regwrite(a, v)


        self.cmd_regwrite(self.mcu.REG_PCLK, 1)
        self.cmd_regwrite(self.mcu.REG_PCLK_FREQ, 0xD12)
        self.finish()

        self.w = 1024
        self.h = 600

    def setup_480x272(self):
        self.Clear()
        self.swap()
        setup = [
            (self.mcu.REG_DITHER, 0),
            (self.mcu.REG_GPIO_DIR, 0xFF),
            (self.mcu.REG_GPIO, 0xFF),
            (self.mcu.REG_CSPREAD, 0),
            (self.mcu.REG_PCLK_POL, 1),

            (self.mcu.REG_HCYCLE, 548),
            (self.mcu.REG_HOFFSET, 43),
            (self.mcu.REG_HSIZE, 480),

            (self.mcu.REG_HSYNC1, 41),
            (self.mcu.REG_HSYNC0, 0),

            (self.mcu.REG_VCYCLE, 292),
            (self.mcu.REG_VOFFSET, 12),
            (self.mcu.REG_VSIZE, 272),

            (self.mcu.REG_VSYNC1, 10),
            (self.mcu.REG_VSYNC0, 0),
        ]
        for (a, v) in setup:
            self.cmd_regwrite(a, v)

        if ( self.generation == self.GENERATION_EVE3() or 
            self.generation == self.GENERATION_EVE4() ):
            self.cmd_regwrite(self.mcu.REG_ADAPTIVE_FRAMERATE, 0)
            self.cmd_regwrite(self.mcu.REG_PCLK_FREQ, 0xD12)

        self.cmd_regwrite(self.mcu.REG_PCLK, 5)
        self.finish()

        self.w = 480
        self.h = 272

    # For BT817/8 based module with 1280x800 LCD
    def setup_1280x800(self):
        self.Clear()
        self.swap()
        setup = [
            (self.mcu.REG_OUTBITS, 0),
            (self.mcu.REG_PCLK, 0),
            (self.mcu.REG_DITHER, 0),
            (self.mcu.REG_GPIO_DIR, 0xffff),
            (self.mcu.REG_GPIO, 0xffff),
            (self.mcu.REG_CSPREAD, 0),
            (self.mcu.REG_PCLK_POL, 0),
            (self.mcu.REG_PCLK_2X, 1),
            #(self.mcu.REG_ADAPTIVE_FRAMERATE, 0),

            (self.mcu.REG_HCYCLE, 1411),
            (self.mcu.REG_HOFFSET, 120),
            (self.mcu.REG_HSIZE, 1280),

            (self.mcu.REG_HSYNC1, 100),
            (self.mcu.REG_HSYNC0, 0),

            (self.mcu.REG_VCYCLE, 815),
            (self.mcu.REG_VOFFSET, 14),
            (self.mcu.REG_VSIZE, 800),

            (self.mcu.REG_VSYNC1, 10),
            (self.mcu.REG_VSYNC0, 0),
            (self.mcu.REG_PCLK_FREQ, 0x8B1),
        ]
        for (a, v) in setup:
            self.cmd_regwrite(a, v)

        self.cmd_regwrite(self.mcu.REG_PCLK, 1)
        self.finish()
		
        self.w = 1280
        self.h = 800		

    # Some higher-level functions

    def get_inputs(self):
        self.finish()
        t = _Touch(*struct.unpack("HHIhhhhB", self.rd(REG_TOUCH_RAW_XY, 17)))

        r = _Tracker(*struct.unpack("HH", self.rd(self.mcu.REG_TRACKER, 4)))

        if not hasattr(self, "prev_touching"):
            self.prev_touching = False
        touching = (t.x != -32768)
        press = touching and not self.prev_touching
        release = (not touching) and self.prev_touching
        s = _State(touching, press, release)
        self.prev_touching = touching

        self.inputs = _Inputs(t, r, s)
        return self.inputs

    def swap(self):
        self.Display()
        self.cmd_swap()
        self.flush()
        self.cmd_dlstart()
        self.cmd_loadidentity()

    def calibrate(self):
        self.Clear()
        self.cmd_text(self.w // 2, self.h // 2, 29, 0x0600, "Tap the dot")
        self.cmd_calibrate(0)
        self.cmd_dlstart()

    def screenshot(self, dest):
        import time
        REG_SCREENSHOT_EN    = 0x302010 # Set to enable screenshot mode
        REG_SCREENSHOT_Y     = 0x302014 # Y line register
        REG_SCREENSHOT_START = 0x302018 # Screenshot start trigger
        REG_SCREENSHOT_BUSY  = 0x3020e8 # Screenshot ready flags
        REG_SCREENSHOT_READ  = 0x302174 # Set to enable readout
        RAM_SCREENSHOT       = 0x3c2000 # Screenshot readout buffer

        self.finish()

        pclk = self.rd32(self.mcu.REG_PCLK)
        self.wr32(self.mcu.REG_PCLK, 0)
        time.sleep(0.001)
        self.wr32(self.mcu.REG_SCREENSHOT_EN, 1)
        self.wr32(0x0030201c, 32)
        
        for ly in range(self.h):
            print(ly, "/", self.h)
            self.wr32(self.mcu.REG_SCREENSHOT_Y, ly)
            self.wr32(self.mcu.REG_SCREENSHOT_START, 1)
            time.sleep(.002)
            # while (self.raw_read(self.mcu.REG_SCREENSHOT_BUSY) | self.raw_read(self.mcu.REG_SCREENSHOT_BUSY + 4)): pass
            while self.rd(self.mcu.REG_SCREENSHOT_BUSY, 8) != bytes(8):
                pass
            self.wr32(self.mcu.REG_SCREENSHOT_READ, 1)
            bgra = self.rd(RAM_SCREENSHOT, 4 * self.w)
            (b,g,r,a) = [bgra[i::4] for i in range(4)]
            line = bytes(sum(zip(r,g,b), ()))
            dest(line)
            self.wr32(self.mcu.REG_SCREENSHOT_READ, 0)
        self.wr32(self.mcu.REG_SCREENSHOT_EN, 0)
        self.wr32(self.mcu.REG_PCLK, pclk)

    def screenshot_im(self):
        self.ssbytes = b""
        def appender(s):
            self.ssbytes += s
        self.screenshot(appender)
        from PIL import Image
        return Image.frombytes("RGB", (self.w, self.h), self.ssbytes)

    def load(self, f):
        while True:
            s = f.read(512)
            if not s:
                return
            self.cc(align4(s))

