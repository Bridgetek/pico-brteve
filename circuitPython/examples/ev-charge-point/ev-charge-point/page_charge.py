from brteve.brt_eve_bt817_8 import BrtEve
from brteve.brt_eve_common import f16

from .eve_helper import eve_helper
from .gesture import gesture
from . import language
from . import common

class page_charge():
    def init(self, eve: BrtEve, helper:eve_helper, lan:language.language):
        self.eve = eve
        self.helper = helper
        self.eve = eve
        self.lan = lan

        self.TR_BEGIN            = {'addr':4096}
        self.TR_CAR              = {'addr':320192, 'size':81600, 'x':0, 'y':0, 'width':600,
            'height':306, 'format':eve.ASTC_6x6}
        self.TR_BATTERY          = {'addr':401792, 'size':45632, 'x':0, 'y':0, 'width':300,
            'height':152, 'format':eve.ASTC_4x4}
        self.TR_BAT_GREEN        = {'addr':447424, 'size':4672 , 'x':0, 'y':0, 'width':40 ,
            'height':116, 'format':eve.ASTC_4x4}
        self.TR_BAT_RED          = {'addr':452096, 'size':4672 , 'x':0, 'y':0, 'width':40 ,
            'height':116, 'format':eve.ASTC_4x4}
        self.TR_CHARGE_BUTTON    = {'addr':456768, 'size':63552, 'x':0, 'y':0, 'width':252,
            'height':252, 'format':eve.ASTC_4x4}
        self.TR_CHARGE_TEXT      = {'addr':520320, 'size':896  , 'x':0, 'y':0, 'width':24 ,
            'height':36 , 'format':eve.ASTC_4x4}
        self.TR_ROUND_ENERGY     = {'addr':521216, 'size':7104 , 'x':0, 'y':0, 'width':84 ,
            'height':84 , 'format':eve.ASTC_4x4}
        self.TR_EXIT_BUTTON      = {'addr':528320, 'size':25344, 'x':0, 'y':0, 'width':352,
            'height':72 , 'format':eve.ASTC_4x4}
        self.TR_QR_CODE          = {'addr':553664, 'size':23104, 'x':0, 'y':0, 'width':150,
            'height':150, 'format':eve.ASTC_4x4}
        self.energy_x = self.round_energy_x + self.TR_ROUND_ENERGY["width"]

    def __init__(self):
        self.eve = 0
        self.helper = 0
        self.eve = 0
        self.lan = 0

        self.TR_BEGIN            = 0
        self.TR_BATTERY          = 0
        self.TR_CAR              = 0
        self.TR_BAT_GREEN        = 0
        self.TR_BAT_RED          = 0
        self.TR_CHARGE_BUTTON    = 0
        self.TR_CHARGE_TEXT      = 0
        self.TR_ROUND_ENERGY     = 0
        self.TR_EXIT_BUTTON      = 0
        self.TR_QR_CODE          = 0

        self.BATTERY_CELL_X =[39, 90, 139, 191, 240]
        self.BATTERY_CELL_Y = 19

        self.TR_RIGHT_MARGIN          =   (150)
        self.TR_RIGHT_BUTTON_MARGIN   =   (50)
        self.SLIDER_X = 660
        self.MAX_ENERGY = 250
        self.MAX_TIME = 120
        self.MAX_COST = 32
        self.MAX_FACTOR = 8
        self.charge_energy = 0
        self.charge_time = 0
        self.charge_cost = 0
        self.charge_factor = 0
        self.factor_count = 0
        self.round_energy_x = 400
        self.energy_x = 0
        self.s_charging = 0
        self.s_energy = 0
        self.s_kWh = 0
        self.s_time = 0
        self.s_minutes = 0
        self.s_cost = 0
        self.s_currency = 0
        self.x_ani = 0
        self.start_ani = 0
        self.X_ANI_STEP = 8
        self.y_ani = 0
        self.start_y_ani = 0
        self.r_scale = 0
        self.r_scale_step = 0
        self.RE_SCALE = 0.02
        self.charge_energy = 25
        self.charge_time = self.MAX_TIME * 0.09
        self.charge_cost = self.MAX_COST * 0.095
        self.charge_factor = 0.15
        self.factor_count = 0
        self.x_ani = 0
        self.start_ani = 0
        self.y_ani = 0
        self.start_y_ani = 0
        self.r_scale = 1.0
        self.r_scale_step = self.RE_SCALE
        self.gesture = gesture()

    def event(self):
        eve=self.eve
        helper=self.helper
        self.gesture.renew(eve)

        gt=self.gesture.sGesture

        common.SS_START = helper.current_milli_time()

        if gt.isTouch:
            if self.factor_count <= self.MAX_FACTOR:
                self.factor_count+=1
                self.charge_factor += 0.025 * self.factor_count

        if (self.charge_energy >= self.MAX_ENERGY-1  and
            self.charge_time >= self.MAX_TIME-1  and
            self.charge_cost >= self.MAX_COST-1  and
            self.start_ani == 0):
            self.start_ani = 1

        if self.x_ani >= common.H_CENTER(self.TR_CAR["width"]):
            self.start_y_ani = 1

        if self.y_ani + common.Y_CENTER(self.TR_CAR, 510) <= 100:
            return 0 #end this page
        return 1

    def draw_round_energy(self):
        eve=self.eve

        self.r_scale += self.r_scale_step
        if self.r_scale >= 1.35:
            self.r_scale = 1.35
            self.r_scale_step = -self.RE_SCALE
        elif self.r_scale <= 1.0:
            self.r_scale = 1.0
            self.r_scale_step = self.RE_SCALE

        if self.start_ani:
            self.r_scale_step = 0.0

        w = self.TR_ROUND_ENERGY["width"] * 2
        h = (int)(self.TR_ROUND_ENERGY["height"] * (1 + self.r_scale * 0.5))

        self.eve.cmd_setbitmap(common.ADDR(self.TR_ROUND_ENERGY, self.TR_BEGIN), eve.ASTC_4x4,
                self.TR_ROUND_ENERGY["width"], self.TR_ROUND_ENERGY["height"])
        self.eve.BitmapSize(eve.BILINEAR, eve.BORDER, eve.BORDER, w, h)
        self.eve.BitmapSizeH(w >> 9, h >> 9)

        self.eve.Begin(eve.BITMAPS)
        self.eve.SaveContext()

        self.eve.cmd_loadidentity()
        self.eve.cmd_scale(f16(self.r_scale), f16(self.r_scale))
        self.eve.cmd_translate(f16(1) , f16(1))
        self.eve.cmd_setmatrix()
        self.eve.Vertex2f((self.round_energy_x - self.TR_ROUND_ENERGY["width"]
            * (self.r_scale - 1) * 0.5),
            (self.y_ani + common.Y_BATTERY + (self.TR_BATTERY["height"]
            - self.TR_ROUND_ENERGY["height"]) * 0.5
            - self.TR_ROUND_ENERGY["height"] * (self.r_scale - 1) * 0.5))
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

        # Draw Text: header and footer
        self.eve.ColorRGB(0, 0, 0)
        self.eve.cmd_text(common.X_HEADER, common.Y_HEADER, common.HF_TITLE, 0,
            lan.lan_active.s_charging)
        self.eve.ColorRGB(255, 255, 255)

        # Draw Car
        helper.draw_image(self.TR_CAR, common.ADDR(self.TR_CAR, self.TR_BEGIN),
            self.x_ani + 296 - self.TR_CAR["width"] * 0.5,
        self.y_ani + common.Y_CENTER(self.TR_CAR, 510))

        # Battery background
        bat_x = common.SCREEN_WIDTH - self.TR_BATTERY["width"] - self.TR_RIGHT_MARGIN
        helper.draw_image(self.TR_BATTERY, common.ADDR(self.TR_BATTERY, self.TR_BEGIN),
            bat_x, self.y_ani + common.Y_BATTERY)

        # Battery Red
        helper.draw_image(self.TR_BAT_RED, common.ADDR(self.TR_BAT_RED, self.TR_BEGIN),
            common.SCREEN_WIDTH - self.TR_BATTERY["width"] - self.TR_RIGHT_MARGIN
            + self.BATTERY_CELL_X[4],
            self.y_ani + common.Y_BATTERY + self.BATTERY_CELL_Y)

        # Draw line between Round Energy and Battery
        self.eve.ColorRGB(104, 167, 22)
        self.eve.LineWidth(2 )#*16)
        self.eve.Begin(eve.LINES)
        self.eve.Vertex2f((self.round_energy_x + self.TR_ROUND_ENERGY["width"]),
            (self.y_ani + common.Y_BATTERY + self.TR_BATTERY["height"] * 0.5))
        self.eve.Vertex2f((bat_x), (self.y_ani + common.Y_BATTERY
            + self.TR_BATTERY["height"] * 0.5))
        self.eve.End()
        self.eve.ColorRGB(255, 255, 255)

        # Draw Round Energy
        self.draw_round_energy()

        # Draw charging button
        if self.start_ani == 0:

            self.eve.ColorRGB(255, 255, 255)
            self.eve.LineWidth(2 )#* 16)
            self.eve.Begin(eve.LINES)
            self.eve.Vertex2f((self.energy_x),      (self.y_ani + common.Y_BATTERY
                + self.TR_BATTERY["height"] * 0.5))
            self.eve.Vertex2f((self.energy_x + 10), (self.y_ani + common.Y_BATTERY
                + self.TR_BATTERY["height"] * 0.5))
            self.eve.End()

            self.energy_x += self.charge_factor * 20
            if self.energy_x >= (bat_x - 10):
                self.energy_x = self.round_energy_x + self.TR_ROUND_ENERGY["width"]

        # Draw Slider
        x_ani_slider = self.x_ani * 2

        self.eve.ColorRGB(219, 219, 219)
        self.eve.LineWidth(common.SLIDER_HEIGHT)
        self.eve.Begin(eve.LINES)

        for i in range(3):
            self.eve.Vertex2f((x_ani_slider + self.SLIDER_X),
                (common.SLIDER_Y + i * common.SLIDER_GAP))
            self.eve.Vertex2f((x_ani_slider + self.SLIDER_X + common.SLIDER_WIDTH),
                (common.SLIDER_Y + i * common.SLIDER_GAP))

        slider_x = 0
        self.charge_energy += self.charge_factor
        self.charge_time += self.charge_factor * self.MAX_TIME / self.MAX_ENERGY
        self.charge_cost += self.charge_factor * self.MAX_COST / self.MAX_ENERGY

        if self.charge_energy > self.MAX_ENERGY:
            self.charge_energy = self.MAX_ENERGY

        if self.charge_time > self.MAX_TIME:
            self.charge_time = self.MAX_TIME

        if self.charge_cost > self.MAX_COST:
            self.charge_cost = self.MAX_COST

        # sync with Bar Type LCD
        slider_x = self.SLIDER_X + self.charge_energy * common.SLIDER_WIDTH / self.MAX_ENERGY
        self.eve.ColorRGB(104, 167, 22)
        self.eve.Vertex2f((x_ani_slider + self.SLIDER_X), (common.SLIDER_Y))
        self.eve.Vertex2f((x_ani_slider + slider_x), (common.SLIDER_Y))

        slider_x = self.SLIDER_X + self.charge_time * common.SLIDER_WIDTH / self.MAX_TIME
        self.eve.ColorRGB(0, 85, 255)
        self.eve.Vertex2f((x_ani_slider + self.SLIDER_X), (common.SLIDER_Y + common.SLIDER_GAP))
        self.eve.Vertex2f((x_ani_slider + slider_x), (common.SLIDER_Y + common.SLIDER_GAP))

        slider_x = self.SLIDER_X + self.charge_cost * common.SLIDER_WIDTH / self.MAX_COST
        self.eve.ColorRGB(255, 0, 127)
        self.eve.Vertex2f((x_ani_slider + self.SLIDER_X), (common.SLIDER_Y + 2
            * common.SLIDER_GAP))
        self.eve.Vertex2f((x_ani_slider + slider_x), (common.SLIDER_Y + 2 * common.SLIDER_GAP))

        self.eve.End()

        # Battery Green
        self.eve.ColorRGB(255, 255, 255)
        for i in range((int)(self.charge_energy/50)):
            helper.draw_image(self.TR_BAT_GREEN, common.ADDR(self.TR_BAT_GREEN, self.TR_BEGIN),
                        common.SCREEN_WIDTH - self.TR_BATTERY["width"] -
                        self.TR_RIGHT_MARGIN + self.BATTERY_CELL_X[4 - i], self.y_ani
                        + common.Y_BATTERY + self.BATTERY_CELL_Y)

        # Draw Number
        HF_VALUE = common.HF_NUMBER
        HF_UNIT = 31

        self.eve.ColorRGB(0, 0, 0)
        # Energy
        self.eve.cmd_number(x_ani_slider + self.SLIDER_X,  common.SLIDER_Y - 55,
            HF_VALUE, 0, self.charge_energy)
        self.eve.cmd_number(x_ani_slider + self.SLIDER_X + common.SLIDER_WIDTH ,
            common.SLIDER_Y - 55, HF_UNIT, eve.OPT_RIGHTX, self.MAX_ENERGY)

        # Time
        self.eve.cmd_number(x_ani_slider + self.SLIDER_X,  common.SLIDER_Y
            + common.SLIDER_GAP - 55, HF_VALUE, 0, self.charge_time)
        self.eve.cmd_number(x_ani_slider + self.SLIDER_X + common.SLIDER_WIDTH,
            common.SLIDER_Y + common.SLIDER_GAP - 55, HF_UNIT, eve.OPT_RIGHTX, self.MAX_TIME)

        # Cost
        self.eve.cmd_number(x_ani_slider + self.SLIDER_X,  common.SLIDER_Y + 2
            * common.SLIDER_GAP - 55, HF_VALUE, 0, self.charge_cost)
        self.eve.cmd_number(x_ani_slider + self.SLIDER_X + common.SLIDER_WIDTH,
            common.SLIDER_Y + 2 * common.SLIDER_GAP - 55, HF_UNIT, eve.OPT_RIGHTX, self.MAX_COST)

        # Draw Unit
        font_size = common.HF_SMALL
        unit_gap = 12
        if lan.lan_active == lan.lan_cn:
            unit_gap = 15

        self.eve.ColorRGB(150, 150, 150)
        self.eve.cmd_text(x_ani_slider + self.SLIDER_X, common.SLIDER_Y + unit_gap,
            font_size, 0, lan.lan_active.s_energy)
        self.eve.cmd_text(x_ani_slider + self.SLIDER_X +common.SLIDER_WIDTH,
            common.SLIDER_Y + unit_gap, font_size, eve.OPT_RIGHTX, lan.lan_active.s_kWh)

        self.eve.cmd_text(x_ani_slider + self.SLIDER_X, common.SLIDER_Y
            + common.SLIDER_GAP + unit_gap, font_size, 0, lan.lan_active.s_time)
        self.eve.cmd_text(x_ani_slider + self.SLIDER_X +common.SLIDER_WIDTH,
            common.SLIDER_Y + common.SLIDER_GAP + unit_gap, font_size, eve.OPT_RIGHTX,
            lan.lan_active.s_minutes)

        self.eve.cmd_text(x_ani_slider + self.SLIDER_X, common.SLIDER_Y + 2
            * common.SLIDER_GAP + unit_gap, font_size, 0, lan.lan_active.s_cost)
        self.eve.cmd_text(x_ani_slider + self.SLIDER_X +common.SLIDER_WIDTH,
            common.SLIDER_Y + 2 * common.SLIDER_GAP + unit_gap, font_size, eve.OPT_RIGHTX,
            lan.lan_active.s_currency)

        if (self.start_ani != 0  and  self.start_y_ani == 0):
            self.x_ani += self.X_ANI_STEP

        if self.start_y_ani!=0:
            self.y_ani -= self.X_ANI_STEP

        return 1

    def deinit(self):
        pass
