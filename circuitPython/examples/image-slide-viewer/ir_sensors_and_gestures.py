import os
import sys
import time
sys.path.append("lib/adafruit")
sys.path.append("lib/adafruit/adafruit_apds9960")

from brteve.brt_eve_bt817_8 import BrtEve
import busio
import board

from adafruit_apds9960 import apds9960 as APDS9960_Sensor

import pulseio
import adafruit_irremote

from ui_astc import ui_astc
from ui_jpeg import ui_jpeg
from gesture import gesture

class ir_sensors_and_gestures:
    def __init__(self, eve: BrtEve):
        self.eve=eve

        self.show_jpeg('/sd/Test/image_slide_viewer')

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
        eve = self.eve
        while 1:
            try:
                arr = os.listdir(path)
            except Exception:
                print("No such folder", path)
                continue
            break

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
        touch_ges = gesture(self.eve)
        while True:
            ges = self.get_left_right(apds, pulsein, decoder)
            touch_ges.renew()

            if ges == 'left' or touch_ges.sIsSwipe == touch_ges.SWIPE_LEFT:
                demo.swipe_left()
            elif ges == 'right' or touch_ges.sIsSwipe == touch_ges.SWIPE_RIGHT:
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
            except Exception:
                print("Failed to decode IR pulse")
                pulsein.clear()
                pulsein.resume()
        return 'none'
