from alarm_clock.layout import layout
from alarm_clock.helper import helper
from alarm_clock.gesture import gesture
import time

from .ui_world_clock import ui_world_clock
from .ui_stopwatch import ui_stopwatch
from .ui_bedtime import ui_bedtime
from .ui_alarm import ui_alarm
from .tags import *
from .datetime import init_datetime, yyyy_mt_dd_hh_mm_ss_ms_weekday_weeks_weekl_mons_monl, now

import sys
if sys.implementation.name == "circuitpython":
    from brteve.brt_eve_bt817_8 import BrtEve
else:
    from ....lib.brteve.brt_eve_bt817_8 import BrtEve

# controler class
class alarm_clock():
    def __init__(self, eve: BrtEve) -> None:
        self.eve = eve
        self.helper = helper(eve)
        self.gesture = gesture()
        self.layout = layout(eve, self.helper)

        self.ui_alarm         = ui_alarm(eve, self.helper, self.gesture, self.layout)
        self.ui_bedtime       = ui_bedtime(eve, self.helper, self.gesture, self.layout)
        self.ui_stopwatch     = ui_stopwatch(eve, self.helper, self.gesture, self.layout)
        self.ui_world_clock   = ui_world_clock(eve, self.helper, self.gesture, self.layout)

        # date and time
        milis_offset = 1630999128 * 1000 # get this from internet
        init_datetime(milis_offset)

        self.ui_active = self.ui_alarm
        self.count_3touch = 0
        self.last_x = 0

        while 1:
            self.loop()

    def flip_screen_3touch(self):
        eve = self.eve
        x = self.gesture.get().touchX
        radius = 10

        if not self.gesture.get().isTouch:
            return

        if abs(x-self.last_x) <= radius:
            self.count_3touch += 1
        else:
            self.count_3touch = 0
            self.last_x = 0
            return

        if self.count_3touch >= 3:
            self.layout.flip()
            self.count_3touch = 0

        self.last_x = x

    def get_event(self):
        eve = self.eve
        self.gesture.renew(eve)
        #self.flip_screen_3touch()

        tag = self.gesture.get().tagPressed
        # print("tag", tag, self.gesture.get().tagReleased, self.gesture.get().tagPressed)

        if tag == tag_alarm_clock_menu_1:
            self.ui_active = self.ui_alarm
        elif tag == tag_alarm_clock_menu_2:
            self.ui_active = self.ui_world_clock
        elif tag == tag_alarm_clock_menu_3:
            self.ui_active = self.ui_stopwatch
        elif tag == tag_alarm_clock_menu_4:
            self.ui_active = self.ui_bedtime

        return 0

    def template(self):
        eve=self.eve
        helper = self.helper

        self.layout.draw_asset(0, 'background', 0, 0)

        # boundary
        rounded = 60
        eve.ColorRGB(0x2D, 0x2F, 0x41)
        eve.Begin(eve.RECTS)
        eve.LineWidth(rounded)
        eve.Vertex2f(self.layout.APP_X + rounded / 2, self.layout.APP_Y + rounded / 2)
        eve.Vertex2f(self.layout.APP_X + self.layout.APP_W - rounded / 2, self.layout.APP_H + self.layout.APP_Y - rounded / 2)

        # divider
        eve.ColorRGB(0x77, 0x77, 0x77)
        eve.Begin(eve.LINES)
        eve.LineWidth(1)
        eve.Vertex2f(self.layout.APP_X + self.layout.MENU_W, self.layout.APP_Y)
        eve.Vertex2f(self.layout.APP_X + self.layout.MENU_W, self.layout.APP_H + self.layout.APP_Y)

        y=self.layout.APP_Y + 100

        img='menu_icon_007'
        if self.ui_active == self.ui_alarm:
            img = 'menu_icon_006'
        y=y+ 40 + 64
        self.layout.draw_asset(tag_alarm_clock_menu_1 , img , self.layout.APP_X + self.layout.MENU_W / 2 - 64 / 2 + 3, y)
        self.eve.cmd_text(self.layout.APP_X + self.layout.MENU_W/2, y + 55, 22, eve.OPT_CENTERX, "Alarm");

        img='menu_icon_005'
        if self.ui_active == self.ui_world_clock:
            img = 'menu_icon_004'
        y=y+ 40 + 64
        self.layout.draw_asset(tag_alarm_clock_menu_2, img, self.layout.APP_X + self.layout.MENU_W / 2 - 64 / 2 + 3, y)
        self.eve.cmd_text(self.layout.APP_X + self.layout.MENU_W/2, y + 55, 22, eve.OPT_CENTERX, "Clock");

        img='menu_icon_003'
        if self.ui_active == self.ui_stopwatch:
            img = 'menu_icon_002'
        y=y+ 40 + 64
        self.layout.draw_asset(tag_alarm_clock_menu_3, img, self.layout.APP_X + self.layout.MENU_W / 2 - 72 / 2 + 3, y)
        self.eve.cmd_text(self.layout.APP_X + self.layout.MENU_W/2, y + 60, 22, eve.OPT_CENTERX, "Timer");

        img='menu_icon_001'
        if self.ui_active == self.ui_bedtime:
            img = 'menu_icon_000'
        y=y+ 40 + 64
        self.layout.draw_asset(tag_alarm_clock_menu_4, img, self.layout.APP_X + self.layout.MENU_W / 2 - 64 / 2 + 3, y)
        self.eve.cmd_text(self.layout.APP_X + self.layout.MENU_W/2, y + 55, 22, eve.OPT_CENTERX, "Bedtime");

        yyyy, mt, dd, hh, mm, ss, ms, weekday, weeks, weekl, mons, monl = \
            yyyy_mt_dd_hh_mm_ss_ms_weekday_weeks_weekl_mons_monl(now())
        timestr = str(dd)+' '+str(mons) + ',' + str(yyyy) + ' ' + self.helper.zfill(str(hh), 2) + ':' + self.helper.zfill(str(mm), 2) + ':' + self.helper.zfill(str(ss), 2)
        self.eve.cmd_text(self.layout.APP_X + self.layout.MENU_W + 5, self.layout.APP_H - 15, 22, 0, timestr)

    def loop(self):
        self.eve.cmd_dlstart()
        self.eve.ClearColorRGB(255, 255, 255)
        self.eve.Clear(1, 1, 1)
        self.eve.VertexFormat(3)

        if self.layout.portrait:
            self.eve.cmd_setrotate(2)
        else:
            self.eve.cmd_setrotate(0)

        ev = self.get_event()
        self.template()

        interrupt = 0
        for i in [self.ui_alarm, self.ui_bedtime, self.ui_stopwatch, self.ui_world_clock]:
            if i.interrupt():
                interrupt = 1
                break
        if not interrupt:
            self.ui_active.draw()

        self.eve.Display()
        self.eve.cmd_swap()
        self.eve.flush()








