from ui_lds.gesture import gesture
from ui_lds.layout import layout
from ui_lds.helper import helper
import time
from .datetime import hh_mm, hh_mm_ss_ms, milis, now, print_weekday, random
from .tags import *
import sys
from .ui_4in1_sensor import ui_4in1_text

if sys.implementation.name == "circuitpython":
    from brteve.brt_eve_bt817_8 import BrtEve
else:
    from ....lib.brteve.brt_eve_bt817_8 import BrtEve

# controler class
class ui():
    def __init__(self, eve: BrtEve) -> None:
        self.eve = eve
        self.helper = helper(eve)
        self.gesture = gesture()
        self.layout = layout(eve, self.helper)
        self.milis_start = 0
        self.milis_stop = 0
        self.running = 0
        self.pause = 0
        self.split = []
        self.ui_4in1_text=ui_4in1_text(eve, self.helper, self.gesture, self.layout)
        
        self.ui_active = self.ui_4in1_text
        self.count_3touch = 0
        self.last_x = 0
   
    def get_event(self):
        eve = self.eve
        self.gesture.renew(eve)
        #self.flip_screen_3touch()

        tag = self.gesture.get().tagPressed
        if ( tag>0 ): print("tag", tag, self.gesture.get().tagReleased, self.gesture.get().tagPressed)

        if tag == tag_ui_timer_reset:
             
            self.split = []
        elif tag == tag_ui_timer_start:
            if self.running == 1:
                return
            self.running = 1
            
        elif tag == tag_ui_timer_pause:
            if self.pause == 0:
                self.pause = 1
            else:
                self.pause = 0
            return
        elif tag == tag_ui_timer_split:
            """
            max_list = 5
            if len(self.split) == max_list:
                self.split = self.split[1:max_list]
            self.split.append(s)
            """
        else:
            if self.running == 1 and self.pause == 0:
                self.milis_stop = milis()
 

        return tag

    

    def start(self):
        self.eve.cmd_dlstart()
        self.eve.Clear(1, 1, 1)
        self.eve.VertexFormat(3)

        if self.layout.portrait:
            self.eve.cmd_setrotate(2)
        else:
            self.eve.cmd_setrotate(0)

    def flush(self):
        self.eve.Display()
        self.eve.cmd_swap()
        self.eve.flush()


    def drawBtn(self):
        eve = self.eve
        eve.ColorRGB(0xff, 0xff, 0xff)

        watch_w=304
        watch_h=392
        ymargin = 30
        y = watch_h + ymargin
        btn_w = 60
        btn_h = 30

        xmargin =10
        center = 200
        x1 = 50

        x2 = x1 + btn_w + xmargin
        x3 = x2 + btn_w + xmargin
        x4 = x3 + btn_w + xmargin

        eve.Tag(tag_ui_timer_reset)
        eve.cmd_button(x1, y, btn_w, btn_h, 23, 0, "Reset")
        eve.Tag(tag_ui_timer_pause)
        eve.cmd_button(x2, y, btn_w, btn_h, 23, 0, "Pause" if self.pause == 0 else "Resume")
        eve.Tag(tag_ui_timer_start)
        eve.cmd_button(x3, y, btn_w, btn_h, 23, 0, "Start")
        eve.Tag(tag_ui_timer_split)
        eve.cmd_button(x4, y, btn_w, btn_h, 23, 0, "Split")
    def draw(self,x,y,title):
        eve = self.eve


        eve.ColorRGB(0, 0, 0)
        #self.eve.ClearColorRGB(255, 255, 255)
        self.eve.VertexFormat(3)


        ev = self.get_event()
        eve.ColorRGB(255, 255, 255)
        #eve.ColorRGB(0, 0, 0)
        #x = (self.layout.APP_X + self.layout.MENU_W) / 2 + (self.layout.APP_X + self.layout.APP_W) / 2
        eve.cmd_text(x, y, 29, eve.OPT_CENTERX, title)
 
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
        for i in [self.ui_4in1_text ]:
            if i.interrupt():
                interrupt = 1
                break
        if not interrupt:
            self.ui_active.draw()

        self.eve.Display()
        self.eve.cmd_swap()
        self.eve.flush() 







