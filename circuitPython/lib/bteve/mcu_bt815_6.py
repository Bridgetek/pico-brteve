class Mcu_BT815_6():
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

        self.REG_ID                 = 0x302000
        self.REG_FRAMES             = 0x302004
        self.REG_CLOCK              = 0x302008
        self.REG_FREQUENCY          = 0x30200C
        self.REG_CPURESET           = 0x302020
        self.REG_HCYCLE             = 0x30202C
        self.REG_HOFFSET            = 0x302030
        self.REG_HSIZE              = 0x302034
        self.REG_HSYNC0             = 0x302038
        self.REG_HSYNC1             = 0x30203C
        self.REG_VCYCLE             = 0x302040
        self.REG_VOFFSET            = 0x302044
        self.REG_VSIZE              = 0x302048
        self.REG_VSYNC0             = 0x30204C
        self.REG_VSYNC1             = 0x302050
        self.REG_DLSWAP             = 0x302054
        self.REG_ROTATE             = 0x302058
        self.REG_OUTBITS            = 0x30205C
        self.REG_DITHER             = 0x302060
        self.REG_SWIZZLE            = 0x302064
        self.REG_CSPREAD            = 0x302068
        self.REG_PCLK_POL           = 0x30206C
        self.REG_PCLK               = 0x302070
        self.REG_TAG_X              = 0x302074
        self.REG_TAG_Y              = 0x302078
        self.REG_TAG                = 0x30207C
        self.REG_VOL_PB             = 0x302080
        self.REG_VOL_SOUND          = 0x302084
        self.REG_SOUND              = 0x302088
        self.REG_PLAY               = 0x30208C
        self.REG_GPIO_DIR           = 0x302090
        self.REG_GPIO               = 0x302094
        self.REG_GPIOX_DIR          = 0x302098
        self.REG_GPIOX              = 0x30209C
        self.REG_INT_FLAGS          = 0x3020A8
        self.REG_INT_EN             = 0x3020AC
        self.REG_INT_MASK           = 0x3020B0
        self.REG_PLAYBACK_START     = 0x3020B4
        self.REG_PLAYBACK_LENGTH    = 0x3020B8
        self.REG_PLAYBACK_READPTR   = 0x3020BC
        self.REG_PLAYBACK_FREQ      = 0x3020C0
        self.REG_PLAYBACK_FORMAT    = 0x3020C4
        self.REG_PLAYBACK_LOOP      = 0x3020C8
        self.REG_PLAYBACK_PLAY      = 0x3020CC
        self.REG_PWM_HZ             = 0x3020D0
        self.REG_PWM_DUTY           = 0x3020D4
        self.REG_MACRO_0            = 0x3020D8
        self.REG_MACRO_1            = 0x3020DC
        self.REG_CMD_READ           = 0x3020F8
        self.REG_CMD_WRITE          = 0x3020FC
        self.REG_CMD_DL             = 0x302100
        self.REG_TOUCH_MODE         = 0x302104
        self.REG_CTOUCH_EXTENDED    = 0x302108
        self.REG_TOUCH_ADC_MODE     = 0x302108
        self.REG_TOUCH_CHARGE       = 0x30210C
        self.REG_TOUCH_SETTLE       = 0x302110
        self.REG_TOUCH_OVERSAMPLE   = 0x302114
        self.REG_TOUCH_RZTHRESH     = 0x302118
        self.REG_CTOUCH_TOUCH1_XY   = 0x30211C
        self.REG_TOUCH_RAW_XY       = 0x30211C
        self.REG_CTOUCH_TOUCH4_Y    = 0x302120
        self.REG_TOUCH_RZ           = 0x302120
        self.REG_TOUCH_SCREEN_XY    = 0x302124
        self.REG_TOUCH_TAG_XY       = 0x302128
        self.REG_TOUCH_TAG          = 0x30212C
        self.REG_TOUCH_TRANSFORM_A  = 0x302150
        self.REG_TOUCH_TRANSFORM_B  = 0x302154
        self.REG_TOUCH_TRANSFORM_C  = 0x302158
        self.REG_TOUCH_TRANSFORM_D  = 0x30215C
        self.REG_TOUCH_TRANSFORM_E  = 0x302160
        self.REG_TOUCH_TRANSFORM_F  = 0x302164
        self.REG_TOUCH_CONFIG       = 0x302168
        self.REG_CTOUCH_TOUCH4_X    = 0x30216C
        self.REG_SPI_WIDTH          = 0x302188
        self.REG_CTOUCH_TOUCH2_XY   = 0x30218C
        self.REG_TOUCH_DIRECT_XY    = 0x30218C
        self.REG_CTOUCH_TOUCH3_XY   = 0x302190
        self.REG_TOUCH_DIRECT_Z1Z2  = 0x302190
        self.REG_CMDB_SPACE         = 0x302574
        self.REG_CMDB_WRITE         = 0x302578
        self.REG_PLAYBACK_PAUSE     = 0x3025EC
        self.REG_FLASH_STATUS       = 0x3025F0
        self.REG_TRACKER            = 0x309000
        self.REG_TRACKER_1          = 0x309004
        self.REG_TRACKER_2          = 0x309008
        self.REG_TRACKER_3          = 0x30900C
        self.REG_TRACKER_4          = 0x309010
        self.REG_MEDIAFIFO_READ     = 0x309014
        self.REG_MEDIAFIFO_WRITE    = 0x309018
        self.REG_FLASH_SIZE         = 0x309024
        self.REG_PLAY_CONTROL       = 0x30914E
        self.REG_CTOUCH_MODE        = 0x302108
        self.REG_CTOUCH_RAW_XY      = 0x30211C
        self.REG_CTOUCH_TOUCH_XY    = 0x302124
        self.REG_CTOUCH_TAG_XY      = 0x302128
        self.REG_CTOUCH_TAG         = 0x30212C
        self.REG_CTOUCH_TAG1_XY     = 0x302130
        self.REG_CTOUCH_TAG1        = 0x302134
        self.REG_CTOUCH_TAG2_XY     = 0x302138
        self.REG_CTOUCH_TAG2        = 0x30213C
        self.REG_CTOUCH_TAG3_XY     = 0x302140
        self.REG_CTOUCH_TAG3        = 0x302144
        self.REG_CTOUCH_TAG4_XY     = 0x302148
        self.REG_CTOUCH_TAG4        = 0x30214C

    def cmd_animdraw(self, *args):
        self.cmd(0x56, "i", args)

    def cmd_animframe(self, *args):
        self.cmd(0x5a, "hhII", args)

    def cmd_animstart(self, *args):
        self.cmd(0x53, "iII", args)

    def cmd_animstop(self, *args):
        self.cmd(0x54, "i", args)
    
    def cmd_animxy(self, *args):
        self.cmd(0x55, "iii", args)
    
    def cmd_append(self, *args):    
        self.cmd(0x1e, "II", args)
        def cmd_appendf(self, *args):
            self.cmd(0x59, "II", args)
    def cmd_bgcolor(self, *args):
        self.cmd(0x09, "I", args)
    def cmd_bitmap_transform(self, *args):
        self.cmd(0x21, "iiiiiiiiiiiiI", args)

    def cmd_button(self, *args):
        self.cmd(0x0d, "hhhhhH", args[:6])
        self.fstring(args[6:])

    def cmd_calibrate(self, *args):
        self.cmd(0x15, "I", args)

    def cmd_clearcache(self):
        self.cmd0(0x4f)

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

    def cmd_fillwidth(self, *args):
        self.cmd(0x58, "I", args)

    def cmd_flashattach(self):
        self.cmd0(0x49)

    def cmd_flashdetach(self):
        self.cmd0(0x48)

    def cmd_flasherase(self):
        self.cmd0(0x44)

    def cmd_flashfast(self):
        self.cmd(0x4a, "I", (0xdeadbeef,))

    def cmd_flashread(self, *args):
        self.cmd(0x46, "III", args)

    def cmd_flashsource(self, *args):
        self.cmd(0x4e, "I", args)

    def cmd_flashspidesel(self):
        self.cmd0(0x4b)

    def cmd_flashspirx(self, ptr, num):
        self.cmd(0x4d, "II", (ptr, num))

    def cmd_flashspitx(self, b):
        self.cmd(0x4c, "I", (len(b),))
        self.cc(align4(b))

    def cmd_flashupdate(self, *args):
        self.cmd(0x47, "III", args)

    def cmd_flashwrite(self, a, b):
        self.cmd(0x45, "II", (a, len(b)))
        self.cc(b)

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

    def cmd_gradienta(self, *args):
        self.cmd(0x57, "hhIhhI", args)

    def cmd_inflate(self, *args):
        self.cmd(0x22, "I", args)

    def cmd_inflate2(self, *args):
        self.cmd(0x50, "II", args)

    def cmd_interrupt(self, *args):
        self.cmd(0x02, "I", args)

    def cmd_keys(self, *args):
        self.cmd(0x0e, "hhhhhH", args[:6])
        self.cstring(args[6])

    def cmd_loadimage(self, *args):
        self.cmd(0x24, "iI", args)

    def cmd_loadidentity(self):
        self.cmd0(0x26)

    def cmd_logo(self):
        self.cmd0(0x31)

    def cmd_mediafifo(self, *args):
        self.cmd(0x39, "II", args)
    
    def cmd_memcpy(self, *args):
        self.cmd(0x1d, "III", args)

    def cmd_memcrc(self, *args):
        self.cmd(0x18, "III", args)

    def cmd_memset(self, *args):
        self.cmd(0x1b, "III", args)

    def cmd_memwrite(self, *args):
        self.cmd(0x1a, "II", args)

    def cmd_memzero(self, *args):
        self.cmd(0x1c, "II", args)

    def cmd_number(self, *args):
        args_int = tuple(int(el) for el in args)
        self.cmd(0x2e, "hhhHi", args_int)
    
    def cmd_playvideo(self, *args):
        self.cmd(0x3a, "I", args)

    def cmd_progress(self, *args):
        self.cmd(0x0f, "hhhhHHI", args)

    def cmd_regread(self, *args):
        self.cmd(0x19, "II", args)

    def cmd_resetfonts(self):
        self.cmd0(0x52)

    def cmd_romfont(self, *args):
        self.cmd(0x3f, "II", args)
    
    def cmd_rotate(self, a):
        self.cmd(0x29, "i", (furmans(a), ))

    def cmd_rotatearound(self, x, y, a, s = 1):
        self.cmd(0x51, "iiii", (x, y, furmans(a), f16(s)))

    def cmd_scale(self, sx, sy):
        self.cmd(0x28, "ii", (f16(sx), f16(sy)))

    def cmd_screensaver(self):
        self.cmd0(0x2f)

    def cmd_scrollbar(self, *args):
        self.cmd(0x11, "hhhhHHHH", args)

    def cmd_setbase(self, *args):
        self.cmd(0x38, "I", args)
    
    def cmd_setbitmap(self, *args):
        self.cmd(0x43, "IHhi", args)

    def cmd_setfont(self, *args):
        self.cmd(0x2b, "II", args)

    def cmd_setfont2(self, *args):
        self.cmd(0x3b, "III", args)

    def cmd_setmatrix(self):
        self.cmd0(0x2a)

    def cmd_setrotate(self, *args):
        self.cmd(0x36, "I", args)

    def cmd_setscratch(self, *args):
        self.cmd(0x3c, "I", args)

    def cmd_sketch(self, *args):
        self.cmd(0x30, "hhHHII", args)
    def cmd_slider(self, *args):
        self.cmd(0x10, "hhhhHHI", args)

    def cmd_snapshot(self, *args):
        self.cmd(0x1f, "I", args)

    def cmd_snapshot2(self, *args):
        self.cmd(0x37, "IIhhhh", args)

    def cmd_spinner(self, *args):
        self.cmd(0x16, "hhHH", args)

    def cmd_stop(self):
        self.cmd0(0x17)

    def cmd_swap(self):
        self.cmd0(0x01)

    def cmd_sync(self):
        self.cmd0(0x42)

    def cmd_text(self, *args):
        args_int = tuple(int(el) for el in args[0:4])
        self.cmd(0x0c, "hhhH", args_int[0:4])
        self.fstring(args[4:])

    def cmd_toggle(self, *args):
        self.cmd(0x12, "hhhhHH", args[0:6])
        label = (args[6].encode() + b'\xff' + args[7].encode())
        self.fstring((label,) + args[8:])

    def cmd_track(self, *args):
        self.cmd(0x2c, "hhhhi", args)
    
    def cmd_translate(self, tx, ty):
        self.cmd(0x27, "ii", (f16(tx), f16(ty)))

    def cmd_videoframe(self, *args):
        self.cmd(0x41, "II", args)

    def cmd_videostart(self):
        self.cmd(0x40, "", ())

    def cmd_videostartf(self):
        self.cmd(0x5f, "", ())

