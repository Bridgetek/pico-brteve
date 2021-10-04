""" Common class for BridgeTek's EVE chips"""
import sys
import array

if sys.implementation.name == "circuitpython":
    from _eve import _EVE   # pylint: disable=import-error
else:
    from ._eve_ import _EVE

def const(num):
    """Constant wrapper
    :param num: Input number
    :return: Constant number
    """
    return num

_B0 = b"\x00"

def align4( num):
    """ Align number to multiple of 4
    :param num: input bytes object
    :return: the bytes object extended so that its length is a multiple of 4
    """
    return num + _B0 * (-len(num) & 3)

def f16(value):
    """ Multiply with maximum value of a number 16bit (0xFFFF)
    :param value: Input value
    :return: value * 0xFFFF
    """
    return int(round(65536 * value))

def furmans(deg):
    """ Given an angle in degrees, return it in Furmans
    :param deg: Input degree
    :return: value * 0xFFFF
    """
    return 0xFFFF & f16(deg / 360.0)

def args_to_integer(func):
    """ Delegate function, to convert every number of a function to type int
    :param func: Call-back method
    :return: value from callback method
    """
    def wrapper(*args):
        args_int = []
        args_int.append(args[0])  # self param

        # command params
        for arg in args[1::]:
            if isinstance(arg, float):
                args_int.append(int(arg))
            else:
                args_int.append(arg)

        # call the command
        result = func(*args_int)
        return result
    return wrapper

