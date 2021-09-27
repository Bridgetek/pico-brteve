import time

from .helper import helper
from .gesture import gesture
from .datetime import hh_mm, milis, now_hh_mm, now_hh_mm_ss, print_weekday, random, set_timnezone
from .layout import layout
from .tags import *
from . import datetime
from .scroller import scroller
from .timezone import timezone
from .dimension2d import clock_hand

import sys
if sys.implementation.name == "circuitpython":
    from brteve.brt_eve_bt817_8 import BrtEve
else:
    from ....lib.brteve.brt_eve_bt817_8 import BrtEve

class ui_world_clock():
    def __init__(self, eve: BrtEve, helper: helper, gesture: gesture, layout: layout):
        self.eve = eve
        self.helper = helper
        self.gesture = gesture
        self.layout = layout
        self.scroller = scroller()
        self.scroller.set_friction(0.99)
        self.scroller.set_speed(0.2)

        self.utc_hh=0
        self.utc_mm=0
        self.utc_id=76 # SG time
        self.utc_hh_init=-8 #SG time

        self.is_changing_utc = 0
        self.utc_id_candidate = 0

    def utc_time(self, utc_id):
        timestr = timezone[utc_id]
        hh = (int)(timestr[4:7]) + self.utc_hh_init
        mm = (int)(timestr[8:10])
        name = timestr
        return hh, mm, name

    def event(self):
        gesture = self.gesture
        eve = self.eve

        if self.gesture.get().isSwipe:
            self.is_changing_utc = 1
            return

        if gesture.get().tagReleased == tag_ui_clock_cancel:
            self.is_changing_utc = 0

        if gesture.get().tagReleased == tag_ui_clock_set:
            self.utc_id = self.utc_id_candidate
            hh, mm, _ = self.utc_time(self.utc_id)
            set_timnezone(hh, mm)
            self.is_changing_utc = 0

    def selecting_utc(self, text_y):
        eve = self.eve
        layout = self.layout
        range_ = 5
        distance_y = 40
        w = layout.APP_W * 8 / 10
        h = range_ * 2 * distance_y
        num_timezone = len(timezone)

        self.offset_y, veloc = self.scroller.get_offset_velocity(self.gesture.get().touchY)
        scroller_max = num_timezone * distance_y
        self.scroller.set_limit(scroller_max, -scroller_max)

        ylimit_top = self.utc_id * distance_y
        ylimit_bottom = -(num_timezone - self.utc_id) * distance_y

        if self.offset_y > ylimit_top:
            self.offset_y = ylimit_top - 1
            self.scroller.set_offset_vloc(self.offset_y, 0)
        elif self.offset_y < ylimit_bottom:
            self.offset_y = ylimit_bottom + 1
            self.scroller.set_offset_vloc(self.offset_y, 0)

        font_h_28 = 0
        offset_y = self.offset_y % (distance_y + font_h_28)
        self.utc_id_candidate = self.utc_id - round(self.offset_y / distance_y + 0.5) # + 0.5 = roundup
        utc_start = max(0, self.utc_id_candidate - range_)
        utc_end   = min(num_timezone, self.utc_id_candidate + range_)

        # boundary
        rounded = 60
        eve.ColorRGB(0x2D, 0x2F, 0x41)
        eve.ColorA(210)
        eve.Begin(eve.RECTS)
        eve.LineWidth(rounded)
        eve.Vertex2f(self.layout.APP_X + rounded / 2, self.layout.APP_Y + rounded / 2)
        eve.Vertex2f(self.layout.APP_X + self.layout.APP_W - rounded / 2, self.layout.APP_H + self.layout.APP_Y - rounded / 2)

        # utc list
        x = layout.APP_X + layout.APP_W / 2 - w/2
        y = layout.APP_Y + layout.APP_H / 2 - h/2
        y = text_y - range_ * distance_y + offset_y

        # highlight selecting one
        eve.ColorRGB(0x85, 0x82, 0x82)
        eve.ColorA(255)
        eve.Begin(eve.RECTS)
        eve.LineWidth(10)
        font_h = 15
        eve.Vertex2f(x, text_y + distance_y / 3)
        eve.Vertex2f(x + w, text_y + distance_y + font_h)

        eve.ScissorXY((int)(x), (int)(y))
        eve.ScissorSize((int)(w), (int)(h))
        for i in range(self.utc_id_candidate - range_, self.utc_id_candidate + range_):
            if i not in range(utc_start,utc_end):
                y += distance_y
                continue
            eve.ColorA(120)
            eve.ColorRGB(0xff, 0xff, 0xff)
            utc = timezone[i]
            font_no=28
            # highlight selecting one
            if i == self.utc_id_candidate:
                eve.ColorA(255)
                eve.ColorRGB(0x19, 0x44, 0x6C)
                font_no=29
            # other UTC in range
            eve.cmd_text(x, y, font_no, 0, utc)
            y += distance_y

        # reset scissor
        eve.ScissorXY(0, 0)
        eve.ScissorSize(2048, 2048)

        # set button
        btn_w = 150
        btn_h = 40
        padding = 30
        btn_y = layout.APP_Y + layout.APP_H - 30 - btn_h
        eve.Tag(tag_ui_clock_set)
        eve.cmd_button(layout.APP_X + padding, btn_y, btn_w, btn_h, 28, 0, "Set timezone")
        eve.Tag(tag_ui_clock_cancel)
        eve.cmd_button(layout.APP_X + layout.APP_W - padding - btn_w, btn_y, btn_w, btn_h, 28, 0, "Cancel")

    def interrupt(self):
        return 0

    def draw(self):
        eve = self.eve
        layout = self.layout

        eve.ColorRGB(255, 255, 255)
        title="World Clock"
        x = (self.layout.APP_X + self.layout.MENU_W) / 2 + (self.layout.APP_X + self.layout.APP_W) / 2
        eve.cmd_text(x, 20, 29, eve.OPT_CENTERX, title)

        x = self.layout.APP_X + self.layout.MENU_W / 2 + self.layout.APP_W / 2 - 264 / 2
        y = self.layout.APP_Y + self.layout.APP_H / 2  - 264
        self.layout.draw_asset(
            0 , 'clockface' , x, y)

        utc_id = self.utc_id
        if self.is_changing_utc:
            utc_id = self.utc_id_candidate
        self.utc_hh, self.utc_mm, utc_name = self.utc_time(utc_id)
        hh, mm, ss = now_hh_mm_ss()

        radius_bottom = 264 / 2
        radius_top = 20
        centerx = x + 264 / 2
        centery = y + 264 / 2
        gaps = 9

        # hour
        clock_hand(eve, centerx, centery, radius_bottom * 5 / 10, radius_top    , hh % 12, 12, [0xff, 0, 0]       , 8)
        clock_hand(eve, centerx, centery, radius_bottom * 5 / 10, radius_top + 2, hh % 12, 12, [0xff, 0xff, 0xff] , 4)

        # minute
        clock_hand(eve, centerx, centery, radius_bottom * 7 / 10, radius_top    , mm     , 60, [0, 0xff, 0]       , 4)

        # second
        clock_hand(eve, centerx, centery, radius_bottom * 8 / 10, radius_top    , ss     , 60, [0, 0, 0]          , 2)

        self.event()
        if self.is_changing_utc:
            self.selecting_utc( y + 264 + 30) # 264 = clock dimension, 30 = gaps
        else:
            eve.cmd_fillwidth(layout.APP_X + layout.APP_W - x - 20)
            eve.ColorRGB(255, 255, 255)
            eve.cmd_text(layout.screen_w / 2 + layout.MENU_W / 2, y + 264 + 30, 28, eve.OPT_FILL | eve.OPT_CENTERX, utc_name.strip())

        time.sleep(0.01)