from brteve.brt_eve_bt817_8 import BrtEve

from .eve_helper import eve_helper
from .gesture import gesture
from . import language
from . import common

class page_screensaver():
    ANI_STEP=10

    def init(self, eve: BrtEve, helper:eve_helper, lan:language.language):
        self.eve = eve
        self.helper = helper
        self.eve = eve
        self.lan = lan

    def __init__(self):
        self.eve = 0
        self.helper = 0
        self.eve = 0
        self.lan = 0

        self.ani_color = 0
        self.ani_color_step  = self.ANI_STEP
        self.frame_addr = common.SS_FLASH_ADDR_FRAME_0
        self.frame_index = 0
        self.gesture = gesture()

    def event(self):
        eve=self.eve
        self.gesture.renew(eve)

        gt=self.gesture.sGesture
        tag = gt.tagReleased

        if tag == common.SS_TAG:
            return 0 #end this page
        return 1

    def draw(self):
        if self.event() == 0:
            return 0 #drawing ends

        lan=self.lan

        self.eve.cmd_flashread(self.helper.RAM_G_SIZE - common.SS_FRAME_SIZE,
            self.frame_addr, common.SS_FRAME_SIZE)

        if lan.lan_active == lan.lan_cn:
            self.eve.cmd_setfont2(common.HF_BOTTOM, lan.FontBottomCH["xf_addr"]
                - lan.FontBegin["xf_addr"], 0)
        else:
            self.eve.cmd_setfont2(common.HF_BOTTOM, lan.FontBottom["xf_addr"]
                - lan.FontBegin["xf_addr"], 0)

        self.eve.BitmapHandle(0)
        self.eve.cmd_setbitmap(self.helper.RAM_G_SIZE - common.SS_FRAME_SIZE,
            self.eve.ASTC_10x10, common.SCREEN_WIDTH, common.SCREEN_HEIGHT)
        self.eve.Tag(common.SS_TAG)
        self.eve.Begin(self.eve.BITMAPS)
        self.eve.Vertex2f(0, 0)
        self.eve.End()

        self.eve.ColorRGB(0, 0, 0)
        self.eve.ColorA((int)(self.ani_color))
        self.eve.cmd_text((int)(common.SCREEN_WIDTH / 2), common.Y_FOOTER,
            common.HF_BOTTOM, self.eve.OPT_CENTERX, lan.lan_active.s_pleaseTouch)
        self.eve.ColorRGB(255, 255, 255)

        self.ani_color += self.ani_color_step
        if self.ani_color >= 255:
            self.ani_color = 255
            self.ani_color_step = -self.ANI_STEP
        elif self.ani_color <= 0:
            self.ani_color = 0
            self.ani_color_step = self.ANI_STEP * 0.5

        self.frame_index +=1
        self.frame_addr += common.SS_FRAME_SIZE

        if self.frame_index >= common.SS_FRAME_COUNT:
            self.frame_index = 0
            self.frame_addr = common.SS_FLASH_ADDR_FRAME_0
        return 1

    def deinit(self):
        pass
