import random
import bteve as eve

rr = random.randrange

gd = eve.ME815A_WH70C_Pico()
gd.init()
gd.setup_800_480()

while True:
    gd.VertexFormat(2)
    gd.Clear()
    gd.Begin(eve.POINTS)
    for i in range(100):
        gd.ColorRGB(rr(256), rr(256), rr(256))
        gd.PointSize(rr(gd.w // 6))
        gd.Vertex2f(rr(gd.w), rr(gd.h))
    gd.swap()
