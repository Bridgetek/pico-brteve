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
        #self.show_jpeg('/sd/Test/image_slide_viewer')
        #self.show_jpeg('pico-brteve/circuitPython/examples/image_slide_viewer')
        #mkdir pico-brteve\circuitPython\examples\image_slide_viewer
        #self.show_jpeg('pico-brteve/circuitPython/examples/image_viewer')
        self.show_jpeg('image_viewer')

    def deinit(self):   
        print("deinit image_viewer")
        if self.i2c!=None :
            self.i2c.deinit()
            print("deinit i2c ")
        if self.pulsein!=None:
             self.pulsein.deinit()
             print("deinit pulsein ")           
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
        pulsein, decoder=self.init_IR()
        use_touch=True
        if use_touch:touch_ges = gesture(self.eve)
        #touch_ges = gesture()
        tag_Back=1
        t = 0
        while True:
            ges = self.get_left_right(apds, pulsein, decoder)
            if use_touch:touch=touch_ges.renew()
 
            if use_touch:
                if ges == 'left' or touch_ges.sIsSwipe == touch_ges.SWIPE_LEFT:
                    demo.swipe_left()
                elif ges == 'right' or touch_ges.sIsSwipe == touch_ges.SWIPE_RIGHT:
                    demo.swipe_right()

                #if ges == 'down'  or touch_ges.sIsSwipe == touch_ges.SWIPE_DOWN :
                if ges == 'up'  or touch_ges.sIsSwipe == touch_ges.SWIPE_UP :
                    print("back")
                    break
            else:
                if ges == 'left' :
                     demo.swipe_left()
                elif ges == 'right' :
                     demo.swipe_right()
                elif ges == 'down' :
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
            #apds.enable_color = True
            apds.enable_gesture = True
        except  Exception as e:
                print("init_APDS9960 exception:",e)   
                apds=None         
        return apds

    def init_IR(self):
        try:
            self.pulsein = pulseio.PulseIn(board.GP1, maxlen=512, idle_state=True)
            #pulsein = pulseio.PulseIn(board.GP26, maxlen=512, idle_state=True)
            self.pulsein.clear()
            self.decoder = adafruit_irremote.GenericDecode()
        except  Exception as e:
                print("init_IR exception:",e)   
                self.pulsein=None  
                decoder=None
        return self.pulsein, self.decoder

    def get_left_right(self, apds, pulsein, decoder):
        if pulsein is not None: pulsein.clear()
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
        else:
            pass

        # pulsein.clear()
        # pulsein.resume()
        time.sleep(0.01)
        #pulses = decoder.read_pulses(pulsein,blocking=False, max_pulse=2000)
        pulses=None
        # pulsein.pause()
        if pulses is not None:
            # print("Pulses:", pulses)
            try:
                code = decoder.decode_bits(pulses)
                #print("Decoded:", code)
                direction=code[3]
                if direction == 48:
                    print("left")
                    return 'left'
                elif direction == 122:
                    print("right")
                    return 'right'
            except Exception:
                print("Failed to decode IR pulse")
                pulsein.clear()
                pulsein.resume()
        return 'none'

# if __name__ == '__main__':
#     from brteve.brt_eve_bt817_8 import BrtEve
#     from brteve.brt_eve_rp2040 import BrtEveRP2040
#     host = BrtEveRP2040()
#     eve = BrtEve(host)
#     #eve.init(resolution="1280x800", touch="goodix")
#     eve.init(resolution="800x480", touch="capacity")
# 
#     spi1 = eve.spi_sdcard()
#     #eve.calibrate()
#     eve.finish()
#     app=ir_sensors_and_gestures(eve)