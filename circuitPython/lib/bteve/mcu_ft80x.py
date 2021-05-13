class Mcu_FT80X():
    def __init__(self):        # Chip identifier macros
        # Macros related to inbuilt font
        self.NUMCHAR_PERFONT            = (128)         #number of font characters per bitmap handle
        self.FONT_TABLE_SIZE            = (148)         #size of the font table - utilized for loopup by the graphics engine
        self.FONT_TABLE_POINTER         = (0xFFFFC)    #pointer to the inbuilt font tables starting from bitmap handle 16

        # Synthesized sound macros
        self.SILENCE                    = 0x00

        self.SQUAREWAVE                 = 0x01
        self.SINEWAVE                   = 0x02
        self.SAWTOOTH                   = 0x03
        self.TRIANGLE                   = 0x04

        self.BEEPING                    = 0x05
        self.ALARM                      = 0x06
        self.WARBLE                     = 0x07
        self.CAROUSEL                   = 0x08

        # Synthesized sound frequencies, midi note macros
        self.MIDI_A0                    = 21
        self.MIDI_A_0                   = 22
        self.MIDI_B0                    = 23
        self.MIDI_C1                    = 24
        self.MIDI_C_1                   = 25
        self.MIDI_D1                    = 26
        self.MIDI_D_1                   = 27
        self.MIDI_E1                    = 28
        self.MIDI_F1                    = 29
        self.MIDI_F_1                   = 30
        self.MIDI_G1                    = 31
        self.MIDI_G_1                   = 32
        self.MIDI_A1                    = 33
        self.MIDI_A_1                   = 34
        self.MIDI_B1                    = 35
        self.MIDI_C2                    = 36
        self.MIDI_C_2                   = 37
        self.MIDI_D2                    = 38
        self.MIDI_D_2                   = 39
        self.MIDI_E2                    = 40
        self.MIDI_F2                    = 41
        self.MIDI_F_2                   = 42
        self.MIDI_G2                    = 43
        self.MIDI_G_2                   = 44
        self.MIDI_A2                    = 45
        self.MIDI_A_2                   = 46
        self.MIDI_B2                    = 47
        self.MIDI_C3                    = 48
        self.MIDI_C_3                   = 49
        self.MIDI_D3                    = 50
        self.MIDI_D_3                   = 51
        self.MIDI_E3                    = 52
        self.MIDI_F3                    = 53
        self.MIDI_F_3                   = 54
        self.MIDI_G3                    = 55
        self.MIDI_G_3                   = 56
        self.MIDI_A3                    = 57
        self.MIDI_A_3                   = 58
        self.MIDI_B3                    = 59
        self.MIDI_C4                    = 60
        self.MIDI_C_4                   = 61
        self.MIDI_D4                    = 62
        self.MIDI_D_4                   = 63
        self.MIDI_E4                    = 64
        self.MIDI_F4                    = 65
        self.MIDI_F_4                   = 66
        self.MIDI_G4                    = 67
        self.MIDI_G_4                   = 68
        self.MIDI_A4                    = 69
        self.MIDI_A_4                   = 70
        self.MIDI_B4                    = 71
        self.MIDI_C5                    = 72
        self.MIDI_C_5                   = 73
        self.MIDI_D5                    = 74
        self.MIDI_D_5                   = 75
        self.MIDI_E5                    = 76
        self.MIDI_F5                    = 77
        self.MIDI_F_5                   = 78
        self.MIDI_G5                    = 79
        self.MIDI_G_5                   = 80
        self.MIDI_A5                    = 81
        self.MIDI_A_5                   = 82
        self.MIDI_B5                    = 83
        self.MIDI_C6                    = 84
        self.MIDI_C_6                   = 85
        self.MIDI_D6                    = 86
        self.MIDI_D_6                   = 87
        self.MIDI_E6                    = 88
        self.MIDI_F6                    = 89
        self.MIDI_F_6                   = 90
        self.MIDI_G6                    = 91
        self.MIDI_G_6                   = 92
        self.MIDI_A6                    = 93
        self.MIDI_A_6                   = 94
        self.MIDI_B6                    = 95
        self.MIDI_C7                    = 96
        self.MIDI_C_7                   = 97
        self.MIDI_D7                    = 98
        self.MIDI_D_7                   = 99
        self.MIDI_E7                    = 100
        self.MIDI_F7                    = 101
        self.MIDI_F_7                   = 102
        self.MIDI_G7                    = 103
        self.MIDI_G_7                   = 104
        self.MIDI_A7                    = 105
        self.MIDI_A_7                   = 106
        self.MIDI_B7                    = 107
        self.MIDI_C8                    = 108

        # Display rotation
        self.DISPLAY_0                  = 0    #0 degrees rotation
        self.DISPLAY_180                = 1    #180 degrees rotation

        # Maximum display display resolution supported by graphics engine
        self.MAX_DISPLAYWIDTH           = (512)
        self.MAX_DISPLAYHEIGHT          = (512)

        # Coprocessor reset related macros
        self.RESET_HOLD_COPROCESSOR     = 1
        self.RESET_RELEASE_COPROCESSOR  = 0

        # Macros for sound play and stop
        self.SOUND_PLAY                 = 1
        self.AUDIO_PLAY                 = 1

        # Macros for audio playback parameters
        self.AUDIO_SAMPLINGFREQ_MIN     = 8*1000
        self.AUDIO_SAMPLINGFREQ_MAX     = 48*1000

        #coprocessor error macros
        self.COPRO_ERROR                = 0xfff

        # Memory definitions
        self.RAM_G                      = 0x000000
        self.ROM_CHIPID                 = 0x0C0000
        self.ROM_FONT                   = 0x0BB23C
        self.ROM_FONT_ADDR              = 0x0FFFFC
        self.RAM_DL                     = 0x100000
        self.RAM_PAL                    = 0x102000
        self.RAM_CMD                    = 0x108000
        self.RAM_SCREENSHOT             = 0x1C2000

        # Memory buffer sizes
        self.RAM_G_SIZE                 = 256*1024
        self.CMDFIFO_SIZE               = 4*1024
        self.RAM_DL_SIZE                = 8*1024
        self.RAM_PAL_SIZE               = 1*1024

        # Register definitions
        self.REG_ID                     = 0x102400
        self.REG_FRAMES                 = 0x102404
        self.REG_CLOCK                  = 0x102408
        self.REG_FREQUENCY              = 0x10240C
        self.REG_SCREENSHOT_EN          = 0x102410
        self.REG_SCREENSHOT_Y           = 0x102414
        self.REG_SCREENSHOT_START       = 0x102418
        self.REG_CPURESET               = 0x10241C
        self.REG_TAP_CRC                = 0x102420
        self.REG_TAP_MASK               = 0x102424
        self.REG_HCYCLE                 = 0x102428
        self.REG_HOFFSET                = 0x10242C
        self.REG_HSIZE                  = 0x102430
        self.REG_HSYNC0                 = 0x102434
        self.REG_HSYNC1                 = 0x102438
        self.REG_VCYCLE                 = 0x10243C
        self.REG_VOFFSET                = 0x102440
        self.REG_VSIZE                  = 0x102444
        self.REG_VSYNC0                 = 0x102448
        self.REG_VSYNC1                 = 0x10244C
        self.REG_DLSWAP                 = 0x102450
        self.REG_ROTATE                 = 0x102454
        self.REG_OUTBITS                = 0x102458
        self.REG_DITHER                 = 0x10245C
        self.REG_SWIZZLE                = 0x102460
        self.REG_CSPREAD                = 0x102464
        self.REG_PCLK_POL               = 0x102468
        self.REG_PCLK                   = 0x10246C
        self.REG_TAG_X                  = 0x102470
        self.REG_TAG_Y                  = 0x102474
        self.REG_TAG                    = 0x102478
        self.REG_VOL_PB                 = 0x10247C
        self.REG_VOL_SOUND              = 0x102480
        self.REG_SOUND                  = 0x102484
        self.REG_PLAY                   = 0x102488
        self.REG_GPIO_DIR               = 0x10248C
        self.REG_GPIO                   = 0x102490
        self.REG_INT_FLAGS              = 0x102498
        self.REG_INT_EN                 = 0x10249C
        self.REG_INT_MASK               = 0x1024A0
        self.REG_PLAYBACK_START         = 0x1024A4
        self.REG_PLAYBACK_LENGTH        = 0x1024A8
        self.REG_PLAYBACK_READPTR       = 0x1024AC
        self.REG_PLAYBACK_FREQ          = 0x1024B0
        self.REG_PLAYBACK_FORMAT        = 0x1024B4
        self.REG_PLAYBACK_LOOP          = 0x1024B8
        self.REG_PLAYBACK_PLAY          = 0x1024BC
        self.REG_PWM_HZ                 = 0x1024C0
        self.REG_PWM_DUTY               = 0x1024C4
        self.REG_MACRO_0                = 0x1024C8
        self.REG_MACRO_1                = 0x1024CC
        self.REG_SCREENSHOT_BUSY        = 0x1024D8
        self.REG_CMD_READ               = 0x1024E4
        self.REG_CMD_WRITE              = 0x1024E8
        self.REG_CMD_DL                 = 0x1024EC
        self.REG_TOUCH_MODE             = 0x1024F0
        self.REG_TOUCH_ADC_MODE         = 0x1024F4
        self.REG_TOUCH_CHARGE           = 0x1024F8
        self.REG_TOUCH_SETTLE           = 0x1024FC
        self.REG_TOUCH_OVERSAMPLE       = 0x102500
        self.REG_TOUCH_RZTHRESH         = 0x102504
        self.REG_TOUCH_RAW_XY           = 0x102508
        self.REG_TOUCH_RZ               = 0x10250C
        self.REG_TOUCH_SCREEN_XY        = 0x102510
        self.REG_TOUCH_TAG_XY           = 0x102514
        self.REG_TOUCH_TAG              = 0x102518
        self.REG_TOUCH_TRANSFORM_A      = 0x10251C
        self.REG_TOUCH_TRANSFORM_B      = 0x102520
        self.REG_TOUCH_TRANSFORM_C      = 0x102524
        self.REG_TOUCH_TRANSFORM_D      = 0x102528
        self.REG_TOUCH_TRANSFORM_E      = 0x10252C
        self.REG_TOUCH_TRANSFORM_F      = 0x102530
        self.REG_SCREENSHOT_READ        = 0x102554
        self.REG_TRIM                   = 0x10256C
        self.REG_TOUCH_DIRECT_XY        = 0x102574
        self.REG_TOUCH_DIRECT_Z1Z2      = 0x102578
        self.REG_TRACKER                = 0x109000

   