class Mcu_BT817_8():
    def __init__(self):
        # Bitmap format macros
        self.PALETTED565            = (14)       # FT810
        self.PALETTED4444           = (15)       # FT810
        self.PALETTED8              = (16)       # FT810
        self.L2                     = (17)       # FT810
        self.GLFORMAT               = (31)       # FT815
        self.ASTC_4x4               = (0x93B0)   # BT815
        self.ASTC_5x4               = (0x93B1)   # BT815
        self.ASTC_5x5               = (0x93B2)   # BT815
        self.ASTC_6x5               = (0x93B3)   # BT815
        self.ASTC_6x6               = (0x93B4)   # BT815
        self.ASTC_8x5               = (0x93B5)   # BT815
        self.ASTC_8x6               = (0x93B6)   # BT815
        self.ASTC_8x8               = (0x93B7)   # BT815
        self.ASTC_10x5              = (0x93B8)   # BT815
        self.ASTC_10x6              = (0x93B9)   # BT815
        self.ASTC_10x8              = (0x93BA)   # BT815
        self.ASTC_10x10             = (0x93BB)   # BT815
        self.ASTC_12x10             = (0x93BC)   # BT815
        self.ASTC_12x12             = (0x93BD)   # BT815

        # Widget command macros
        self.OPT_FULLSCREEN         = (8)
        self.OPT_MEDIAFIFO          = (16)
        self.OPT_FORMAT             = (4096)     # For 815
        self.OPT_FILL               = (8192)     # For 815

        self.RAM_CMD                = (0x308000)
        self.RAM_DL                 = (0x300000)
        self.REG_CLOCK              = (0x302008)
        self.REG_CMDB_SPACE         = (0x302574)
        self.REG_CMDB_WRITE         = (0x302578)
        self.REG_CMD_DL             = (0x302100)
        self.REG_CMD_READ           = (0x3020f8)
        self.REG_CMD_WRITE          = (0x3020fc)
        self.REG_CPURESET           = (0x302020)
        self.REG_CSPREAD            = (0x302068)
        self.REG_DITHER             = (0x302060)
        self.REG_DLSWAP             = (0x302054)
        self.REG_FRAMES             = (0x302004)
        self.REG_FREQUENCY          = (0x30200c)
        self.REG_GPIO               = (0x302094)
        self.REG_GPIO_DIR           = (0x302090)
        self.REG_HCYCLE             = (0x30202c)
        self.REG_HOFFSET            = (0x302030)
        self.REG_HSIZE              = (0x302034)
        self.REG_HSYNC0             = (0x302038)
        self.REG_HSYNC1             = (0x30203c)
        self.REG_ID                 = (0x302000)
        self.REG_INT_EN             = (0x3020ac)
        self.REG_INT_FLAGS          = (0x3020a8)
        self.REG_INT_MASK           = (0x3020b0)
        self.REG_MACRO_0            = (0x3020d8)
        self.REG_MACRO_1            = (0x3020dc)
        self.REG_OUTBITS            = (0x30205c)
        self.REG_PCLK               = (0x302070)
        self.REG_PCLK_POL           = (0x30206c)
        self.REG_PLAY               = (0x30208c)
        self.REG_PLAYBACK_FORMAT    = (0x3020c4)
        self.REG_PLAYBACK_FREQ      = (0x3020c0)
        self.REG_PLAYBACK_LENGTH    = (0x3020b8)
        self.REG_PLAYBACK_LOOP      = (0x3020c8)
        self.REG_PLAYBACK_PLAY      = (0x3020cc)
        self.REG_PLAYBACK_READPTR   = (0x3020bc)
        self.REG_PLAYBACK_START     = (0x3020b4)
        self.REG_PWM_DUTY           = (0x3020d4)
        self.REG_PWM_HZ             = (0x3020d0)
        self.REG_ROTATE             = (0x302058)
        self.REG_SOUND              = (0x302088)
        self.REG_SWIZZLE            = (0x302064)
        self.REG_TAG                = (0x30207c)
        self.REG_TAG_X              = (0x302074)
        self.REG_TAG_Y              = (0x302078)
        self.REG_TAP_CRC            = (0x302024)
        self.REG_TOUCH_ADC_MODE     = (0x302108)
        self.REG_TOUCH_CHARGE       = (0x30210c)
        self.REG_TOUCH_DIRECT_XY    = (0x30218c)
        self.REG_TOUCH_DIRECT_Z1Z2  = (0x302190)
        self.REG_TOUCH_MODE         = (0x302104)
        self.REG_TOUCH_OVERSAMPLE   = (0x302114)
        self.REG_TOUCH_RAW_XY       = (0x30211c)
        self.REG_TOUCH_RZ           = (0x302120)
        self.REG_TOUCH_RZTHRESH     = (0x302118)
        self.REG_TOUCH_SCREEN_XY    = (0x302124)
        self.REG_TOUCH_SETTLE       = (0x302110)
        self.REG_TOUCH_TAG          = (0x30212c)
        self.REG_TOUCH_TAG_XY       = (0x302128)
        self.REG_TOUCH_TRANSFORM_A  = (0x302150)
        self.REG_TOUCH_TRANSFORM_B  = (0x302154)
        self.REG_TOUCH_TRANSFORM_C  = (0x302158)
        self.REG_TOUCH_TRANSFORM_D  = (0x30215c)
        self.REG_TOUCH_TRANSFORM_E  = (0x302160)
        self.REG_TOUCH_TRANSFORM_F  = (0x302164)
        self.REG_TRACKER            = (0x309000)
        self.REG_TRIM               = (0x302180)
        self.REG_VCYCLE             = (0x302040)
        self.REG_VOFFSET            = (0x302044)
        self.REG_VOL_PB             = (0x302080)
        self.REG_VOL_SOUND          = (0x302084)
        self.REG_VSIZE              = (0x302048)
        self.REG_VSYNC0             = (0x30204c)
        self.REG_VSYNC1             = (0x302050)

        # 810 registers
        self.REG_MEDIAFIFO_BASE     = (0x30901c) 
        self.REG_MEDIAFIFO_READ     = (0x309014) 
        self.REG_MEDIAFIFO_SIZE     = (0x309020) 
        self.REG_MEDIAFIFO_WRITE    = (0x309018) 
        self.REG_GPIOX              = (0x30209c)
        self.REG_GPIOX_DIR          = (0x302098)

        # 815 registers
        self.REG_FLASH_SIZE         = (0x00309024) 
        self.REG_FLASH_STATUS       = (0x003025f0) 
        self.REG_ADAPTIVE_FRAMERATE = (0x0030257c) 

        # 817 registers
        self.REG_PCLK_FREQ          = (0x00302614)
        self.REG_PCLK_2X            = (0x00302618)
        self.REG_TOUCH_CONFIG       = (0x302168)