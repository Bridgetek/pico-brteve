from brteve.brt_eve_bt817_8 import BrtEve
from  main_menu.brt_eve_rp2040_dmx import BrtEveRP2040_dmx as BrtEveRP2040

from  .gesture import gesture
import supervisor
import microcontroller
import math
import time
import array
# import uctypes
import ulab.numpy as np
import board
from .dmx512 import dmx512

tag_count=1
tag_reset=tag_count;tag_count+=1
tag_Back=tag_count;tag_count+=1
tag_colorpicker=tag_count;tag_count+=1
tag_lightness=tag_count;tag_count+=1
tag_white=tag_count;tag_count+=1

 

tag_all_red=tag_count;tag_count+=1
tag_all_green=tag_count;tag_count+=1
tag_all_blue=tag_count;tag_count+=1
tag_all_dark=tag_count;tag_count+=1

class dmx_ui(object):
    def __init__(self , eve: BrtEve):
        self.eve=eve      
        self._COLOR_GREEN=[0x90, 0xC8, 0x3A]
        self._COLOR_GRAY=[0x33, 0x33, 0x33]
        self._COLOR_WARNING=[0xD4, 0x21, 0x33]
        self._COLOR_YELLOW=[0xFF, 0xFF, 0x00]
        self.dmx = dmx512(512,board.GP8)
        self.addrDMX=4

        w = self.eve.lcd_width
        h = self.eve.lcd_height
        left = w/2 - 252/2
        self.images = {
                              #addr0           size1  w2  h3  tag4 scale5 x6                y7
            'circular_colorwheel'    : [128576 - 4096 , 63552 , 252, 252,  0,  1,     100  , 50      ],
            #'gs-16b-2c-44100hz.raw'    : [144960 - 4096 , 699328 , 250, 250,  0,  1,     100  , 50      ],
        } 
            
        img = self.images['circular_colorwheel']
        eve.cmd_flashread(img[0], img[0]+4096, img[1])  
        eve.finish()

        self.tWhitePercent=0

        self.radius=125
        self.sat=0.5
        self.hue=50
        self.lightness=128
        self.rgb=self.hsv_to_rgb(self.hue/360.0,self.sat,self.lightness)
        self.x0=100
        self.y0=90
        self.hColorwheel=0
        self.message='please pick color'
        self.workMode='text'
        self.last_timeout =  time.monotonic_ns() / 1000_000
        self.wInterval=100 # write to DMX   interval 
        self.c_w=0
        self.counter=0
        self.lastTouch=time.monotonic_ns() / 1000_000
        self.touchCounter=0
        self.longTouch=0
        self.snapCounter=0

        
    def hsv_to_rgb(self,h, s, v):
        i = math.floor(h*6)
        f = h*6 - i
        p = v * (1-s)
        q = v * (1-f*s)
        t = v * (1-(1-f)*s)

        r, g, b = [
            (v, t, p),
            (q, v, p),
            (p, v, t),
            (p, q, v),
            (t, p, v),
            (v, p, q),
        ][int(i%6)]

        return round(r), round(g), round(b)


    def getSat(self,x0,   y0,   x1,   y1,   r):
        sizeX = math.pow((x1 - x0), 2) 
        sizeY = math.pow((y1 - y0), 2)
        sat=math.sqrt(sizeX + sizeY) / r
        return sat
                         
    def getHue(self,x0,  y0,  x1,  y1):
        angleInRadian, angleInDegree=0,0

        if x0 == x1:
             if y1 < y0:
                 return 90
             elif y1 > y0:
                 return 270
             else:
                 return 0
        elif x1 > x0:
            if y1 > y0:
                angleInRadian = math.atan2((y1 - y0), (x1 - x0));
                angleInDegree = 360 - (angleInRadian * 180 / math.pi);
            else:    
                angleInRadian = math.atan2((y0 - y1), (x1 - x0));
                angleInDegree = angleInRadian * 180 / math.pi;
        else:
            if y1 > y0:
                angleInRadian = math.atan2((y1 - y0), (x0 - x1));
                angleInDegree = 180 + (angleInRadian * 180 / math.pi);
            else:
                angleInRadian = math.atan2((y0 - y1), (x0 - x1));
                angleInDegree = 180 - (angleInRadian * 180 / math.pi);

        return angleInDegree

    def updateRGB(self):
            if self.sat>1:
                 return 
            self.message="sat:%5.3f,hue:%d,rgb:%x %x %x"%(self.sat,self.hue,self.rgb[0],self.rgb[1],self.rgb[2])
            #print(self.message)
            self.writeOneFrame(self.rgb,self.tWhitePercent)
    def writeOneFrame(self,rgb,tWhitePercent):
          self.dmx.setStart4ch(self.addrDMX,rgb[1], rgb[0], rgb[2] ,int(tWhitePercent*2.55))
          self.dmx.write_frame() # re-write  
    def processEvent(self,tag,touch):
        tagReleased=touch.tagReleased
        if tagReleased==tag_lightness:
            self.rgb=self.hsv_to_rgb(self.hue/360.0,self.sat,self.lightness)  
            self.updateRGB()
            #print("release lightness",self.lightness)
        elif tagReleased==tag_white:
            self.writeOneFrame(self.rgb,self.tWhitePercent)

            
        if not touch.isTouch:
            return 0
    
        if tag == tag_Back:
            return -1
        elif tag == tag_lightness:
            vv=touch.tagTrackTouched>>16
            self.lightness=255*(vv/65535)
            self.tWhitePercent=0
            #print("lightness",self.lightness)
        elif tag == tag_white:
           vv=touch.tagTrackTouched>>16
           self.tWhitePercent=100*(vv/65535)
           self.rgb=(0,0,0)
           #print("tWhitePercent",self.tWhitePercent)
        elif tag == tag_colorpicker:
            #print("touch",touch.touchX,touch.touchY)
            x0=self.x0+self.radius
            y0=self.y0+self.radius
            x1=touch.touchX
            y1=touch.touchY
            if x1==32768 or y1==32768:
                print("invalid touch tag_colorpicker",x1,y1)
                return 0
            self.sat=self.getSat(x0,y0,x1,y1,self.radius)
            self.hue=self.getHue(x0,y0,x1,y1)
            self.rgb=self.hsv_to_rgb(self.hue/360.0,self.sat,self.lightness)
            self.tWhitePercent=0
            self.updateRGB()
 
        return 0
 
    def loop(self):
        eve=self.eve
        assetdir = "dmx512/"
        self.dmx.off(self.addrDMX)
        self.dmx.off(self.addrDMX)
        
        self.dmx.setStart4ch(self.addrDMX,self.rgb[1], self.rgb[0], self.rgb[2] ,int(self.tWhitePercent*2.55))
        self.dmx.setStart4ch(self.addrDMX,self.rgb[1], self.rgb[0], self.rgb[2] ,int(self.tWhitePercent*2.55))
        touch_ges = gesture(self.eve)
        while True:
            eve.cmd_dlstart() 
            eve.VertexFormat(2)
            eve.ClearColorRGB(0, 0, 0)    
            eve.Clear(1, 1, 1) 
            eve.ColorRGB(0xff, 0xff, 0xff)
            
            eve.cmd_fgcolor(0x003870)  # default
            eve.cmd_bgcolor(0x002040)  # 

            eve.cmd_text(10, 5, 30, 0, "DMX512" )  
            eve.Tag(tag_Back)
            eve.cmd_button(700, 5, 85,35,30, 0, "Back")
            
            x=self.x0; y=self.y0
            w=300
            h=320
                        
            eve.Tag(tag_colorpicker)                        
            img = self.images['circular_colorwheel']
            eve.cmd_setbitmap(img[0], eve.ASTC_4x4, 2*self.radius, 2*self.radius)
            eve.Tag(tag_colorpicker)
            eve.Begin(eve.BITMAPS)
            eve.Vertex2f(self.x0, self.y0)

            eve.TagMask(0)
            eve.ColorRGB(self.rgb[0], self.rgb[1], self.rgb[2])
            eve.Begin(eve.RECTS)
            eve.LineWidth(5)
            eve.Vertex2f(x+350, y+100)
            eve.Vertex2f(x +350+w, y+h)
            
            
            h=25
            eve.cmd_fgcolor(0xffffff)
            eve.cmd_bgcolor(0xffffff)   
            eve.ColorRGB(0xff,0xff,0xff)
            eve.cmd_text(x+350, y-20, 28, 0, "White LED lightness" )
            eve.TagMask(1)
            eve.Tag(tag_white)
            eve.cmd_track(x+350, y+35, w, h+20, tag_white)
            eve.cmd_slider(x+350, y+35, w, h, 0, self.tWhitePercent, 100)

            

            y= y+300
            eve.cmd_fgcolor(0xffffff)
            eve.cmd_bgcolor(0xffffff)  
            eve.ColorRGB(0xff,0xff,0xff)
            eve.Tag(tag_lightness)
            eve.cmd_track(x, y-10, w, h+30, tag_lightness)   # h bigger get better
            eve.cmd_slider(x, y, w, h, 0, self.lightness, 255)    
        
            touch=touch_ges.renew()

            eve.Display()
            eve.cmd_swap()   
            eve.flush() 
            if self.processEvent(touch.tagPressed,touch)<0: break
            ms = time.monotonic_ns() / 1000_000
            if ms - self.last_timeout > self.wInterval:
                self.last_timeout =  ms
                self.updateRGB()       
            time.sleep(0.0001)
        self.rgb=(0,0,0)
        self.tWhitePercent=0
        self.writeOneFrame(self.rgb,self.tWhitePercent)
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
        self.dmx.deinit()
        del self.dmx 


if __name__ == '__main__':
    # File "/lib/brteve/brt_eve_rp2040.py", line 23, in pin
    # self.pin_dcx_eve_ili9488 = pin(board.GP8) #D/CX pin of ILI9488
     
    host = BrtEveRP2040()
    eve = BrtEve(host)
    #self.eve = BrtEveExt(self.host)
    #eve.init(resolution="1280x800", touch="goodix")
    eve.init(resolution="800x480", touch="capacity")
    #eve=self.eve
    #eve.calibrate()
    # eve.wr32(eve.REG_TOUCH_TRANSFORM_A, 0xfffefefc) # pre setting for 1280x800 lcd
    # eve.wr32(eve.REG_TOUCH_TRANSFORM_B, 0xfffffcbf)
    # eve.wr32(eve.REG_TOUCH_TRANSFORM_C, 0x506adb4)
    # eve.wr32(eve.REG_TOUCH_TRANSFORM_D, 0xfffffed1)
    # eve.wr32(eve.REG_TOUCH_TRANSFORM_E, 0xfffefc79)
    # eve.wr32(eve.REG_TOUCH_TRANSFORM_F, 0x32c3211)

    dmx_ui(eve).loop()