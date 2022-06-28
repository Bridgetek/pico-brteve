import time

from .helper import helper
from .gesture import gesture
from brteve.brt_eve_bt817_8 import BrtEve

APP_RATIO = 1.9
APP_PADDING_TOP = 10
class layout():
    def __init__(self, eve: BrtEve, helper: helper) -> None:
        self.eve = eve
        self.helper = helper

        self.portrait = 0
        self.screen_w = eve.lcd_width
        self.screen_h = eve.lcd_height

        #if self.eve.lcd_height < 600: # Portrait mode
        if self.portrait ==1 : # Portrait mode ,QF
            self.screen_w = eve.lcd_height
            self.screen_h = eve.lcd_width
            self.portrait = 1

        self.APP_H = self.screen_h - APP_PADDING_TOP * 2
        self.APP_W = self.APP_H / APP_RATIO
        self.APP_X = self.screen_w / 2 - self.APP_W / 2
        self.APP_Y = APP_PADDING_TOP
        self.MENU_W = self.APP_W / 7  # 5-->7

        self.images = {
                              #addr0           size1  w2  h3  tag4 scale5 x6                y7
            'unified.blob'  : [0      - 4096 , 4096 , 0,  0,   0,  0,     0, 0],
            'circle_72x72'  : [4096   - 4096 , 5184 , 72, 72,  0,  1,     0, 0],
            'circle_140x140': [17792  - 4096 , 19648, 140,140, 0,  1,     0, 0],
            'circle_92x92'  : [9280   - 4096 , 8512 , 92, 92,  0,  1,     0, 0],
            'circular_colorwheel'    : [128576 - 4096 , 63552 , 252, 252,  0,  1,     100  , 50      ],
            'fanOn'    : [227520 - 4096 , 35392 , 188, 188,  0,  1,     100  , 50      ],
            'fanOff'    : [192128 - 4096 , 35392 , 188, 188,  0,  1,     100  , 50      ],
        } 

        img = self.images['fanOn']
        eve.cmd_flashread(img[0], img[0]+4096, img[1])  
        img = self.images['fanOff']
        eve.cmd_flashread(img[0], img[0]+4096, img[1])
        self.imagesMCU = {
            # id                 location                                                         ramg address  width  height
            'm_active':['ui_lds/motion_sensor_active_COMPRESSED_RGBA_ASTC_4x4_KHR.raw',0,192,168],
            'm_inactive':['ui_lds/motion_sensor_inactive_COMPRESSED_RGBA_ASTC_4x4_KHR.raw',0,192,168],
            #'m_active':['ui_lds/motionSensor_active_s_188x188_COMPRESSED_RGBA_ASTC_4x4_KHR.raw',0,188,188],
            #'m_inactive':['ui_lds/motionSensor_inactive_s_188x188_COMPRESSED_RGBA_ASTC_4x4_KHR.raw',0,188,188],
        }

        address = 512*1024
        for index in self.imagesMCU:
            address = self.round_to_nearest(address, 4)
            self.imagesMCU[index][1] = address
            #print(address,self.imagesMCU[index])
            address = self.eve.write_file(address, self.imagesMCU[index][0])


        eve.finish()


    def round_to_nearest(self, n, m):
        return (n + m - 1) // m * m


    def draw_asset_MCU(self, tag, index, x, y, fm,scale=1):
        helper = self.helper
        self.helper.image_setup_scale(scale)
        helper.image_draw_from_ram_g(self.imagesMCU[index][1],
            x, y,
            self.imagesMCU[index][2], self.imagesMCU[index][3],
            fm, 0, tag, self.eve.OPT_DITHER)
        
    def draw_asset4(self, tag, index, x, y):
        helper = self.helper
        helper.image_draw_from_ram_g(self.images[index][0],
            x, y,
            self.images[index][2], self.images[index][3],
            self.eve.ASTC_4x4, 0, tag, self.eve.OPT_DITHER)


    def draw_asset(self, tag, index, x, y):
        helper = self.helper
        helper.image_draw_from_ram_g(self.images[index][1],
            x, y,
            self.images[index][2], self.images[index][3],
            self.eve.ASTC_8x8, 0, tag, self.eve.OPT_DITHER)
            #self.eve.ASTC_8x8, 0, tag, 0)

    def flip(self):
        eve = self.eve
        if self.portrait == 0:
            self.portrait = 1
            self.screen_w = eve.lcd_height
            self.screen_h = eve.lcd_width
        else:
            self.portrait = 0
            self.screen_w = eve.lcd_width
            self.screen_h = eve.lcd_height

        self.APP_H = self.screen_h - APP_PADDING_TOP * 2
        self.APP_W = self.APP_H / APP_RATIO
        self.APP_X = self.screen_w / 2 - self.APP_W / 2
        self.APP_Y = APP_PADDING_TOP
        self.MENU_W = self.APP_W / 5




