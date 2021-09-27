import sys
from brteve.brt_eve_common import f16

if sys.implementation.name == "circuitpython":
    from brteve.brt_eve_bt817_8 import BrtEve
    from brteve.brt_eve_rp2040 import BrtEveRP2040

    host = BrtEveRP2040()
    eve = BrtEve(host)
    eve.init(resolution="1280x800", touch="goodix")
    gd = eve
else:
    import bteve as eve
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
showbytes(40, "Flash ID", gd.transfer_read(0, 3))

gd.cmd_flashspidesel()
gd.cmd_flashspitx(bytes([0x4b]))
gd.cmd_flashspirx(0, 4 + 8)
gd.finish()
showbytes(80, "Unique ID", gd.transfer_read(4, 8))

scale = 12
gd.cmd_setbitmap(4, gd.L1, 8, 8)
gd.BitmapSize(gd.NEAREST, gd.BORDER, gd.BORDER, 8 * scale, 8 * scale)
gd.BlendFunc(gd.SRC_ALPHA, 0)
gd.cmd_scale(f16(scale), f16(scale))
gd.cmd_setmatrix()
gd.Vertex2f(10, 120)
gd.swap()
