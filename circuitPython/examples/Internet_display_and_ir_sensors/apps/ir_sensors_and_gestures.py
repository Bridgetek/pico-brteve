import sys
sys.path.append("../lib/adafruit")
sys.path.append("../lib/adafruit/adafruit_apds9960")

import time
import random
import math
import bteve as eve_module
import busio
import board
import sdcardio
import storage
import os

import adafruit_vc0706 as adafruit_vc0706_
from adafruit_apds9960 import apds9960 as APDS9960_Sensor
from adafruit_apds9960 import colorutility

import pulseio
import adafruit_irremote

class gui2_astc:
    def __init__(self, eve:eve_module.Brt_PicoEve_Module) -> None:
        self.eve=eve
        self.x = 0
        self.y = 0
        self.w = 0
        self.h = 0
        self.index = 0
        self.image_num = 0
        self.images=[]
        self.names=[]
        self.sizes=[]
        pass
        
    def set_images(self, images, names, size):
        self.images=images
        self.names=names
        self.sizes=size
        print("--------------------------------")
        print(self.sizes)
        print(self.sizes[self.index])
        print(self.sizes[self.index][0])
        print("--------------------------------")
        self.image_num=len(images)
        self.draw_1_image(images[self.index], self.names[self.index], self.sizes[self.index])        

    def swipe_left(self):
        self.swipe_image('left')
        self.index+=1
        if self.index >= self.image_num:
            self.index=0
        self.draw_1_image(self.images[self.index], self.names[self.index], self.sizes[self.index])

    def swipe_right(self):
        self.swipe_image('right')
        self.index -=1
        if self.index < 0:
            self.index=self.image_num-1
            
        self.draw_1_image(self.images[self.index], self.names[self.index], self.sizes[self.index])

    def swipe_image(self, direc):
        return
        eve=self.eve
        offset = 0
        
        x = self.x
        y = self.y
        w = self.w
        h = self.h
        name = self.name

        if direc=='right':
            direc=-1
        else:
            direc=1
        
        while offset < (x + w):
            eve.ClearColorRGB(255, 255, 255)
            eve.Clear()
            eve.ColorRGB(255, 255, 255)
            eve.VertexFormat(4)
            
            eve.Begin(eve.BITMAPS)
            eve.Vertex2f(x-offset*direc, y)

            tx = x - offset*direc + w/2 - len(name) * 5
            ty = y + h + 10

            eve.ColorRGB(0, 0, 0)
            eve.cmd_text(tx, ty, 30, 0, name)
            eve.swap()
            offset += 10 + offset/5
            time.sleep(0.01)

        eve.ClearColorRGB(255, 255, 255)
        eve.Clear()
        eve.ColorRGB(255, 255, 255)
        eve.swap()

    def draw_1_image(self,image, name, size):
        eve=self.eve

        print(size)
        print(size[0])
        w=(int)(size[0])
        h=(int)(size[1])

        addr=0
        wp=addr
        CHUNK= 32
        with open(image, 'rb') as file:
            while chunk := file.read(CHUNK):
                eve.wr(wp, chunk)
                wp += CHUNK

        eve.ClearColorRGB(255, 255, 255)
        eve.Clear()
        eve.ColorRGB(255, 255, 255)
        eve.VertexFormat(4)
    
        eve.cmd_setbitmap(addr, eve.ASTC_8x8, w, h);
        # eve.cmd_setbitmap(addr, eve.ASTC_4x4, w, h);

        x=eve.w/2-w/2
        y=eve.h/2-h/2

        eve.Begin(eve.BITMAPS)
        eve.Vertex2f(x, y)

        tx = x + w/2 - len(name) * 5
        ty = y+ h + 10

        eve.ColorRGB(0, 0, 0)
        eve.cmd_text(tx, ty, 30, 0, name)
        eve.swap()
        eve.flush()

        self.x = x
        self.y = y
        self.w = w
        self.h = h
        self.name = name

