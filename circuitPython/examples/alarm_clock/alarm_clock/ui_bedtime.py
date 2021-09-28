import time

from .helper import helper
from .gesture import gesture
from .datetime import hh_mm, hh_mm_ss_ms, milis, now, now_hh_mm, now_hh_mm_ss, print_weekday, random
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

class ui_bedtime():
    def __init__(self, eve: BrtEve, helper: helper, gesture: gesture, layout: layout):
        self.eve = eve
        self.helper = helper
        self.gesture = gesture
        self.layout = layout

        self.tag_hh_selected  = 0
        self.tag_mm_selected  = 0

        self.hh=22
        self.mm=00
        self.hh_clone=self.hh
        self.mm_clone=self.mm
        self.time_changing = 0

        self.tag_hh2_selected  = 0
        self.tag_mm2_selected  = 0
        self.hh2=7
        self.mm2=00
        self.hh2_clone=self.hh2
        self.mm2_clone=self.mm2
        self.time2_changing = 0

        self.dim_inactive_duration = 10*1000
        self.last_touch_milis = milis() - self.dim_inactive_duration

    def event(self):
        gesture = self.gesture
        track = gesture.get().tagTrackTouched

        # get new clock
        track = gesture.get().tagTrackTouched
        tag = track & 0xFF
        angel = track >> 16
        if tag == tag_ui_alarm_rotate:
            hh = round(angel * 12 / 65535  + 6 ) % 12   # +6 because 0 degree = 6th hour
            mm = round(angel * 60 / 65535  + 30) % 60 # +30 because 0 degree = 30minute

            #default is hhh selected is none
            if self.time_changing == 0 and self.time2_changing == 0:
                self.time_changing = 1
                self.time2_changing = 0
                self.tag_hh_selected = 1
                self.tag_mm_selected  = 0
                self.hh_clone = self.hh

            if  self.time_changing == 1 and self.tag_hh_selected == 1:
                pm_time = 0
                if self.hh_clone == 11 and hh == 0:
                    pm_time = 12
                elif self.hh_clone == 23 and hh == 0:
                    pm_time = 0
                elif self.hh_clone >= 12:
                    pm_time = 12

                self.hh_clone = pm_time + hh
            elif self.time2_changing == 1 and self.tag_hh2_selected == 1:
                pm_time = 0
                if self.hh2_clone == 11 and hh == 0:
                    pm_time = 12
                elif self.hh2_clone == 23 and hh == 0:
                    pm_time = 0
                elif self.hh2_clone >= 12:
                    pm_time = 12

                self.hh2_clone = pm_time + hh
            elif self.time_changing == 1 and self.tag_mm_selected == 1:
                self.mm_clone = mm
            elif self.time2_changing == 1 and self.tag_mm2_selected == 1:
                self.mm2_clone = mm

        tag = gesture.get().tagReleased
        if tag == tag_ui_alarm_hh:
            self.time_changing = 1
            self.time2_changing = 0
            self.tag_hh_selected  = 1
            self.tag_mm_selected  = 0
        elif tag == tag_ui_alarm_mm:
            self.time_changing = 1
            self.time2_changing = 0
            self.tag_mm_selected  = 1
            self.tag_hh_selected  = 0
        elif tag == tag_ui_alarm_hh2:
            self.time_changing = 0
            self.time2_changing = 1
            self.tag_hh2_selected  = 1
            self.tag_mm2_selected  = 0
        elif tag == tag_ui_alarm_mm2:
            self.time_changing = 0
            self.time2_changing = 1
            self.tag_mm2_selected  = 1
            self.tag_hh2_selected  = 0
        elif tag == tag_ui_clock_set:
            self.time_changing = 0
            self.time2_changing = 0

            self.hh =self.hh_clone
            self.mm =self.mm_clone
            self.hh2 =self.hh2_clone
            self.mm2 =self.mm2_clone

        elif tag == tag_ui_clock_cancel:
            self.time_changing = 0
            self.time2_changing = 0

            self.hh_clone =self.hh
            self.mm_clone =self.mm
            self.hh2_clone =self.hh2
            self.mm2_clone =self.mm2

    def interrupt(self):
        eve = self.eve
        layout = self.layout

        # exit dim mode
        if self.gesture.get().isTouch:
            self.last_touch_milis = milis()

        dim_level = 255
        now = milis()
        if now - self.last_touch_milis < self.dim_inactive_duration:
            return 0

        hh, mm, ss = now_hh_mm_ss()
        if (hh > self.hh or (hh == self.hh and mm >= self.mm))  and (hh < self.hh2 or (hh == self.hh2 and mm >= self.mm2)):
            # dim screen
            x = 0
            y = 0

            w = eve.lcd_width
            h = eve.lcd_height
            if layout.portrait == 1:
                h = eve.lcd_width
                w = eve.lcd_height

            eve.ColorA((int)(dim_level))
            eve.ColorRGB(0, 0, 0)
            eve.Begin(eve.RECTS)
            eve.LineWidth(16)
            eve.Vertex2f(x, y)
            eve.Vertex2f(x + w, y + h)

            # clock
            eve.ColorA(50)
            hh, mm, ss = now_hh_mm_ss()
            clock_size = h / 4
            x = w / 2
            y = h / 2
            eve.cmd_clock(x, y, clock_size, 0, hh, mm, ss, 0)

            eve.ColorRGB(0xff, 0xff, 0xff)
            eve.cmd_text(x, h - 20, 25, eve.OPT_CENTER, "Sleeping time")
            return 1
        return 0

    def draw(self):
        eve = self.eve
        layout = self.layout
        helper = self.helper
        gesture = self.gesture

        self.event()

        hh = self.hh_clone
        mm = self.mm_clone
        time_changed = hh != self.hh or mm != self.mm

        hh2 = self.hh2_clone
        mm2 = self.mm2_clone
        time2_changed = hh2 != self.hh2 or mm2 != self.mm2

        eve.ColorRGB(255, 255, 255)
        title="Bed time"
        y = 20
        xcenter = self.layout.APP_X + self.layout.MENU_W + (self.layout.APP_W - self.layout.MENU_W) / 2
        eve.cmd_text(xcenter, y, 29, eve.OPT_CENTERX, title)

        y += 100
        padding = 10
        w = layout.APP_W - layout.MENU_W - padding * 2
        eve.cmd_fillwidth(w)
        eve.cmd_text(xcenter, y, 29, eve.OPT_CENTER | eve.OPT_FILL, "Set Bedtime and silent your device")

        panel_w=layout.APP_W - layout.MENU_W

        y = y + 200

        clocksize = round(panel_w * 8 / 10 / 2)

        # clock face
        eve.cmd_bgcolor(0x777777)
        eve.cmd_fgcolor(0xffff)
        eve.cmd_gradcolor(0xffffff)
        clockx = xcenter
        clocky = y
        clock_hh = hh
        clock_mm = mm
        if self.time2_changing == 1:
            clock_hh = hh2
            clock_mm = mm2

        eve.cmd_clock(clockx, clocky, clocksize, eve.OPT_NOSECS, clock_hh, clock_mm, 0, 0);
        # track the cmd_clock
        eve.cmd_track(clockx, clocky, 1, 1, tag_ui_alarm_rotate)
        eve.ColorA(0)
        widgets_point(eve, clockx, clocky, clocksize, -1, tag_ui_alarm_rotate)
        eve.ColorA(255)

        # sleep icon
        # icon_size = 64
        # self.layout.draw_asset(0 , 'zzzclock' , x - icon_size / 2, y - icon_size / 2)

        eve.BlendFunc(eve.SRC_ALPHA, eve.ZERO) # remove gradient

        # time
        color_hh = str('0x777777')
        color_text = str('0xffffff')
        w = max(60, round(panel_w / 6)) # w atleast 60
        h = round(w / 2)
        x = round(xcenter - w + 5)
        y += clocksize + 20

        # hh:mm
        # hh background
        widgets_box(eve, x, y, w, h, 1, [0x77, 0x77, 0x77], tag_ui_alarm_hh)
        widgets_box(eve, x, y + w + 10, w, h, 1, [0x77, 0x77, 0x77], tag_ui_alarm_hh)
        # mm background
        x += w
        widgets_box(eve, x, y, w, h, 1, [0x77, 0x77, 0x77], tag_ui_alarm_mm)
        widgets_box(eve, x, y + w + 10, w, h, 1, [0x77, 0x77, 0x77], tag_ui_alarm_mm)

        eve.BlendFunc(eve.SRC_ALPHA, eve.ONE_MINUS_SRC_ALPHA) # remove gradient

        eve.cmd_text(x - w - 5, y + h/2, 23, eve.OPT_RIGHTX | eve.OPT_CENTERY, "Sleep: ")
        eve.cmd_text(x - w - 5, y + h/2 + w + 10, 23, eve.OPT_RIGHTX | eve.OPT_CENTERY, "Wake up: ")

        # hh:mm text
        eve.ColorRGB(int('0x'+color_text[2:4], 16), int('0x'+color_text[4:6], 16), int('0x'+color_text[6:8], 16))
        eve.cmd_text(x, y + h/2, 30, eve.OPT_CENTER, ' : ')

        if self.time_changing == 1 and self.tag_hh_selected == 1:
            eve.ColorRGB(0xFF, 0, 0)
        eve.Tag(tag_ui_alarm_hh)
        eve.cmd_text(x - w/2, y + h/2, 30, eve.OPT_CENTER, self.helper.zfill(str(hh), 2))

        eve.ColorRGB(int('0x'+color_text[2:4], 16), int('0x'+color_text[4:6], 16), int('0x'+color_text[6:8], 16))
        eve.cmd_text(x, y + h/2 + w + 10, 30, eve.OPT_CENTER, ' : ')
        if self.time2_changing == 1 and self.tag_hh2_selected == 1:
            eve.ColorRGB(0xFF, 0, 0)
        else:
            eve.ColorRGB(int('0x'+color_text[2:4], 16), int('0x'+color_text[4:6], 16), int('0x'+color_text[6:8], 16))
        eve.Tag(tag_ui_alarm_hh2)
        eve.cmd_text(x - w/2, y + h/2 + w + 10, 30, eve.OPT_CENTER, self.helper.zfill(str(hh2), 2))

        if self.time_changing == 1 and self.tag_mm_selected == 1:
            eve.ColorRGB(0xFF, 0, 0)
        else:
            eve.ColorRGB(int('0x'+color_text[2:4], 16), int('0x'+color_text[4:6], 16), int('0x'+color_text[6:8], 16))
        eve.Tag(tag_ui_alarm_mm)
        eve.cmd_text(x + w/2, y + h/2, 30, eve.OPT_CENTER, self.helper.zfill(str(mm), 2))

        if self.time2_changing == 1 and self.tag_mm2_selected == 1:
            eve.ColorRGB(0xFF, 0, 0)
        else:
            eve.ColorRGB(int('0x'+color_text[2:4], 16), int('0x'+color_text[4:6], 16), int('0x'+color_text[6:8], 16))
        eve.Tag(tag_ui_alarm_mm2)
        eve.cmd_text(x + w/2, y + h/2 + w + 10, 30, eve.OPT_CENTER, self.helper.zfill(str(mm2), 2))

        #save/cancel
        if (self.time_changing == 1 and time_changed) or (self.time2_changing == 1 and time2_changed):
            btn_w = 90
            btn_h = 30
            padding = 20
            btn_y = y + 120
            btn_x1 = xcenter - padding/2 - btn_w
            btn_x2 = xcenter + padding/2
            eve.Tag(tag_ui_clock_set)
            eve.cmd_button(btn_x1, btn_y, btn_w, btn_h, 28, 0, "Save")
            eve.Tag(tag_ui_clock_cancel)
            eve.cmd_button(btn_x2, btn_y, btn_w, btn_h, 28, 0, "Cancel")
        eve.ColorRGB(0xFF, 0xFF, 0xFF) #reset color
