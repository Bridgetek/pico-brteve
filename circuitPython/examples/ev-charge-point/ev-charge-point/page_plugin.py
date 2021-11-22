from brteve.brt_eve_bt817_8 import BrtEve

from .eve_helper import eve_helper
from .gesture import gesture
from . import language
from . import common

class page_plugin():
    def init(self, eve: BrtEve, helper:eve_helper, lan:language.language):
        self.eve = eve
        self.helper = helper
        self.eve = eve
        self.lan = lan

        self.PL_BEGIN ={'addr':4096}
        self.PL_MALE         ={'addr':223744, 'size':45760, 'x':0, 'y':0, 'width':325,
            'height':220, 'format':self.eve.ASTC_5x5}
        self.PL_FEMALE       ={'addr':269504, 'size':25344, 'x':0, 'y':0, 'width':180,
            'height':220, 'format':self.eve.ASTC_5x5}
        self.PL_FEMALE_GREEN ={'addr':294848, 'size':25344, 'x':0, 'y':0, 'width':180,
            'height':220, 'format':self.eve.ASTC_5x5}
        self.TR_CAR          ={'addr':320192, 'size':81600, 'x':0, 'y':0, 'width':600,
            'height':306, 'format':self.eve.ASTC_6x6}
        self.TR_BEGIN        ={"addr":4096}

        self.LEFT_STONE = common.SCREEN_WIDTH * 0.5 - self.PL_MALE["width"]\
            - self.PL_MALE["width"] * 0.35
        self.RIGHT_STONE = common.SCREEN_WIDTH * 0.5 - self.PL_MALE["width"]
        self.x = -self.PL_MALE["width"]
        self.x_start = self.LEFT_STONE

    def __init__(self):
        self.eve = 0
        self.helper = 0
        self.eve = 0
        self.lan = 0

        self.STEP_X = 10

        self.PL_BEGIN        = 0
        self.PL_MALE         = 0
        self.PL_FEMALE       = 0
        self.PL_FEMALE_GREEN = 0
        self.TR_CAR          = 0
        self.TR_BEGIN        = 0

        self.PLUG_PREPARE = 0
        self.PLUGGING=1
        self.PLUGGED=2
        self.PLUG_END=3

        self.page_state = self.PLUG_PREPARE
        self.LEFT_STONE = 0
        self.RIGHT_STONE = 0

        self.x_start = 0
        self.x_move = 5

        self.x = 0

        self.f_x_start = common.SCREEN_WIDTH * 0.5
        self.f_x = self.f_x_start
        self.f_x_move = 2

        self.F_LEFT_STONE = self.f_x_start
        self.F_RIGHT_STONE = self.f_x_start + self.f_x_move * 5

        self.female_ani_flag = 0

        self.f_x = common.SCREEN_WIDTH
        self.y_ani = 0

        self.count_frame = 0
        self.alpha_in = 0
        self.alpha_out = 255
        self.x_car = 0
        self.gesture = gesture()

    def event(self):
        eve=self.eve
        self.gesture.renew(eve)

        gt=self.gesture.sGesture

        if (gt.isTouch  and  self.page_state == self.PLUGGING):
            self.page_state = self.PLUGGED

    def draw(self):
        eve=self.eve
        helper=self.helper
        lan=self.lan

        if self.event() == 0:
            return 0 #drawing ends

        self.eve.cmd_setfont2(common.HF_TITLE, lan.FontTitle["xf_addr"]
            - lan.FontBegin["xf_addr"], 0)
        self.eve.cmd_setfont2(common.HF_BOTTOM, lan.FontBottom["xf_addr"]
            - lan.FontBegin["xf_addr"], 0)

        # Draw Text: footer
        if (self.page_state != self.PLUG_PREPARE  and  self.page_state != self.PLUG_END):
            self.eve.ColorRGB(0, 0, 0)
            self.eve.cmd_text((int)(common.SCREEN_WIDTH * 0.5), common.Y_FOOTER,
                common.HF_BOTTOM, eve.OPT_CENTERX, lan.lan_active.s_pleaseTouch)
            self.eve.ColorRGB(255, 255, 255)

        # Draw Plug Male and Female
        if self.page_state == self.PLUG_PREPARE:

            self.eve.ColorA(self.alpha_in)
            helper.draw_image(self.PL_MALE  , common.ADDR(self.PL_MALE  , self.PL_BEGIN),
                self.x  , common.V_CENTER(self.PL_MALE["height"]))
            helper.draw_image(self.PL_FEMALE, common.ADDR(self.PL_FEMALE, self.PL_BEGIN),
                self.f_x, common.V_CENTER(self.PL_FEMALE["height"]))
            self.eve.ColorA(255)

            self.alpha_in += 5
            self.alpha_in = min(self.alpha_in, 255)

            self.x += self.STEP_X
            self.f_x -= self.STEP_X * 1.2

            if self.x >= self.x_start:
                self.x = self.x_start
                if self.f_x <= self.f_x_start:
                    self.page_state = self.PLUGGING
                    self.f_x = self.f_x_start

        elif self.page_state == self.PLUGGING:
            self.eve.ColorRGB(0, 0, 0)
            self.eve.cmd_text(common.X_HEADER, common.Y_HEADER, common.HF_TITLE, 0,
                lan.lan_active.s_plugIn)
            self.eve.ColorRGB(255, 255, 255)

            # Male
            helper.draw_image(self.PL_MALE, common.ADDR(self.PL_MALE, self.PL_BEGIN),
                self.x, common.V_CENTER(self.PL_MALE["height"]))

            self.x += self.x_move
            if (self.x >= self.RIGHT_STONE or self.x <= self.LEFT_STONE):
                self.x_move = -self.x_move

            if self.x >= self.RIGHT_STONE:
                self.female_ani_flag = 1

            # Female
            f_img = self.PL_FEMALE
            if self.female_ani_flag:
                f_img = self.PL_FEMALE_GREEN


            helper.draw_image(f_img, common.ADDR(f_img, self.PL_BEGIN), self.f_x_start,
                common.V_CENTER(f_img["height"]))
            if self.female_ani_flag:
                self.f_x_start += self.f_x_move

            if self.f_x_start >= self.F_RIGHT_STONE:
                self.f_x_move = -self.f_x_move

            if self.f_x_start <= self.F_LEFT_STONE:
                self.f_x_move = abs(self.f_x_move)
                self.f_x_start = self.F_LEFT_STONE
                self.female_ani_flag = 0

        elif self.page_state == self.PLUGGED:
            self.eve.ColorRGB(0, 170, 255)
            self.eve.cmd_text(common.X_HEADER, common.Y_HEADER, common.HF_TITLE, 0,
                lan.lan_active.s_connected)
            self.eve.ColorRGB(255, 255, 255)

            # Male
            helper.draw_image(self.PL_MALE, common.ADDR(self.PL_MALE, self.PL_BEGIN),
                self.RIGHT_STONE + 48, common.V_CENTER(self.PL_MALE["height"]))

            # Female
            helper.draw_image(self.PL_FEMALE_GREEN, common.ADDR(self.PL_FEMALE_GREEN,
                self.PL_BEGIN), common.SCREEN_WIDTH * 0.5,
                common.V_CENTER(self.PL_FEMALE_GREEN["height"]))

            self.count_frame+=1
            if self.count_frame >= 30:
                self.page_state = self.PLUG_END
                self.x_car = -self.TR_CAR["width"]
                self.y_ani = common.V_CENTER(self.PL_MALE["height"])

        else:
            self.eve.ColorA(self.alpha_out)

            self.alpha_out -= 5
            self.alpha_out = max(self.alpha_out, 0)

            # Male
            helper.draw_image(self.PL_MALE, common.ADDR(self.PL_MALE, self.PL_BEGIN),
                self.RIGHT_STONE + 48, self.y_ani)
            # Female
            helper.draw_image(self.PL_FEMALE_GREEN, common.ADDR(self.PL_FEMALE_GREEN,
                self.PL_BEGIN), common.SCREEN_WIDTH * 0.5, self.y_ani)
            self.eve.ColorA(255)

            # Car
            helper.draw_image(self.TR_CAR, common.ADDR(self.TR_CAR, self.TR_BEGIN),
                self.x_car, common.Y_CENTER(self.TR_CAR, 510))

            self.y_ani += 15
            self.x_car += self.STEP_X

            if self.x_car >= 0:
                return 0 #end this page

        return 1

    def deinit(self):
        pass
