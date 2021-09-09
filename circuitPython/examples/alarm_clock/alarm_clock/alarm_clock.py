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
    from ..lib.brteve.brt_eve_bt817_8 import BrtEve

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

        self.enabling_icon = tag_alarm_clock_menu_1
        while 1: 
            self.loop()

    def get_event(self):
        eve = self.eve
        self.gesture.renew(eve)

        tag = self.gesture.get().tagPressed
        # print("tag", tag, self.gesture.get().tagReleased, self.gesture.get().tagPressed)
        
        if tag in (tag_alarm_clock_menu_1, tag_alarm_clock_menu_2, tag_alarm_clock_menu_3, tag_alarm_clock_menu_4):
            self.enabling_icon = tag
        
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
        if self.enabling_icon == tag_alarm_clock_menu_1:
            img = 'menu_icon_006'
        self.layout.draw_asset(tag_alarm_clock_menu_1 , img , self.layout.APP_X + self.layout.MENU_W / 2 - 64 / 2 + 3, y:=y+ 40 + 64)        
        self.eve.cmd_text(self.layout.APP_X + self.layout.MENU_W/2, y + 55, 22, eve.OPT_CENTERX, "Alarm");

        img='menu_icon_005'
        if self.enabling_icon == tag_alarm_clock_menu_2:
            img = 'menu_icon_004'
        self.layout.draw_asset(tag_alarm_clock_menu_2, img, self.layout.APP_X + self.layout.MENU_W / 2 - 64 / 2 + 3, y:=y+ 40 + 64)
        self.eve.cmd_text(self.layout.APP_X + self.layout.MENU_W/2, y + 55, 22, eve.OPT_CENTERX, "Clock");
        
        img='menu_icon_003'
        if self.enabling_icon == tag_alarm_clock_menu_3:
            img = 'menu_icon_002'
        self.layout.draw_asset(tag_alarm_clock_menu_3, img, self.layout.APP_X + self.layout.MENU_W / 2 - 72 / 2 + 3, y:=y+ 40 + 64)
        self.eve.cmd_text(self.layout.APP_X + self.layout.MENU_W/2, y + 60, 22, eve.OPT_CENTERX, "Timer");
        
        img='menu_icon_001'
        if self.enabling_icon == tag_alarm_clock_menu_4:
            img = 'menu_icon_000'
        self.layout.draw_asset(tag_alarm_clock_menu_4, img, self.layout.APP_X + self.layout.MENU_W / 2 - 64 / 2 + 3, y:=y+ 40 + 64)
        self.eve.cmd_text(self.layout.APP_X + self.layout.MENU_W/2, y + 55, 22, eve.OPT_CENTERX, "Bedtime");

        yyyy, mt, dd, hh, mm, ss, ms, weekday, weeks, weekl, mons, monl = \
            yyyy_mt_dd_hh_mm_ss_ms_weekday_weeks_weekl_mons_monl(now())
        timestr = str(dd)+' '+str(mons) + ',' + str(yyyy) + ' ' + str(hh) + ':' + str(mm) + ':' + str(ss)
        self.eve.cmd_text(self.layout.APP_X + self.layout.MENU_W + 5, self.layout.APP_H - 15, 22, 0, timestr)
    
    def loop(self):
        self.eve.cmd_dlstart()
        self.eve.ClearColorRGB(255, 255, 255)
        self.eve.Clear(1, 1, 1)
        self.eve.VertexFormat(3)

        if self.layout.portrait:
            self.eve.cmd_setrotate(2)
        
        ev = self.get_event()
        self.template()
        self.ui_active.draw()

        self.eve.Display()
        self.eve.cmd_swap()
        self.eve.flush()


        



    

    