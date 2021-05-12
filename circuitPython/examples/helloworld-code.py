import sys
import bteve as eve

gd = eve.Brt_PicoEve_Module()  # Default is MM2040 with LCD 1280x800 capacity touch
gd.init()

gd.ClearColorRGB(0x20, 0x40, 0x20)
gd.Clear()
gd.cmd_text(gd.w // 2, gd.h // 2, 31, gd.OPT_CENTER, "Hello world")
gd.swap()
