import random
import bteve as eve

rr = random.randrange

gd = eve.Brt_PicoEve_Module()  # Default is MM2040 with LCD 1280x800 capacity touch
gd.init()

while True:
    gd.VertexFormat(2)
    gd.Clear()
    gd.Begin(gd.POINTS)
    for i in range(100):
        gd.ColorRGB(rr(256), rr(256), rr(256))
        gd.PointSize(rr(gd.w // 6))
        gd.Vertex2f(rr(gd.w), rr(gd.h))
    gd.swap()
