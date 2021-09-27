import math

def polar_xy(radius, degree, ox, oy):
    degree = math.radians(degree)
    x = ox + radius * math.sin(degree)
    y = oy + radius * math.cos(degree)
    return x, y

def clock_hand(eve, centerx, centery, radius_bottom, radius_top, value, max_value, RGB, border):
    deg = -(int)(value * 360        / max_value + 180) % 360
    x1, y1 = polar_xy(radius_bottom        , deg, centerx, centery)
    x2, y2 = polar_xy(radius_top , deg, centerx, centery)

    eve.Begin(eve.LINES)
    eve.LineWidth(border)
    eve.ColorRGB(RGB[0], RGB[1], RGB[2])
    eve.Vertex2f(x1, y1)
    eve.Vertex2f(x2, y2)

    return x1, y1, x2, y2