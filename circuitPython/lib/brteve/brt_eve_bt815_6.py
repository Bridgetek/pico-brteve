""" BridgeTek BT815 and BT816's definitions """
from .brt_eve_bt81x_ft81x_common import BrtEveBT81xFT81xCommon
from .brt_eve_common import align4, args_to_integer, const
from .brt_eve_storage.brt_eve_storage import BrtEveStorage

class BrtEve(BrtEveBT81xFT81xCommon): # pylint: disable=too-many-public-methods
    """BT815 and BT816 specific commands, options and registers"""

    # Bitmap formats
    TEXT8X8                = const(9)     # bits per pixel = 8
    TEXTVGA                = const(10)    # bits per pixel = 8
    BARGRAPH               = const(11)    # bits per pixel = 8
    PALETTED565            = const(14)    # bits per pixel = 8
    PALETTED4444           = const(15)    # bits per pixel = 8
    PALETTED8              = const(16)    # bits per pixel = 8
    L2                     = const(17)    # bits per pixel = 2
    ASTC_10x10             = const(37819) # bits per pixel = 1.28
    ASTC_10x5              = const(37816) # bits per pixel = 2.13
    ASTC_10x6              = const(37817) # bits per pixel = 2.00
    ASTC_10x8              = const(37818) # bits per pixel = 1.60
    ASTC_12x10             = const(37820) # bits per pixel = 1.07
    ASTC_12x12             = const(37821) # bits per pixel = 0.89
    ASTC_4x4               = const(37808) # bits per pixel = 8.00
    ASTC_5x4               = const(37809) # bits per pixel = 6.40
    ASTC_5x5               = const(37810) # bits per pixel = 5.12
    ASTC_6x5               = const(37811) # bits per pixel = 4.27
    ASTC_6x6               = const(37812) # bits per pixel = 3.56
    ASTC_8x5               = const(37813) # bits per pixel = 3.20
    ASTC_8x6               = const(37814) # bits per pixel = 2.67
    ASTC_8x8               = const(37815) # bits per pixel = 2.56

    # Parameter OPTION
    OPT_3D                 = const(0)
    OPT_RGB565             = const(0)
    OPT_MONO               = const(1)
    OPT_NODL               = const(2)
    OPT_FLAT               = const(256)
    OPT_SIGNED             = const(256)
    OPT_CENTERX            = const(512)
    OPT_CENTERY            = const(1024)
    OPT_CENTER             = const(1536)
    OPT_RIGHTX             = const(2048)
    OPT_NOBACK             = const(4096)
    OPT_FLASH              = const(64)
    OPT_FORMAT             = const(4096)
    OPT_NOTEAR             = const(4)
    OPT_FULLSCREEN         = const(8)
    OPT_MEDIAFIFO          = const(16)
    OPT_SOUND              = const(32)
    OPT_FILL               = const(8192)

    # flash status
    FLASH_STATUS_INIT      = const(0)
    FLASH_STATUS_DETACHED  = const(1)
    FLASH_STATUS_BASIC     = const(2)
    FLASH_STATUS_FULL      = const(3)

    # Memory Map
    RAM_G                  = const(0x000000)
    ROM_FONT               = const(0x1E0000)
    ROM_FONTROOT           = const(0x2FFFFC)
    RAM_DL                 = const(0x300000)
    RAM_REG                = const(0x302000)
    RAM_CMD                = const(0x308000)
    RAM_ERR_REPORT         = const(0x309800)
    Flash_address          = const(0x800000)
    RAM_G_SIZE             = const(1024 * 1024)
    ROM_FONT_SIZE          = const(1152 * 1024)
    ROM_FONTROOT_SIZE      = const(4)
    RAM_DL_SIZE            = const(8 * 1024)
    RAM_REG_SIZE           = const(4 * 1024)
    RAM_CMD_SIZE           = const(4 * 1024)
    RAM_ERR_REPORT_SIZE    = const(128)

    # Registers
    REG_CLOCK              = const(0x302008)
    REG_CMD_DL             = const(0x302100)
    REG_CMD_READ           = const(0x3020F8)
    REG_CMD_WRITE          = const(0x3020FC)
    REG_CMDB_SPACE         = const(0x302574)
    REG_CMDB_WRITE         = const(0x302578)
    REG_CPURESET           = const(0x302020)
    REG_CSPREAD            = const(0x302068)
    REG_CTOUCH_EXTENDED    = const(0x302108)
    REG_CTOUCH_MODE        = const(0x302108)
    REG_CTOUCH_RAW_XY      = const(0x30211C)
    REG_CTOUCH_TAG         = const(0x30212C)
    REG_CTOUCH_TAG_XY      = const(0x302128)
    REG_CTOUCH_TAG1        = const(0x302134)
    REG_CTOUCH_TAG1_XY     = const(0x302130)
    REG_CTOUCH_TAG2        = const(0x30213C)
    REG_CTOUCH_TAG2_XY     = const(0x302138)
    REG_CTOUCH_TAG3        = const(0x302144)
    REG_CTOUCH_TAG3_XY     = const(0x302140)
    REG_CTOUCH_TAG4        = const(0x30214C)
    REG_CTOUCH_TAG4_XY     = const(0x302148)
    REG_CTOUCH_TOUCH_XY    = const(0x302124)
    REG_CTOUCH_TOUCH1_XY   = const(0x30211C)
    REG_CTOUCH_TOUCH2_XY   = const(0x30218C)
    REG_CTOUCH_TOUCH3_XY   = const(0x302190)
    REG_CTOUCH_TOUCH4_X    = const(0x30216C)
    REG_CTOUCH_TOUCH4_Y    = const(0x302120)
    REG_DITHER             = const(0x302060)
    REG_DLSWAP             = const(0x302054)
    REG_FLASH_SIZE         = const(0x309024)
    REG_FLASH_STATUS       = const(0x3025F0)
    REG_FRAMES             = const(0x302004)
    REG_FREQUENCY          = const(0x30200C)
    REG_GPIO               = const(0x302094)
    REG_GPIO_DIR           = const(0x302090)
    REG_GPIOX              = const(0x30209C)
    REG_GPIOX_DIR          = const(0x302098)
    REG_HCYCLE             = const(0x30202C)
    REG_HOFFSET            = const(0x302030)
    REG_HSIZE              = const(0x302034)
    REG_HSYNC0             = const(0x302038)
    REG_HSYNC1             = const(0x30203C)
    REG_ID                 = const(0x302000)
    REG_INT_EN             = const(0x3020AC)
    REG_INT_FLAGS          = const(0x3020A8)
    REG_INT_MASK           = const(0x3020B0)
    REG_MACRO_0            = const(0x3020D8)
    REG_MACRO_1            = const(0x3020DC)
    REG_MEDIAFIFO_READ     = const(0x309014)
    REG_MEDIAFIFO_WRITE    = const(0x309018)
    REG_OUTBITS            = const(0x30205C)
    REG_PCLK               = const(0x302070)
    REG_PCLK_POL           = const(0x30206C)
    REG_PLAY               = const(0x30208C)
    REG_PLAY_CONTROL       = const(0x30914E)
    REG_PLAYBACK_FORMAT    = const(0x3020C4)
    REG_PLAYBACK_FREQ      = const(0x3020C0)
    REG_PLAYBACK_LENGTH    = const(0x3020B8)
    REG_PLAYBACK_LOOP      = const(0x3020C8)
    REG_PLAYBACK_PAUSE     = const(0x3025EC)
    REG_PLAYBACK_PLAY      = const(0x3020CC)
    REG_PLAYBACK_READPTR   = const(0x3020BC)
    REG_PLAYBACK_START     = const(0x3020B4)
    REG_PWM_DUTY           = const(0x3020D4)
    REG_PWM_HZ             = const(0x3020D0)
    REG_ROTATE             = const(0x302058)
    REG_SOUND              = const(0x302088)
    REG_SPI_WIDTH          = const(0x302188)
    REG_SWIZZLE            = const(0x302064)
    REG_TAG                = const(0x30207C)
    REG_TAG_X              = const(0x302074)
    REG_TAG_Y              = const(0x302078)
    REG_TOUCH_ADC_MODE     = const(0x302108)
    REG_TOUCH_CHARGE       = const(0x30210C)
    REG_TOUCH_CONFIG       = const(0x302168)
    REG_TOUCH_DIRECT_XY    = const(0x30218C)
    REG_TOUCH_DIRECT_Z1Z2  = const(0x302190)
    REG_TOUCH_MODE         = const(0x302104)
    REG_TOUCH_OVERSAMPLE   = const(0x302114)
    REG_TOUCH_RAW_XY       = const(0x30211C)
    REG_TOUCH_RZ           = const(0x302120)
    REG_TOUCH_RZTHRESH     = const(0x302118)
    REG_TOUCH_SCREEN_XY    = const(0x302124)
    REG_TOUCH_SETTLE       = const(0x302110)
    REG_TOUCH_TAG          = const(0x30212C)
    REG_TOUCH_TAG_XY       = const(0x302128)
    REG_TOUCH_TRANSFORM_A  = const(0x302150)
    REG_TOUCH_TRANSFORM_B  = const(0x302154)
    REG_TOUCH_TRANSFORM_C  = const(0x302158)
    REG_TOUCH_TRANSFORM_D  = const(0x30215C)
    REG_TOUCH_TRANSFORM_E  = const(0x302160)
    REG_TOUCH_TRANSFORM_F  = const(0x302164)
    REG_TRACKER            = const(0x309000)
    REG_TRACKER_1          = const(0x309004)
    REG_TRACKER_2          = const(0x309008)
    REG_TRACKER_3          = const(0x30900C)
    REG_TRACKER_4          = const(0x309010)
    REG_VCYCLE             = const(0x302040)
    REG_VOFFSET            = const(0x302044)
    REG_VOL_PB             = const(0x302080)
    REG_VOL_SOUND          = const(0x302084)
    REG_VSIZE              = const(0x302048)
    REG_VSYNC0             = const(0x30204C)
    REG_VSYNC1             = const(0x302050)

    def __init__(self, host):
        BrtEveBT81xFT81xCommon.__init__(self)
        self.host = host
        self.eve = self

        self.eve_type = "bt815_6"
        self.storage = BrtEveStorage(self.eve)

    # Same with FT81X
    @args_to_integer
    def cmd_loadidentity(self):
        """  instructs the coprocessor engine to set the current matrix to the identity matrix, so
             that the coprocessor engine is able to form the new matrix as requested by CMD_SCALE,
             CMD_ROTATE,CMD_TRANSLATE command.
        :return: none
        """
        self.cmd0(0x26)

    # BT815/6 commands
    @args_to_integer
    def cmd_animdraw(self, ch):  # pylint: disable=invalid-name
        """ draws one or more active animations
        :param ch: Animation channel, 0-31.  If ch is -1, then it draws all undrawn animations in
                   ascending order.
        :return: none
        """
        self.cmd(0x56, "i", (ch,))

    @args_to_integer
    def cmd_animframe(self, x, y, aoptr, frame):  # pylint: disable=invalid-name
        """ draws the specified frame of an animation
        :param x:  x screen coordinate for the animation center, in pixels.
        :param y:  y screen coordinate for the animation center, in pixels.
        :param aoptr:  The address of the animation object in flash memory.
        :param frame:  Frame number to draw, starting from zero.
        :return: none
        """
        self.cmd(0x5a, "hhII", (x, y, aoptr, frame))

    @args_to_integer
    def cmd_animstart(self, ch, aoptr, loop):  # pylint: disable=invalid-name
        """ Send TODO to EVE
        :param ch: Animation channel, 0-31. If no channel is available,
                   then an “out of channels” exception is raised.
        :param aoptr: The address of the animation object in flash memory.
        :param loop: Loop flags. ANIM_ONCE plays the animation once, then cancel it. ANIM_LOOP pays
                     the animation in a loop. ANIM_HOLD plays the animation once, then displays the
                     final frame.
        :return: none
        """
        self.cmd(0x53, "iII", (ch, aoptr, loop))

    @args_to_integer
    def cmd_animstop(self, ch):  # pylint: disable=invalid-name
        """ stops one or more active animations
        :param ch:  Animation channel, 0-31. If ch is -1, then all animations are stopped.
        :return: none
        """
        self.cmd(0x54, "i", (ch,))

    @args_to_integer
    def cmd_animxy(self, ch, x, y):  # pylint: disable=invalid-name
        """ sets the coordinates of an animation
        :param ch: Animation channel, 0-31.
        :param x: x screen coordinate for the animation center, in pixels
        :param y: y screen coordinate for the animation center, in pixels
        :return: none
        """
        self.cmd(0x55, "iii", (ch, x, y))

    @args_to_integer
    def cmd_appendf(self, ptr, num):
        """ appends data from flash to the display list
        :param ptr:  start of source commands in flash memory. Must be 64-byte aligned.
                     Start address of first block is from zero.
        :param num:  number of bytes to copy. This must be a multiple of 4
        :return: none
        """
        self.cmd(0x59, "II", (ptr, num))

    @args_to_integer
    def cmd_bitmap_transform(self,  x0, y0, x1, y1, x2, y2, tx0, ty0, tx1, ty1, tx2, ty2, result ):  # pylint: disable=invalid-name, too-many-arguments
        """ computes a bitmap transform and appends commands BITMAP_TRANSFORM_A –
            BITMAP_TRANSFORM_F to the display list
        :param x0,y0:  Point 0 screen coordinate, in pixels
        :param x1,y1: Point 1 screen coordinate, in pixels
        :param x2,y2: Point 2 screen coordinate, in pixels
        :param tx0,ty0: Point 0 bitmap coordinate, in pixels
        :param tx1,ty1: Point 1 bitmap coordinate, in pixels
        :param tx2,ty2: Point 2 bitmap coordinate, in pixels
        :param result: result return. Set to -1 on success, or 0 if it is not possible to find the
                       solution matrix.
        :return: none
        """
        self.cmd(0x21, "iiiiiiiiiiiiI",
        (x0, y0, x1, y1, x2, y2, tx0, ty0, tx1, ty1, tx2, ty2, result))

    @args_to_integer
    def cmd_clearcache(self):
        """ clears the graphics system’s flash cache
        :return: none
        """
        self.cmd0(0x4f)

    @args_to_integer
    def cmd_fillwidth(self, s):  # pylint: disable=invalid-name
        """ sets the pixel fill width for CMD_TEXT,CMD_BUTTON,CMD_BUTTON with the OPT_FILL option
        :param s: line fill width, in pixels
        :return: none
        """
        self.cmd(0x58, "I", (s,))

    @args_to_integer
    def cmd_flashattach(self):
        """ causes EVE to re-connect to the attached SPI flash storage
        :return: none
        """
        self.cmd0(0x49)

    @args_to_integer
    def cmd_flashdetach(self):
        """ causes EVE to put the SPI device lines into hi-Z state
        :return: none
        """
        self.cmd0(0x48)

    @args_to_integer
    def cmd_flasherase(self):
        """ erases the attached flash storage
        :return: none
        """
        self.cmd0(0x44)

    @args_to_integer
    def cmd_flashfast(self):
        """ causes the BT81X chip to drive the attached flash in full-speed mode
        :return: none
        """
        self.cmd(0x4a, "I", (0xdeadbeef,))

    @args_to_integer
    def cmd_flashread(self, dest, src, num):
        """ reads data from flash into main memory
        :param dest: Destination address in RAM_G. Must be 4-byte aligned.
                     Start address of first block is from zero.
        :param src: source address in flash memory. Must be 64-byte aligned.
        :param num: number of bytes to write, must be multiple of 4
        :return: none
        """
        self.cmd(0x46, "III", (dest, src, num))

    @args_to_integer
    def cmd_flashsource(self, ptr):
        """ specifies the source address for flash data loaded by the CMD_LOADIMAGE,
            CMD_PLAYVIDEO, CMD_VIDEOSTART and CMD_INFLATE2 commands with the OPT_FLASH  option.
        :param ptr: flash address, must be 64-byte aligned. Start address of first block is from
                    zero.
        :return: none
        """
        self.cmd(0x4e, "I", (ptr,))

    @args_to_integer
    def cmd_flashspidesel(self):
        """ de-asserts the SPI CS signal.  It is only valid when the flash has been detached,
            using CMD_FLASHDETACH.
        :return: none
        """
        self.cmd0(0x4b)

    @args_to_integer
    def cmd_flashspirx(self, ptr, num):
        """ receives bytes from the flash SPI interface, and writes them to main memory. It is
            only valid when the flash has been detached, using CMD_FLASHDETACH.
        :param ptr:  destination address in RAM_G
        :param num:  number of bytes to receive
        :return: none
        """
        self.cmd(0x4d, "II", (ptr, num))

    @args_to_integer
    def cmd_flashspitx(self, num):
        """ transmits the following bytes over the flash SPI interface. It is only valid when the
            flash has been detached, using CMD_FLASHDETACH.
        :param num: number of bytes to transmit
        :return: none
        """
        self.cmd(0x4c, "I", (len(num),))
        self.cc(align4(num))

    @args_to_integer
    def cmd_flashupdate(self, dest, src, num):
        """ writes the given data to flash
        :param dest: Destination address in flash memory. Must be 4096-byte aligned.
                     Start address of first block is from zero.
        :param src: source address in main memory RAM_G. Must be 4-byte aligned.
        :param num: number of bytes to write, must be multiple of 4096
        :return: none
        """
        self.cmd(0x47, "III", (dest, src, num))

    @args_to_integer
    def cmd_flashwrite(self, ptr, num):  # pylint: disable=invalid-name
        """ writes the following inline data to flash storage. The storage should have been
            previously erased using CMD_FLASHERASE.
        :param ptr: Destination address in flash memory. Must be 256-byte aligned.
                    Start address of first block is from zero.
        :param num: Number of bytes to write, must be multiple of 256
        :return: none
        """
        self.cmd(0x45, "II", (ptr, len(num)))
        self.cc(num)

    @args_to_integer
    def cmd_gradienta(self, x0, y0, argb0, x1, y1, argb1):  # pylint: disable=invalid-name, too-many-arguments
        """ Send TODO to EVE
        :param x0: x-coordinate of point 0, in pixels
        :param y0: y-coordinate of point 0, in pixels
        :param argb0: Color of point 0, as a 32-bit ARGB number. A is the most significant 8 bits,
                      B is the least. So 0x80ff0000 is 50% transparent bright red, and 0xff0000ff
                      is solid blue.
        :param x1: x-coordinate of point 1, in pixels
        :param y1: y-coordinate of point 1, in pixels
        :param argb1: Color of point 1
        :return: none
        """
        self.cmd(0x57, "hhIhhI", (x0, y0, argb0, x1, y1, argb1))

    @args_to_integer
    def cmd_inflate2(self, ptr, options):
        """ decompress the following compressed data into RAM_G
        :param ptr: destination address to put the decompressed data.
        :param options: If option OPT_MEDIAFIFO is given, the compressed data is sourced from the
                        media FIFO.
                        If option OPT_FLASH is given, then flash memory is the source. Otherwise,
                        giving zero value and the compressed data shall be followed immediately.
        :return: none
        """
        self.cmd(0x50, "II", (ptr, options))

    @args_to_integer
    def cmd_resetfonts(self):
        """ loads bitmap handles 16-31 with their default fonts
        :return: none
        """
        self.cmd0(0x52)

    @args_to_integer
    def cmd_rotatearound(self, x, y, a, s = 1):  # pylint: disable=invalid-name
        """ apply a rotation and scale around a specified coordinate
        :param x: center of rotation/scaling, x-coordinate
        :param y: center of rotation/scaling, x-coordinate
        :param a: clockwise rotation angle, in units of 1/65536 of a circle
        :param s: scale factor, in signed 16.16 bit fixed-point form
        :return: none
        """
        self.cmd(0x51, "iiii", (x, y, a, s))

    @args_to_integer
    def cmd_sync(self):
        """ waits for the end of the video scan out period, then it returns immediately
        :return: none
        """
        self.cmd0(0x42)

    @args_to_integer
    def cmd_videostartf(self):
        """ returns all the attributes of the bitmap made by the previous CMD_LOADIMAGE,
            CMD_PLAYVIDEO, CMD_VIDEOSTART or CMD_VIDEOSTARTF.
        :param source: Output parameter; source address of bitmap.
        :param fmt: Output parameter; format of the bitmap
        :param w: Width of bitmap, in pixels
        :param h: Height of bitmap, in pixels
        :param palette: Palette data of the bitmap if fmt is PALETTED565 or PALETTED4444.
                        Otherwise zero.
        :return: none
        """
        self.cmd0(0x5f)
