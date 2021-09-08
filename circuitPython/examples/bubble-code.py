""" Buble example for Raspbery Pico RP2040 with EVE module MM817EV from BridgeTek and LCD 800x480"""
import random
import math
from brteve.brt_eve_bt817_8 import BrtEve
from brteve.brt_eve_rp2040 import BrtEveRP2040

host = BrtEveRP2040()
eve = BrtEve(host)
eve.init(resolution="1280x800", touch="goodix")

random.seed(6)
rr = random.randrange

# Via https://sashamaps.net/docs/resources/20-colors/
colors20 = [
    (230, 25, 75), (60, 180, 75), (255, 225, 25),
    (0, 130, 200), (245, 130, 48), (145, 30, 180),
    (70, 240, 240), (240, 50, 230), (210, 245, 60),
    (250, 190, 212), (0, 128, 128), (220, 190, 255),
    (170, 110, 40), (255, 250, 200), (128, 0, 0),
    (170, 255, 195), (128, 128, 0), (255, 215, 180),
    (0, 0, 128), (128, 128, 128), (255, 255, 255),
    (0, 0, 0)
]

ths = [2 * math.pi * i / 1024 for i in range(1024)]
rots = [(math.sin(th), math.cos(th)) for th in ths]

class bubble:
    def __init__(self):
        self.x = rr(eve.lcd_width)
        self.y = rr(eve.lcd_height)
        self.rgb = random.choice(colors20)
        self.r = rr(50, eve.lcd_width // 4)
        self.a = rr(1024)
        self.w = rr(-100, 100) / 10

    def draw(self, num):
        eve.ColorRGB(*self.rgb)
        eve.PointSize(self.r)
        (dx, dy) = rots[int(self.a + num * self.w) & 1023]
        eve.Vertex2f(self.x + self.r * dx, self.y + self.r * dy)

bubbles = [bubble() for i in range(90)]

for i in range(999999):
    eve.Clear()                              # black background
    eve.VertexFormat(4)                      # full-screen
    eve.ColorA(192)                          # 75% opacity
    eve.Begin(eve.POINTS)                    # large POINTS are circles
    for b in bubbles:
        b.draw(i)
    eve.swap()
