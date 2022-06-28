import sys
if sys.implementation.name == "circuitpython":
    from brteve.brt_eve_bt817_8 import BrtEve
else:
    from ..lib.brteve.brt_eve_bt817_8 import BrtEve

def _color_tag(eve, color, tag):
    if(tag):
        eve.Tag(tag)
    if color != -1:
        eve.ColorRGB(color[0], color[1], color[2])

def widgets_point(eve, x, y, radius, color=-1, tag=0):
    if radius < 1:
        return
    _color_tag(eve, color, tag)
    eve.Begin(eve.POINTS)
    eve.PointSize(radius * 2)
    eve.Vertex2f(x, y)

def widgets_box(eve, x, y, w, h, rounded = 2, color = -1, tag=0):
    if w<1 or h< 1:
        return
    _color_tag(eve, color, tag)
    eve.Begin(eve.RECTS)
    eve.LineWidth(rounded)
    eve.Vertex2f(x + rounded / 2, y + rounded / 2)
    eve.Vertex2f(x + w - rounded / 2, y + h - rounded / 2)


