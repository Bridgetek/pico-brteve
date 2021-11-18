from brteve.brt_eve_bt817_8 import BrtEve

from .eve_helper import eve_helper
from .gesture import gesture
from . import language
from . import common

class page_authenticate():
    def init(self, eve: BrtEve, helper:eve_helper, lan:language.language):
        self.eve = eve
        self.helper = helper
        self.lan = lan

        self.ATH_BEGIN = {'addr':4096}
        self.ATH_TAP_POINT = {'addr':4096, 'size':47872, 'x':0, 'y':0, 'width':520,
            'height':368, 'format':eve.ASTC_8x8}
        self.ATH_CARD      = {'addr':51968, 'size':32832, 'x':0, 'y':0, 'width':456,
            'height':288, 'format':eve.ASTC_8x8}

        self.ATH_PAY = [
            {'addr':84800 ,  'size':7104, 'x':0, 'y':0, 'width':148, 'height':48,
                'format':eve.ASTC_4x4},
            {'addr':91904 ,  'size':4608, 'x':0, 'y':0, 'width':76 , 'height':60,
                'format':eve.ASTC_4x4},
            {'addr':96512 ,  'size':5568, 'x':0, 'y':0, 'width':116, 'height':48,
                'format':eve.ASTC_4x4},
            {'addr':102080,  'size':8128, 'x':0, 'y':0, 'width':184, 'height':44,
                'format':eve.ASTC_4x4},
            {'addr':110208,  'size':7360, 'x':0, 'y':0, 'width':108, 'height':68,
                'format':eve.ASTC_4x4},
        ]
        self.card_x_start = common.H_CENTER(self.ATH_CARD["width"])
        self.card_x = self.card_x_start
        self.LEFT_STONE = (int)(common.SCREEN_WIDTH * 0.5 - self.ATH_CARD["width"] * 0.75)
        self.RIGHT_STONE = (int)(common.SCREEN_WIDTH * 0.5 - self.ATH_CARD["width"] * 0.25)

    def __init__(self):
        self.eve = 0
        self.helper = 0
        self.lan = 0

        self.ATH_BEGIN = 0
        self.ATH_TAP_POINT = 0
        self.ATH_CARD = 0
        self.ATH_PAY = 0

        self.TAP_POINT = 0
        self.WHITE_CARD = 1
        self.AUTHENTICATED = 2
        self.MAX_CARD_ANI_COUNT = 4
        self.page_state = self.TAP_POINT
        self.card_x_start = 0
        self.card_x = self.card_x_start
        self.card_move = 10
        self.card_ani_count = 0
        self.LEFT_STONE = 0
        self.RIGHT_STONE = 0
        self.count_frame = 0
        self.alpha = 255
        self.alpha_step = -5
        self.pay_ani = 0
        self.gesture = gesture()

    def event(self):
        eve=self.eve
        helper = self.helper
        self.gesture.renew(eve)
        gt=self.gesture.sGesture
        tag = gt.tagReleased

        if tag == common.ATH_TAG_TAP_POINT:
            self.page_state = self.WHITE_CARD
            common.SS_START = helper.current_milli_time()

        if self.count_frame > 30:
            return 0 #end this page
        return 1

    def draw(self):
        helper=self.helper
        lan=self.lan

        if self.event() == 0:
            return 0 #drawing ends

        self.eve.cmd_setfont2(common.HF_TITLE, lan.FontTitle["xf_addr"]
            - lan.FontBegin["xf_addr"], 0)
        self.eve.cmd_setfont2(common.HF_BOTTOM, lan.FontBottom["xf_addr"]
            - lan.FontBegin["xf_addr"], 0)

        if self.page_state == self.TAP_POINT:
            # Draw header text
            self.eve.ColorRGB(0, 0, 0)
            self.eve.cmd_text(common.X_HEADER, common.Y_HEADER, common.HF_TITLE, 0,
                lan.lan_active.s_tapYourCard)
            self.eve.ColorRGB(255, 255, 255)

            # Draw Tap Point
            self.eve.ColorA(self.alpha)
            helper.draw_image_with_tag(self.ATH_TAP_POINT, common.ADDR(self.ATH_TAP_POINT,
                self.ATH_BEGIN),
                    common.H_CENTER(self.ATH_TAP_POINT["width"]),
                        common.V_CENTER(self.ATH_TAP_POINT["height"]), common.ATH_TAG_TAP_POINT)
            self.eve.ColorA(255)
            self.alpha += self.alpha_step
            if self.alpha >= 255:
                self.alpha = 255
                self.alpha_step = -10
                self.pay_ani+=1
                self.pay_ani %= common.PAY_NUM
            elif self.alpha <= 10:
                self.alpha = 10
                self.alpha_step = 5

        elif self.page_state == self.WHITE_CARD:
            # Draw header text
            self.eve.ColorRGB(0, 0, 0)
            self.eve.cmd_text(common.X_HEADER, common.Y_HEADER, common.HF_TITLE, 0,
                lan.lan_active.s_checkingTheCard)
            self.eve.ColorRGB(255, 255, 255)

            # Draw White Card
            helper.draw_image(self.ATH_CARD, common.ADDR(self.ATH_CARD, self.ATH_BEGIN),
                self.card_x, common.V_CENTER(self.ATH_CARD["height"]))
            self.card_x += self.card_move
            if (self.card_x >= self.RIGHT_STONE or self.card_x <= self.LEFT_STONE):
                self.card_move = -self.card_move
                self.card_ani_count +=1

            if (self.card_ani_count >= self.MAX_CARD_ANI_COUNT
                and self.card_x >= common.H_CENTER(self.ATH_CARD["width"])):
                self.page_state = self.AUTHENTICATED
                self.count_frame = 0
            self.pay_ani = common.PAY_NUM
            common.SS_START = helper.current_milli_time()

        else:
            self.eve.ColorRGB(0, 170, 255)
            self.eve.cmd_text(common.X_HEADER, common.Y_HEADER, common.HF_TITLE, 0,
                lan.lan_active.s_authenticated)
            self.eve.ColorRGB(255, 255, 255)

            # Draw White Card
            helper.draw_image(self.ATH_CARD, common.ADDR(self.ATH_CARD, self.ATH_BEGIN),
                self.card_x, common.V_CENTER(self.ATH_CARD["height"]))

            self.count_frame+=1

        # Draw Text "Pay with"
        hf_paywith = common.HF_TITLE
        if lan.lan_active == lan.lan_gm:
            hf_paywith = 30

        self.eve.ColorRGB(0, 0, 0)
        self.eve.cmd_text(common.ATH_PAYWITH_X, common.ATH_PAYWITH_Y, hf_paywith, 0,
            lan.lan_active.s_payWith)
        self.eve.ColorRGB(255, 255, 255)

        # Draw Pay Items
        pay_alpha = 255
        for i in range(common.PAY_NUM):
            if self.pay_ani == i:
                pay_alpha = self.alpha
            else:
                pay_alpha = 255

            self.eve.ColorA(pay_alpha)
            helper.draw_image(self.ATH_PAY[i], common.ADDR(self.ATH_PAY[i], self.ATH_BEGIN),
                (int)(common.ATH_PAY_ROOM * i + (common.ATH_PAY_ROOM - self.ATH_PAY[i]["width"])
                * 0.5),
                (int)(common.ATH_PAY_VERTICAL_CENTER - self.ATH_PAY[i]["height"] * 0.5))
            self.eve.ColorA(255)

        return 1

    def deinit(self):
        pass
