import os
import sys
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "..")))
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "../lib")))

from lib.brteve.brt_eve_bt817_8 import BrtEve
from lib.brteve.brt_eve_telemetrix import BrtEveTelemetrix

host = BrtEveTelemetrix()
eve = BrtEve(host)
eve.init(resolution="1280x800", touch="goodix")

eve.ClearColorRGB(0x20, 0x40, 0x20)
eve.Clear()
eve.cmd_text(eve.lcd_width // 2, eve.lcd_height // 2, 31, eve.OPT_CENTER, "Hello world")
eve.swap()