class BrtEveCommon(_EVE): # pylint: disable=too-many-public-methods
    """EVE common definition class"""

    # system clock
    EVE_SYSCLK_DEFAULT     = const(0x00)  # default 60mhz on ft810, 48mhz on ft800

    EVE_SYSCLK_72M         = const(0x06)  # EVE > FT810
    EVE_SYSCLK_60M         = const(0x05)  # EVE > FT810

    EVE_SYSCLK_48M         = const(0x04)
    EVE_SYSCLK_36M         = const(0x03)
    EVE_SYSCLK_24M         = const(0x02)

    EVE_PLL_48M            = const(0x62)
    EVE_PLL_36M            = const(0x61)
    EVE_PLL_24M            = const(0x64)

    # Chip identifier macros
    CHIP_ID_FT810          = const(0x10)
    CHIP_ID_FT811          = const(0x11)
    CHIP_ID_FT812          = const(0x12)
    CHIP_ID_FT813          = const(0x13)
    CHIP_ID_BT815          = const(0x15)
    CHIP_ID_BT816          = const(0x16)
    CHIP_ID_BT817          = const(0x17)
    CHIP_ID_BT818          = const(0x18)

    # coprocessor error macros
    COPRO_ERROR            = const(0xFFF)

    # Bitmap Swizzle
    RED                    = const(2)
    GREEN                  = const(3)
    BLUE                   = const(4)
    ALPHA                  = const(5)

    # Macros used for graphics commands
    NEVER                  = const(0)
    LESS                   = const(1)
    LEQUAL                 = const(2)
    GREATER                = const(3)
    GEQUAL                 = const(4)
    EQUAL                  = const(5)
    NOTEQUAL               = const(6)
    ALWAYS                 = const(7)

    # Bitmap format macros
    ARGB1555               = const(0)
    L1                     = const(1)
    L4                     = const(2)
    L8                     = const(3)
    RGB332                 = const(4)
    ARGB2                  = const(5)
    ARGB4                  = const(6)
    RGB565                 = const(7)
    PALETTED               = const(8)
    TEXT8X8                = const(9)
    TEXTVGA                = const(10)
    BARGRAPH               = const(11)

    # Bitmap filter type macros
    NEAREST                = const(0)
    BILINEAR               = const(1)

    # Bitmap wrap type macros
    BORDER                 = const(0)
    REPEAT                 = const(1)

    # Stencil macros
    KEEP                   = const(1)
    REPLACE                = const(2)
    INCR                   = const(3)
    DECR                   = const(4)
    INVERT                 = const(5)

    # Graphics display list swap macros
    DLSWAP_DONE            = const(0)
    DLSWAP_LINE            = const(1)
    DLSWAP_FRAME           = const(2)

    # Interrupt bits
    INT_SWAP               = const(0x01)
    INT_TOUCH              = const(0x02)
    INT_TAG                = const(0x04)
    INT_SOUND              = const(0x08)
    INT_PLAYBACK           = const(0x10)
    INT_CMDEMPTY           = const(0x20)
    INT_CMDFLAG            = const(0x40)
    INT_CONVCOMPLETE       = const(0x80)

    # Touch mode macros
    TMODE_OFF              = const(0)
    TMODE_ONESHOT          = const(1)
    TMODE_FRAME            = const(2)
    TMODE_CONTINUOUS       = const(3)

    # Alpha blending macros
    ZERO                   = const(0)
    ONE                    = const(1)
    SRC_ALPHA              = const(2)
    DST_ALPHA              = const(3)
    ONE_MINUS_SRC_ALPHA    = const(4)
    ONE_MINUS_DST_ALPHA    = const(5)

    # Graphics primitives macros
    BITMAPS                = const(1)
    POINTS                 = const(2)
    LINES                  = const(3)
    LINE_STRIP             = const(4)
    EDGE_STRIP_R           = const(5)
    EDGE_STRIP_L           = const(6)
    EDGE_STRIP_A           = const(7)
    EDGE_STRIP_B           = const(8)
    RECTS                  = const(9)

    # Widget command macros
    OPT_MONO               = const(1)
    OPT_NODL               = const(2)
    OPT_FLAT               = const(256)
    OPT_CENTERX            = const(512)
    OPT_CENTERY            = const(1024)
    OPT_CENTER             = const(OPT_CENTERX | OPT_CENTERY)
    OPT_NOBACK             = const(4096)
    OPT_NOTICKS            = const(8192)
    OPT_NOHM               = const(16384)
    OPT_NOPOINTER          = const(16384)
    OPT_NOSECS             = const(32768)
    OPT_NOHANDS            = const(49152)
    OPT_RIGHTX             = const(2048)
    OPT_SIGNED             = const(256)

    # Audio sample type macros
    LINEAR_SAMPLES         = const(0)  # 8bit signed samples
    ULAW_SAMPLES           = const(1)  # 8bit ulaw samples
    ADPCM_SAMPLES          = const(2)  # 4bit ima adpcm samples

    # The built-in audio samples
    HARP                   = const(0x40)
    XYLOPHONE              = const(0x41)
    TUBA                   = const(0x42)
    GLOCKENSPIEL           = const(0x43)
    ORGAN                  = const(0x44)
    TRUMPET                = const(0x45)
    PIANO                  = const(0x46)
    CHIMES                 = const(0x47)
    MUSICBOX               = const(0x48)
    BELL                   = const(0x49)
    CLICK                  = const(0x50)
    SWITCH                 = const(0x51)
    COWBELL                = const(0x52)
    NOTCH                  = const(0x53)
    HIHAT                  = const(0x54)
    KICKDRUM               = const(0x55)
    POP                    = const(0x56)
    CLACK                  = const(0x57)
    CHACK                  = const(0x58)
    MUTE                   = const(0x60)
    UNMUTE                 = const(0x61)

    # Host command macros
    ACTIVE                 = const(0x00)  # Place EVE in active state
    STANDBY                = const(0x41)  # Place EVE in Standby (clk running)
    SLEEP                  = const(0x42)  # Place EVE in Sleep (clk off)
    PWRDOWN                = const(0x50)  # Place EVE in Power Down (core off)
    CLKEXT                 = const(0x44)  # Select external clock source
    CLKINT                 = const(0x48)  # Select internal clock source
    CLK48M                 = const(0x62)  # Select 48MHz PLL output
    CLK36M                 = const(0x61)  # Select 36MHz PLL output
    CORERST                = const(0x68)  # Reset core - all registers default and processors reset

    def cstring(self, s_value):
        """ Send a string TODO XXX, the string will be alligned to 4
        :param s_value: Input value
        :return: none
        """
        if isinstance(s_value, str):
            s_value = bytes(s_value, "utf-8")
        self.cc(align4(s_value + _B0))

    def fstring(self, s_value):
        """ Send a string and its format params TODO XXX, the string will be alligned to 4
        :param s_value: Input value
        :return: none
        """
        self.cstring(s_value[0])
        self.cc(bytes(array.array("i", s_value[1:])))

    @args_to_integer
    def cmd_append(self, ptr, num):
        """ appends more commands resident in RAM_G to the current display list memory
            address where the offset is specified in REG_CMD_DL
        :param num: Number of bytes to copy. This must be a multiple of 4
        :param ptr: Start of source commands in flash memory. Must be 64-byte aligned
        :return: none
        """
        self.cmd(0x1E, "II", (ptr, num))

    @args_to_integer
    def cmd_bgcolor(self, c): # pylint: disable=invalid-name
        """ Send set the background color
        :param c: New background color, as a 24-bit RGB number.
                  Red is the most significant 8 bits,
                  blue is the least. So 0xff0000 is bright red.

                  Background color is applicable for things that the user cannot move E.g. behind
                  gauges and sliders etc.
        :return: none
        """
        self.cmd(0x09, "I", (c,))

    @args_to_integer
    def cmd_button(self, x, y, w, h, font, options, *text_and_format): # pylint: disable=invalid-name,too-many-arguments
        """ draw a button with a UTF-8 label
        :param x: X-coordinate of button top-left, in pixels
        :param y: Y-coordinate of button top-left, in pixels
        :param w: width of button, in pixels
        :param h: height of button, in pixels
        :param font: bitmap handle to specify the font used in the button label.
        :param options: By default, the button is drawn with a 3D effect and the value is zero.
                        OPT_FLAT removes the 3D effect.  The value of OPT_FLAT is 256.
        :param s: Button label. It must be one string terminated with null character, i.e. '\0' in C
                  language. UTF-8 encoded. If OPT_FILL is not given then the string may contain
                  newline (\n) characters, indicating line breaks. See 5.7 String Formatting.
        :return: none
        """
        self.cmd(0x0D, "hhhhhH", (x, y, w, h, font, options))
        self.fstring(text_and_format)

    @args_to_integer
    def cmd_calibrate(self):
        """ execute the touch screen calibration routine
        :param result: output parameter; written with 0 on failure of calibration.
        :return: none
        """
        self.cmd0(0x15)

    @args_to_integer
    def cmd_clock(self, x, y, r, options, uh, um, us, ms): # pylint: disable=invalid-name,too-many-arguments
        """ draw a analog clock
        :param x: x-coordinate of clock center, in pixels
        :param y: y-coordinate of clock center, in pixels
        :param options:By default the clock dial is drawn with a 3D effect,
                       and the name of this option is OPT_3D.
                       Option OPT_FLAT removes the 3D effect.
                       With option OPT_NOBACK, the background is not drawn.
                       With option OPT_NOTICKS, the twelve hour ticks are not drawn.
                       With option OPT_NOSECS, the seconds hand is not drawn.
                       With option OPT_NOHANDS, no hands are drawn.
                       With option OPT_NOHM, no hour and minutes hands are drawn.
        :param h: hours
        :param m: minutes
        :param s: seconds
        :param ms: milliseconds
        :return: none
        """
        self.cmd(0x14, "hhhHHHHH", (x, y, r, options, uh, um, us, ms))

    @args_to_integer
    def cmd_coldstart(self):
        """ sets the coprocessor engine to default reset states
        :return: none
        """
        self.cmd0(0x32)

    @args_to_integer
    def cmd_dial(self, x, y, r, options, val): # pylint: disable=invalid-name,too-many-arguments
        """ draw a rotary dial control
        :param x: x-coordinate of dial center, in pixels
        :param y: y-coordinate of dial center, in pixels
        :param r: radius of dial, in pixels.
        :param Options: By default the dial is drawn with a 3D effect,
                     and the value of options is zero.
                     Options OPT_FLAT remove the 3D effect and its value is 256
        :param val: Specify the position of dial points by setting value between 0 and 65535
                   inclusive. 0 means that the dial points straight down, 0x4000 left, 0x8000 up,
                   and0xc000 right.
        :return: none
        """
        self.cmd(0x2D, "hhhHI", (x, y, r, options, val))

    @args_to_integer
    def cmd_dlstart(self):
        """ starts a new display list
        :return: none
        """
        self.cmd0(0x00)

    @args_to_integer
    def cmd_fgcolor(self, c):  # pylint: disable=invalid-name
        """ set the foreground color
        :param c: New foreground color, as a 24-bit RGB number.
                  Red is the most significant 8 bits,
                  blue is the least. So 0xff0000 is bright red.
                  Foreground color is applicable for things that the
                  user can move such as handles and buttons ("affordances").
        :return: none
        """
        self.cmd(0x0A, "I", (c,))

    @args_to_integer
    def cmd_gauge(self, x, y, r, options, major, minor, val, range_):  # pylint: disable=invalid-name,too-many-arguments
        """ draw a Gauge
        :param x: X-coordinate of gauge center, in pixels
        :param y: Y-coordinate of gauge center, in pixels
        :param r: Radius of the gauge, in pixels
        :param options: By default the gauge dial is drawn with a 3D effect,
                        and the value of options is zero.
                        OPT_FLAT removes the 3D effect.
                        With option OPT_NOBACK, the background is not drawn.
                        With option OPT_NOTICKS, the tick marks are not drawn.
                        With option OPT_NOPOINTER, the pointer is not drawn.
        :param major: Number of major subdivisions on the dial, 1-10
        :param minor: Number of minor subdivisions on the dial, 1-10
        :param val: Gauge indicated value, between 0 and range, inclusive
        :param range: Maximum value
        :return: none
        """
        self.cmd(0x13, "hhhHHHHH", (x, y, r, options, major, minor, val, range_))

    @args_to_integer
    def cmd_getmatrix(self):
        """ retrieves the current matrix within the context of the coprocessor engine
        :param ptr a: output parameter; written with matrix coefficient a.
                      See the parameter of the command BITMAP_TRANSFORM_A for formatting.
        :param ptr b: output parameter; written with matrix coefficient b.
                      See the parameter b of the command BITMAP_TRANSFORM_B for formatting.
        :param ptr c: output parameter; written with matrix coefficient c.
                      See the parameter c of the command BITMAP_TRANSFORM_C for formatt
        :param ptr d: output parameter; written with matrix coefficient d.
                      See the parameter d of the command BITMAP_TRANSFORM_D for formatt
        :param ptr e: output parameter; written with matrix coefficient e.
                      See the parameter e of the command BITMAP_TRANSFORM_E for formatt
        :param ptr f: output parameter; written with matrix coefficient f.
                      See the parameter f of the command BITMAP_TRANSFORM_F for formatting.
        :return: none
        """
        a = b = c = d = e = f = 0 # pylint: disable=invalid-name
        self.cmd(0x33, "iiiiii", (a, b, c, d, e, f))

    @args_to_integer
    def cmd_getprops(self):
        """ returns the source address and size of the bitmap loaded by the previous CMD_LOADIMAGE
        :param ptr: The address of the image in RAM_G which was decoded by
                    the last CMD_LOADIMAGE before this command.
                    It is an output parameter.
        :param width: The width of the image which was decoded by the last
                      CMD_LOADIMAGE before this command.
                      It is an output parameter.
        :param height:The height of the image which was decoded by the last
                      CMD_LOADIMAGE before this command.
                      It is an output parameter.
        :return: none
        """
        ptr = width = height = 0
        self.cmd(0x25, "III", (ptr, width, height))

    @args_to_integer
    def cmd_getptr(self):
        """ returns the first unallocated memory location
        :param result: The end address of decompressed data done by CMD_INFLATE.
        :return: none
        """
        result = 0
        self.cmd(0x23, "I", (result,))

    @args_to_integer
    def cmd_gradcolor(self, c):  # pylint: disable=invalid-name
        """ set the 3D Button Highlight Color
        :param c: New highlight gradient color, as a 24-bit RGB number.
                  Red is the most significant 8bits, blue is the least.
                  So 0xff0000 is bright red.
                  Gradient is supported only for Button and Keys widgets.
        :return: none
        """
        self.cmd(0x34, "I", (c,))

    @args_to_integer
    def cmd_gradient(self, x0, y0, rgb0, x1, y1, rgb1):  # pylint: disable=invalid-name,too-many-arguments
        """ draw a smooth color gradient
        :param x0: x-coordinate of point 0, in pixels
        :param y0: y-coordinate of point 0, in pixels
        :param rgb0: Color of point 0, as a 24-bit RGB number.
                     R is the most significant8 bits, B is the least.
                     So 0xff0000 is bright red.
        :param x1: x-coordinate of point 1, in pixels
        :param y1: y-coordinate of point 1, in pixels
        :param rgb1: Color of point 1
        :return: none
        """
        self.cmd(0x0B, "hhIhhI", (x0, y0, rgb0, x1, y1, rgb1))

    @args_to_integer
    def cmd_inflate(self, ptr):  # pylint: disable=invalid-name
        """ decompress the following compressed data into RAM_G
        :param ptr: Destination address in RAM_G. The data byte should immediately follow in the
                    command buffer.
        :return: none
        """
        self.cmd(0x22, "I", (ptr,))

    @args_to_integer
    def cmd_interrupt(self, ms):  # pylint: disable=invalid-name
        """ trigger Interrupt CMDFLAG
        :param ms: The delay before the interrupt triggers, in milliseconds.
                   The interrupt is guaranteed not to fire before this delay.
                   If ms are zero, the interrupt fires immediately.
        :return: none
        """
        self.cmd(0x02, "I", (ms,))

    @args_to_integer
    def cmd_keys(self, x, y, w, h, font, options, *s):  # pylint: disable=invalid-name,too-many-arguments
        """ draw a row of keys
        :param x: x-coordinate of keys top-left, in pixels
        :param y: y-coordinate of keys top-left, in pixels
        :param font: Bitmap handle to specify the font used in key label.
                     The valid range is from 0 to 31 options
                     By default the keys are drawn with a 3D effect,
                     and the value of option is zero.

                     OPT_FLAT removes the 3D effect. If OPT_CENTER is given,
                     the keys are drawn at minimum size centered within the w x h rectangle.
                     Otherwise the keys are expanded so that they completely fill the available
                     space. If an ASCII code is specified, that key is drawn 'pressed'
                     - i.e. in background color with any 3D effect removed.
        :param w: The width of the keys
        :param h: The height of the keys
        :param s: key labels, one character per key. The TAG value is set to the ASCII
                  value of each key, so that key presses can be detected using the REG_TOUCH_TAG
                  register.
        :return: none
        """
        self.cmd(0x0E, "hhhhhH", (x, y, w, h, font, options))
        self.cstring(s)

    @args_to_integer
    def cmd_loadimage(self, ptr, options):  # pylint: disable=invalid-name
        """ load a JPEG or PNG image
        :param ptr: Destination address
        :param Option: OPT_MONO forces the bitmap to be monochrome in L8 format. Otherwise, the
                       image is loaded depends on the type of input image:
                       For JPEG images, the bitmap is loaded as either a RGB565 or L8 format
                       bitmap, depending on the original image. If OPT_MONO is given, L8 is
                       used.
                       For PNG images, the PNG standard defines several image color formats
        :return: none
        """
        self.cmd(0x24, "iI", (ptr, options))

    @args_to_integer
    def cmd_logo(self):
        """ causes the coprocessor engine to play back a short animation of the Bridgetek logo
        :return: none
        """
        self.cmd0(0x31)

    @args_to_integer
    def cmd_memcpy(self, dest, src, num):
        """ copy a block of memory
        :param dest: address of the destination memory block
        :param src: address of the source memory block
        :param num: number of bytes to copy
        :return: none
        """
        self.cmd(0x1D, "III", (dest, src, num))

    @args_to_integer
    def cmd_memcrc(self, ptr, num):
        """ computes a CRC-32 for a block of memory
        :param ptr: Starting address of the memory block
        :param num: Number of bytes in the source memory block
        :param result: Output parameter; written with the CRC-32 after command execution.
                       The completion of this function is detected when the value of
                       REG_CMD_READ is equal to REG_CMD_WRITE.
        :return: none
        """
        result = 0
        self.cmd(0x18, "III", (ptr, num, result))

    @args_to_integer
    def cmd_memset(self, ptr, value, num):  # pylint: disable=invalid-name
        """ fill memory witth a byte value
        :param ptr: Starting address of the memory block
        :param value: Value to be written to memory
        :param num: Number of bytes in the memory block
        :return: none
        """
        self.cmd(0x1B, "III", (ptr, value, num))

    @args_to_integer
    def cmd_memwrite(self, ptr, num):  # pylint: disable=invalid-name
        """ writes the following bytes into the memory
        :param ptr:          The memory address to be written
        :param num: Number of bytes to be written.
        :return: none
        """
        self.cmd(0x1A, "II", (ptr, num))

    @args_to_integer
    def cmd_memzero(self, ptr, num):  # pylint: disable=invalid-name
        """ write zero to a block of memory
        :param ptr: Starting address of the memory block
        :param num: Number of bytes in the memory block
        :return: none
        """
        self.cmd(0x1C, "II", (ptr, num))

    @args_to_integer
    def cmd_number(self, x, y, font, options, n):  # pylint: disable=invalid-name,too-many-arguments
        """ draw a number
        :param x: x-coordinate of text base, in pixels
        :param y: y-coordinate of text base, in pixels
        :param font: font to use for text, 0-31. See ROM and RAM Fonts
        :param options: By default (x,y) is the top-left pixel of the text.
                        OPT_CENTERX centers the text horizontally,
                        OPT_CENTERY centers it vertically.
                        OPT_CENTER centers the text in both directions.
                        OPT_RIGHTX right-justifies the text, so that the x is the rightmost pixel.

        :param n: The number to display, is either unsigned or signed 32-bit,
                  in the base specified in the preceding CMD_SETBASE.
                  If no CMD_SETBASE appears before CMD_NUMBER, it will be in decimal base.
        :return: none
        """
        self.cmd(0x2E, "hhhHi", (x, y, font, options, n))

    @args_to_integer
    def cmd_progress(self, x, y, w, h, options, val, urange):  # pylint: disable=invalid-name,too-many-arguments
        """ draw a progress bar
        :param x: x-coordinate of progress bar top-left, in pixels
        :param y: y-coordinate of progress bar top-left, in pixels
        :param w: width of progress bar, in pixels
        :param h: height of progress bar, in pixels
        :param options: By default the progress bar is drawn with a 3D effect,
                        and the value of options is zero.
                        Options OPT_FLAT remove the 3D effect and its value is 256
        :param val: Displayed value of progress bar, between 0 and range inclusive
        :param range: Maximum value
        :return: none
        """
        self.cmd(0x0F, "hhhhHHI", (x, y, w, h, options, val, urange))

    @args_to_integer
    def cmd_regread(self, ptr):
        """ read a register value
        :param ptr: Address of the register to be read
        :param result: The register value to be read at ptr address.
        :return: none
        """
        result = 0
        self.cmd(0x19, "II", (ptr, result))

    @args_to_integer
    def cmd_rotate(self, a):  # pylint: disable=invalid-name
        """ rotation to the current matrix
        :param a: Clockwise rotation angle, in units of 1/65536 of a circle
        :return: none
        """
        self.cmd(0x29, "i", (a,))

    @args_to_integer
    def cmd_scale(self, sx, sy):  # pylint: disable=invalid-name
        """ apply a scale to the current matrix
        :param sx: x scale factor, in signed 16. 16 bit fixed-point form.
        :param sy: y scale factor, in signed 16. 16 bit fixed-point form.
        :return: none
        """
        self.cmd(0x28, "ii", (sx, sy))

    @args_to_integer
    def cmd_screensaver(self):
        """ Send cmd_screensaver TODO XXX
        :return: none
        """
        self.cmd0(0x2F)

    @args_to_integer
    def cmd_scrollbar(self, x, y, w, h, options, uval, usize, urange):  # pylint: disable=invalid-name,too-many-arguments
        """ start an animated screensaver
        :param x: x-coordinate of scroll bar top-left, in pixels
        :param y: y-coordinate of scroll bar top-left, in pixels
        :param w: Width of scroll bar, in pixels. If width is greater than height,
                  the scroll bar is drawn horizontally
        :param h: Height of scroll bar, in pixels. If height is greater than width,
                  the scroll bar is drawn vertically
        :param options: By default the scroll bar is drawn with a 3D effect,
                        and the value of options is zero.
                        Options OPT_FLAT remove the 3D effect and its value is 256
        :param val: Displayed value of scroll bar, between 0 and range inclusive
        :param range: Maximum value
        :return: none
        """
        self.cmd(0x11, "hhhhHHHH", (x, y, w, h, options, uval, usize, urange))

    @args_to_integer
    def cmd_setfont(self, font, ptr):  # pylint: disable=invalid-name
        """ register one custom defined bitmap font into the coprocessor engine
        :param font: The bitmap handle from 0 to 31
        :param ptr: The metrics block address in RAM_G. 4 bytes aligned is required.
        :return: none
        """
        self.cmd(0x2B, "II", (font, ptr))

    @args_to_integer
    def cmd_setmatrix(self):
        """ assigns the value of the current matrix to the bitmap transform matrix
        :return: none
        """
        self.cmd0(0x2A)

    @args_to_integer
    def cmd_sketch(self, x, y, uw, uh, ptr, uformat):  # pylint: disable=invalid-name,too-many-arguments
        """ start a continuous sketch update
        :param x: x-coordinate of sketch area top-left, in pixels
        :param y: y-coordinate of sketch area top-left, in pixels
        :param w: Width of sketch area, in pixels
        :param h: Height of sketch area, in pixels
        :param ptr: Base address of sketch bitmap
        :param format: Format of sketch bitmap, either L1 or L8
        :return: none
        """
        self.cmd(0x30, "hhHHII", (x, y, uw, uh, ptr, uformat))

    @args_to_integer
    def cmd_slider(self, x, y, w, h, options, uval, urange):  # pylint: disable=invalid-name,too-many-arguments
        """ draw a slider
        :param x: x-coordinate of slider top-left, in pixels
        :param y: y-coordinate of slider top-left, in pixels
        :param w: width of slider, in pixels.
                  If width is greater than height, the scroll bar is drawn horizontally
        :param h: height of slider, in pixels.
                  If height is greater than width, the scroll bar is drawn vertically
        :param options: By default the slider is drawn with a 3D effect.
                        OPT_FLAT removes the 3D effect
        :param val: Displayed value of slider, between 0 and range inclusive
        :param range: Maximum value
        :return: none
        """
        self.cmd(0x10, "hhhhHHI", (x, y, w, h, options, uval, urange))

    @args_to_integer
    def cmd_snapshot(self, ptr):  # pylint: disable=invalid-name
        """ causes the coprocessor engine to take a snapshot of the current screen, and write
            the result into RAM_G as an ARGB4 bitmap
        :param ptr: Snapshot destination address, in RAM_G
        :return: none
        """
        self.cmd(0x1F, "I", (ptr,))

    @args_to_integer
    def cmd_spinner(self, x, y, ustyle, uscale):  # pylint: disable=invalid-name
        """ start an animated spinner
        :param x: The X coordinate of top left of spinner
        :param y: The Y coordinate of top left of spinner
        :param style: The style of spinner. Valid range is from 0 to 3.
        :param scale: The scaling coefficient of spinner. 0 means no scaling.
        :return: none
        """
        self.cmd(0x16, "hhHH", (x, y, ustyle, uscale))

    @args_to_integer
    def cmd_stop(self):
        """ inform the coprocessor engine to stop the periodic operation, which is
            triggered by CMD_SKETCH , CMD_SPINNER or CMD_SCREENSAVER
        :return: none
        """
        self.cmd0(0x17)

    @args_to_integer
    def cmd_swap(self):
        """ swap the current display list
        :return: none
        """
        self.cmd0(0x01)

    @args_to_integer
    def cmd_text(self, x, y, font, options, *s):  # pylint: disable=invalid-name
        """ draw a UTF-8 Text string
        :param x: x-coordinate of text base, in pixels
        :param y: y-coordinate of text base, in pixels
        :param font: Font to use for text, 0-31. See ROM and RAM Fonts
        :param options: By default (x,y) is the top-left pixel of the text,
                        and the value of options is zero.
                        OPT_CENTERX centers the text horizontally
                        OPT_CENTERY centers it vertically.
                        OPT_CENTER centers the text in both directions.
                        OPT_RIGHTX right-justifies the text, so that the x is the rightmost pixel.
                        OPT_FORMAT processes the text as a format string, see String formatting.
                        OPT_FILL breaks the text at spaces into multiple lines,
                        with maximum width set by CMD_FILLWIDTH.
        :param s: Text string, UTF-8 encoding. If OPT_FILL is not given then the string may contain
                 newline (\n) characters, indicating line breaks.
        :return: none
        """
        self.cmd(0x0C, "hhhH", (x, y, font, options))
        self.fstring(s)

    @args_to_integer
    def cmd_toggle(self, x, y, w, font, options, state, *s):  # pylint: disable=invalid-name,too-many-arguments
        """ draw a toggle switch with UTF-8 labels
        :param x: x-coordinate of top-left of toggle, in pixels
        :param y: y-coordinate of top-left of toggle, in pixels
        :param w: width of toggle, in pixels
        :param font: font to use for text, 0-31. See ROM and RAM Fonts
        :param options: By default the toggle is drawn with a 3D effect,
                        and the value of options is zero.
                        Options OPT_FLAT remove the 3D effect and its value is 256
        :param state: state of the toggle: 0 is off, 65535 is on.
        :param s: string labels for toggle,UTF-8 encoding. A character value of 255
                  (in C it can be written as \xff) separates the label strings.
        :return: none
        """
        self.cmd(0x12, "hhhhHH", (x, y, w, font, options, state))
        text = s[0].split('\xff')
        label = ''
        label = text[0].encode()
        if len(text) > 1:
            label = label + b"\xff" +text[1].encode()
        self.fstring((label,) + s[1::])

    @args_to_integer
    def cmd_track(self, x, y, w, h, tag):  # pylint: disable=invalid-name,too-many-arguments
        """ track touches for a graphics object
        :param x: For linear tracker functionality, x-coordinate of track area top-left, in pixels.
                  For rotary tracker functionality, x-coordinate of track area center, in pixels.
        :param y: For linear tracker functionality, y-coordinate of track area top-left, in pixels.
                  For rotary tracker functionality, y-coordinate of track area center, in pixels.
        :param w: Width of track area, in pixels.
        :param h: Height of track area, in pixels.
        :param tag: tag of the graphics object to be tracked, 1-255
        :return: none
        """
        self.cmd(0x2C, "hhhhi", (x, y, w, h, tag))

    @args_to_integer
    def cmd_translate(self, tx, ty):  # pylint: disable=invalid-name
        """ apply a translation to the current matrix
        :param tx: x translate factor, in signed 16.16 bit fixed-point form.
        :param ty: y translate factor, in signed 16.16 bit fixed-point form.
        :return: none
        """
        self.cmd(0x27, "ii", (tx, ty))
