import sys
import random
import math
import bteve as eve_module
import busio
import board
import sdcardio
import storage
import os
import time
from random import randint

class Page2_UI:
    def __init__(self, eve:eve_module.Brt_PicoEve_Module) -> None:
        self.eve=eve
        self.CMD_PRIV = 1
        self.CMD_NEXT = 2
        self.CMD_BACK = 3

        self._oldtag = 0;

    def start_load_image(self):
        eve=self.eve

        # start drawing
        eve.ClearColorRGB(255, 255, 255)
        eve.Clear()
        eve.ColorRGB(255, 255, 255)
        eve.VertexFormat(4)

        self.eve.cmd_loadimage(0, 0)

    def write_image_buffer(self, buff):
        self.eve.cc(eve_module.align4(buff))
        pass

    def message(self, title, info):
        eve=self.eve
        
        eve.ClearColorRGB(255, 255, 255)
        eve.Clear()
        eve.ColorRGB(255, 255, 255)
        eve.VertexFormat(4)

        tx=eve.w/2 - len(title) * 5
        ty=eve.h/2
        
        eve.ColorRGB(0, 0, 0)
        eve.cmd_text(tx, ty, 30, 0, title)
        eve.cmd_text(tx, ty + 50, 25, 0, info)
        eve.swap()

    def draw_image(self, img, title):
        eve=self.eve
        if img != '': # showing an local image from sdcard
            eve.ClearColorRGB(255, 255, 255)
            eve.Clear()
            eve.ColorRGB(255, 255, 255)
            eve.VertexFormat(4)

            eve.cmd_loadimage(0, 0)
            eve.load(open(img, "rb"))

        else: # showing internet image directly
            pass 
        # validate image
        eve.flush()
        rp=eve.EVE_Cmd_wp();
        eve.cmd_getprops(0, 0, 0)
        eve.flush()
        w=eve.rd32(eve.RAM_CMD + rp+4*2);
        h=eve.rd32(eve.RAM_CMD + rp+4*3);
        if w<0 or w > eve.w:
            print("Image invalid")
            return 0
        if h<0 or h > eve.h:
            print("Image invalid")
            return 0

        x=eve.w/2-w/2
        y=eve.h/2-h/2
        eve.Begin(eve.BITMAPS)
        eve.Vertex2f(x, y)

        tx = x + w/2 - len(title) * 7
        ty = y+ h + 10
        eve.ColorRGB(0, 0, 0)
        eve.cmd_text(tx, ty, 30, 0, title)
        # control button
        w=100
        h=(int)(eve.h / 2)

        eve.cmd_text(w/2, eve.h/2, 30, 0, "<")
        eve.cmd_text((int)(eve.w - w/2), eve.h/2, 30, 0, ">")

        eve.ColorA(0)
        eve.Tag(self.CMD_PRIV)
        eve.cmd_button(0, (int)(eve.h / 2 - h/2), w, h, 25, 0, "Previous")
        eve.cmd_track(0, (int)(eve.h / 2 - h/2), w, h, self.CMD_PRIV)
        
        eve.Tag(self.CMD_NEXT)
        eve.cmd_button(eve.w - w, (int)(eve.h / 2 - h/2), w, h, 25, 0, "Next")
        eve.cmd_track(eve.w - w, (int)(eve.h / 2 - h/2), w, h, self.CMD_NEXT)

        w=100
        h=50
        eve.ColorA(200)
        eve.cmd_fgcolor(0xb9b900)
        eve.ColorRGB(255, 255, 255)
        eve.Tag(self.CMD_BACK)
        eve.cmd_button(2, 2, w, h, 20, 0, "Back")
        eve.cmd_button(2, 2, w, h, 20, 0, "Back")

        # padding = 15
        # eve.cmd_button(padding, eve.h - h - padding, w, h, 25, 0, "Back")
        # eve.Tag(2)
        # eve.cmd_button(eve.w - w - padding, eve.h - h - padding, w, h, 25, 0, "Next")
        # eve.Tag(2)
        # eve.cmd_button(eve.w - w - padding, eve.h - h - padding, w, h, 25, 0, "Back")
        print("-------")
        eve.swap()
        eve.flush()
        print("-------")
    
    def get_comand(self):
        eve=self.eve
        tag = eve.rd32(eve.REG_TOUCH_TAG) & 0xFF;
        if tag == 0:
            tag = eve.rd32(eve.REG_TRACKER) & 0xFF;

        if tag == 1:
            return self.CMD_PRIV
            
        if tag == 2:
            return self.CMD_NEXT

        if tag == 3:
            return self.CMD_BACK

        return 0

