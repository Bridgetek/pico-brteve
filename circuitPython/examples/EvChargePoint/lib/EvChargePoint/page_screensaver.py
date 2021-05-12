from .gesture import gesture
from .gesture import Gesture_Touch
import bteve
from .eve_helper import eve_helper

from . import language
from . import common

import sys

class page_screensaver():
    ANI_STEP=10
    
    def init(self, eve:bteve, gd:Brt_PicoEve_Module, helper:eve_helper, lan:language.language):

        self.eve = eve
        self.helper = helper
        self.gd = gd
        self.lan = lan

        self.ani_color = 0
        self.ani_color_step  = self.ANI_STEP
        self.frame_addr = common.SS_FLASH_ADDR_FRAME_0
        self.frame_index = 0

    def event(self):
        gd=self.gd
        eve=self.eve
        gesture.renew(gd, eve)

        gt=gesture.get()
        tag = gt.tagReleased

        if (tag == common.SS_TAG) :
            return 0 #end this page
        return 1

    def draw(self):
        if self.event() == 0:
            return 0 #drawing ends

        lan=self.lan

        self.gd.cmd_flashread(self.helper.RAM_G_SIZE - common.SS_FRAME_SIZE, self.frame_addr, common.SS_FRAME_SIZE)

        if lan.lan_active == lan.lan_cn:
            self.gd.cmd_setfont2(common.HF_BOTTOM, lan.FontBottomCH["xf_addr"] - lan.FontBegin["xf_addr"], 0)
        else:
            self.gd.cmd_setfont2(common.HF_BOTTOM, lan.FontBottom["xf_addr"] - lan.FontBegin["xf_addr"], 0)

        self.gd.BitmapHandle(0)
        self.gd.cmd_setbitmap(self.helper.RAM_G_SIZE - common.SS_FRAME_SIZE, self.gd.ASTC_10x10, common.SCREEN_WIDTH, common.SCREEN_HEIGHT)
        self.gd.Tag(common.SS_TAG)
        self.gd.Begin(self.gd.BITMAPS)
        self.gd.Vertex2f(0, 0)
        self.gd.End()
    
        self.gd.ColorRGB(0, 0, 0)
        self.gd.ColorA((int)(self.ani_color))
        self.gd.cmd_text((int)(common.SCREEN_WIDTH / 2), common.Y_FOOTER, common.HF_BOTTOM, self.gd.OPT_CENTERX, lan.lan_active.s_pleaseTouch)
        self.gd.ColorRGB(255, 255, 255)
        
        self.ani_color += self.ani_color_step
        if (self.ani_color >= 255):
            self.ani_color = 255
            self.ani_color_step = -self.ANI_STEP
        elif (self.ani_color <= 0):
            self.ani_color = 0
            self.ani_color_step = self.ANI_STEP * 0.5
        
        self.frame_index +=1
        self.frame_addr += common.SS_FRAME_SIZE

        if (self.frame_index >= common.SS_FRAME_COUNT):
            self.frame_index = 0
            self.frame_addr = common.SS_FLASH_ADDR_FRAME_0
    
        return 1

    def deinit(self):
        pass

