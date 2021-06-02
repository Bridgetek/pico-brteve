import struct
import array
from collections import namedtuple

_B0 = b'\x00'
def align4(s):
    """
    :param bytes s: input bytes object
    :return: the bytes object extended so that its length is a multiple of 4
    """
    return s + _B0 * (-len(s) & 3)

def f16(v):
    return int(round(65536 * v))

def furmans(deg):
    """ Given an angle in degrees, return it in Furmans """
    return 0xffff & f16(deg / 360.0)

class EVE():
    def __init__(self):
        # Macros for general purpose
        self.DISABLE                    = 0
        self.ENABLE                     = 1
        self.FALSE                      = 0
        self.TRUE                       = 1

        # Bitmap Swizzle
        self.RED                        = (2)
        self.GREEN                      = (3)
        self.BLUE                       = (4)
        self.ALPHA                      = (5)

        # Macros used for graphics commands
        self.NEVER                      = 0
        self.LESS                       = 1
        self.LEQUAL                     = 2
        self.GREATER                    = 3
        self.GEQUAL                     = 4
        self.EQUAL                      = 5
        self.NOTEQUAL                   = 6
        self.ALWAYS                     = 7
        
        # Bitmap format macros
        self.ARGB1555                   = 0
        self.L1                         = 1
        self.L4                         = 2
        self.L8                         = 3
        self.RGB332                     = 4
        self.ARGB2                      = 5
        self.ARGB4                      = 6
        self.RGB565                     = 7
        self.PALETTED                   = 8
        self.TEXT8X8                    = 9
        self.TEXTVGA                    = 10
        self.BARGRAPH                   = 11
        
        # Bitmap filter type macros
        self.NEAREST                    = 0
        self.BILINEAR                   = 1

        # Bitmap wrap type macros
        self.BORDER                     = 0
        self.REPEAT                     = 1
        # Stencil macros
        self.KEEP                       = 1
        self.REPLACE                    = 2
        self.INCR                       = 3
        self.DECR                       = 4
        self.INVERT                     = 5

        # Graphics display list swap macros
        self.DLSWAP_DONE                = 0
        self.DLSWAP_LINE                = 1
        self.DLSWAP_FRAME               = 2

        # Interrupt bits
        self.INT_SWAP                   = 0x01
        self.INT_TOUCH                  = 0x02
        self.INT_TAG                    = 0x04
        self.INT_SOUND                  = 0x08
        self.INT_PLAYBACK               = 0x10
        self.INT_CMDEMPTY               = 0x20
        self.INT_CMDFLAG                = 0x40
        self.INT_CONVCOMPLETE           = 0x80

        # Touch mode macros
        self.TMODE_OFF                  = 0
        self.TMODE_ONESHOT              = 1
        self.TMODE_FRAME                = 2
        self.TMODE_CONTINUOUS           = 3

        # Alpha blending macros
        self.ZERO                       = 0
        self.ONE                        = 1
        self.SRC_ALPHA                  = 2
        self.DST_ALPHA                  = 3
        self.ONE_MINUS_SRC_ALPHA        = 4
        self.ONE_MINUS_DST_ALPHA        = 5

        # Graphics primitives macros
        self.BITMAPS                    = 1
        self.POINTS                     = 2
        self.LINES                      = 3
        self.LINE_STRIP                 = 4
        self.EDGE_STRIP_R               = 5
        self.EDGE_STRIP_L               = 6
        self.EDGE_STRIP_A               = 7
        self.EDGE_STRIP_B               = 8
        self.RECTS                      = 9

        # Widget command macros
        self.OPT_MONO                   = 1
        self.OPT_NODL                   = 2
        self.OPT_FLAT                   = 256
        self.OPT_CENTERX                = 512
        self.OPT_CENTERY                = 1024
        self.OPT_CENTER                 = (self.OPT_CENTERX | self.OPT_CENTERY)
        self.OPT_NOBACK                 = 4096
        self.OPT_NOTICKS                = 8192
        self.OPT_NOHM                   = 16384
        self.OPT_NOPOINTER              = 16384
        self.OPT_NOSECS                 = 32768
        self.OPT_NOHANDS                = 49152
        self.OPT_RIGHTX                 = 2048
        self.OPT_SIGNED                 = 256

        # Audio sample type macros
        self.LINEAR_SAMPLES             = 0    #8bit signed samples
        self.ULAW_SAMPLES               = 1    #8bit ulaw samples
        self.ADPCM_SAMPLES              = 2    #4bit ima adpcm samples

        # The built-in audio samples
        self.HARP                       = 0x40
        self.XYLOPHONE                  = 0x41
        self.TUBA                       = 0x42
        self.GLOCKENSPIEL               = 0x43
        self.ORGAN                      = 0x44
        self.TRUMPET                    = 0x45
        self.PIANO                      = 0x46
        self.CHIMES                     = 0x47
        self.MUSICBOX                   = 0x48
        self.BELL                       = 0x49

        self.CLICK                      = 0x50
        self.SWITCH                     = 0x51
        self.COWBELL                    = 0x52
        self.NOTCH                      = 0x53
        self.HIHAT                      = 0x54
        self.KICKDRUM                   = 0x55
        self.POP                        = 0x56
        self.CLACK                      = 0x57
        self.CHACK                      = 0x58

        self.MUTE                       = 0x60
        self.UNMUTE                     = 0x61

        # Host command macros
        self.ACTIVE                     = 0x00 # Place FT800 in active state
        self.STANDBY                    = 0x41 # Place FT800 in Standby (clk running)
        self.SLEEP                      = 0x42 # Place FT800 in Sleep (clk off)
        self.PWRDOWN                    = 0x50 # Place FT800 in Power Down (core off)
        self.CLKEXT                     = 0x44 # Select external clock source
        self.CLKINT                     = 0x48 # Select internal clock source
        self.CLK48M                     = 0x62 # Select 48MHz PLL output
        self.CLK36M                     = 0x61 # Select 36MHz PLL output
        self.CORERST                    = 0x68 # Reset core - all registers default and processors reset

    def cstring(self, s):
        if type(s) == str:
            s = bytes(s, "utf-8")
        self.cc(align4(s + _B0))

    def fstring(self, aa):
        self.cstring(aa[0])
        # XXX MicroPython is currently lacking array.array.tobytes()
        self.cc(bytes(array.array("i", aa[1:])))

    def cmd_append(self, *args):
        self.cmd(0x1e, "II", args)

    def cmd_bgcolor(self, *args):
        self.cmd(0x09, "I", args)

    def cmd_bitmap_transform(self, *args):
        self.cmd(0x21, "iiiiiiiiiiiiI", args)

    def cmd_touch_transform(self, *args):
        self.cmd(0x20, "iiiiiiiiiiiiI", args)

    def cmd_button(self, *args):
        self.cmd(0x0d, "hhhhhH", args[:6])
        self.fstring(args[6:])

    def cmd_calibrate(self, *args):
        self.cmd(0x15, "I", args)

    def cmd_clock(self, *args):
        self.cmd(0x14, "hhhHHHHH", args)

    def cmd_coldstart(self):
        self.cmd0(0x32)

    def cmd_dial(self, x, y, r, options, val):
        self.cmd(0x2d, "hhhHI", (x, y, r, options, furmans(val)))

    def cmd_dlstart(self):
        self.cmd0(0x00)

    def cmd_fgcolor(self, *args):
        self.cmd(0x0a, "I", args)

    def cmd_gauge(self, *args):
        self.cmd(0x13, "hhhHHHHH", args)

    def cmd_getmatrix(self, *args):
        self.cmd(0x33, "iiiiii", args)

    def cmd_getprops(self, *args):
        self.cmd(0x25, "III", args)

    def cmd_getptr(self, *args):
        self.cmd(0x23, "I", args)

    def cmd_gradcolor(self, *args):
        self.cmd(0x34, "I", args)

    def cmd_gradient(self, *args):
        self.cmd(0x0b, "hhIhhI", args)

    def cmd_inflate(self, *args):
        self.cmd(0x22, "I", args)

    def cmd_interrupt(self, *args):
        self.cmd(0x02, "I", args)

    def cmd_keys(self, *args):
        self.cmd(0x0e, "hhhhhH", args[:6])
        self.cstring(args[6])

    def cmd_loadidentity(self):
        self.cmd0(0x26)

    def cmd_loadimage(self, *args):
        self.cmd(0x24, "iI", args)

    def cmd_logo(self):
        self.cmd0(0x31)

    def cmd_memcpy(self, *args):
        self.cmd(0x1d, "III", args)

    def cmd_memcrc(self, *args):
        self.cmd(0x18, "III", args)

    def cmd_memset(self, *args):
        self.cmd(0x1b, "III", args)

    def cmd_memwrite(self, *args):
        self.cmd(0x1a, "II", args)

    def cmd_regwrite(self, ptr, val):
        self.cmd(0x1a, "III", (ptr, 4, val))

    def cmd_memzero(self, *args):
        self.cmd(0x1c, "II", args)

    def cmd_number(self, *args):
        args_int = tuple(int(el) for el in args)
        self.cmd(0x2e, "hhhHi", args_int)

    def cmd_progress(self, *args):
        self.cmd(0x0f, "hhhhHHI", args)

    def cmd_regread(self, *args):
        self.cmd(0x19, "II", args)

    def cmd_rotate(self, a):
        self.cmd(0x29, "i", (furmans(a), ))

    def cmd_scale(self, sx, sy):
        self.cmd(0x28, "ii", (f16(sx), f16(sy)))

    def cmd_screensaver(self):
        self.cmd0(0x2f)

    def cmd_scrollbar(self, *args):
        self.cmd(0x11, "hhhhHHHH", args)

    def cmd_setfont(self, *args):
        self.cmd(0x2b, "II", args)

    def cmd_setmatrix(self):
        self.cmd0(0x2a)

    def cmd_sketch(self, *args):
        self.cmd(0x30, "hhHHII", args)

    def cmd_slider(self, *args):
        self.cmd(0x10, "hhhhHHI", args)

    def cmd_snapshot2(self, *args):
        self.cmd(0x37, "IIhhhh", args)

    def cmd_snapshot(self, *args):
        self.cmd(0x1f, "I", args)

    def cmd_spinner(self, *args):
        self.cmd(0x16, "hhHH", args)

    def cmd_stop(self):
        self.cmd0(0x17)

    def cmd_swap(self):
        self.cmd0(0x01)

    def cmd_text(self, *args):
        args_int = tuple(int(el) for el in args[0:4])
        self.cmd(0x0c, "hhhH", args_int[0:4])
        self.fstring(args[4:])

    def cmd_toggle(self, *args):
        self.cmd(0x12, "hhhhHH", args[0:6])
        label = (args[6].encode() + b'\xff' + args[7].encode())
        self.fstring((label,) + args[8:])

    def cmd_touch_transform(self, *args):
        self.cmd(0x20, "iiiiiiiiiiiiI", args)

    def cmd_track(self, *args):
        self.cmd(0x2c, "hhhhi", args)

    def cmd_translate(self, tx, ty):
        self.cmd(0x27, "ii", (f16(tx), f16(ty)))

    #
    # The new 810 commands
    #

    def cmd_romfont(self, *args):
        self.SaveContext()
        self.cmd(0x3f, "II", args)
        self.RestoreContext()

    def cmd_mediafifo(self, *args):
        self.cmd(0x39, "II", args)

    def cmd_sync(self):
        self.cmd0(0x42)

    def cmd_setrotate(self, *args):
        self.cmd(0x36, "I", args)

    def cmd_setbitmap(self, *args):
        self.cmd(0x43, "IHhi", args)

    def cmd_setfont2(self, *args):
        self.cmd(0x3b, "III", args)

    def cmd_videoframe(self, *args):
        self.cmd(0x41, "II", args)

    def cmd_videostart(self):
        self.cmd(0x40, "", ())

    def cmd_videostartf(self):
        self.cmd(0x5f, "", ())

    def cmd_playvideo(self, *args):
        self.cmd(0x3a, "I", args)

    def cmd_setscratch(self, *args):
        self.cmd(0x3c, "I", args)

    #
    # 815 commands
    #

    def cmd_setbase(self, *args):
        self.cmd(0x38, "I", args)

    def cmd_rotatearound(self, x, y, a, s = 1):
        self.cmd(0x51, "iiii", (x, y, furmans(a), f16(s)))

    def cmd_flasherase(self):
        self.cmd0(0x44)

    def cmd_flashwrite(self, a, b):
        self.cmd(0x45, "II", (a, len(b)))
        self.cc(b)

    def cmd_flashupdate(self, *args):
        self.cmd(0x47, "III", args)

    def cmd_flashread(self, *args):
        self.cmd(0x46, "III", args)

    def cmd_flashdetach(self):
        self.cmd0(0x48)

    def cmd_flashattach(self):
        self.cmd0(0x49)

    def cmd_flashfast(self):
        self.cmd(0x4a, "I", (0xdeadbeef,))

    def cmd_flashspidesel(self):
        self.cmd0(0x4b)

    def cmd_flashspitx(self, b):
        self.cmd(0x4c, "I", (len(b),))
        self.cc(align4(b))

    def cmd_flashspirx(self, ptr, num):
        self.cmd(0x4d, "II", (ptr, num))

    def cmd_flashsource(self, *args):
        self.cmd(0x4e, "I", args)

    def cmd_inflate2(self, *args):
        self.cmd(0x50, "II", args)

    def cmd_fillwidth(self, *args):
        self.cmd(0x58, "I", args)

    def cmd_appendf(self, *args):
        self.cmd(0x59, "II", args)

    def cmd_animframe(self, *args):
        self.cmd(0x5a, "hhII", args)

    def cmd_nop(self):
        self.cmd0(0x5b)

    # Some higher-level functions

    def get_inputs(self):
        self.finish()
        t = _Touch(*struct.unpack("HHIhhhhB", self.rd(REG_TOUCH_RAW_XY, 17)))

        r = _Tracker(*struct.unpack("HH", self.rd(REG_TRACKER, 4)))

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

        pclk = self.rd32(REG_PCLK)
        self.wr32(REG_PCLK, 0)
        time.sleep(0.001)
        self.wr32(REG_SCREENSHOT_EN, 1)
        self.wr32(0x0030201c, 32)
        
        for ly in range(self.h):
            print(ly, "/", self.h)
            self.wr32(REG_SCREENSHOT_Y, ly)
            self.wr32(REG_SCREENSHOT_START, 1)
            time.sleep(.002)
            # while (self.raw_read(REG_SCREENSHOT_BUSY) | self.raw_read(REG_SCREENSHOT_BUSY + 4)): pass
            while self.rd(REG_SCREENSHOT_BUSY, 8) != bytes(8):
                pass
            self.wr32(REG_SCREENSHOT_READ, 1)
            bgra = self.rd(RAM_SCREENSHOT, 4 * self.w)
            (b,g,r,a) = [bgra[i::4] for i in range(4)]
            line = bytes(sum(zip(r,g,b), ()))
            dest(line)
            self.wr32(REG_SCREENSHOT_READ, 0)
        self.wr32(REG_SCREENSHOT_EN, 0)
        self.wr32(REG_PCLK, pclk)

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

