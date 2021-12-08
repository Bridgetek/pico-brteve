import time


from .tags import *
from .helper_gesture import helper_gesture
from .helper_image import helper_image
from .widgets import widgets_box, widgets_point
from .helper_scroller import helper_scroller

import sys
if sys.implementation.name == "circuitpython":
    from brteve.brt_eve_bt817_8 import BrtEve
else:
    from ....lib.brteve.brt_eve_bt817_8 import BrtEve

_text_height = 30
_text_font = 29
_text_playing_box_h = 150
_scroller_h = 20
_text_playing_box_padding = 10
_margin = 30

class ui():
    def __init__(self, eve) -> None:
        self.eve = eve
        w = self.eve.lcd_width
        h = self.eve.lcd_height
        self.helper_img = helper_image(eve)
        self.helper_scroller = helper_scroller(0, h/2)
        self.helper_gesture = helper_gesture(eve)
        self.files = []
        self.file_playing_id = -1
        self.file_selected_id = -1
        self.file_selected_id_double_tap = -1
        self.selected_timeout = 0
        self.files_released  = 0

        self.percent = 0

        self.vol = 0

        self.play_icon   = 'play_36x28'
        self.random_icon = 'mixoff_48x48'
        self.loop_icon   = 'loopall_72x72'

        self.file_list_box_h = h - 252 if h/3*2 < 252 else h/3*2 # 252 is volume button size
        self.file_list_box_h -= (_text_playing_box_h + _scroller_h)

        left = w/2 - 252/2
        top = h - 252
        self.images = {
                              #addr0           size1  w2  h3  tag4 scale5 x6                y7
            'unified.blob'  : [0      - 4096 , 4096 , 0,  0,   0,  0,     0, 0],
            'circle_72x72'  : [4096   - 4096 , 5184 , 72, 72,  0,  1,     0, 0],
            'circle_140x140': [17792  - 4096 , 19648, 140,140, 0,  1,     0, 0],
            'circle_92x92'  : [9280   - 4096 , 8512 , 92, 92,  0,  1,     0, 0],
            'circle_92x92_1': [9280   - 4096 , 8512 , 92, 92,  0,  1,     left - 97         , h - 92      ],
            'circle_92x92_2': [9280   - 4096 , 8512 , 92, 92,  0,  1,     left - 97 * 2     , h - 92      ],
            'circle_92x92_3': [9280   - 4096 , 8512 , 92, 92,  0,  1,     left + 252 + 5    , h - 92      ],
            'circle_92x92_4': [9280   - 4096 , 8512 , 92, 92,  0,  1,     left + 252 + 102  , h - 92      ],
            'knob_252x252'  : [37440  - 4096 , 63552, 252,252, 0,  1,     left              , h - 252     ],
            'loop1_72x72'   : [100992 - 4096 , 5184 , 72, 72,  0,  1,     left - 100        , h - 252 + 30],
            'loopall_72x72' : [106176 - 4096 , 5184 , 72, 72,  0,  1,     left - 100        , h - 252 + 30],
            'loopoff_72x72' : [111360 - 4096 , 5184 , 72, 72,  0,  1,     left - 100        , h - 252 + 30],
            'mixoff_48x48'  : [116544 - 4096 , 2304 , 48, 48,  0,  1,     left + 252 + 52   , h - 252 + 30],
            'mixon_48x48'   : [118848 - 4096 , 2304 , 48, 48,  0,  1,     left + 252 + 52   , h - 252 + 30],
            'mute_48x48'    : [121152 - 4096 , 2304 , 48, 48,  0,  1,     left + 100        , h - 150     ],
            'next_36x28'    : [123456 - 4096 , 1024 , 36, 28,  0,  1,     left - 97 + 25    , h - 60      ],
            'prev_36x28'    : [126528 - 4096 , 1024 , 36, 28,  0,  1,     left - 97 * 2 + 23, h - 60      ],
            'pause_36x28'   : [124480 - 4096 , 1024 , 36, 28,  0,  1,     left + 252 + 31   , h - 60      ],
            'play_36x28'    : [125504 - 4096 , 1024 , 36, 28,  0,  1,     left + 252 + 31   , h - 60      ],
            'stop_36x28'    : [127552 - 4096 , 1024 , 36, 28,  0,  1,     left + 252 + 128  , h - 60      ],
        } 

        count = 1
        for img in self.images:
            self.images[img][4] = count
            count+=1

        eve.cmd_flashread(0, 4096, eve.RAM_G_SIZE)
        eve.finish()

        self.render()

    def file_list(self, files):
        self.files = files
        textbox_len = len(self.files) * _text_height
        h = self.eve.lcd_height

        scroller_limit = min(0, -textbox_len + self.file_list_box_h)
        self.helper_scroller.set_limit(0, scroller_limit)
        self.helper_scroller.get_offset_velocity(0)
    
    def set_playing_file_id(self, fid):
        self.file_playing_id = fid

    def draw_img(self, img_id, tag = 0):
        eve = self.eve
        img = self.images[img_id]

        #print('tag=', tag)
        if img[5] != 1:
            self.helper_img.image_setup_scale( img[5])
        self.helper_img.image_draw_from_ram_g(
            img[0], img[6], img[7], img[2], img[3], eve.ASTC_4x4, 0, tag, self.eve.OPT_DITHER)
    
    def render_file_list(self):
        eve = self.eve
        w = eve.lcd_width
        h = eve.lcd_height
        touch_y = self.helper_gesture.get().touchY

        tag = self.helper_gesture.get().tagPressed
        if tag != tag_filelist:
            offset_y, veloc = self.helper_scroller.get_offset_velocity(32768)
            self.files_released = 1
        else:
            offset_y, veloc = self.helper_scroller.get_offset_velocity(touch_y)

        box_w = 500
        x = max(w/2 - box_w/2 , 0)

        y = offset_y
        count = -1
        for i in self.files:
            y += _text_height
            count +=1
            
            if y < _text_playing_box_padding: continue
            if y > self.file_list_box_h - _text_playing_box_padding: continue

            if self.helper_gesture.get().isSwipe == False \
               and touch_y > y                            \
               and touch_y < y + _text_height:
                    self.file_selected_id_double_tap = -1
                    if self.file_selected_id > -1 and count == self.file_selected_id and self.files_released:
                        # Play a new file
                        self.file_selected_id_double_tap = count

                    self.file_selected_id = count
                    self.files_released = 0
                    ms = time.monotonic_ns() / 1000_000
                    self.selected_timeout = ms

            if self.files[self.file_playing_id] == i:
                eve.ColorRGB(0, 255, 0)
            else:
                eve.ColorRGB(255, 255, 255)
            eve.cmd_text(x, y, _text_font, 0, i)
        eve.ColorRGB(255, 255, 255)

        if self.file_selected_id > -1:
            timeout = 2000
            ms = time.monotonic_ns() / 1000_000
            if ms - self.selected_timeout < timeout:
                eve.cmd_text(w/2, self.file_list_box_h + _margin, 30, 
                eve.OPT_CENTER, "Selected " + self.files[self.file_selected_id])
            else:
                self.file_selected_id = -1
                self.file_selected_id_double_tap = -1

        elif self.file_playing_id > -1 and self.play_icon == 'pause_36x28':
            eve.cmd_text(w/2, self.file_list_box_h + _margin, 30, 
                eve.OPT_CENTER, "Playing " + self.files[self.file_playing_id])
        else:
            eve.cmd_text(w/2, self.file_list_box_h + _margin, 30, 
                eve.OPT_CENTER, "Ready")

    def render_scroller(self):
        eve = self.eve
        w = eve.lcd_width
        h = eve.lcd_height
        box_w = 500
        x = max(w/2 - box_w/2 , 0)
        if self.play_icon != 'pause_36x28':
            return

        eve.cmd_progress(x, self.file_list_box_h + 20 + _margin,
            500, _scroller_h, 0, (int)(self.percent), 100)

    def render_volume(self):
        eve = self.eve

        knob='knob_252x252'
        img = self.images[knob]
        x = img[6]
        y = img[7]
        w = img[2]
        h = img[3]

        self.helper_img.image_setup_rotate( self.vol * 270 / 255)
        self.draw_img(knob)
        track_x = x + w/2
        track_y = y + h/2
                
        eve.cmd_track(track_x, track_y, 1, 1, tag_volume)
        eve.ColorA(0)
        widgets_point(eve, track_x, track_y, w/2, [0, 255, 0], tag_volume)
        eve.ColorA(255)
        if self.vol == 0:
            self.draw_img('mute_48x48')

        if self.helper_gesture.get().tagPressed == tag_volume:
            if self.vol == 0:
                self.eve.ColorRGB(255, 0, 0)
            else:
                self.eve.ColorRGB(0, 255, 0)
            self.eve.cmd_text(10, 10, 30, 0, 'Volume: ' + str(int(self.vol * 100 / 255)) + '%')
            self.eve.ColorRGB(255, 255, 255)

    def render(self):
        eve = self.eve
        self.helper_gesture.renew()

        eve.cmd_dlstart()
        eve.ClearColorRGB(255, 255, 255)
        eve.Clear(1, 1, 1)
        eve.VertexFormat(3)

        w = eve.lcd_width
        h = eve.lcd_height

        # app background
        widgets_box(eve, -10, -10, w + 20, h + 20, 10, [0x1A, 0x1A, 0x1A])
        
        # file list box
        widgets_box(eve, 10    , 0, w - 20    , 10 + self.file_list_box_h    , 10, [0x19, 0x19, 0x19])
        widgets_box(eve, 10 + 1, 1, w - 20 - 2, 10 + self.file_list_box_h - 2, 8 , [0x28, 0x28, 0x28])
        widgets_box(eve, 10 + 2, 2, w - 20 - 4, 10 + self.file_list_box_h - 4, 6 , [0x1d, 0x1d, 0x1d])
        widgets_box(eve, 10 + 3, 3, w - 20 - 6, 10 + self.file_list_box_h - 6, 4 , [0x00, 0x00, 0x00], tag_filelist)

        eve.ColorRGB(255, 255, 255)
        self.draw_img('circle_92x92_1', tag_next)
        self.draw_img('circle_92x92_2', tag_prev)
        self.draw_img('circle_92x92_3',tag_play)
        self.draw_img('circle_92x92_4', tag_stop)
        self.draw_img(self.loop_icon, tag_loop)
        self.draw_img(self.random_icon, tag_random)
        self.draw_img('next_36x28', tag_next)
        #print('tag_play', tag_play, tag_pause)
        self.draw_img(self.play_icon, tag_play)
        self.draw_img('prev_36x28' , tag_prev)
        self.draw_img('stop_36x28' , tag_stop)
        
        self.render_file_list()
        self.render_volume()
        self.render_scroller()

        eve.Display()
        eve.cmd_swap()
        eve.flush()
    
    @property
    def file_selected(self):
        ret = self.file_selected_id_double_tap
        if self.file_selected_id_double_tap > -1:
            self.file_selected_id_double_tap = -1
            return ret
        return -1

    def set_volume(self, vol):
        self.vol = vol
        
    def play(self, is_playing):
        if is_playing == True:
            self.play_icon = 'pause_36x28'
        else:
            self.play_icon = 'play_36x28'
        #self.render()

    def stop(self):
        return self.play(False)

    def set_loop(self, loop_type):
        eve = self.eve
        if loop_type == 0:
            self.loop_icon = 'loopoff_72x72'
        elif loop_type == 1:
            self.loop_icon = 'loop1_72x72'
        else: #loop_type == 2:
            self.loop_icon = 'loopall_72x72'

    def set_random(self, is_enable):
        eve = self.eve
        if is_enable:
            self.random_icon = 'mixon_48x48'
        else:
            self.random_icon = 'mixoff_48x48'

    def progress(self, percent):
        self.percent = percent