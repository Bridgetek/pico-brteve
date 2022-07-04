import os
import sys
import time
sys.path.append("lib/adafruit")
sys.path.append("lib/adafruit/adafruit_apds9960")

from brteve.brt_eve_rp2040 import BrtEveRP2040
from brteve.brt_eve_bt817_8 import BrtEve
import busio
import board

from adafruit_apds9960 import apds9960 as APDS9960_Sensor

import pulseio
import adafruit_irremote

from .ui_astc import ui_astc
from .ui_jpeg import ui_jpeg
from .gesture import gesture

class image_viewer:
    def __init__(self, eve: BrtEve):
        self.eve=eve
        self.proximity=-1
        self.i2c=None
        self.pulsein=None
        self.decoder=None
        self.show_jpeg('image_viewer')

    def deinit(self):   
        if self.i2c!=None :
            self.i2c.deinit()
        if self.pulsein!=None:
             self.pulsein.deinit()       
    def get_w_x_h(self, imagename):
        """ Imagename should have format: ..._wxh_COMPRESSED...raw
        """
        s= imagename.split("_COMPRESSED")[0]
        s= s.split("_")[-1]
        w=s.split("x")[0]
        h=s.split("x")[1]
        print(s, w, h)
        return w, h

    def show_astc(self, path):
        eve = self.eve
        arr = os.listdir(path)
        images=[]
        names=[]
        size=[]
        for f in arr:
            if '.raw' in f:
                images.append(path + '/' +f)
                names.append(f)
                w,h=self.get_w_x_h(f)
                size.append([w,h])

        apds=self.init_APDS9960()
        pulsein, decoder=self.init_IR()

        demo=ui_astc(eve)
        demo.set_images(images, names, size)

        while True:
            ges = self.get_left_right(apds, pulsein, decoder)
            if ges == 'left':
                demo.swipe_left()
            elif ges == 'right':
                demo.swipe_right()

    def show_jpeg(self, path):
        print("show_jpeg ")
        eve = self.eve
        while 1:
            try:
                arr = os.listdir(path)
            except Exception:
                print("No such folder", path)
                continue
            break
        print(arr)
        images=[]
        names=[]
        for f in arr:
            if '.jpg' in f or '.png' in f:
                images.append(path + '/' +f)
                names.append(f)

        demo=ui_jpeg(eve)
        demo.set_images(images, names)
        apds=self.init_APDS9960()
        touch_ges = gesture(self.eve)
        tag_Back=1
        t = 0
        while True:
            ges = self.get_left_right(apds)
            touch=touch_ges.renew() 
            if ges == 'left' or touch_ges.sIsSwipe == touch_ges.SWIPE_LEFT:
                demo.swipe_left()
            elif ges == 'right' or touch_ges.sIsSwipe == touch_ges.SWIPE_RIGHT:
                demo.swipe_right()
            if ges == 'up'  or touch_ges.sIsSwipe == touch_ges.SWIPE_UP :
                print("back")
                break
        eve.cmd_dlstart()
        eve.VertexFormat(2)
        eve.ClearColorRGB(0, 0, 0)  
        eve.Clear(1, 1, 1) 
        eve.ColorRGB(0xff, 0xff, 0xff)
        eve.cmd_fgcolor(0x003870)  
        eve.cmd_bgcolor(0x002040)   
        eve.Display()
        eve.cmd_swap()   
        eve.flush() 
            

    def init_APDS9960(self):
        try:
            self.i2c=busio.I2C(board.GP21, board.GP20)
            apds = APDS9960_Sensor.APDS9960(self.i2c)
            apds.enable_proximity = True
            apds.enable_gesture = True
        except  Exception as e:
                print("init_APDS9960 exception:",e)   
                apds=None         
        return apds
 

    def get_left_right(self, apds):
        time.sleep(0.01)
        if apds is  None :return 'none'
        if (self.proximity!=apds.proximity):
            self.proximity=apds.proximity
        ges = apds.gesture()
        if ges == 0x01:
            print("up")
            return 'up'
        elif ges == 0x02:
            print("down")
            return 'down'
        elif ges == 0x03:
            print("left")
            return 'left'
        elif ges == 0x04:
            print("right")
            return 'right'
