""" BridgeTek FT81X's definitions """
from .brt_eve_bt81x_ft81x_common import BrtEveBT81xFT81xCommon
from .brt_eve_common import args_to_integer, const

class BrtEve(BrtEveBT81xFT81xCommon):
    """FT81X specific commands, options and registers"""

    # Memory Map
    RAM_G                      = const(0x000000)
    RAM_DL                     = const(0x300000)
    RAM_REG                    = const(0x302000)
    RAM_CMD                    = const(0x308000)
    RAM_G_SIZE                 = const(1024 * 1024)
    RAM_DL_SIZE                = const(8 * 1024)
    RAM_REG_SIZE               = const(4 * 1024)
    RAM_CMD_SIZE               = const(4 * 1024)

    # Register definitions
    REG_CLOCK                  = const(0x302008)
    REG_CMD_DL                 = const(0x302100)
    REG_CMD_READ               = const(0x3020F8)
    REG_CMD_WRITE              = const(0x3020FC)
    REG_CMDB_SPACE             = const(0x302574)
    REG_CMDB_WRITE             = const(0x302578)
    REG_CPURESET               = const(0x302020)
    REG_CSPREAD                = const(0x302068)
    REG_CTOUCH_EXTENDED        = const(0x302108)
    REG_CTOUCH_MODE            = const(0x302104)
    REG_CTOUCH_RAW_XY          = const(0x30211C)
    REG_CTOUCH_TAG             = const(0x30212C)
    REG_CTOUCH_TAG_XY          = const(0x302128)
    REG_CTOUCH_TAG1            = const(0x302134)
    REG_CTOUCH_TAG1_XY         = const(0x302130)
    REG_CTOUCH_TAG2            = const(0x30213C)
    REG_CTOUCH_TAG2_XY         = const(0x302138)
    REG_CTOUCH_TAG3            = const(0x302144)
    REG_CTOUCH_TAG3_XY         = const(0x302140)
    REG_CTOUCH_TAG4            = const(0x30214C)
    REG_CTOUCH_TAG4_XY         = const(0x302148)
    REG_CTOUCH_TOUCH_XY        = const(0x302124)
    REG_CTOUCH_TOUCH1_XY       = const(0x30211C)
    REG_CTOUCH_TOUCH2_XY       = const(0x30218C)
    REG_CTOUCH_TOUCH3_XY       = const(0x302190)
    REG_CTOUCH_TOUCH4_X        = const(0x30216C)
    REG_CTOUCH_TOUCH4_Y        = const(0x302120)
    REG_DITHER                 = const(0x302060)
    REG_DLSWAP                 = const(0x302054)
    REG_FRAMES                 = const(0x302004)
    REG_FREQUENCY              = const(0x30200C)
    REG_GPIO                   = const(0x302094)
    REG_GPIO_DIR               = const(0x302090)
    REG_GPIOX                  = const(0x30209C)
    REG_GPIOX_DIR              = const(0x302098)
    REG_HCYCLE                 = const(0x30202C)
    REG_HOFFSET                = const(0x302030)
    REG_HSIZE                  = const(0x302034)
    REG_HSYNC0                 = const(0x302038)
    REG_HSYNC1                 = const(0x30203C)
    REG_ID                     = const(0x302000)
    REG_INT_EN                 = const(0x3020AC)
    REG_INT_FLAGS              = const(0x3020A8)
    REG_INT_MASK               = const(0x3020B0)
    REG_MACRO_0                = const(0x3020D8)
    REG_MACRO_1                = const(0x3020DC)
    REG_MEDIAFIFO_READ         = const(0x309014)
    REG_MEDIAFIFO_WRITE        = const(0x309018)
    REG_OUTBITS                = const(0x30205C)
    REG_PCLK                   = const(0x302070)
    REG_PCLK_POL               = const(0x30206C)
    REG_PLAY                   = const(0x30208C)
    REG_PLAYBACK_FORMAT        = const(0x3020C4)
    REG_PLAYBACK_FREQ          = const(0x3020C0)
    REG_PLAYBACK_LENGTH        = const(0x3020B8)
    REG_PLAYBACK_LOOP          = const(0x3020C8)
    REG_PLAYBACK_PLAY          = const(0x3020CC)
    REG_PLAYBACK_READPTR       = const(0x3020BC)
    REG_PLAYBACK_START         = const(0x3020B4)
    REG_PWM_DUTY               = const(0x3020D4)
    REG_PWM_HZ                 = const(0x3020D0)
    REG_ROTATE                 = const(0x302058)
    REG_SOUND                  = const(0x302088)
    REG_SPI_WIDTH              = const(0x302188)
    REG_SWIZZLE                = const(0x302064)
    REG_TAG                    = const(0x30207C)
    REG_TAG_X                  = const(0x302074)
    REG_TAG_Y                  = const(0x302078)
    REG_TOUCH_ADC_MODE         = const(0x302108)
    REG_TOUCH_CHARGE           = const(0x30210C)
    REG_TOUCH_CONFIG           = const(0x302168)
    REG_TOUCH_DIRECT_XY        = const(0x30218C)
    REG_TOUCH_DIRECT_Z1Z2      = const(0x302190)
    REG_TOUCH_MODE             = const(0x302104)
    REG_TOUCH_OVERSAMPLE       = const(0x302114)
    REG_TOUCH_RAW_XY           = const(0x30211C)
    REG_TOUCH_RZ               = const(0x302120)
    REG_TOUCH_RZTHRESH         = const(0x302118)
    REG_TOUCH_SCREEN_XY        = const(0x302124)
    REG_TOUCH_SETTLE           = const(0x302110)
    REG_TOUCH_TAG              = const(0x30212C)
    REG_TOUCH_TAG_XY           = const(0x302128)
    REG_TOUCH_TRANSFORM_A      = const(0x302150)
    REG_TOUCH_TRANSFORM_B      = const(0x302154)
    REG_TOUCH_TRANSFORM_C      = const(0x302158)
    REG_TOUCH_TRANSFORM_D      = const(0x30215C)
    REG_TOUCH_TRANSFORM_E      = const(0x302160)
    REG_TOUCH_TRANSFORM_F      = const(0x302164)
    REG_TRACKER                = const(0x309000)
    REG_TRACKER_1              = const(0x309004)
    REG_TRACKER_2              = const(0x309008)
    REG_TRACKER_3              = const(0x30900C)
    REG_TRACKER_4              = const(0x309010)
    REG_TRIM                   = const(0x302180)
    REG_VCYCLE                 = const(0x302040)
    REG_VOFFSET                = const(0x302044)
    REG_VOL_PB                 = const(0x302080)
    REG_VOL_SOUND              = const(0x302084)
    REG_VSIZE                  = const(0x302048)
    REG_VSYNC0                 = const(0x30204C)
    REG_VSYNC1                 = const(0x302050)

    def __init__(self, host):
        BrtEveBT81xFT81xCommon.__init__(self)

        self.host = host
        self.eve = self

        self.eve_type = "ft81x"

    # FT81X commands
    @args_to_integer
    def cmd_csketch(self, x, y, w, h, ptr, fmt, freq):  # pylint: disable=invalid-name, too-many-arguments
        """ This command is the legacy command from the FT801 chip. Users are recommended to use
            “CMD_SKETCH” for FT81X since it works for both RTE and CTE.
        :param x: x-coordinate of sketch area top-left, in pixels
        :param y: y-coordinate of sketch area top-left, in pixels
        :param w: Width of sketch area, in pixels
        :param h: Height of sketch area, in pixels
        :param ptr: Base address of sketch bitmap
        :param format: Format of sketch bitmap, either L1 or L8
        :param Freq: Deprecated.
        :return: none
        """
        self.cmd0(0x35, "hhHHIHH", (x, y, w, h, ptr, fmt, freq))

    @args_to_integer
    def cmd_loadidentity(self):
        """ instructs the co-processor engine of the FT81X to set the current matrix to the
            identity matrix
        :return: none
        """
        self.cmd0(0x26)