class gui1_jpeg:
    def __init__(self, eve:eve_module.Brt_PicoEve_Module) -> None:
        self.eve=eve
        self.x = 0
        self.y = 0
        self.w = 0
        self.h = 0
        self.index = 0
        self.image_num = 0
        self.images=[]
        self.names=[]
        pass
        
    def set_images(self, images, names):
        self.images=images
        self.names=names
        self.image_num=len(images)
        self.draw_1_image(images[self.index], self.names[self.index])

    def swipe_left(self):
        self.swipe_image('left')
        self.index+=1
        if self.index >= self.image_num:
            self.index=0
        self.draw_1_image(self.images[self.index], self.names[self.index])

    def swipe_right(self):
        self.swipe_image('right')
        self.index -=1
        if self.index < 0:
            self.index=self.image_num-1
            
        self.draw_1_image(self.images[self.index], self.names[self.index])

    def swipe_image(self, direc):
        eve=self.eve
        offset = 0
        
        x = self.x
        y = self.y
        w = self.w
        h = self.h
        name = self.name

        if direc=='right':
            direc=-1
        else:
            direc=1
        
        while offset < (x + w):
            eve.ClearColorRGB(255, 255, 255)
            eve.Clear()
            eve.ColorRGB(255, 255, 255)
            eve.VertexFormat(4)
            
            eve.Begin(eve.BITMAPS)
            eve.Vertex2f(x-offset*direc, y)

            tx = x - offset*direc + w/2 - len(name) * 5
            ty = y + h + 10

            eve.ColorRGB(0, 0, 0)
            eve.cmd_text(tx, ty, 30, 0, name)
            eve.swap()
            offset += 10 + offset/5
            time.sleep(0.01)

        eve.ClearColorRGB(255, 255, 255)
        eve.Clear()
        eve.ColorRGB(255, 255, 255)
        eve.swap()

    def draw_1_image(self,image, name):
        eve=self.eve
        eve.swap()

        eve.ClearColorRGB(255, 255, 255)
        eve.Clear()
        eve.ColorRGB(255, 255, 255)
        eve.VertexFormat(4)

        eve.cmd_loadimage(0, 0)
        eve.load(open(image, "rb"))
    
        eve.flush()
        rp=eve.EVE_Cmd_wp();
        eve.cmd_getprops(0, 0, 0)
        eve.flush()
        rp2=eve.EVE_Cmd_wp(); 
        ptr=eve.rd32(eve.RAM_CMD + rp+4*1);
        w=eve.rd32(eve.RAM_CMD + rp+4*2);
        h=eve.rd32(eve.RAM_CMD + rp+4*3);

        if w<0 or w > eve.w:
            w=500
        if h<0 or h > eve.h:
            h=500

        x=eve.w/2-w/2
        y=eve.h/2-h/2

        eve.Begin(eve.BITMAPS)
        eve.Vertex2f(x, y)

        tx = x + w/2 - len(name) * 5
        ty = y+ h + 10

        eve.ColorRGB(0, 0, 0)
        eve.cmd_text(tx, ty, 30, 0, name)
        eve.swap()
        eve.flush()

        self.x = x
        self.y = y
        self.w = w
        self.h = h
        self.name = name

class ir_sensors_and_gestures:
    def __init__(self, eve):
        self.eve=eve

        #self.show_astc('/sd/Test/DemoImageSlideShow/8x8')
        self.show_jpeg('/sd/Test/DemoImageSlideShow/jpg/compressed')
    
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

        demo=gui2_astc(eve)
        demo.set_images(images, names, size)

        while True:
            gesture = self.get_left_right(apds, pulsein, decoder)
            if gesture == 'left':
                demo.swipe_left()
            elif gesture == 'right':
                demo.swipe_right()

    def show_jpeg(self, path):
        eve = self.eve

        arr = os.listdir(path)
        images=[]
        names=[]
        for f in arr:
            if '.jpg' in f:
                images.append(path + '/' +f)
                names.append(f)

        demo=gui1_jpeg(eve)
        demo.set_images(images, names)

        apds=self.init_APDS9960()
        pulsein, decoder=self.init_IR()
        while True:
            gesture = self.get_left_right(apds, pulsein, decoder)
            if gesture == 'left':
                demo.swipe_left()
            elif gesture == 'right':
                demo.swipe_right()

    def init_APDS9960(self):
        i2c=busio.I2C(board.GP21, board.GP20)
        apds = APDS9960_Sensor.APDS9960(i2c)
        apds.enable_proximity = True
        #apds.enable_color = True
        apds.enable_gesture = True
        return apds

    def init_IR(self):
        pulsein = pulseio.PulseIn(board.GP1, maxlen=512, idle_state=True)
        pulsein.clear()
        decoder = adafruit_irremote.GenericDecode()
        return pulsein, decoder

    def get_left_right(self, apds, pulsein, decoder):
        pulsein.clear()
        time.sleep(0.1)
        gesture = apds.gesture()
        if gesture == 0x01:
            print("up")
            return 'up'
        elif gesture == 0x02:
            print("down")
            return 'down'
        elif gesture == 0x03:
            print("left")
            return 'left'
        elif gesture == 0x04:
            print("right")
            return 'right'
        else:
            pass

        # pulsein.clear()
        # pulsein.resume()
        time.sleep(0.1)
        pulses = decoder.read_pulses(pulsein,blocking=False, max_pulse=2000)
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
            except:   
                print("Failed to decode IR pulse")
                pulsein.clear()
                pulsein.resume()



        
