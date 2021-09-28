import time

from .helper import helper
from .gesture import gesture
from .datetime import hh_mm, hh_mm_ss_ms, milis, now, print_weekday, random
from .layout import layout
from .tags import *
from . import datetime
from .scroller import scroller
from .dimension2d import polar_xy, clock_hand
from .widgets import widgets_box, widgets_point

import sys
if sys.implementation.name == "circuitpython":
    from brteve.brt_eve_bt817_8 import BrtEve
else:
    from ....lib.brteve.brt_eve_bt817_8 import BrtEve

class ui_stopwatch():
    def __init__(self, eve: BrtEve, helper: helper, gesture: gesture, layout: layout):
        self.eve = eve
        self.helper = helper
        self.gesture = gesture
        self.layout = layout
        self.milis_start = 0
        self.milis_stop = 0
        self.running = 0
        self.pause = 0
        self.split = []

    def timestr(self):
        helper = self.helper
        duration = self.milis_stop - self.milis_start
        ms = duration % 1000
        ss_num = duration / 1000
        ss = round(ss_num) % 60
        ss_float = round(ss_num, 5) % 60
        mm_num = ss_num / 60
        mm = round(mm_num - 0.5) % 60 # -0.5 =  rounddown
        mm_float = round(mm_num - 0.5, 5) % 60
        hh = round(mm_num / 60 - 0.5) # -0.5 =  rounddown

        # limit the timer to 30 hours
        if hh > 30:
            hh = 30
            mm = mm_float = 60

        s = helper.zfill(str(hh), 2) + ':' + \
              helper.zfill(str(mm), 2) + ':' + \
              helper.zfill(str(ss), 2) + '.' + \
              helper.zfill(str(ms), 3)

        return s, hh, mm_float, ss_float, ms

    def event(self):
        eve = self.eve
        layout = self.layout
        ges = self.gesture

        tag = ges.get().tagReleased
        if tag == tag_ui_timer_reset:
            self.milis_start = 0
            self.milis_stop = 0
            self.running = 0
            self.pause = 0
            self.split = []
        elif tag == tag_ui_timer_start:
            if self.running == 1:
                return
            self.running = 1
            self.milis_start = milis()
            self.milis_stop = self.milis_start
        elif tag == tag_ui_timer_pause:
            if self.pause == 0:
                self.pause = 1
            else:
                self.pause = 0
            return
        elif tag == tag_ui_timer_split:
            s, hh, mm, ss, ms = self.timestr()
            max_list = 5
            if len(self.split) == max_list:
                self.split = self.split[1:max_list]
            self.split.append(s)
        else:
            if self.running == 1 and self.pause == 0:
                self.milis_stop = milis()
    def interrupt(self):
        return 0

    def draw(self):
        eve = self.eve
        layout = self.layout
        helper=self.helper
        watch_w=304
        watch_h=392

        self.event()
        s, hh, mm, ss, ms = self.timestr()

        eve.ColorRGB(255, 255, 255)
        title="Timer"
        x = (self.layout.APP_X + self.layout.MENU_W) / 2 + (self.layout.APP_X + self.layout.APP_W) / 2
        eve.cmd_text(x, 20, 29, eve.OPT_CENTERX, title)

        x = self.layout.APP_X + self.layout.MENU_W / 2 + self.layout.APP_W / 2 - watch_w / 2
        y = self.layout.APP_Y + self.layout.APP_H / 2  - watch_h + 100 # 100 = margin top
        self.layout.draw_asset(0 , 'stopface' , x, y)

        # hour
        centerx = x + watch_w/2 - 2
        centery = y + watch_w / 2 + 27
        radius_bottom = -15
        radius_top = 20
        clock_hand(eve, centerx, centery, radius_bottom * 5 / 10, radius_top    , hh % 12, 30, [0xff, 0, 0]       , 4)
        widgets_point(eve, centerx, centery, 5, [0xff, 0, 0])
        widgets_point(eve, centerx, centery, 2, [0xff, 0xff, 0xff])

        # minute
        centerx = x + watch_w / 2 - 2
        centery = y + watch_w / 2 + 84
        radius_bottom = -30
        radius_top = watch_w / 3 + 5
        clock_hand(eve, centerx, centery, radius_bottom * 7 / 10, radius_top    , mm % 60, 60, [0, 0xff, 0]       , 3)

        # second
        centerx = x + watch_w / 2 - 2
        centery = y + watch_w / 2 + 84
        radius_bottom = -50
        radius_top = watch_w / 3 + 10
        clock_hand(eve, centerx, centery, radius_bottom * 7 / 10, radius_top    , ss % 60, 60, [0, 0, 0]       , 1)

        # center point
        widgets_point(eve, centerx, centery, 10, [0, 0xff, 0])
        widgets_point(eve, centerx, centery, 5, [0xff, 0xff, 0xff])

        # digital timer
        box_w = watch_h / 3
        box_h = 30
        box_y = centery + 20
        box_x = centerx - box_w / 2
        widgets_box(eve, box_x, box_y, box_w, box_h, 20, [0x00, 0xff, 0xff])
        eve.ColorRGB(0, 0, 0)
        eve.cmd_text(box_x + box_w / 2, box_y + box_h / 2, 28, eve.OPT_CENTER, s)

        ymargin = 30
        y += watch_h + ymargin
        btn_w = 60
        btn_h = 30
        xmargin = (layout.APP_W - layout.MENU_W - btn_w*4) / 5
        center = layout.screen_w / 2
        x1 = center + layout.MENU_W/2 - btn_w - btn_w - xmargin - xmargin / 2
        x2 = x1 + btn_w + xmargin
        x3 = x2 + btn_w + xmargin
        x4 = x3 + btn_w + xmargin

        eve.ColorRGB(0xff, 0xff, 0xff)
        eve.Tag(tag_ui_timer_reset)
        eve.cmd_button(x1, y, btn_w, btn_h, 23, 0, "Reset")
        eve.Tag(tag_ui_timer_pause)
        eve.cmd_button(x2, y, btn_w, btn_h, 23, 0, "Pause" if self.pause == 0 else "Resume")
        eve.Tag(tag_ui_timer_start)
        eve.cmd_button(x3, y, btn_w, btn_h, 23, 0, "Start")
        eve.Tag(tag_ui_timer_split)
        eve.cmd_button(x4, y, btn_w, btn_h, 23, 0, "Split")

        # split log
        ymargin = 10
        box_y = y + btn_h + ymargin
        box_w = layout.APP_W - layout.MENU_W - 20
        ymargin2 = 35
        box_h = layout.APP_Y + layout.APP_H - ymargin2 - box_y
        box_x = layout.screen_w / 2 + layout.MENU_W/2 - box_w / 2
        widgets_box(eve, box_x, box_y, box_w, box_h, 20, [0xff, 0xff, 0xff])
        time.sleep(0.01)

        x = box_x + 5
        y = box_y + 5
        distance = 30
        eve.ColorRGB(0, 0, 0)
        for i in self.split:
            eve.cmd_text(x, y, 28, 0, i)
            y+=distance