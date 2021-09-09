import time

from .helper import helper
from .gesture import gesture
from .datetime import hh_mm, milis, print_weekday, random
from .layout import layout
from .tags import *
from . import datetime
from .scroller import scroller

import sys
if sys.implementation.name == "circuitpython":
    from brteve.brt_eve_bt817_8 import BrtEve
else:
    from ..lib.brteve.brt_eve_bt817_8 import BrtEve

class ui_world_clock():
    def __init__(self, eve: BrtEve, helper: helper, gesture: gesture, layout: layout):
        self.eve = eve
        self.helper = helper
        self.gesture = gesture
        self.layout = layout

    def draw(self):
        eve = self.eve

        eve.ColorRGB(255, 255, 255)
        title="World Clock"
        x = (self.layout.APP_X + self.layout.MENU_W) / 2 + (self.layout.APP_X + self.layout.APP_W) / 2 
        eve.cmd_text(x, 20, 29, eve.OPT_CENTERX, title)

        time.sleep(0.01)