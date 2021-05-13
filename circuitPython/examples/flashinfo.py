import sys
import bteve as eve

if sys.implementation.name == "circuitpython":
    gd = eve.Brt_PicoEve_Module()  # Default is MM2040 with LCD 1280x800 capacity touch
else:
    from spidriver import SPIDriver
    gd = eve.Gameduino(SPIDriver(sys.argv[1]))
gd.init()

gd.Clear()
gd.ScissorSize(480, 272)
gd.ClearColorRGB(0x00, 0x20, 0x40)
gd.Clear()

gd.cmd_flashdetach()

def showbytes(y, pre, bb):
    msg = pre + ":   " + ":".join(["%02X" % x for x in bb])
    print(msg)
    gd.cmd_text(10, y, 28, 0, msg)

gd.cmd_flashspidesel()
gd.cmd_flashspitx(bytes([0x9f]))
gd.cmd_flashspirx(0, 3)
gd.finish()
showbytes(40, "Flash ID", gd.rd(0, 3))

gd.cmd_flashspidesel()
gd.cmd_flashspitx(bytes([0x4b]))
gd.cmd_flashspirx(0, 4 + 8)
gd.finish()
showbytes(80, "Unique ID", gd.rd(4, 8))

scale = 12
gd.cmd_setbitmap(4, gd.L1, 8, 8)
gd.BitmapSize(gd.NEAREST, gd.BORDER, gd.BORDER, 8 * scale, 8 * scale)
gd.BlendFunc(gd.SRC_ALPHA, 0)
gd.cmd_scale(scale, scale)
gd.cmd_setmatrix()
gd.Vertex2f(10, 120)
gd.swap()