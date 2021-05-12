from .gesture import gesture
from .gesture import Gesture_Touch
import bteve
from .eve_helper import eve_helper

from . import language
from . import common

import sys

class page_language():
    def init(self, eve:bteve, gd:Brt_PicoEve_Module, helper:eve_helper, lan:language.language):

        self.eve = eve
        self.helper = helper
        self.gd = gd
        self.lan = lan

        self.x_ani = 0
        self.y_ani = 0
        self.start_ani_out = 0

        self.X_STEP = 10
        self.Y_STEP = 7

        self.CL_BEGIN = {'addr':4096}
        self.CL_CIRCLE = [
            {'addr':117568, 'size':35392, 'x':0, 'y':0, 'width':280, 'height':280, 'format':gd.ASTC_6x6},
            {'addr':152960, 'size':35392, 'x':0, 'y':0, 'width':280, 'height':280, 'format':gd.ASTC_6x6},
            {'addr':188352, 'size':35392, 'x':0, 'y':0, 'width':280, 'height':280, 'format':gd.ASTC_6x6},
        ]

        self.CL_TAGS = [common.CL_TAG_CIRCLE_EN, common.CL_TAG_CIRCLE_DE, common.CL_TAG_CIRCLE_CN]
        self.frame_addr= common.SS_FLASH_ADDR_FRAME_0
        self.frame_index= 0

        self.LANG= [language.s_languageEnglish, language.s_languageGerman, language.s_languageChinese]

    def event(self):
        gd=self.gd
        eve=self.eve
        lan=self.lan
        helper = self.helper
        gesture.renew(gd, eve)
        gt=gesture.get()

        tag = gt.tagReleased

        if tag == 2 or tag == 4 or tag == 3:
            if tag == common.CL_TAG_CIRCLE_EN:
                lan.switch(lan.LANGUAGE_EN)
            elif tag == common.CL_TAG_CIRCLE_DE:
                lan.switch(lan.LANGUAGE_GM)
            else:
                lan.switch(lan.LANGUAGE_CN)

            self.start_ani_out = 1
            common.SS_START = helper.current_milli_time()

        if (self.start_ani_out == 1):
            if (self.x_ani <= -self.CL_CIRCLE[0]["width"] * 0.5):
                return 0 #end this page
            
    def draw(self):
        gd=self.gd
        eve=self.eve
        helper=self.helper
        lan=self.lan

        if self.event() == 0:
            return 0 #drawing ends

        self.gd.cmd_flashread(helper.RAM_G_SIZE - common.SS_FRAME_SIZE, self.frame_addr, common.SS_FRAME_SIZE)
        self.gd.cmd_setfont2(common.HF_TITLE, lan.FontTitle["xf_addr"] - lan.FontBegin["xf_addr"], 0)
        self.gd.cmd_setfont2(common.HF_LANG_CN, lan.FontLangCH["xf_addr"] - lan.FontBegin["xf_addr"], 0)

        self.gd.ColorA(85)
        self.gd.BitmapHandle(1)
        self.gd.cmd_setbitmap(helper.RAM_G_SIZE - common.SS_FRAME_SIZE, self.gd.ASTC_10x10, common.SCREEN_WIDTH, common.SCREEN_HEIGHT)
        self.gd.Begin(gd.BITMAPS)
        self.gd.Vertex2f(0, 0)
        self.gd.End()
        self.gd.ColorA(255)

        self.frame_index+=1
        self.frame_addr += common.SS_FRAME_SIZE

        if (self.frame_index >= common.SS_FRAME_COUNT) :
            self.frame_index = 0
            self.frame_addr = common.SS_FLASH_ADDR_FRAME_0

        # Draw text
        self.gd.ColorRGB(0, 0, 0 )
        self.gd.cmd_text(common.X_HEADER, common.Y_HEADER, common.HF_TITLE, 0, lan.lan_active.s_chooseLanguage)
        self.gd.ColorRGB(255, 255, 255)

        # Draw circle
        GAP = 100
        x = self.CL_CIRCLE[0]["width"] * 3 + GAP * 2

        if self.start_ani_out == 0:
            self.x_ani += self.X_STEP
            self.y_ani += self.Y_STEP

            if (self.x_ani - self.CL_CIRCLE[0]["width"] >= 120):
                self.x_ani = self.CL_CIRCLE[0]["width"] + 120

                self.gd.ColorRGB(0, 0, 0)
                
                for i in range(common.CIRCLE_NUM):
                    handler=31
                    if i == 2:
                        handler=common.HF_LANG_CN
                    self.gd.cmd_text((int)(common.H_CENTER(x) + i * (self.CL_CIRCLE[i]["width"] + GAP) + self.CL_CIRCLE[i]["width"] * 0.5),
                                common.V_CENTER(self.CL_CIRCLE[i]["height"]) + self.CL_CIRCLE[i]["height"],
                                handler, gd.OPT_CENTERX, self.LANG[i])
                
                self.gd.ColorRGB(255, 255, 255)
        
            if (self.y_ani >= common.V_CENTER(self.CL_CIRCLE[2]["height"])):
                self.y_ani = common.V_CENTER(self.CL_CIRCLE[2]["height"])
        else:
            self.x_ani -= self.X_STEP
            self.y_ani += self.Y_STEP * 2;               
        
        helper.draw_image_with_tag(self.CL_CIRCLE[0], common.ADDR(self.CL_CIRCLE[0], self.CL_BEGIN),
            self.x_ani - self.CL_CIRCLE[0]["width"], common.V_CENTER(self.CL_CIRCLE[0]["height"]),
            self.CL_TAGS[0])
        
        helper.draw_image_with_tag(self.CL_CIRCLE[1], common.ADDR(self.CL_CIRCLE[1], self.CL_BEGIN),
            common.H_CENTER(self.CL_CIRCLE[1]["width"]), self.y_ani,
            self.CL_TAGS[1]);  
        
        helper.draw_image_with_tag(self.CL_CIRCLE[2], common.ADDR(self.CL_CIRCLE[2], self.CL_BEGIN),
            common.SCREEN_WIDTH - self.x_ani, common.V_CENTER(self.CL_CIRCLE[2]["height"]),
            self.CL_TAGS[2])

        return 1

    def deinit(self):
        pass

