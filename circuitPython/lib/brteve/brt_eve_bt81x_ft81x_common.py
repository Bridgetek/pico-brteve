""" BridgeTek BT815 and BT816's definitions """
from .brt_eve_module import BrtEveModule
from .brt_eve_common import args_to_integer

class BrtEveBT81xFT81xCommon(BrtEveModule):
    """BT815/6 and FT81X common commands"""

    def __init__(self):
        BrtEveModule.__init__(self)

    @args_to_integer
    def cmd_mediafifo(self, ptr, size):
        """ set up a streaming media FIFO. Allocate the specified area of RAM_G and set
            it up as streaming media FIFO
        :param ptr: starting address of media FIFO
        :param size: number of bytes of media FIFO
        :return: none
        """
        self.cmd(0x39, "II", (ptr, size))

    @args_to_integer
    def cmd_playvideo(self, opts):
        """ plays back MJPEG-encoded AVI video
        :param opts: The options of playing video
        :return: none
        """
        self.cmd(0x3a, "I", (opts,))

    @args_to_integer
    def cmd_romfont(self, font, romslot):
        """ load a ROM font into bitmap handle
        :param font: bitmap handle number , 0~31
        :param romslot: ROM font number, 16~34
        :return: none
        """
        self.cmd(0x3f, "II", (font, romslot))

    @args_to_integer
    def cmd_setbase(self, b):  # pylint: disable=invalid-name
        """ set the base for number output
        :param b: Numeric base, valid values are from 2 to 36:
                  2 for binary,
                  8 for octal,
                  10 for decimal,
                  16 for hexadecimal
        :return: none
        """
        self.cmd(0x38, "I", (b,))

    @args_to_integer
    def cmd_setbitmap(self, source, fmt, width, height):
        """  generate the corresponding display list commands for given bitmap informat
        :param source: Source address for bitmap, in RAM_G or flash memory as a BITMAP_
                       parameter
        :param fmt: Bitmap format, see the definition in BITMAP_EXT_FORMAT.
        :param width: bitmap width, in pixels. 2 bytes value.
        :param height: bitmap height, in pixels. 2 bytes value.
        :return: none
        """
        self.cmd(0x43, "IHhi", (source, fmt, width, height))

    @args_to_integer
    def cmd_setfont2(self, font, ptr, firstchar):
        """ set up a custom font
        :param font: The bitmap handle from 0 to 31
        :param ptr: 32 bit aligned memory address in RAM_G of font metrics block
        :param firstchar: The ASCII value of first character in the font.
                          For an extended font block, this should be zero.
        :return: none
        """
        self.cmd(0x3b, "III", (font, ptr, firstchar))

    @args_to_integer
    def cmd_setrotate(self, r):  # pylint: disable=invalid-name
        """ rotate the screen
        :param r: The value from 0 to 7. The same definition as the value in REG_ROTATE
        :return: none
        """
        self.cmd(0x36, "I", (r,))

    @args_to_integer
    def cmd_setscratch(self, handle):
        """  set the scratch bitmap for widget use
        :param handle: bitmap handle number, 0~31
        :return: none
        """
        self.cmd(0x3c, "I", (handle,))

    @args_to_integer
    def cmd_snapshot2(self, fmt, ptr, x, y, w, h):  # pylint: disable=invalid-name, too-many-arguments
        """ causes the coprocessor to take a snapshot of part of the current screen,
            and write it into graphics memory as a bitmap
        :param fmt: Output bitmap format, one of RGB565, ARGB4 or 0x20. The value 0x20
                    ARGB8 format snapshot.
        :param ptr: Snapshot destination address, in RAM_G
        :param x: x-coordinate of snapshot area top-left, in pixels
        :param y: y-coordinate of snapshot area top-left, in pixels
        :param w: width of snapshot area, in pixels. Note when fmt is 0x20, i.e. in ARG
                  the value of width shall be doubled.
        :param h: height of snapshot area, in pixels
        :return: none
        """
        self.cmd(0x37, "IIhhhh", (fmt, ptr, x, y, w, h))

    @args_to_integer
    def cmd_videoframe(self, dst, ptr):
        """ load the next frame of a video
        :param dst: Memory location to load the frame data, this will be located in RAM
        :param ptr: Completion pointer
        :return: none
        """
        self.cmd(0x41, "II", (dst, ptr))

    @args_to_integer
    def cmd_videostart(self):
        """ initialize video frame decoder
        :return: none
        """
        self.cmd0(0x40)
