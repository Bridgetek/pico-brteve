class Mcu_BridgeTek_General():
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
