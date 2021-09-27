""" BridgeTek BT817 and BT818's definitions """
from .brt_eve_bt815_6 import BrtEve as BrtEve_BT815_BT816
from .brt_eve_common import args_to_integer, const

class BrtEve(BrtEve_BT815_BT816): # pylint: disable=too-many-public-methods
    """BT817 and BT818 specific commands, options and registers"""

    OPT_DITHER = (256)

    # Registers
    REG_ADAPTIVE_FRAMERATE = const(0x30257C)
    REG_AH_HCYCLE_MAX      = const(0x302610)
    REG_ANIM_ACTIVE        = const(0x30902C)
    REG_PCLK_2X            = const(0x302618)
    REG_PCLK_FREQ          = const(0x302614)
    REG_UNDERRUN           = const(0x30260C)

    def __init__(self, host):
        BrtEve_BT815_BT816.__init__(self, host)
        self.eve_type = "bt817_8"

    # BT817/8 commands
    @args_to_integer
    def cmd_animframeram(self, x, y, aoptr, frame):  # pylint: disable=invalid-name
        """ draws the specified frame of an animation in RAM
        :param x: x screen coordinate for the animation center, in pixels.
        :param y: y screen coordinate for the animation center, in pixels.
        :param aoptr: The address of the animation object in RAM. Must be 64-byte aligned.
        :param frame: Frame number to draw, starting from zero.
        :return: none
        """
        self.cmd(0x6d, "hhII", (x, y, aoptr, frame))

    @args_to_integer
    def cmd_animstartram(self, ch, aoptr, loop):  # pylint: disable=invalid-name
        """ start an animation in RAM_G
        :param ch: Animation channel, 0-31. If no channel is available,
                   then an “out of channels” exception is raised.
        :param aoptr: Pointer to the animation object in RAM. Must be 64-byte aligned.
        :param loop: Loop ﬂags. ANIM_ONCE plays the animation once, then cancels it.
                     ANIM LOOP plays the animation in a loop. ANIM_HOLD plays the animation once,
                     then displays the ﬁnal frame.
        :return: none
        """
        self.cmd0(0x6e, "iII", (ch, aoptr, loop))

    @args_to_integer
    def cmd_apilevel(self, level):
        """ sets the API level used by the coprocessor
        :param level: API level to use. Level 1 is BT815 compatible, and is the default.
                      Level 2 is BT817/8.
        :return: none
        """
        self.cmd(0x63, "I", (level,))

    @args_to_integer
    def cmd_calibratesub(self, x, y, w, h):  # pylint: disable=invalid-name
        """ execute the touch screen calibration routine for a subwindow
        :param x: x-coordinate of top-left of subwindow, in pixels.
        :param y: y-coordinate of top-left of subwindow, in pixels.
        :param w: width of subwindow, in pixels.
        :param h: height of subwindow, in pixels.
        :param result: output parameter; written with 0 on failure.
        :return: none
        """
        result = 0
        self.cmd(0x60, "HHHHI", (x, y, w, h, result))

    @args_to_integer
    def cmd_calllist(self, a):  # pylint: disable=invalid-name
        """ calls a command list
        :param a: memory address of the command list
        :return: none
        """
        self.cmd(0x67, "I", (a,))

    @args_to_integer
    def cmd_endlist(self):
        """ terminates the compilation of a command list into RAM_G
        :return: none
        """
        self.cmd0(0x69)

    @args_to_integer
    def cmd_fontcache(self, font, ptr, num):
        """ enables the font cache
        :param font: font handle to cache. Must be an extended format font.
                     If 255, then the font cache is disabled.
        :param ptr: Start of cache area, 64-byte aligned.
        :param num: Size of cache area in bytes, 4 byte aligned. Must be at least 16 Kbytes.
        :return: none
        """
        self.cmd(0x6b, "IiI", (font, ptr, num))

    @args_to_integer
    def cmd_fontcachequery(self):
        """ queries the capacity and utilization of the font cache
        :param total: Output parameter; Total number of available bitmaps in the cache, in bytes.
        :param used: Output parameter; Number of used bitmaps in the cache, in bytes
        :return: none
        """
        total = used = 0
        self.cmd(0x6c, "Ii", (total, used))

    @args_to_integer
    def cmd_getimage(self, source, fmt, w, h, palette): # pylint: disable=invalid-name,too-many-arguments
        """ returns all the attributes of the bitmap made by the previous CMD_LOADIMAGE,
            CMD_PLAYVIDEO, CMD_VIDEOSTART or CMD_VIDEOSTARTF
        :param source: Output parameter; source address of bitmap.
        :param fmt: Output parameter; format of the bitmap
        :param w: Width of bitmap, in pixels
        :param h: Height of bitmap, in pixels
        :param palette: Palette data of the bitmap if fmt is PALETTED565 or PALETTED4444.
                        Otherwise zero.
        :return: none
        """
        self.cmd(0x64, "IIIII", (source, fmt, w, h, palette))

    @args_to_integer
    def cmd_hsf(self, hsf):
        """ Non-square LCD support
        :param w: Output pixel width, which must be less than REG_HSIZE. 0 disables HSF.
        :return: none
        """
        self.cmd(0x62, "I", (hsf,))

    @args_to_integer
    def cmd_newlist(self, a):  # pylint: disable=invalid-name
        """ starts the compilation of a command list into RAM_G
        :param a: memory address of start of command list
        :return: none
        """
        self.cmd(0x68, "I", (a,))

    @args_to_integer
    def cmd_pclkfreq(self, ftarget, rounding):
        """ sets REG_PCLK_FREQ to generate the closest possible frequency to the one equested
        :param ftarget: Target frequency, in Hz.
        :param rounding: Approxmation mode. Valid values are 0, -1, 1.
                         0 is nearest,
                         -1 is highest frequency less then or equal to target,
                         1 is lowest frequency greater than or equal to target.
        :param factual: Output parameter; Actual frequency achieved. If no frequency was found,
                        it is zero.
        :return: none
        """
        factual = 0
        self.cmd(0x6a, "IiI", (ftarget, rounding, factual))

    @args_to_integer
    def cmd_return(self):
        """ ends a command list
        :return: none
        """
        self.cmd0(0x66)

    @args_to_integer
    def cmd_testcard(self):
        """ loads a display list with a testcard graphic,
        :return: none
        """
        self.cmd0(0x61)

    @args_to_integer
    def cmd_wait(self, us):  # pylint: disable=invalid-name
        """ waits for a speciﬁed number of microseconds
        :param us: Delay time, in microseconds
        :return: none
        """
        self.cmd(0x65, "I", (us,))
