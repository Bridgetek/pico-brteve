import time

from .helper import helper
from .gesture import gesture
from brteve.brt_eve_bt817_8 import BrtEve

class layout():
    def __init__(self, eve: BrtEve, helper: helper) -> None:
        self.eve = eve
        self.helper = helper

        APP_RATIO = 1.9
        APP_PADDING_TOP = 10

        self.portrait = 0
        self.screen_w = eve.lcd_width
        self.screen_h = eve.lcd_height
        if self.eve.lcd_height < 600:
            self.screen_w = eve.lcd_height
            self.screen_h = eve.lcd_width
            self.portrait = 1

        self.APP_H = self.screen_h - APP_PADDING_TOP * 2
        self.APP_W = self.APP_H / APP_RATIO
        self.APP_X = self.screen_w / 2 - self.APP_W / 2
        self.APP_Y = APP_PADDING_TOP
        self.MENU_W = self.APP_W / 5

        self.images = {
            'background'   : ['alarm_clock/img/big_background_1280x976_COMPRESSED_RGBA_ASTC_8x8_KHR.raw' , 0 , 1280, 976],
            'clock_face'   : ['alarm_clock/img/clock_face_224x224_COMPRESSED_RGBA_ASTC_8x8_KHR.raw'      , 0 , 224 , 224],
            'menu_icon_000': ['alarm_clock/img/000_64x64_COMPRESSED_RGBA_ASTC_8x8_KHR.raw'               , 0 , 64  , 64 ],
            'menu_icon_001': ['alarm_clock/img/001_64x64_COMPRESSED_RGBA_ASTC_8x8_KHR.raw'               , 0 , 64  , 64 ], # bed time
            'menu_icon_002': ['alarm_clock/img/006_64x64_COMPRESSED_RGBA_ASTC_8x8_KHR.raw'               , 0 , 64  , 64 ],
            'menu_icon_003': ['alarm_clock/img/007_64x64_COMPRESSED_RGBA_ASTC_8x8_KHR.raw'               , 0 , 64  , 64 ], # timer
            'menu_icon_004': ['alarm_clock/img/002_64x64_COMPRESSED_RGBA_ASTC_8x8_KHR.raw'               , 0 , 64  , 64 ],
            'menu_icon_005': ['alarm_clock/img/003_64x64_COMPRESSED_RGBA_ASTC_8x8_KHR.raw'               , 0 , 64  , 64 ], # clock
            'menu_icon_006': ['alarm_clock/img/004_64x64_COMPRESSED_RGBA_ASTC_8x8_KHR.raw'               , 0 , 64  , 64 ],
            'menu_icon_007': ['alarm_clock/img/005_64x64_COMPRESSED_RGBA_ASTC_8x8_KHR.raw'               , 0 , 64  , 64 ], # alarm
            'new_alarm'    : ['alarm_clock/img/new_alarm_56x56_COMPRESSED_RGBA_ASTC_8x8_KHR.raw'         , 0 , 56  , 56 ],
            'new_alarm_hover': ['alarm_clock/img/new_alarm_hover_56x56_COMPRESSED_RGBA_ASTC_8x8_KHR.raw' , 0 , 56, 56 ],
        }

        address = 0
        for index in self.images:
            address = self.round_to_nearest(address, 4)
            self.images[index][1] = address
            address = self.eve.write_file(address, self.images[index][0])

    def round_to_nearest(self, n, m):
        return (n + m - 1) // m * m

    def draw_asset(self, tag, index, x, y):
        helper = self.helper
        helper.image_draw_from_ram_g(self.images[index][1], 
            x, y, 
            self.images[index][2], self.images[index][3], 
            # self.eve.ASTC_8x8, 0, tag, self.eve.OPT_DITHER)
            self.eve.ASTC_8x8, 0, tag, 0)

        
    
