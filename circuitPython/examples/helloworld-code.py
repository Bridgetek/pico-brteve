from brteve.brt_eve_bt817_8 import BrtEve
from brteve.brt_eve_rp2040 import BrtEveRP2040

host = BrtEveRP2040()
eve = BrtEve(host)
eve.init(resolution="1280x800", touch="goodix")

eve.ClearColorRGB(0x20, 0x40, 0x20)
eve.Clear()
eve.cmd_text(eve.lcd_width // 2, eve.lcd_height // 2, 31, eve.OPT_CENTER, "Hello world")
eve.swap()
