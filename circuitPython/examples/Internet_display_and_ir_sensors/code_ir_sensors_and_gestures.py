import sys
if sys.implementation.name == 'circuitpython':
    import bteve as eve_module
else:
    from .lib import bteve as eve_module

eve = eve_module.Brt_PicoEve_Module()  # Default is MM2040 with LCD 1280x800 Goodix touch
spi1 = eve.sd_spis()

eve.set_lcd(eve.LCD_WSVGA_1024X600())
eve.set_touch(eve.TOUCH_CAPACITIVE())
eve.init()

from apps.ir_sensors_and_gestures import ir_sensors_and_gestures
app=ir_sensors_and_gestures(eve)
