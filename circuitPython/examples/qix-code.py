import math
import random
from brteve.brt_eve_bt817_8 import BrtEve
from brteve.brt_eve_rp2040 import BrtEveRP2040

host = BrtEveRP2040()
eve = BrtEve(host)
eve.init(resolution="1280x800", touch="goodix")

def tri(triangle):
    # triangle wave
    f = math.fmod(triangle, 1.0)
    return 2 * abs(f - 0.5)

def at(triangle, ff):
    x = eve.lcd_width * tri(ff[0] + ff[1] * triangle)
    y = eve.lcd_height * tri(ff[2] + ff[3] * triangle)
    eve.Vertex2f(x, y)

F = [random.random() for i in range(18)]

t = 0

while True:
    eve.Clear()
    eve.VertexFormat(3)
    eve.Begin(eve.LINE_STRIP)
    for dt in range(10):
        tn = (t - dt) / 75
        eve.ColorRGB(
            int(255 * tri(F[12] + F[13] * tn)),
            int(255 * tri(F[14] + F[15] * tn)),
            int(255 * tri(F[16] + F[17] * tn)))
        at(tn, F[0:4])
        at(tn, F[4:8])
        at(tn, F[8:12])

    eve.swap()
    t += 1
