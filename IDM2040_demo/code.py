from main_menu.tags_all import *
import microcontroller
import watchdog
import time
import sys
from main_menu.brt_eve_rp2040_dmx import BrtEveRP2040_dmx as BrtEveRP2040

if sys.implementation.name == "circuitpython":
    from brteve.brt_eve_bt817_8 import BrtEve
else:
    from ....lib.brteve.brt_eve_bt817_8 import BrtEve


class main_app():

    def __init__(self):
        self.host = BrtEveRP2040()
        self.eve = BrtEve(self.host)
        #self.eve = BrtEveExt(self.host)
        #eve.init(resolution="1280x800", touch="goodix")
        #self.eve.init(resolution="1280x800", touch="capacity")  # old device
        self.eve.init(resolution="800x480", touch="capacity")
        #self.eve.calibrate()
        # eve.wr32(eve.REG_TOUCH_TRANSFORM_A, 0xfffefefc) # pre setting for 1280x800 lcd
        # eve.wr32(eve.REG_TOUCH_TRANSFORM_B, 0xfffffcbf)
        # eve.wr32(eve.REG_TOUCH_TRANSFORM_C, 0x506adb4)
        # eve.wr32(eve.REG_TOUCH_TRANSFORM_D, 0xfffffed1)
        # eve.wr32(eve.REG_TOUCH_TRANSFORM_E, 0xfffefc79)
        # eve.wr32(eve.REG_TOUCH_TRANSFORM_F, 0x32c3211)

        eve=self.eve
        self.x0=20
        self.y0=10
        self.x1=50
        self.y1=120
        self.xmargin=100
        self.ymargin=50
        self.lastTouch=time.monotonic_ns() / 1000_000
        self.touchCounter=0
        self.longTouch=0
        
    def drawBtn(self):
        eve = self.eve
        eve.ColorRGB(0xff, 0xff, 0xff)
        y =  self.y1
        btn_w = 300
        btn_h = 60
        xmargin =self.xmargin=100
        center = 200
        x1 = self.x1
        ymargin =self.ymargin
        x2 = x1 + btn_w + xmargin
        x3 = x2 + btn_w + xmargin
        x4 = x3 + btn_w + xmargin
        x5 = x4 + btn_w + xmargin
        y2 = y + btn_h + ymargin
        y3 = y2 + btn_h + ymargin
        y4 = y3 + btn_h + ymargin
        eve.Tag(tag_cube_demo)
        eve.cmd_button(x1, y, btn_w, btn_h, 31, 0, "3D Cube Demo")
        eve.Tag(tag_blinka_dema)
        eve.cmd_button(x2, y, btn_w, btn_h, 31, 0, "Blinka Demo")
        eve.Tag(tag_image_View)
        eve.cmd_button(x1, y2, btn_w, btn_h, 31, 0, "Image Viewer")
        eve.Tag(tag_dmx512_demo)
        eve.cmd_button(x2, y2, btn_w, btn_h, 31, 0, "DMX512 Demo")        
        eve.Tag(tag_audio_playback)
        eve.cmd_button(x1, y3, btn_w, btn_h, 31, 0, "Audio Playback")
        eve.Tag(tag_video_playback)
        eve.cmd_button(x2, y3, btn_w, btn_h, 31, 0, "Video Playback")
  
      
    def get_event(self):
        eve = self.eve     
        tag = eve.rd32(eve.REG_TOUCH_TAG) & 0xFF       
        return tag
                   
    def processEvent(self,tag):
        eve = self.eve
        is_touch = eve.rd32(eve.REG_TOUCH_RAW_XY) != 0xFFFFFFFF
        if tag == tag_cube_demo:
                from cube.cube import cube 
                cube(eve).loop()
        elif tag == tag_blinka_dema:
                from blinka.blinka_rotate import blinka_rotate  
                blinka_rotate(eve).run()
        elif tag == tag_dmx512_demo:
            from  dmx512.dmx_ui import dmx_ui  
            dmx_ui(eve).loop()                  
        elif tag == tag_image_View:
            from image_viewer.image_viewer import image_viewer  
            spi1 = eve.spi_sdcard()
            eve.finish()
            try:
                app=image_viewer(eve)
                app.deinit()
                self.showFreeMem()
            except  Exception as e:
                print("Exception:",e)
        elif tag == tag_audio_playback:
            from audio_play.audio_play import audio_play             
            audio_play(eve)
        elif tag == tag_video_playback:
             import video2 as demo
             sdcard= "/sd/"
             demo.start(sdcard, eve)
             time.sleep(0.01)

    def loop(self):
        eve = self.eve
        eve.cmd_dlstart() 
        eve.ClearColorRGB(0, 0, 0) 
        eve.Clear(1, 1, 1)        
        eve.cmd_text(self.x0, self.y0, 31, 0, "IDM2040 Demo")
        self.drawBtn()
        ev = self.get_event()
        try:
            eve.Display()
            eve.cmd_swap() 
            eve.flush() 
            eve.cmd_loadidentity() 
        except  Exception as e:
            print("exception:",e)
        self.processEvent(ev)
        eve.Tag(0)
        time.sleep(0.01)

if __name__ == '__main__':
    mainMenu=main_app()
    try:
        while 1:
            mainMenu.loop()
    except Exception as e:
        print("Other exception:",e)
        microcontroller.reset()
