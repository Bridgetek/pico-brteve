from brteve.brt_eve_bt817_8 import BrtEve
from brteve.brt_eve_common import f16, furmans

from .eve_helper import eve_helper
from .gesture import gesture
from . import language
from . import common

class page_transition():
    def init(self, eve: BrtEve, helper:eve_helper, lan:language.language):
        self.helper = helper
        self.eve = eve
        self.lan = lan

        self.TR_BEGIN            = {'addr':4096}
        self.TR_CHARGE_BUTTON    ={'addr':456768, 'size':63552, 'x':0, 'y':0, 'width':252,
            'height':252, 'format':self.eve.ASTC_4x4}
        self.TR_CHARGE_TEXT      ={'addr':520320, 'size':896  , 'x':0, 'y':0, 'width':24 ,
            'height':36 , 'format':self.eve.ASTC_4x4}
        self.TR_ROUND_ENERGY     ={'addr':521216, 'size':7104 , 'x':0, 'y':0, 'width':84 ,
            'height':84 , 'format':self.eve.ASTC_4x4}
        self.TR_EXIT_BUTTON      ={'addr':528320, 'size':25344, 'x':0, 'y':0, 'width':352,
            'height':72 , 'format':self.eve.ASTC_4x4}
        self.TR_QR_CODE          ={'addr':553664, 'size':23104, 'x':0, 'y':0, 'width':150,
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
        self.helper = 0
        self.eve = 0
        self.lan = 0
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

        self.BATTERY_CELL_X =[39, 90, 139, 191, 240]
        self.BATTERY_CELL_Y = 19

        self.TR_PREPARE = 0
        self.TR_ANI_SLIDER=1
        self.TR_WAIT_START=2
        self.TR_END=3
        self.page_state = self.TR_PREPARE
        self.SLIDER_X = 365
        self.angle = 0
        self.ROTATE_ANGLE = 1
        self.x_ani = 0
        self.x_car = 0
        self.start_ani = 0
        self.X_ANI_STEP = 8
        self.alpha=0
        self.alpha_step=0
        self.width_slider_ani= 0
        self.r_scale=0
        self.r_scale_step=0
        self.ENERGY_SCALE = 0.03

        self.angle = 0
        self.start_ani = 0
        self.x_ani = common.SCREEN_WIDTH - self.SLIDER_X + 50
        self.x_car = 0
        self.page_state = self.TR_PREPARE

        self.alpha = 255
        self.alpha_step = -5
        self.width_slider_ani= 0

        self.r_scale = 1.0
        self.r_scale_step = self.ENERGY_SCALE

        self.gesture = gesture()
    def event(self):
        eve=self.eve
        helper=self.helper
        self.gesture.renew(eve)

        gt=self.gesture.sGesture

        tag = gt.tagReleased
        if (tag == common.TR_TAG_CHARGE_BUTTON  and  self.page_state == self.TR_WAIT_START):
            self.page_state = self.TR_END
            common.SS_START = helper.current_milli_time()

        if (self.page_state == self.TR_END  and  self.SLIDER_X + self.x_ani >= 660):
            return 0 #end this page
        return 1

    def draw_energy(self):
        eve=self.eve

        self.r_scale += self.r_scale_step
        if self.r_scale >= 1.7:
            self.r_scale = 1.7
            self.r_scale_step = -self.ENERGY_SCALE
        elif self.r_scale <= 1.0:
            self.r_scale = 1.0
            self.r_scale_step = self.ENERGY_SCALE


        w = self.TR_CHARGE_TEXT["width"] * 2
        h = self.TR_CHARGE_TEXT["height"] * 2
        self.eve.cmd_setbitmap(common.ADDR(self.TR_CHARGE_TEXT, self.TR_BEGIN), eve.ASTC_4x4,
                self.TR_CHARGE_TEXT["width"], self.TR_CHARGE_TEXT["height"])
        self.eve.BitmapSize(eve.BILINEAR, eve.BORDER, eve.BORDER, w, h)
        self.eve.BitmapSizeH(w >> 9, h >> 9)

        self.eve.Begin(eve.BITMAPS)
        self.eve.SaveContext()

        self.eve.cmd_loadidentity()
        self.eve.cmd_scale(f16(self.r_scale), f16(self.r_scale))
        self.eve.cmd_translate(f16(1) , f16(1))
        self.eve.cmd_setmatrix()

        x = self.x_ani + common.SCREEN_WIDTH - self.TR_CHARGE_BUTTON["width"] * 0.5\
            - common.TR_RIGHT_BUTTON_MARGIN - 10\
            - self.TR_CHARGE_TEXT["width"] * (self.r_scale - 1) * 0.5
        y = common.Y_CENTER(self.TR_CHARGE_TEXT, 435) - self.TR_CHARGE_TEXT["height"]\
            * (self.r_scale - 1) * 0.5

        self.eve.Vertex2f((x), (y))
        self.eve.End()
        self.eve.RestoreContext()

    def draw(self):
        eve=self.eve
        helper=self.helper
        lan=self.lan

        if self.event() == 0:
            return 0 #drawing ends

        self.eve.cmd_setfont2(common.HF_TITLE, lan.FontTitle["xf_addr"]
            - lan.FontBegin["xf_addr"], 0)
        self.eve.cmd_setfont2(common.HF_SMALL, lan.FontSmall["xf_addr"]
            - lan.FontBegin["xf_addr"], 0)
        self.eve.cmd_setfont2(common.HF_NUMBER, lan.FontNumber["xf_addr"]
            - lan.FontBegin["xf_addr"], 0)

        # Draw Text: header
        self.eve.ColorRGB(0, 0, 0)
        self.eve.cmd_text(common.X_HEADER, common.Y_HEADER, common.HF_TITLE, 0,
            lan.lan_active.s_transactionEstimation)
        self.eve.ColorRGB(255, 255, 255)

        HF_START_CHARGING = 29
        y_start_charging = common.Y_CENTER(self.TR_CHARGE_BUTTON, 490) \
            + self.TR_CHARGE_BUTTON["height"] * 0.5 + 20
        if lan.lan_active == lan.lan_cn:
            HF_START_CHARGING = common.HF_TITLE
            y_start_charging += 15

        # print('self.page_state',self.page_state)

        if self.page_state == self.TR_PREPARE:
            #the car
            helper.draw_image(self.TR_CAR, common.ADDR(self.TR_CAR, self.TR_BEGIN),
                self.x_car, common.Y_CENTER(self.TR_CAR, 510))

            # start charing button without the text
            helper.draw_image(self.TR_CHARGE_BUTTON, common.ADDR(self.TR_CHARGE_BUTTON,
                self.TR_BEGIN),
                self.x_ani + common.SCREEN_WIDTH - self.TR_CHARGE_BUTTON["width"]
                - common.TR_RIGHT_BUTTON_MARGIN,
                common.Y_CENTER(self.TR_CHARGE_BUTTON, 490))

            # text: "Start charing" is an image
            helper.draw_image(self.TR_CHARGE_TEXT, common.ADDR(self.TR_CHARGE_TEXT, self.TR_BEGIN),
                self.x_ani + common.SCREEN_WIDTH - self.TR_CHARGE_BUTTON["width"] * 0.5
                - common.TR_RIGHT_BUTTON_MARGIN - 10,
                common.Y_CENTER(self.TR_CHARGE_TEXT, 435))

            self.eve.ColorRGB(0, 0, 0)
            self.eve.cmd_text(
                        self.x_ani + common.SCREEN_WIDTH - self.TR_CHARGE_BUTTON["width"] * 0.5
                        - common.TR_RIGHT_BUTTON_MARGIN, y_start_charging,
                        HF_START_CHARGING, eve.OPT_CENTER, lan.lan_active.s_startCharging)
            self.eve.ColorRGB(255, 255, 255)

            self.x_car -= 5
            self.x_ani -= self.X_ANI_STEP * 2
            if self.x_ani <= 0:
                self.x_ani = 0
                self.page_state = self.TR_ANI_SLIDER

            return 1
        elif self.page_state == self.TR_ANI_SLIDER:
            y_slider=0
            x_slider=0

            # the car
            helper.draw_image(self.TR_CAR, common.ADDR(self.TR_CAR, self.TR_BEGIN),
                self.x_ani - self.TR_CAR["width"] * 0.5, common.Y_CENTER(self.TR_CAR, 510))
            #start charing btn
            helper.draw_image(self.TR_CHARGE_BUTTON, common.ADDR(self.TR_CHARGE_BUTTON,
                self.TR_BEGIN),
                self.x_ani + common.SCREEN_WIDTH - self.TR_CHARGE_BUTTON["width"]
                - common.TR_RIGHT_BUTTON_MARGIN,
                common.Y_CENTER(self.TR_CHARGE_BUTTON, 490))
            # start charing text
            helper.draw_image(self.TR_CHARGE_TEXT, common.ADDR(self.TR_CHARGE_TEXT, self.TR_BEGIN),
                self.x_ani + common.SCREEN_WIDTH - self.TR_CHARGE_BUTTON["width"] * 0.5
                - common.TR_RIGHT_BUTTON_MARGIN - 10,
                common.Y_CENTER(self.TR_CHARGE_TEXT, 435))
            self.eve.ColorRGB(0, 0, 0)

            self.eve.cmd_text(
                self.x_ani + common.SCREEN_WIDTH - self.TR_CHARGE_BUTTON["width"] * 0.5
                - common.TR_RIGHT_BUTTON_MARGIN, y_start_charging,
                HF_START_CHARGING, eve.OPT_CENTER, lan.lan_active.s_startCharging)
            self.eve.ColorRGB(255, 255, 255)


            # Draw Slider
            self.eve.ColorRGB(219, 219, 219)
            self.eve.LineWidth(common.SLIDER_HEIGHT)
            self.eve.Begin(eve.LINES)

            for i in range(3):
                y_slider = (common.SLIDER_Y + i * common.SLIDER_GAP)
                x_slider = self.SLIDER_X + (common.SLIDER_WIDTH - self.width_slider_ani) * 0.5
                self.eve.Vertex2f(x_slider, y_slider)
                self.eve.Vertex2f(x_slider + self.width_slider_ani, y_slider)

            self.width_slider_ani+= 20

            if self.width_slider_ani>= common.SLIDER_WIDTH:
                self.page_state = self.TR_WAIT_START

            return 1
        else:
            # Draw Car
            helper.draw_image(self.TR_CAR, common.ADDR(self.TR_CAR, self.TR_BEGIN),
                self.x_ani - self.TR_CAR["width"] * 0.5, common.Y_CENTER(self.TR_CAR, 510))
            # Battery background
            helper.draw_image(self.TR_BATTERY, common.ADDR(self.TR_BATTERY, self.TR_BEGIN),
                common.SCREEN_WIDTH - self.TR_BATTERY["width"] - common.TR_RIGHT_MARGIN,
                common.Y_BATTERY)
            # Battery Red
            helper.draw_image(self.TR_BAT_RED, common.ADDR(self.TR_BAT_RED, self.TR_BEGIN),
                common.SCREEN_WIDTH - self.TR_BATTERY["width"] - common.TR_RIGHT_MARGIN
                + self.BATTERY_CELL_X[4], common.Y_BATTERY + self.BATTERY_CELL_Y)

            if self.page_state == self.TR_END:
                self.x_ani += self.X_ANI_STEP

        # Draw Charging Button Outter
        self.eve.SaveContext()
        self.eve.Tag(common.TR_TAG_CHARGE_BUTTON)
        self.eve.cmd_setbitmap(common.ADDR(self.TR_CHARGE_BUTTON, self.TR_BEGIN), eve.ASTC_4x4,
            self.TR_CHARGE_BUTTON["width"], self.TR_CHARGE_BUTTON["height"])
        self.eve.BitmapSize(eve.BILINEAR, eve.BORDER, eve.BORDER,
            self.TR_CHARGE_BUTTON["width"], self.TR_CHARGE_BUTTON["height"])
        self.eve.BitmapSizeH(self.TR_CHARGE_BUTTON["width"] >> 9,
            self.TR_CHARGE_BUTTON["height"] >> 9)

        self.eve.Begin(eve.BITMAPS)
        self.eve.cmd_loadidentity()

        tx=(self.TR_CHARGE_BUTTON["width"] - 1) / 2
        ty=(self.TR_CHARGE_BUTTON["height"] - 1) / 2
        self.eve.cmd_translate(f16(tx), f16(ty))

        self.eve.cmd_rotate(furmans(self.angle * 1 / 360))

        tx=-1 * (self.TR_CHARGE_BUTTON["width"] - 1) / 2
        ty=-1 * (self.TR_CHARGE_BUTTON["height"] - 1) / 2
        self.eve.cmd_translate(f16(tx), f16(ty))

        self.eve.cmd_setmatrix()
        self.eve.Vertex2f(
            (self.x_ani + common.SCREEN_WIDTH - self.TR_CHARGE_BUTTON["width"]
            - common.TR_RIGHT_BUTTON_MARGIN),
            (common.Y_CENTER(self.TR_CHARGE_BUTTON, 490)))
        self.eve.End()

        self.angle += self.ROTATE_ANGLE
        if self.angle >= 360:
            self.angle = 0

        self.eve.RestoreContext()

        # Draw Charging Button Inner
        self.draw_energy()
        self.eve.ColorRGB(0, 0, 0)
        self.eve.Tag(common.TR_TAG_CHARGE_BUTTON)
        self.eve.cmd_text(
            self.x_ani + common.SCREEN_WIDTH - self.TR_CHARGE_BUTTON["width"] * 0.5
            - common.TR_RIGHT_BUTTON_MARGIN, y_start_charging,
            HF_START_CHARGING, eve.OPT_CENTER, lan.lan_active.s_startCharging)
        self.eve.Tag(0)
        self.eve.ColorRGB(255, 255, 255)

        # Draw Slider
        self.eve.ColorRGB(219, 219, 219)
        self.eve.LineWidth(common.SLIDER_HEIGHT)
        self.eve.Begin(eve.LINES)

        for i in range(3):
            self.eve.Vertex2f((self.x_ani + self.SLIDER_X),
                (common.SLIDER_Y + i*common.SLIDER_GAP))
            self.eve.Vertex2f((self.x_ani + self.SLIDER_X + common.SLIDER_WIDTH),
                (common.SLIDER_Y + i * common.SLIDER_GAP))

        self.eve.ColorRGB(104, 167, 22)
        self.eve.Vertex2f((self.x_ani + self.SLIDER_X), (common.SLIDER_Y))
        self.eve.Vertex2f((self.x_ani + self.SLIDER_X + common.SLIDER_WIDTH * 0.1),
            (common.SLIDER_Y))
        self.eve.End()

        # Draw Number
        common.HF_VALUE = common.HF_NUMBER
        self.eve.ColorRGB(0, 0, 0)
        self.eve.cmd_setfont2(common.HF_TITLE, lan.FontTitle["xf_addr"]
            - lan.FontBegin["xf_addr"], 0)
        # Energy
        self.eve.cmd_number(self.x_ani + self.SLIDER_X, common.SLIDER_Y - 55,
            common.HF_VALUE, 0, 25)
        self.eve.cmd_number(self.x_ani + self.SLIDER_X + common.SLIDER_WIDTH,
            common.SLIDER_Y - 55, 31, eve.OPT_RIGHTX, 250)

        # Time
        self.eve.cmd_number(self.x_ani + self.SLIDER_X,
            common.SLIDER_Y + common.SLIDER_GAP - 55, common.HF_VALUE, 0, 0)
        self.eve.cmd_number(self.x_ani + self.SLIDER_X + common.SLIDER_WIDTH,
            common.SLIDER_Y + common.SLIDER_GAP - 55, 31, eve.OPT_RIGHTX, 120)

        # Cost
        self.eve.cmd_number(self.x_ani + self.SLIDER_X,
            common.SLIDER_Y + 2 * common.SLIDER_GAP - 55, common.HF_VALUE, 0, 0)
        self.eve.cmd_number(self.x_ani + self.SLIDER_X + common.SLIDER_WIDTH,
            common.SLIDER_Y + 2 * common.SLIDER_GAP - 55, 31, eve.OPT_RIGHTX, 32)

        # Draw Unit
        font_size = common.HF_SMALL
        unit_gap = 12

        if lan.lan_active == lan.lan_cn:
            unit_gap = 15

        self.eve.ColorRGB(150, 150, 150)
        self.eve.cmd_text(self.x_ani + self.SLIDER_X, common.SLIDER_Y + unit_gap, font_size, 0,
            lan.lan_active.s_energy)
        self.eve.cmd_text(self.x_ani + self.SLIDER_X + common.SLIDER_WIDTH,
            common.SLIDER_Y + unit_gap, font_size, eve.OPT_RIGHTX, lan.lan_active.s_kWh)

        self.eve.cmd_text(self.x_ani + self.SLIDER_X,
            common.SLIDER_Y + common.SLIDER_GAP + unit_gap, font_size, 0, lan.lan_active.s_time)
        self.eve.cmd_text(self.x_ani + self.SLIDER_X + common.SLIDER_WIDTH,
            common.SLIDER_Y + common.SLIDER_GAP + unit_gap, font_size, eve.OPT_RIGHTX,
            lan.lan_active.s_minutes)

        self.eve.cmd_text(self.x_ani + self.SLIDER_X,
            common.SLIDER_Y + 2 * common.SLIDER_GAP + unit_gap,
            font_size, 0, lan.lan_active.s_cost)
        self.eve.cmd_text(self.x_ani + self.SLIDER_X + common.SLIDER_WIDTH,
            common.SLIDER_Y + 2 * common.SLIDER_GAP + unit_gap, font_size,
            eve.OPT_RIGHTX, lan.lan_active.s_currency)

        return 1

    def deinit(self):
        pass
