import os
import time
import random

from .tags import *
from .helper_gesture import helper_gesture
from .ui import ui
from .audio_eve import audio_eve

import sys
if sys.implementation.name == "circuitpython":
    from brteve.brt_eve_bt817_8 import BrtEve
else:
    from ....lib.brteve.brt_eve_bt817_8 import BrtEve

class audio_playback():
    def __init__(self, eve: BrtEve) -> None:
        self.eve = eve
        self.ui = ui(eve)

        fifo_start = 1024*512
        fifo_len = 1024*16
        self.audio_eve = audio_eve(eve, fifo_start, fifo_len)
        self.helper_gesture = helper_gesture(eve)

        self.next_command = 0

        self.files = []
        self.file_selected_id = 0
        self.loop = 0 # no loop, 1 = loop 1, 2 = loop all
        self.random = 0 # 1 = enable
        self.ui.set_loop(self.loop)
        self.ui.set_random(self.random)

        self.time_interrupt = 0
        self.file_size = 0

        self.vol = 10
        self.last_angle_volume = 0
        self.ui.set_volume(self.vol)
        self.audio_eve.set_volume(self.vol)

        # get .wav list
        self.media_location='/sd/pico-brteve/circuitPython/examples/audio-playback/media'
        self.scan_file(self.media_location)

        #start ui
        self.ui.file_list(self.files)
        while 1: self.event()
    
    def fifo_interrupt(self, byte_num):
        ges = self.helper_gesture.renew()
        tag = ges.tagReleased

        ms = time.monotonic_ns() / 1000_000
        refresh = 0
        if ms - self.time_interrupt > 1000:
            self.time_interrupt = ms
            percent = byte_num * 100 / self.file_size
            self.ui.progress((int)(percent))
            refresh = 1

        if tag == tag_play: # play/pause is same button
            self.play_pause()
        elif tag == tag_stop: 
            self.stop()
        elif tag == tag_loop: 
            self.set_loop()
        elif tag == tag_next:
            self.stop() #this exit the blocking playback
            self.next_command = self.next_file
        elif tag == tag_prev: 
            self.stop() #this exit the blocking playback
            self.next_command = self.prev_file
        elif tag == tag_random: 
            self.set_random()

        if ges.tagPressed == tag_volume: 
            self.set_volume()

        if ges.tagReleased != 0 or ges.tagPressed == tag_volume or refresh == 1 or ges.tagPressed == tag_filelist:
            file_selected_id = self.ui.file_selected
            if file_selected_id > -1:
                self.file_selected_id = file_selected_id
                self.stop() #this exit the blocking playback
                self.next_command = self.select_file
            self.ui.render()

    def event(self):
        self.ui.render()
        
        eve = self.eve
        ges = self.helper_gesture.renew()
        tag = ges.tagReleased

        if tag == tag_play: # play/pause is same button
            # blocking function
            self.play_pause()        
        elif tag == tag_stop: 
            self.stop()
        elif tag == tag_loop: 
            self.set_loop()
        elif tag == tag_next: 
            self.next_file()
        elif tag == tag_prev: 
            self.prev_file()
        elif tag == tag_random: 
            self.set_random()
        
        if ges.tagPressed == tag_volume: 
            self.set_volume()

        file_selected_id = self.ui.file_selected
        is_playing_end = self.audio_eve.is_end

        if file_selected_id > -1:
            self.file_selected_id = file_selected_id
            self.select_file()

        if self.next_command:
            temp = self.next_command
            self.next_command = 0
            temp()

        if self.audio_eve.is_eof():
            # now playback is end
            if self.loop == 0:
                self.stop()
            elif self.loop == 1:
                self.stop()
                while(self.audio_eve.is_playing()):
                    time.sleep(0.001)
                self.play_pause()
            if self.loop == 2:
                self.next_file()

    def scan_file(self, path):
        for f in os.listdir(path):
            if '.raw' in f:
                self.files.append(f)

    def play_pause(self):
        self.ui.play(not self.audio_eve.is_playing())
        if not self.audio_eve.is_playing():
            self.ui.set_playing_file_id(self.file_selected_id)
            SEEK_SET = 0
            SEEK_CUR = 1
            SEEK_END = 2
            with open(self.media_location +'/'+ self.files[self.file_selected_id], "rb") as file_handler:
                file_handler.seek(0, SEEK_END)
                self.file_size = file_handler.tell()
                file_handler.seek(0, SEEK_SET)

        #self.ui.render()
        # this is a blocking function, use callback function (fifo_interrupt) to check playback status
        self.audio_eve.play(self.fifo_interrupt, 
            self.media_location +'/'+ self.files[self.file_selected_id],
            44100, self.eve.LINEAR_SAMPLES);

    def stop(self):
        if self.audio_eve.is_ready() != True:
            self.audio_eve.stop();
            self.ui.play(self.audio_eve.is_playing())
        else:
            pass
        #self.ui.render()

    def next_file(self):
        if self.random == 0:
            self.file_selected_id += 1
            self.file_selected_id %= len(self.files)
        else:
            self.file_selected_id = random.randint(0, len(self.files) -1)

        self.stop()
        while(self.audio_eve.is_playing()):
            time.sleep(0.001)
        self.play_pause()
        
    def prev_file(self):
        if self.random == 0:
            self.file_selected_id -= 1
            self.file_selected_id %= len(self.files)
        else:
            self.file_selected_id = random.randint(0, len(self.files) -1)

        self.stop()
        while(self.audio_eve.is_playing()):
            time.sleep(0.001)
        self.play_pause()

    def select_file(self):
        self.stop()
        while(self.audio_eve.is_playing()):
            time.sleep(0.001)
        self.play_pause()
        
    def set_loop(self):
        if self.loop == 0:
            self.loop = 1
        elif self.loop == 1:
            self.loop = 2
        elif self.loop == 2:
            self.loop = 0
        self.ui.set_loop(self.loop)
        
    def set_random(self):
        if self.random == 0:
            self.random = 1
        elif self.random == 1:
            self.random = 0
        self.ui.set_random(self.random)
        
    def set_volume(self):
        track = self.helper_gesture.get().tagTrackTouched
        tag = track & 0xFF
        angel = track >> 16
        if tag == tag_volume:
            angel = round(angel * 360 / 65535)
            
            if self.last_angle_volume != 0 and angel != self.last_angle_volume:
                diff = angel - self.last_angle_volume
                if diff > 180: # decreasing, counter clock wise
                    diff = diff - 360
                elif diff < -180: # increasing, clock wise
                    diff = diff + 360

                if (self.vol < 255 and diff > 0) or (self.vol > 0 and diff < 0):
                    self.vol += diff
                    self.vol =max(0, self.vol)
                    self.vol =min(255, self.vol)
                    self.ui.set_volume(self.vol)
                    self.audio_eve.set_volume(self.vol)
            
            self.last_angle_volume = angel
        else:
            self.last_angle_volume = 0
        
    def jump_to(self, time):
        print("Function is not implemented")
        pass

    def fast_forward(self, file):
        print("Function is not implemented")
        pass

    def fast_backward(self, file):
        print("Function is not implemented")
        pass

        