class MoviePlayer:
    def __init__(self, gd, f, mf_base = 0xf0000, mf_size = 0x8000):
        self.gd = gd
        self.f = f
        self.mf_base = mf_base
        self.mf_size = mf_size

        gd.cmd_mediafifo(mf_base, mf_size)
        self.wp = 0
        gd.cmd_regwrite(gd.REG_MEDIAFIFO_WRITE, 0)

    def play(self):
        gd = self.gd
        gd.cmd_playvideo(gd.OPT_MEDIAFIFO | gd.OPT_FULLSCREEN | gd.OPT_NOTEAR)
        gd.cmd_nop()
        gd.flush()
        while not gd.is_idle():
            self.service()
        gd.finish()
        
    def service(self):
        gd = self.gd

        rp = gd.rd32(gd.REG_MEDIAFIFO_READ)
        fullness = (self.wp - rp) % self.mf_size
        SZ = 2048
        # print("rp=%x wp=%x" % (rp, self.wp))
        while fullness < (self.mf_size - SZ):
            s = self.f.read(SZ)
            if not s:
                return
            # print("Writing %x to %x" % (len(s), self.mf_base + self.wp))
            gd.wr(self.mf_base + self.wp, s)
            self.wp = (self.wp + len(s)) % self.mf_size
            gd.wr32(gd.REG_MEDIAFIFO_WRITE, self.wp)
            fullness += len(s)

    # display list commands
    def VERTEX2F(self, x,y)                                : return ((1<<30)|(((x)&32767)<<15)|(((y)&32767)<<0))
    def VERTEX2II(self, x,y,handle,cell)                   : return ((2<<30)|(((x)&511)<<21)|(((y)&511)<<12)|(((handle)&31)<<7)|(((cell)&127)<<0))
    def BITMAP_SOURCE(self, addr)                          : return ((1<<24)|(((addr)&1048575)<<0))
    def CLEAR_COLOR_RGB(self, red,green,blue)              : return ((2<<24)|(((red)&255)<<16)|(((green)&255)<<8)|(((blue)&255)<<0))
    def TAG(self, s)                                       : return ((3<<24)|(((s)&255)<<0))
    def COLOR_RGB(self, red,green,blue)                    : return ((4<<24)|(((red)&255)<<16)|(((green)&255)<<8)|(((blue)&255)<<0))
    def BITMAP_HANDLE(self, handle)                        : return ((5<<24)|(((handle)&31)<<0))
    def CELL(self, cell)                                   : return ((6<<24)|(((cell)&127)<<0))
    def BITMAP_LAYOUT(self, format,linestride,height)      : return ((7<<24)|(((format)&31)<<19)|(((linestride)&1023)<<9)|(((height)&511)<<0))
    def BITMAP_SIZE(self, filter,wrapx,wrapy,width,height) : return ((8<<24)|(((filter)&1)<<20)|(((wrapx)&1)<<19)|(((wrapy)&1)<<18)|(((width)&511)<<9)|(((height)&511)<<0))
    def ALPHA_FUNC(self, func,ref)                         : return ((9<<24)|(((func)&7)<<8)|(((ref)&255)<<0))
    def STENCIL_FUNC(self, func,ref,mask)                  : return ((10<<24)|(((func)&7)<<16)|(((ref)&255)<<8)|(((mask)&255)<<0))
    def BLEND_FUNC(self, src,dst)                          : return ((11<<24)|(((src)&7)<<3)|(((dst)&7)<<0))
    def STENCIL_OP(self, sfail,spass)                      : return ((12<<24)|(((sfail)&7)<<3)|(((spass)&7)<<0))
    def POINT_SIZE(self, size)                             : return ((13<<24)|(((size)&8191)<<0))
    def LINE_WIDTH(self, width)                            : return ((14<<24)|(((width)&4095)<<0))
    def CLEAR_COLOR_A(self, alpha)                         : return ((15<<24)|(((alpha)&255)<<0))
    def COLOR_A(self, alpha)                               : return ((16<<24)|(((alpha)&255)<<0))
    def CLEAR_STENCIL(self, s)                             : return ((17<<24)|(((s)&255)<<0))
    def CLEAR_TAG(self, s)                                 : return ((18<<24)|(((s)&255)<<0))
    def STENCIL_MASK(self, mask)                           : return ((19<<24)|(((mask)&255)<<0))
    def TAG_MASK(self, mask)                               : return ((20<<24)|(((mask)&1)<<0))
    def BITMAP_TRANSFORM_A(self, a)                        : return ((21<<24)|(((a)&131071)<<0))
    def BITMAP_TRANSFORM_B(self, b)                        : return ((22<<24)|(((b)&131071)<<0))
    def BITMAP_TRANSFORM_C(self, c)                        : return ((23<<24)|(((c)&16777215)<<0))
    def BITMAP_TRANSFORM_D(self, d)                        : return ((24<<24)|(((d)&131071)<<0))
    def BITMAP_TRANSFORM_E(self, e)                        : return ((25<<24)|(((e)&131071)<<0))
    def BITMAP_TRANSFORM_F(self, f)                        : return ((26<<24)|(((f)&16777215)<<0))
    def SCISSOR_XY(self, x,y)                              : return ((27<<24)|(((x)&511)<<9)|(((y)&511)<<0))
    def SCISSOR_SIZE(self, width,height)                   : return ((28<<24)|(((width)&1023)<<10)|(((height)&1023)<<0))
    def CALL(self, dest)                                   : return ((29<<24)|(((dest)&65535)<<0))
    def JUMP(self, dest)                                   : return ((30<<24)|(((dest)&65535)<<0))
    def BEGIN(self, prim)                                  : return ((31<<24)|(((prim)&15)<<0))
    def COLOR_MASK(self, r,g,b,a)                          : return ((32<<24)|(((r)&1)<<3)|(((g)&1)<<2)|(((b)&1)<<1)|(((a)&1)<<0))
    def CLEAR(self, c,s,t)                                 : return ((38<<24)|(((c)&1)<<2)|(((s)&1)<<1)|(((t)&1)<<0))
    def END(self, )                                        : return ((33<<24))
    def SAVE_CONTEXT(self, )                               : return ((34<<24))
    def RESTORE_CONTEXT(self, )                            : return ((35<<24))
    def RETURN(self, )                                     : return ((36<<24))
    def MACRO(self, m)                                     : return ((37<<24)|(((m)&1)<<0))
    def DISPLAY(self, )                                    : return ((0<<24))