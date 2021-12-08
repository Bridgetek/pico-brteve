import random
import os
import sys
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "..")))
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "../lib")))

from lib.brteve.brt_eve_bt817_8 import BrtEve
from lib.brteve.brt_eve_telemetrix import BrtEveTelemetrix

host = BrtEveTelemetrix()
eve = BrtEve(host)
eve.init(resolution="1280x800", touch="goodix")

rr = random.randrange
while True:
    eve.VertexFormat(2)
    eve.Clear()
    eve.Begin(eve.POINTS)
    for i in range(100):
        eve.ColorRGB(rr(256), rr(256), rr(256))
        eve.PointSize(rr(eve.lcd_width // 6))
        eve.Vertex2f(rr(eve.lcd_width), rr(eve.lcd_height))
    eve.swap()
    #TODO: check and compare with old version
