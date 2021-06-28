import sys
sys.path.append("../../lib")

import random
import math
import bteve as eve_module
import busio
import board
import sdcardio
import storage
import os
import time

from ..sensors.wifi import wifi 
from ..sensors.air_quality import CCS811_and_BME280_SparkFun
from ..sensors.aio import aio
from ..sensors.sparkfun_max30101_max32664 import SparkFun_MAX30101_And_MAX32664

# Get wifi details and more from a secrets.py file
from .secrets import wifi_secrets, aio_secrets
from .page1_ui import Page1_UI
from .page2_ui import Page2_UI

_BUFFER_DOWNLOADED_IMG = 0
class internet_display:
    def __init__(self, eve:eve_module.Brt_PicoEve_Module, spi1) -> None:
        self.eve=eve
        self.start_time_ns = 0
        self.hh=00 # start hour
        self.mm=00 # start minute
        TOTAL_STEPS = 7
        step = 0
        self._ast_img_id = 0

        print("Start UI...")
        self.page1 = Page1_UI(eve)
        self.page2 = Page2_UI(eve)

        # init CCS811_and_BME280
        print("Initializing Air sensor ...")
        self.init_screen("Initializing Air sensor ...", step, TOTAL_STEPS)
        step += 1
        i2c = busio.I2C(board.GP19, board.GP18)  # uses board.SCL and board.SDA
        self.air = 0
        while 1:
            try:
                self.air = CCS811_and_BME280_SparkFun(i2c)
            except Exception as e:
                print(e)
                time.sleep(0.1)
                continue
            break

        # init SparkFun Pulse Oximeter and Heart Rate Monitor
        print("Initializing Pulse Oximeter and Heart Rate Monitor sensor ...")
        self.init_screen("Initializing Pulse Oximeter and Heart Rate Monitor sensor ...", step, TOTAL_STEPS)
        step += 1
        i2c2 = busio.I2C(board.GP21, board.GP20)  # uses board.SCL and board.SDA
        self.bio=0
        while 1:
            try:
                self.bio = SparkFun_MAX30101_And_MAX32664(i2c2)
                self.bio.configBpm(self.bio.MODE_ONE)
            except Exception as e:
                print(e)
                time.sleep(0.1)
                continue
            break

        #init wifi
        print("Initializing Wifi module ...")
        self.init_screen("Initializing Wifi module ...", step, TOTAL_STEPS)
        step += 1
        self.wifi=0
        while 1:
            try:
                self.wifi = wifi(spi1, wifi_secrets)
            except Exception as e:
                print(e)
                time.sleep(0.1)
                continue
            break

        self.init_screen("Connecting to wifi ...", step, TOTAL_STEPS)
        step += 1
        while 1:
            try:
                self.wifi.connect(cs=board.GP22, rd=board.GP0, rs=board.GP1, clk=board.GP10, MOSI=board.GP11, MISO=board.GP12)
            except Exception as e:
                print(e)
                time.sleep(0.1)
                continue
            break

        #init aio
        print("Connecting to Adafruit IO ...")
        self.init_screen("Connecting to Adafruit IO ...", step, TOTAL_STEPS)
        step += 1
        soc=self.wifi.socket
        esp=self.wifi.esp
        self.aio=aio(soc, esp, aio_secrets, "temp")

        self.init_screen("Fetching realtime clock...", step, TOTAL_STEPS)
        step += 1  
        self.get_time()

        self.init_screen("Starting page 1...", step, TOTAL_STEPS)
        step += 1
        self.page1.start()
        
        self._last_touch = 0
        self.loop()
        
    def loop(self):
        _page_active = 1
        while 1:
            if _page_active == 1:
                self.page_1_update()
            else:
                self.page_2_update()
            
            if self.isTouch() != 0:
                self._last_touch = 0
                if _page_active == 1:
                    _page_active = 0
                else:
                    _page_active = 1
                # _page_active = (_page_active+1)% 2

    def init_screen(self, message, step, totalstep):
        eve=self.eve
        eve.swap()
        eve.ClearColorRGB(255, 255, 255)
        eve.Clear()
        eve.ColorRGB(255, 255, 255)
        eve.VertexFormat(4)

        tx=eve.w/2 - len(message) * 6
        ty=eve.h/2
        
        eve.ColorRGB(0x90, 0xC8, 0x3A)
        eve.cmd_text(tx, ty, 30, 0, message)

        eve.ColorRGB(0, 0, 0)
        eve.cmd_text(eve.w/2 - 130, ty + 200 , 30, 0, "Initializing... (" + str((int)(step / totalstep * 100)) + "%)" )
        eve.swap()
        eve.flush()

    def page_2_update(self):
        page2=self.page2

        img_array = [
            'image0001.jpg',
            'image0002.jpg',
            'image0003.jpg',
            'image0004.jpg',
            'image0005.jpg',
            'image0006.jpg',
        ]

        imagepath='/sd/image_downloaded.jpg'
        while 1:
            img = 'http://ancuviet.com/' + img_array[self._ast_img_id]
            page2.message('Downloading ' + img, ' ')

            r=self.wifi.get(img)        
            t=r.iter_content(chunk_size=(64 * 4))
            num=0

            if _BUFFER_DOWNLOADED_IMG == 1:
                with open(imagepath, "wb") as file2:
                    for buff in t:
                        file2.write(buff)
                        num +=len(buff)
                        page2.message('Downloading ' + img, str(num) + " bytes")
                        if self.isTouch(): return # cancel downloading
                page2.draw_image(imagepath, img)
            else:
                page2.start_load_image()
                with open(imagepath, "wb") as file2:
                    for buff in t:
                        page2.write_image_buffer(buff)
                        num +=len(buff)
                        print('getting image ' + img, str(num) + " bytes")
                page2.draw_image('', img)

            cmd = 0
            while cmd == 0:
                cmd = page2.get_comand()
                time.sleep(0.01)
            
            print("cmd=", cmd)
            if cmd == page2.CMD_PRIV:
                self._ast_img_id -=1
                if self._ast_img_id < 0:
                    self._ast_img_id  = 5
                continue
            if cmd == page2.CMD_NEXT:
                self._ast_img_id +=1        
                self._ast_img_id %= 6
                continue

            if cmd == page2.CMD_BACK:   
                return

    def page_1_update(self):
        while 1:
            try:
                self.bio.readBpm()
                self.aio.get_oi().loop()
                is_finger = self.bio.finger_detected == 3

                temperature= self.air.temperature
                if self.isTouch(): return

                co2 = self.air.co2
                if self.isTouch(): return
                
                tvoc = self.air.tvoc
                if self.isTouch(): return
                
                press = round(self.air.pressure, 2)
                if self.isTouch(): return
                
                hr = self.bio.heart_rate
                if self.isTouch(): return
                
                ox = self.bio.oxygen
                if self.isTouch(): return
                
                humidity = self.air.humidity

                self.update_local(is_finger, temperature, co2, tvoc, press, hr, ox, humidity)
                if self.isTouch(): return
                
                self.update_aio(is_finger, temperature, co2, tvoc, press, hr, ox, humidity)
                print(".", end='')

                time.sleep(0.1)
            except:
                print("page_1_update Error...")
                time.sleep(0.1)
                continue

    def get_time(self):
        for i in range(10):
            try:
                # url = self.aio.aio_url_hh_mm('SG')
                url = self.aio.aio_url_hh_mm('TH')
                
                r = self.wifi.get(url)
                hh_mm = r.text
                print("time = ", hh_mm)

                self.hh = int(hh_mm[0:2])
                self.mm = int(hh_mm[2:4])
                self.page1.hh = int(hh_mm[0:2])
                self.page1.mm = int(hh_mm[2:4])
                self.start_time_ns = time.monotonic_ns()
                self.page1.start_time_ns = self.start_time_ns

                break
            except Exception as e:
                print(e)
                continue

    def update_local(self, is_finger, temperature, co2, tvoc, press, hr, oxy, humidity):
        try:
            self.page1.set_temp(temperature)
            self.page1.set_co2(co2)
            self.page1.set_tvoc(tvoc)
            self.page1.set_pressure(press)

            self.page1.push_humidity(humidity)

            if is_finger > 0:
                self.page1.set_hr(hr)
                self.page1.set_oxygen(oxy)

            self.page1.start()
        except Exception as e:
            print(e)
            print("update_local failed")
            pass

    last_push = 0
    queue_push = 0
    turn_bio_count  = 0
    def update_aio(self, is_finger, temperature, co2, tvoc, press, hr, oxy, humidity):
        try:
            push_per_min=30 # limitation (30 push per min)
            second_per_push = 60 / push_per_min
            BIO_MULTIPLE = 4
            self.page1.message = ""
            push_bio = 0            
            push_air = 0            
            if is_finger != 0:
                if self.turn_bio_count < BIO_MULTIPLE:
                    push_bio = 1
                    push_air = 0  
                    self.turn_bio_count += 1
                else:
                    self.turn_bio_count = 0
                    push_bio = 0
                    push_air = 1
            else:
                self.turn_bio_count = 0
                push_bio = 0
                push_air = 1
            
            if self.queue_push == 1:
                push_bio = 1
                push_air = 0
            elif self.queue_push == 2:
                push_bio = 0
                push_air = 1

            if push_bio == 1:
                self.queue_push = 1
                if time.monotonic_ns() - self.last_push > second_per_push * 1e9 * 2:
                    self.queue_push = 0
                    self.aio.get_oi().publish("heartrate", hr)
                    if self.isTouch(): return
        
                    self.aio.get_oi().publish("oxygen", oxy)
                    if self.isTouch(): return
        
                    self.last_push =  time.monotonic_ns()
                    self.page1.message = "Push biometric data..."  
                    if self.isTouch(): return

            elif push_air == 1:
                self.queue_push = 2
                if time.monotonic_ns() - self.last_push > second_per_push * 1e9 * 5:
                    self.queue_push = 0
                    if self.isTouch(): return
            
                    self.aio.get_oi().publish("co2", co2)
                    if self.isTouch(): return
            
                    self.aio.get_oi().publish("tvoc", tvoc)
                    if self.isTouch(): return
            
                    self.aio.get_oi().publish("humidity", humidity)
                    if self.isTouch(): return
            
                    self.aio.get_oi().publish("pressure", press)
                    if self.isTouch(): return
            
                    self.aio.get_oi().publish("temp", temperature)
                    if self.isTouch(): return
            
                    self.last_push =  time.monotonic_ns()
                    self.page1.message = "Push air data..."
                    if self.isTouch(): return
        except Exception as e:
            print(e)
            pass
    
    def isTouch(self):
        if self._last_touch != 0:
            return self._last_touch
        
        touch = self.eve.rd32(self.eve.REG_TOUCH_RAW_XY)
        is_touch = touch != 0xFFFFFFFF

        self._last_touch = is_touch

        return self._last_touch

        


