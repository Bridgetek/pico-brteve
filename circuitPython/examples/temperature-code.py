import microcontroller
from brteve.brt_eve_bt817_8 import BrtEve
from brteve.brt_eve_rp2040 import BrtEveRP2040

def lerp(t, a, b):
    return a + (b - a) * t

def temp_map(x, x0, x1, y0 = 0, y1 = 1):
    t = (x - x0) / (x1 - x0)
    t = max(0, min(t, 1))
    return lerp(t, y0, y1)

def temperature(eve_obj, sense):
    eve_obj.cmd_romfont(30, 33)
    eve_obj.cmd_romfont(31, 34)

    xincr = .008 * eve_obj.lcd_width
    sparkline = []
    temp = sense()
    frames = 0
    while True:
        # Measure temp continuously until CPU is ready to redraw
        while not eve_obj.is_finished():
            temp = (999 * temp + sense()) / 1000

        eve_obj.cmd_gradient(0, 0, 0x000010, eve_obj.lcd_width, eve_obj.lcd_height, 0x206060)
        eve_obj.VertexFormat(3)
        eve_obj.cmd_text(eve_obj.lcd_width // 2, eve_obj.lcd_height // 9, 31, eve_obj.OPT_CENTER,
            "CPU junction temperature")

        # Update sparkline every few frames
        if (frames % 6) == 0:
            y = temp_map(temp, 0, 50, 0.9 * eve_obj.lcd_height, 0.2 * eve_obj.lcd_height)
            sparkline.append((eve_obj.lcd_width * 0.8, y))
            sparkline = sparkline[-90:]
            sparkline = [(x - xincr, y) for (x, y) in sparkline]
        frames += 1

        # Draw dot and measurement
        (x, y) = sparkline[-1]
        eve_obj.cmd_text(int(x), int(y), 30, eve_obj.OPT_CENTERY, " %.1f C" % temp)
        eve_obj.PointSize(20)
        eve_obj.Begin(eve_obj.POINTS)
        eve_obj.Vertex2f(x, y)

        eve_obj.LineWidth(eve_obj.lcd_width / 200)
        eve_obj.Begin(eve_obj.LINE_STRIP)
        for (x, y) in sparkline:
            eve_obj.Vertex2f(x, y)
        eve_obj.swap()

def celsius():
    return microcontroller.cpu.temperature

host = BrtEveRP2040()
eve = BrtEve(host)
eve.init(resolution="1280x800", touch="goodix")

temperature(eve, celsius)
