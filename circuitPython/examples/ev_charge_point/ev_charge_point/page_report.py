from brteve.brt_eve_bt817_8 import BrtEve

from .eve_helper import eve_helper
from .gesture import gesture
from . import language
from . import common

class page_report():
    def init(self, eve: BrtEve, helper:eve_helper, lan:language.language):

        self.eve = eve
        self.helper = helper
        self.eve = eve
        self.lan = lan

        self.TR_BEGIN            = {'addr':4096}
        self.TR_CHARGE_BUTTON    = {'addr':456768, 'size':63552, 'x':0, 'y':0, 'width':252,
            'height':252, 'format':self.eve.ASTC_4x4}
        self.TR_CHARGE_TEXT      = {'addr':520320, 'size':896  , 'x':0, 'y':0, 'width':24 ,
            'height':36 , 'format':self.eve.ASTC_4x4}
        self.TR_ROUND_ENERGY     = {'addr':521216, 'size':7104 , 'x':0, 'y':0, 'width':84 ,
            'height':84 , 'format':self.eve.ASTC_4x4}
        self.TR_EXIT_BUTTON      = {'addr':528320, 'size':25344, 'x':0, 'y':0, 'width':352,
            'height':72 , 'format':self.eve.ASTC_4x4}
        self.TR_QR_CODE          = {'addr':553664, 'size':23104, 'x':0, 'y':0, 'width':150,
            'height':150, 'format':self.eve.ASTC_4x4}
        self.TR_CAR              = {'addr':320192, 'size':81600, 'x':0, 'y':0, 'width':600,
            'height':306, 'format':self.eve.ASTC_6x6}
        self.TR_BATTERY          = {'addr':401792, 'size':45632, 'x':0, 'y':0, 'width':300,
            'height':152, 'format':self.eve.ASTC_4x4}
        self.TR_BAT_GREEN        = {'addr':447424, 'size':4672 , 'x':0, 'y':0, 'width':40 ,
            'height':116, 'format':self.eve.ASTC_4x4}
        self.TR_BAT_RED          = {'addr':452096, 'size':4672 , 'x':0, 'y':0, 'width':40 ,
            'height':116, 'format':self.eve.ASTC_4x4}

    def __init__(self):
        self.eve = 0
        self.helper = 0
        self.eve = 0
        self.lan = 0

        self.start_ani_out=0
        self.y_car=0
        self.rp_alpha=0

        self.start_ani_out = 0
        self.y_car = 100
        self.rp_alpha = 255

        self.TR_BEGIN            = 0
        self.TR_CHARGE_BUTTON    = 0
        self.TR_CHARGE_TEXT      = 0
        self.TR_ROUND_ENERGY     = 0
        self.TR_EXIT_BUTTON      = 0
        self.TR_QR_CODE          = 0
        self.TR_CAR              = 0
        self.TR_BATTERY          = 0
        self.TR_BAT_GREEN        = 0
        self.TR_BAT_RED          = 0
        self.gesture = gesture()

    def event(self):
        eve=self.eve
        self.gesture.renew(eve)

        gt=self.gesture.sGesture

        tag = gt.tagReleased

        if tag == common.RE_TAG_EXIT_BUTTON:
            self.start_ani_out = 1

        if self.y_car <= -self.TR_CAR["height"]:
            return 0 #end this page
        return 1

    def draw(self):
        eve=self.eve
        lan=self.lan
        helper=self.helper

        if self.event() == 0:
            return 0 #drawing ends

        x = 0
        y = 0

        self.eve.cmd_setfont2(common.HF_TITLE, lan.FontTitle["xf_addr"]
            - lan.FontBegin["xf_addr"], 0)
        self.eve.cmd_setfont2(common.HF_SMALL, lan.FontSmall["xf_addr"]
            - lan.FontBegin["xf_addr"], 0)
        self.eve.cmd_setfont2(common.HF_NUMBER, lan.FontNumber["xf_addr"]
            - lan.FontBegin["xf_addr"], 0)

        # Draw Car
        helper.draw_image(self.TR_CAR, common.ADDR(self.TR_CAR, self.TR_BEGIN),
        common.H_CENTER(self.TR_CAR["width"]), self.y_car)

        if self.start_ani_out:
            self.y_car -= 10
            self.rp_alpha -= 3
            self.rp_alpha = max(self.rp_alpha, 0)

        self.eve.ColorA(self.rp_alpha)

        # Draw QR Code
        helper.draw_image(self.TR_QR_CODE, common.ADDR(self.TR_QR_CODE, self.TR_BEGIN),
            common.SCREEN_WIDTH - self.TR_QR_CODE["width"] - 100,
            100 + (self.TR_CAR["height"] - self.TR_QR_CODE["height"]) * 0.5)

        # Draw Text: header
        self.eve.ColorRGB(0, 0, 0)
        self.eve.cmd_text(common.X_HEADER, common.Y_HEADER, common.HF_TITLE, 0,
            lan.lan_active.s_report)
        self.eve.ColorRGB(255, 255, 255)

        # Draw Exit
        x = common.H_CENTER(self.TR_EXIT_BUTTON["width"])
        y = 700

        self.eve.ColorRGB(0, 85, 255)
        self.eve.cmd_text(x + self.TR_EXIT_BUTTON["width"] * 0.5, y +
        self.TR_EXIT_BUTTON["height"] * 0.5, common.HF_TITLE, eve.OPT_CENTER,
            lan.lan_active.s_exit)
        helper.draw_image_with_tag(self.TR_EXIT_BUTTON,
        common.ADDR(self.TR_EXIT_BUTTON, self.TR_BEGIN), x, y - 3, common.RE_TAG_EXIT_BUTTON)

        # Draw Report
        x = 380
        y = 500
        HF_LEGEND = 30

        if lan.lan_active == lan.lan_cn:
            HF_LEGEND = common.HF_SMALL

        self.eve.ColorRGB(150, 150, 150)
        self.eve.cmd_text(x, y - 3, HF_LEGEND, eve.OPT_RIGHTX, lan.lan_active.s_battery)
        self.eve.cmd_text(common.SCREEN_WIDTH - x, y - 3, HF_LEGEND, eve.OPT_RIGHTX,
            lan.lan_active.s_batteryHealth)
        self.eve.cmd_text(x, y + 67, HF_LEGEND, eve.OPT_RIGHTX, lan.lan_active.s_chargingTime)
        self.eve.cmd_text(common.SCREEN_WIDTH - x, y + 67, HF_LEGEND, eve.OPT_RIGHTX,
            lan.lan_active.s_totalCost)

        self.eve.ColorRGB(0, 85, 255)
        self.eve.cmd_text(x + 10, y - 12, common.HF_NUMBER, 0, "100")
        self.eve.ColorRGB(0, 0, 0)
        self.eve.cmd_text(x + 110, y - 12, 31, 0, "%")

        self.eve.cmd_number(x + 10, y + 57, common.HF_NUMBER, 0, 120)
        self.eve.cmd_text(x + 110, y + 67, HF_LEGEND, 0, lan.lan_active.s_minutes_report)

        self.eve.ColorRGB(0, 85, 255)
        self.eve.cmd_text(common.SCREEN_WIDTH - x + 15, y - 9, common.HF_TITLE, 0,
            lan.lan_active.s_healthy)

        self.eve.ColorRGB(0, 0, 0)
        self.eve.cmd_text(common.SCREEN_WIDTH - x + 15, y + 55, 31, 0, "$")

        self.eve.cmd_text(common.SCREEN_WIDTH - x + 45, y + 57, common.HF_NUMBER, 0, "32")

        return 1

    def deinit(self):
        pass
