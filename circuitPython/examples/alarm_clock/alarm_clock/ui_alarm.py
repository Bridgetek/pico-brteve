import time

from .helper import helper
from .gesture import gesture
from .datetime import hh_mm, milis, now_hh_mm, now_hh_mm_ss, print_weekday, random
from .layout import layout
from .tags import *
from . import datetime
from .scroller import scroller
from .widgets import widgets_point
from .dimension2d import clock_hand

import sys
if sys.implementation.name == "circuitpython":
    from brteve.brt_eve_bt817_8 import BrtEve
else:
    from ....lib.brteve.brt_eve_bt817_8 import BrtEve

PADDING_TOP = 70
ALARM_MARGIN = 30
PADDING_BOTTOM = 30

EXPAND = 1
COLLAPSE = 2

def deepcopy_nested_list(data):
    out = []
    for el in data:
        if isinstance(el, list):
            out.append(deepcopy_nested_list(el))
        else:
            out.append(el)
    return out

class ui_alarm():
    def __init__(self, eve: BrtEve, helper: helper, gesture: gesture, layout: layout):
        self.eve = eve
        self.helper = helper
        self.gesture = gesture
        self.layout = layout
        self.scroller = scroller()
        #self.scroller.set_speed(1/8)

        self._last_veloc = 0
        self.offset_y = 0
        self.alarms = [
            #'name',         'alarm-second', 'repeat: 2, 3, 4, 5, 6, 7, 8', 'on_off',  color, tag, onoff_tag , status = 0
            ['Wake up'      , [6, 00] , [1, 1, 1, 1, 1, 1, 1], 'on' , -1, -1, -1, 0], # status: 0=inactive, 1 = active, 2 = dimissed
            ['Meeting'      , [9, 00] , [0, 0, 1, 0, 0, 0, 0], 'on' , -1, -1, -1, 0],
            ['Go to school' , [8, 00] , [1, 0, 1, 0, 0, 1, 1], 'off', -1, -1, -1, 0],
            [''             , [8, 19], [1, 1, 0, 0, 0, 0, 0], 'on' , -1, -1, -1, 0],
        ]
        self.alarm_new = ['', [6, 00], [0, 0, 0, 0, 0, 0, 0], 'on' , -1, -1, -1, 0]
        self.alarm_clone = ['', [6, 00], [0, 0, 0, 0, 0, 0, 0], 'on' , -1, -1, -1, 0]

        self.tag_min = 0
        self.tag_max = 0

        self.popup = 0

        self.tag_alarm_pressing = 0
        self.tag_alarm_released = 0

        self.tag_pressing = 0
        self.tag_released = 0

        self.tag_new_alarm = 0

        self.tag_hh_selected = 0
        self.tag_mm_selected = 0
        self.alarm_animation_nth=0

        self.alarm_expanding = {
            #, , x, y, w, h, expan_or_collapse, is_done_animate
            'tag': 0, 'step': 0, 'x': 0, 'y': 0, 'w': 0, 'h': 0, 'expan_or_collapse': 0, 'is_done_animate': 0
        }

        self.gradients = [
            [0xa83e32, 0xaef078],
            [0x09ad2f, 0x10170b],
            [0x100994, 0x6e3642],
            [0x6e0eeb, 0xfa6182],
            [0xcc06c9, 0xcfde5f],
            [0x3b8201, 0x27751b],
        ]

        self.default_value()

        self._delay_selection = 0

    def default_value(self):
        index_color = 4
        index_tag = 5
        index_tagonoff = 6
        tag = tag_ui_alarm_rects
        tag_onoff = tag_ui_alarm_onoff
        self.tag_min = tag
        for alarm in self.alarms:
            alarm_color = self.gradients[random(0, len(self.gradients) - 1)]
            alarm_color = self.gradients[(tag + 1 - tag_ui_alarm_rects) % (len(self.gradients) - 1)]
            alarm[index_color] = alarm_color
            #set tag
            alarm[index_tag] = tag
            alarm[index_tagonoff] = tag_onoff


            tag+=1
            tag_onoff+=1
        self.tag_max = tag - 1

    def get_repeat_text(self, alarm):
        _mon=0
        _tue=1
        _wed=2
        _thu=3
        _fri=4
        _sat=5
        _sun=6
        repeat = alarm[2] #{2:0, 3:1, 4:1, 5:1, 6:1, 7:1, 8:1}
        daystr = ""
        alldaycounter = ''
        weekday = 0
        for day in repeat:
            if day == 1:
                alldaycounter += str(weekday)
                daystr += print_weekday(weekday) + ", "
            weekday +=1
        daystr = daystr[0:len(daystr)-2]

        if len(alldaycounter) == 7:
            daystr = "Everyday"
        return daystr

    def event(self):
        self.tag_alarm_pressing = 0
        self.tag_alarm_released = 0

        # Gesture is already renew in alarm_clock.py
        self.offset_y, veloc = self.scroller.get_offset_velocity(self.gesture.get().touchY)
        if veloc != 0:
            self._last_veloc = 1
        if self.popup:
            self.offset_y = 0

        press = self.gesture.get().tagPressed
        release = self.gesture.get().tagReleased
        # release = tag_ui_alarm_new_alarm # test
        self.tag_pressing = press
        self.tag_released = release

        if self.tag_min <= press and self.tag_max >= press:
            self.tag_alarm_pressing = press

        # When vloc > 0: the first tag_released will be ignore
        if self.tag_min <= release and self.tag_max >= release:
            if self._last_veloc > 0:
                self._last_veloc = 0
            else:
                self.tag_alarm_released = release

        if release == tag_ui_alarm_hh:
            self.tag_hh_selected = 1
            self.tag_mm_selected = 0
        if release == tag_ui_alarm_mm:
            self.tag_hh_selected = 0
            self.tag_mm_selected = 1

        if release == tag_ui_alarm_new_alarm:
            self.new_larm()

    def new_larm(self):
        if self.tag_new_alarm != 0 or self.alarm_expanding['tag'] != 0 or self.popup:
            return

        self.tag_max += 1
        self.alarm_new[4] = self.gradients[(self.tag_max + 1 - tag_ui_alarm_rects) % (len(self.gradients) - 1)]
        self.alarm_new[5] = self.tag_max
        self.alarm_new[6] = self.tag_max - tag_ui_alarm_rects + tag_ui_alarm_onoff
        self.alarm_new[7] = 0 # inactive

        self.alarms.append(self.alarm_new)
        self.alarm_expanding['expan_or_collapse'] == EXPAND
        self.tag_new_alarm = self.tag_max
        self.tag_alarm_released = self.tag_max

    def confirm_delete(self):
        eve = self.eve
        layout = self.layout
        helper = self.helper

        panel_w=layout.APP_W - layout.MENU_W
        w = round(layout.APP_W * 8 / 10)
        h = round(w * 2 / 3)
        x = round(layout.APP_X + layout.APP_W / 2 - w / 2)
        y = round(layout.APP_Y + layout.APP_H / 2 - h / 2)
        rounded = 40

        if self.tag_released >= tag_ui_alarm_delete and \
            self.tag_released <= tag_ui_alarm_delete + self.tag_max - tag_ui_alarm_rects:
            self.popup = self.tag_released

        if self.popup:
            border = 2
            eve.ColorRGB(0xf0, 0xf0, 0xf0)
            eve.Begin(eve.RECTS)
            eve.LineWidth(rounded)
            eve.Vertex2f(x + rounded / 2 - border - border/2, y + rounded / 2 - border)
            eve.Vertex2f(x + w - rounded / 2 + border, h + y - rounded / 2 + border)

            eve.ColorRGB(0xeb, 0x2a, 0x94)
            eve.cmd_text(x + w/2, y + 20, 30, eve.OPT_CENTER, "Delete this alarm?")
            wb = w / 4
            hb = wb / 2
            yb = y + h / 2 - hb / 2
            ct = x + w / 2

            eve.ColorRGB(0xff, 0xff, 0xff)
            eve.Tag(tag_ui_alarm_yes)
            eve.cmd_button(ct - wb - 10, yb, wb, hb, 29, 0, 'Yes')
            eve.Tag(tag_ui_alarm_no)
            eve.cmd_button(ct + 10, yb, wb, hb, 29, 0, 'No')

            if self.tag_released == tag_ui_alarm_no:
                self.popup = 0
            if self.tag_released == tag_ui_alarm_yes:
                tag = self.popup - tag_ui_alarm_delete
                self.alarms.pop(tag)
                self.popup = 0

        return self.popup

    def _draw1alarm(self, alarm, count, expanded = 0, ox = 0, oy = 0, ow = 0, oh = 0):
        enable_gradient = 1
        tag_index = 5
        rounded = 40

        eve = self.eve
        layout = self.layout
        helper = self.helper

        panel_w=layout.APP_W - layout.MENU_W
        x=ox
        y=oy
        w=ow
        h=oh

        if not expanded:
            w = round(panel_w * 8 / 10)
            h = round(w / 2.1)
            x = layout.APP_X + layout.MENU_W + round(panel_w / 2 - w / 2)
            y = self.offset_y + layout.APP_Y + PADDING_TOP + (ALARM_MARGIN + h) * count

        if y < layout.APP_Y - h or y > layout.APP_Y + layout.APP_H - PADDING_TOP:
            return

        # border
        if alarm[tag_index] == self.tag_alarm_pressing:
            border = 2
            eve.ColorRGB(0xf0, 0xf0, 0xf0)
            eve.Begin(eve.RECTS)
            eve.LineWidth(rounded)
            eve.Vertex2f(x + rounded / 2 - border - border/2, y + rounded / 2 - border)
            eve.Vertex2f(x + w - rounded / 2 + border, h + y - rounded / 2 + border)

        #boundary
        eve.SaveContext()
        if enable_gradient:
            eve.StencilFunc(eve.NEVER, 0x00, 0x00)
            eve.StencilOp(eve.INCR, eve.INCR)
        eve.ColorRGB(0x80, 0xFF, 0x40)
        eve.Begin(eve.RECTS)
        eve.Tag(alarm[tag_index])
        eve.LineWidth(rounded)
        eve.Vertex2f(x + rounded / 2, y + rounded / 2)
        eve.Vertex2f(x + w - rounded / 2,h + y - rounded / 2)

        # gradient
        alarm_color = alarm[4]
        if enable_gradient:
            eve.StencilFunc(eve.EQUAL, 0x01, 0x01)
            eve.StencilOp(eve.KEEP, eve.KEEP)
            eve.ScissorXY((int)(x), (int)(PADDING_TOP if y < PADDING_TOP else y))
            eve.ScissorSize(w, h)
            eve.cmd_gradient(x+w, y, alarm_color[0], x, y, alarm_color[1])
        eve.RestoreContext()

        #name
        eve.ColorRGB(0xf0, 0xF0, 0xf0)
        eve.cmd_text(x + 10, y + 10, 23, 0, alarm[0])

        #on off
        state = 0
        color = 0x777777
        color2 = 0x777777
        if alarm[3] == 'on':
            state = 65535
            color = 0x66ebff
            color2 = 0x050dfa
        tw = w / 6
        eve.cmd_fgcolor(color2)
        eve.cmd_bgcolor(color)
        tag = tag_ui_alarm_onoff + count
        eve.Tag(tag)
        eve.cmd_toggle(x + w - tw - 15, y + 10, tw, 21, 0, state, '')
        if self.tag_released == tag:
            if self.alarms[count][3] == 'on':
                self.alarms[count][3] = 'off'
            else:
                self.alarms[count][3] = 'on'
            self.alarm_clone[3] = self.alarms[count][3]

        if not expanded:
            #repeat
            #{'all':0, 2:0, 3:1, 4:1, 5:1, 6:1, 7:1, 8:1}
            repeat = self.get_repeat_text(alarm)
            eve.cmd_text(x + 10, y + 30, 23, 0, repeat)

            #time = hh:mm
            text = self.helper.zfill(str(alarm[1][0]), 2) + ':' + self.helper.zfill(str(alarm[1][1]), 2)
            eve.cmd_text(x + 10, y + h - 50, 24, 0, text)

        # delete button
        if not expanded:
            radius = 15
            xc = x + w - radius - 10
            yc = y + h - radius - 10
            tag = tag_ui_alarm_delete + count
            eve.Tag(tag)
            eve.ColorA(255)
            eve.ColorRGB(0x77, 0x77, 0x77)
            eve.Begin(eve.POINTS)
            eve.PointSize(radius * 2)
            eve.Vertex2f(xc, yc)
            eve.ColorRGB(0, 0, 0)
            eve.cmd_text(xc, yc, 28, eve.OPT_CENTER, 'X')

    def _draw_alarm_editing(self, alarm_e, count, ax, ay, aw, ah):
        eve = self.eve
        layout = self.layout
        helper = self.helper
        gesture = self.gesture

        alarm = self.alarm_clone

        hh= alarm[1][0]
        mm = alarm[1][1]

        # clock face
        clocksize = round(aw * 8 / 10 / 2)

        x = layout.screen_w / 2
        y = clocksize * 2 + 30
        eve.cmd_bgcolor(alarm[4][0] / 2)
        eve.cmd_fgcolor(alarm[4][1])
        eve.cmd_gradcolor(alarm[4][1])
        eve.cmd_clock(x, y, clocksize, eve.OPT_NOSECS, hh, mm, 0, 0);

        eve.BlendFunc(eve.SRC_ALPHA, eve.ZERO) # remove gradient

        # time
        color_hh = str('0x000000')
        color_text = str(hex(round(alarm[4][1] / 3)))
        w = max(60, round(aw / 6)) # w atleast 60
        h = round(w / 2)
        x = round(layout.screen_w / 2 - w + 5)
        y += clocksize + 20

        # hh background
        eve.Tag(tag_ui_alarm_hh)
        eve.Begin(eve.RECTS)
        eve.LineWidth(1)
        eve.ColorRGB(int('0x'+color_hh[2:4], 16), int('0x'+color_hh[4:6], 16), int('0x'+color_hh[6:8], 16))
        eve.ColorRGB(0, 0, 0)
        eve.Vertex2f(x, y)
        eve.Vertex2f(x + w, h + y)

        # mm background
        x += w
        eve.Tag(tag_ui_alarm_mm)
        eve.Begin(eve.RECTS)
        eve.ColorRGB(int('0x'+color_hh[2:4], 16), int('0x'+color_hh[4:6], 16), int('0x'+color_hh[6:8], 16))
        eve.Vertex2f(x, y)
        eve.Vertex2f(x + w, h + y)

        eve.BlendFunc(eve.SRC_ALPHA, eve.ONE_MINUS_SRC_ALPHA) # remove gradient

        # hh:mm
        eve.ColorRGB(int('0x'+color_text[2:4], 16), int('0x'+color_text[4:6], 16), int('0x'+color_text[6:8], 16))
        eve.cmd_text(x, y + h/2, 30, eve.OPT_CENTER, ' : ')

        if self.tag_hh_selected == 1:
            eve.ColorRGB(0xFF, 0, 0)
        eve.Tag(tag_ui_alarm_hh)
        eve.cmd_text(x - w/2, y + h/2, 30, eve.OPT_CENTER, self.helper.zfill(str(alarm[1][0]), 2))

        if self.tag_mm_selected == 1:
            eve.ColorRGB(0xFF, 0, 0)
        else:
            eve.ColorRGB(int('0x'+color_text[2:4], 16), int('0x'+color_text[4:6], 16), int('0x'+color_text[6:8], 16))
        eve.Tag(tag_ui_alarm_mm)
        eve.cmd_text(x + w/2, y + h/2, 30, eve.OPT_CENTER, self.helper.zfill(str(alarm[1][1]), 2))

        eve.ColorRGB(0xFF, 0xFF, 0xFF) #reset color

        # track the circle
        if self.tag_hh_selected == 1 or self.tag_mm_selected == 1:
            xc = layout.screen_w / 2
            yc = clocksize * 2 + 30
            eve.cmd_track(xc, yc, 1, 1, tag_ui_alarm_rotate)
            eve.Tag(tag_ui_alarm_rotate)
            eve.ColorA(0)
            eve.Begin(eve.POINTS)
            eve.PointSize(clocksize * 2)
            eve.Vertex2f(xc, yc)
            eve.ColorA(255)

            # get new clock
            track = gesture.get().tagTrackTouched
            tag = track & 0xFF
            angel = track >> 16
            if tag == tag_ui_alarm_rotate:
                hh = round(angel * 12 / 65535 + 6) % 12
                mm = round(angel * 60 / 65535 + 30) % 60
                print(track & 0xff, track >> 16, hh , mm)

                if self.tag_hh_selected == 1:
                    pm_time = 0
                    if self.alarm_clone[1][0] == 11 and hh == 0:
                        pm_time = 12
                    elif self.alarm_clone[1][0] == 23 and hh == 0:
                        pm_time = 0
                    elif self.alarm_clone[1][0] >= 12:
                        pm_time = 12
                    self.alarm_clone[1][0] = pm_time + hh
                else:
                    self.alarm_clone[1][1] = mm

        # repeat days
        color_selected = 0x2ee8e8
        color_unselect = 0x2c2e2e
        margin = 15
        y += h + 50
        w = 45
        h = round(w * 8 / 10)
        x = layout.screen_w / 2 - (5 * w + 4 * margin) / 2
        tag = tag_ui_alarm_weekday
        for i in range(0, 5):
            if alarm[2][i] == 1:
                eve.cmd_fgcolor(color_selected)
            else:
                eve.cmd_fgcolor(color_unselect)
            eve.Tag(tag)
            eve.cmd_button(x, y, w, h, 22, 0, print_weekday(i))
            tag+=1
            x += w + margin

        margin = 10
        y += h + 10
        x = layout.screen_w / 2 - (2 * w + 1 * margin) / 2
        for i in range(5, 7):
            if alarm[2][i] == 1:
                eve.cmd_fgcolor(color_selected)
            else:
                eve.cmd_fgcolor(color_unselect)
            eve.Tag(tag)
            eve.cmd_button(x, y, w, h, 22, 0, print_weekday(i))
            tag+=1
            x += w + margin

        eve.cmd_fgcolor(0)

        # reflect the change
        if self.tag_released > 0:
            print("released", self.tag_released)
            if self.tag_released >= tag_ui_alarm_weekday and \
                self.tag_released <= tag_ui_alarm_weekday_sun:
                daynth = self.tag_released - tag_ui_alarm_weekday
                print('daynth', daynth)
                if alarm[2][daynth] == 1:
                    alarm[2][daynth] = 0
                else:
                    alarm[2][daynth] = 1

        # Save
        btn_w = 120
        btn_h = 40
        x = ax + 10
        y = ay + ah - btn_h - 20
        eve.Tag(tag_ui_alarm_save)
        eve.cmd_button(x, y, btn_w, btn_h, 22, 0, "Save")
        if self.tag_released == tag_ui_alarm_save:
            self.alarms[count] = deepcopy_nested_list(self.alarm_clone)
            self.alarm_expanding['expan_or_collapse'] = COLLAPSE
            self.tag_new_alarm = 0

        # Cancel
        x = ax + aw - btn_w - 10
        eve.Tag(tag_ui_alarm_cancel)
        eve.cmd_button(x, y, btn_w, btn_h, 22, 0, "Cancel")
        if self.tag_released == tag_ui_alarm_cancel:
            self.alarm_expanding['expan_or_collapse'] = COLLAPSE
            self.tag_new_alarm = 0

    def collapse_alarm(self):
        layout = self.layout
        count = self.alarm_expanding['tag'] - tag_ui_alarm_rects

        panel_w=layout.APP_W - layout.MENU_W
        tw = round(panel_w * 8 / 10)
        th = round(tw / 2.1)
        tx = layout.APP_X + layout.MENU_W + round(panel_w / 2 - tw / 2)
        ty = self.offset_y + layout.APP_Y + PADDING_TOP + (ALARM_MARGIN + th) * count

        x = self.alarm_expanding['x']
        y = self.alarm_expanding['y']
        w = self.alarm_expanding['w']
        h = self.alarm_expanding['h']

        devider = 5
        x += round((tx - x) / (devider - self.alarm_expanding['step']))
        y += round((ty - y) / (devider - self.alarm_expanding['step']))
        w += round((tw - w) / (devider - self.alarm_expanding['step']))
        h += round((th - h) / (devider - self.alarm_expanding['step']))
        self.alarm_expanding['step'] = min(devider - 1, self.alarm_expanding['step'] + 1)

        if devider - self.alarm_expanding['step'] == 1:
            self.alarm_expanding['tag'] = 0 # unselect alarm

        return x, y, w, h

    def expand_alarm(self):
        layout = self.layout

        tw = round(layout.APP_W * 8 / 10)
        th = tw * 2
        tx =  round(layout.APP_X + layout.APP_W / 2 - tw / 2)
        ty =  round(layout.APP_Y + layout.APP_H / 2 - th / 2)

        x = self.alarm_expanding['x']
        y = self.alarm_expanding['y']
        w = self.alarm_expanding['w']
        h = self.alarm_expanding['h']

        devider = 8

        x += round((tx - x) / devider * self.alarm_expanding['step'])
        y += round((ty - y) / devider * self.alarm_expanding['step'])
        w += round((tw - w) / devider * self.alarm_expanding['step'])
        h += round((th - h) / devider * self.alarm_expanding['step'])
        self.alarm_expanding['step'] = min(devider - 1, self.alarm_expanding['step'] + 1)

        if devider - self.alarm_expanding['step'] == 1:
            self.alarm_expanding['is_done_animate'] = 1

        return x, y, w, h

    def draw_1_selected_alarm(self):
        tag_index = 5
        eve = self.eve
        layout = self.layout
        helper = self.helper
        is_start_expand = (self.tag_alarm_released != 0 and self.alarm_expanding['tag'] == 0)

        is_alarm_choosing = self.alarm_expanding['tag'] != 0
        count = self.alarm_expanding['tag'] - tag_ui_alarm_rects

        # set expand mode
        if is_start_expand:
            count = self.tag_alarm_released - tag_ui_alarm_rects
            if self.tag_new_alarm:
                count = self.tag_new_alarm - tag_ui_alarm_rects

            panel_w=layout.APP_W - layout.MENU_W
            w = round(panel_w * 8 / 10)
            h = round(w / 2.1)
            x = layout.APP_X + layout.MENU_W + round(panel_w / 2 - w / 2)
            y = self.offset_y + layout.APP_Y + PADDING_TOP + (ALARM_MARGIN + h) * count

            self.alarm_expanding['tag'] = self.tag_alarm_released
            self.alarm_expanding['step'] = 1
            self.alarm_expanding['x'] = x
            self.alarm_expanding['y'] = y
            self.alarm_expanding['w'] = w
            self.alarm_expanding['h'] = h
            self.alarm_expanding['expan_or_collapse'] = EXPAND
            self.alarm_expanding['is_done_animate'] = 0

            # clone the alarm if save or cancel button is press
            self.alarm_clone = deepcopy_nested_list(self.alarms[count])
            return 0

        if not is_alarm_choosing:
            return 0 # no alarm expanding, return

        # do expand selected alarm
        alarm = self.alarms[count]

        if self.alarm_expanding['expan_or_collapse'] == EXPAND:
            x, y, w, h = self.expand_alarm()
        else:
            x, y, w, h = self.collapse_alarm()
        self._draw1alarm(alarm, count, True, x, y, w, h)

        if self.alarm_expanding['expan_or_collapse'] == EXPAND and self.alarm_expanding['is_done_animate'] == 1:
            self._draw_alarm_editing(alarm, count, x, y, w, h)

        return 1

    def _drawalarms(self):
        layout = self.layout
        eve= self.eve

        # draw selected alarm only if have
        if self.draw_1_selected_alarm():
            # reset scissor
            eve.ScissorXY(0, 0)
            eve.ScissorSize(2048, 2048)
            return

        # Yes no popup
        if self.confirm_delete():
            # reset scissor
            eve.ScissorXY(0, 0)
            eve.ScissorSize(2048, 2048)
            return

        # draw all alarm
        panel_w=layout.APP_W - layout.MENU_W
        w = round(panel_w - 10)
        h = round(w / 2.1)
        y = PADDING_TOP
        x = layout.APP_X + layout.MENU_W + 1
        w = round(panel_w - 10)

        scroller_len = len(self.alarms) * (h + ALARM_MARGIN + 10) + PADDING_TOP
        self.scroller.set_limit(0, min(0, self.layout.screen_h - scroller_len))

        eve.ScissorXY((int)(x), (int)(y))
        eve.ScissorSize(w, layout.APP_H - PADDING_TOP - PADDING_BOTTOM)
        count = 0
        for alarm in self.alarms:
            if alarm[5] == self.alarm_expanding['tag']: #this alarm is selected for editting
                count+=1
                continue # expanding alarm is last draw
            self._draw1alarm(alarm, count)
            count+=1

        # reset scissor
        eve.ScissorXY(0, 0)
        eve.ScissorSize(2048, 2048)

    def _alarm_ringing(self, alarm):
        layout = self.layout
        eve= self.eve
        self.alarm_animation_nth = (int)(self.alarm_animation_nth+1)

        # overwride boundary
        rounded = 60
        eve.ColorRGB(0xc3, 0xc3, 0xc3)
        eve.Begin(eve.RECTS)
        eve.LineWidth(rounded)
        eve.Vertex2f(self.layout.APP_X + rounded / 2, self.layout.APP_Y + rounded / 2)
        eve.Vertex2f(self.layout.APP_X + self.layout.APP_W - rounded / 2, self.layout.APP_H + self.layout.APP_Y - rounded / 2)

        ani_nth = self.alarm_animation_nth % 4
        alarm_center= ['ring_alarm', 0, 0]
        alarm_left  = ['ring_left' , -60, -60]
        alarm_right = ['ring_right',  70, -60]
        ani_c = [[2, 2], [3, -3], [1, 3], [-2, -2], [1, 1]]
        ani_l_ = [8, 4, 6, 1, 7]
        ani_r_ = [-7, -1, -6, -4, -8]
        ani_l = []
        ani_r = []
        for (c, l, r) in zip(ani_c, ani_l_, ani_r_):
            ani_l.append([c[0] + l, c[1] + l])
            ani_r.append([c[0] + r, c[1] - r])

        x = layout.APP_X + layout.APP_W / 2
        y = layout.APP_Y + layout.APP_H / 2
        alarm_ani = [ani_c, ani_l, ani_r]
        alarm_img=[alarm_center, alarm_left, alarm_right]
        for (i, j) in zip(alarm_img, alarm_ani):
            img = i[0]
            offset_x = i[1]
            offset_y = i[2]
            w = layout.images[img][2]
            h = layout.images[img][2]
            self.layout.draw_asset(0, img,
                x - w/2 + offset_x + j[ani_nth][0],
                y - h/2 + offset_y + j[ani_nth][1])

        #hand
        center_x = layout.APP_X + layout.APP_W / 2 + ani_c[ani_nth][0] - 2
        center_y = layout.APP_Y + layout.APP_H / 2 + ani_c[ani_nth][1] + 15
        point_size = 6
        widgets_point(eve, center_x - 0, center_y - 0, point_size, [0x00, 0x00, 0x00])

        hh = alarm[1][0]
        clock_hand(eve, center_x, center_y, 15, 40, hh, 12, [0xff, 0x00, 0x00], 8)
        clock_hand(eve, center_x, center_y, 15, 40, hh, 12, [0xff, 0xFF, 0xFF], 4)

        mm = alarm[1][1]
        clock_hand(eve, center_x, center_y, 15, 60, mm, 60, [0x00, 0xff, 0x00], 2)

        center_x = layout.APP_X + layout.APP_W / 2
        center_y = layout.APP_Y + layout.APP_H / 2

        # hour string
        text = self.helper.zfill(str(hh), 2) + ' : ' + self.helper.zfill(str(mm), 2)
        eve.ColorRGB(0xff, 0, 0)
        eve.cmd_text(center_x, center_y - 150, 30, eve.OPT_CENTER, text)

        # dismiss
        x = center_x
        y = layout.APP_Y + layout.APP_H - 20
        eve.ColorRGB(0x77, 0x77, 0x77)
        eve.cmd_text(x, y, 28, eve.OPT_CENTER, "Tap to dismiss")
        eve.ColorRGB(0xff, 0xff, 0xff)

        dismissed = 2
        if self.gesture.get().isTouch: # dimiss
            alarm[7] = dismissed

        # time.sleep(1)

    def interrupt(self):
        # self.alarms = [
        #     #'name',         'alarm-second', 'repeat: 2, 3, 4, 5, 6, 7, 8', 'on_off',  color, tag, onoff_tag , status = 0
        #     ['Wake up'      , [6, 00] , [1, 1, 1, 1, 1, 1, 1], 'on' , -1, -1, -1, 0],
        #     ['Meeting'      , [9, 00] , [0, 0, 1, 0, 0, 0, 0], 'on' , -1, -1, -1, 0],
        #     ['Go to school' , [8, 00] , [1, 0, 1, 0, 0, 1, 1], 'off', -1, -1, -1, 0],
        #     [''             , [14, 30], [1, 1, 0, 0, 0, 0, 0], 'on' , -1, -1, -1, 0],
        # ]
        inactive = 0
        active = 1
        dismissed = 2

        hh, mm, ss = now_hh_mm_ss()
        for i in self.alarms:
            if i[1][0] != hh and i[7] == dismissed :
                i[7] = inactive
                i[7] = inactive

            if i[1][0] == hh and i[1][1] == mm and i[7] == inactive:
                i[7] = active
            if i[7] == active:
                self._alarm_ringing(i)
                return 1
        return 0

    def draw(self):
        eve = self.eve

        self.default_value()
        self.event()
        self._drawalarms()

        eve.ColorRGB(255, 255, 255)
        title="Alarm"
        x = (self.layout.APP_X + self.layout.MENU_W) / 2 + (self.layout.APP_X + self.layout.APP_W) / 2
        eve.cmd_text(x, 20, 29, eve.OPT_CENTERX, title)

        btn_size = 56
        x = self.layout.APP_X + self.layout.APP_W - 56 - 20
        y = self.layout.APP_Y + self.layout.APP_H - 10 - btn_size
        if self.gesture.get().tagPressed == tag_ui_alarm_new_alarm:
            self.layout.draw_asset(tag_ui_alarm_new_alarm, 'new_alarm', x, y)
        else:
            self.layout.draw_asset(tag_ui_alarm_new_alarm, 'new_alarm_hover', x, y)
