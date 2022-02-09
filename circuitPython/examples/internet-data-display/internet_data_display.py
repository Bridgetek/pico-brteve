import time

# Getwifi details and more from a secrets.py file
from secrets import wifi_secrets, aio_secrets
from page1_ui import Page1_UI
from page2_ui import Page2_UI

import board
import busio
from digitalio import DigitalInOut

from sensors.wifi import wifi
from sensors.air_quality import CCS811_and_BME280_SparkFun
from sensors.aio import aio
from sensors.sparkfun_max30101_max32664 import SparkFun_MAX30101_And_MAX32664

from brteve.brt_eve_bt817_8 import BrtEve

_BUFFER_DOWNLOADED_IMG = 1
class internet_display:
    def __init__(self, eve: BrtEve, spi) -> None:
        self.eve=eve
        self.start_time_ns = 0
        self.hh=00 # start hour
        self.mm=00 # start minute
        TOTAL_STEPS = 7
        step = 0
        self._ast_img_id = 0

        self._skip_sensor_bio = 0
        self._skip_sensor_air = 0
        self._skip_sensor_wifi = 0

        print("Start UI...")
        self.page1 = Page1_UI(eve)
        self.page2 = Page2_UI(eve)

        # init CCS811_and_BME280
        self.init_screen("Initializing Air sensor ...", step, TOTAL_STEPS)
        step += 1
        self.i2c = busio.I2C(board.GP19, board.GP18)  # uses board.SCL and board.SDA
        self.air = 0
        self.init_air_sensor(self.i2c)
        if self._skip_sensor_air:
            self.init_screen("Air sensor is skipped", step, TOTAL_STEPS)
            time.sleep(2)

        # init SparkFun Pulse Oximeter and Heart Rate Monitor
        self.init_screen("Initializing Pulse Oximeter and Heart Rate Monitor sensor ...",
            step, TOTAL_STEPS)
        step += 1
        self.i2c2 = busio.I2C(board.GP21, board.GP20)  # uses board.SCL and board.SDA
        self.bio=0
        self.init_bio_sensor(self.i2c2)
        if self._skip_sensor_bio:
            self.init_screen("Biometric sensor is skipped", step, TOTAL_STEPS)
            time.sleep(2)

        #init wifi
        self.init_screen("Initializing Wifi module ...", step, TOTAL_STEPS)
        step += 1
        self.wifi=0
        self.spi = spi
        self.wifi_secrets = wifi_secrets
        self.ini_wifi(self.spi, self.wifi_secrets)
        if self._skip_sensor_wifi:
            self.init_screen("Wifi module is skipped", step, TOTAL_STEPS)
            time.sleep(2)

        if not self._skip_sensor_wifi:
            self.init_screen("Connecting to wifi ...", step, TOTAL_STEPS)
            step += 1
            self.esp32_cs    = DigitalInOut(board.GP22)
            self.esp32_ready = DigitalInOut(board.GP0)
            self.esp32_reset = DigitalInOut(board.GP1)
            self.connect_wifi(self.esp32_cs, self.esp32_ready, self.esp32_reset)
            if self._skip_sensor_wifi:
                self.init_screen("Wifi module is skipped", step, TOTAL_STEPS)
                time.sleep(2)

        if not self._skip_sensor_wifi:
            #init aio
            print("Connecting to Adafruit IO ...")
            self.init_screen("Connecting to Adafruit IO ...", step, TOTAL_STEPS, False)
            step += 1
            soc=self.wifi.socket
            esp=self.wifi.esp
            print(aio_secrets)
            self.aio=aio(soc, esp, aio_secrets, "temp")

            print("Fetching realtime clock...")
            self.init_screen("Fetching realtime clock...", step, TOTAL_STEPS, False)
            step += 1
            self.get_time()

        print("Starting page 1...")
        self.init_screen("Starting page 1...", step, TOTAL_STEPS, False)
        step += 1
        self.page1.start()

        self._last_touch = 0
        self.loop()

    def init_air_sensor(self, i2c):
        print("Initializing Air sensor ...")
        time.sleep(0.5)
        _try=0
        while 1:
            try:
                tag = self.eve.rd32(self.eve.REG_TOUCH_TAG) & 0xFF
                if (tag == 1 or _try == 10):
                    print("Sensor is skipped")
                    self._skip_sensor_air = 1
                    return
                _try += 1
                self.air = CCS811_and_BME280_SparkFun(i2c)
            except Exception as e:
                print(e)
                time.sleep(0.1)
                continue
            break

    def init_bio_sensor(self, i2c):
        print("Initializing Pulse Oximeter and Heart Rate Monitor sensor ...")
        time.sleep(0.5)
        _try=0
        while 1:
            try:
                tag = self.eve.rd32(self.eve.REG_TOUCH_TAG) & 0xFF
                if (tag == 1 or _try == 10):
                    print("Sensor is skipped")
                    self._skip_sensor_bio = 1
                    return
                _try += 1
                self.bio = SparkFun_MAX30101_And_MAX32664(i2c)
                self.bio.configBpm(self.bio.MODE_ONE)
            except Exception as e:
                print(e)
                time.sleep(0.1)
                continue
            break

    def ini_wifi(self, spi, secret):
        print("Initializing Wifi module ...")
        time.sleep(0.5)
        _try=0
        while 1:
            try:
                tag = self.eve.rd32(self.eve.REG_TOUCH_TAG) & 0xFF
                if (tag == 1 or _try == 10):
                    print("Sensor is skipped")
                    self._skip_sensor_wifi = 1
                    return
                _try += 1
                self.wifi = wifi(spi, secret)
            except Exception as e:
                print(e)
                time.sleep(0.1)
                continue
            break

    def connect_wifi(self, esp32_cs, esp32_ready, esp32_reset):
        if self._skip_sensor_wifi: return

        _try=0
        while 1 and self._skip_sensor_wifi == 0:
            try:
                tag = self.eve.rd32(self.eve.REG_TOUCH_TAG) & 0xFF
                if (tag == 1 or _try == 10):
                    print("Sensor is skipped")
                    self._skip_sensor_wifi = 1
                    return
                _try += 1
                self.wifi.connect(esp32_cs, esp32_ready, esp32_reset)
            except Exception as e:
                print(e)
                time.sleep(0.1)
                continue
            break

    def loop(self):
        _page_active = 2
        while 1:
            if _page_active == 1:
                self.page_1_update()
            else:
                self.page_2_update()

            for i in range(30):
                if self.isTouch() != 0:
                    self._last_touch = 0
                    _page_active = (_page_active+1) % 2
                    break

    def init_screen(self, message, step, totalstep, skip_button = True):
        eve=self.eve
        eve.swap()
        eve.ClearColorRGB(255, 255, 255)
        eve.Clear()
        eve.ColorRGB(255, 255, 255)
        eve.VertexFormat(4)

        tx=eve.lcd_width/2 - len(message) * 6
        ty=eve.lcd_height/2

        eve.ColorRGB(0x90, 0xC8, 0x3A)
        eve.cmd_text(tx, ty, 30, 0, message)

        if skip_button ==  True:
            w=100
            h=50
            eve.Tag(1)
            eve.cmd_button(2, 2, w, h, 20, 0, "Skip this sensor")

        eve.ColorRGB(0, 0, 0)
        eve.cmd_text(eve.lcd_width/2 - 130, ty + 200 , 30, 0,
            "Initializing... (" + str((int)(step / totalstep * 100)) + "%)" )
        eve.swap()
        eve.flush()

    def page_2_update(self):
        page2=self.page2
        if self._skip_sensor_wifi:
            return

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
            img = 'https://raw.githubusercontent.com/BRTSG-FOSS/pico-brteve/main/circuitPython/examples/internet-data-display/images/' + img_array[self._ast_img_id]
            page2.message('Downloading ' + img, ' ')
            try:
                r=self.wifi.get(img)
            except Exception as e:
                print(e, img)
                return
            t=r.iter_content(chunk_size=(64 * 4))
            num=0

            if _BUFFER_DOWNLOADED_IMG == 1:
                with open(imagepath, "wb") as file2:
                    for buff in t:
                        file2.write(buff)
                        chunksize=len(buff)
                        if chunksize < 0:
                            print("Image cannot be downloaded")
                            self._ast_img_id +=1
                            self._ast_img_id %= 6
                            return

                        num += chunksize
                        page2.message('Downloading ' + img, str(num) + " bytes")
                        if self.isTouch():
                            return # cancel downloading
                print("Displaying image: ", imagepath)
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
                # clear screen and return
                eve=self.eve
                eve.ClearColorRGB(255, 255, 255)
                eve.Clear()
                eve.ColorRGB(255, 255, 255)
                eve.swap()
                eve.flush()
                return

    def page_1_update(self):
        while 1:
            if not self._skip_sensor_wifi:
                try:
                    self.aio.get_oi().loop()
                except Exception as e:
                    self.page1.message = "Disconnected to AIO, trying to reconnect..."
                    self.page1.start()

                    self.ini_wifi(self.spi, self.wifi_secrets)
                    self.connect_wifi(self.esp32_cs, self.esp32_ready, self.esp32_reset)
                    aio(self.wifi.socket, self.wifi.esp, aio_secrets, "temp")

            is_finger = 0
            hr = 0
            ox = 0
            if not self._skip_sensor_bio:
                try:
                    is_finger, hr, ox = self.get_bio()
                except Exception as e:
                    self.page1.message = "Lost connection to BIO sensor, retrying ..."
                    self.page1.start()
                    self.init_bio_sensor(self.i2c2)

            temperature = 0
            co2 = 0
            tvoc = 0
            press = 0
            humidity = 0
            if not self._skip_sensor_air:
                try:
                    temperature, co2, tvoc, press, humidity = self.get_air()
                except Exception as e:
                    self.page1.message = "Lost connection to AIR sensor, retrying ..."
                    self.page1.start()
                    self.init_air_sensor(self.i2c)

            co2 = round(co2, 1)
            tvoc = round(tvoc, 1)
            press = round(press, 1)
            hr = round(hr, 1)
            ox = round(ox, 1)
            temperature = round(temperature, 1)

            try:
                self.update_local(temperature, co2, tvoc, press, hr, ox, humidity)
                if self.isTouch():
                    return
                self.update_aio(is_finger, temperature, co2, tvoc, press, hr, ox, humidity)
                print(".", end='')
                time.sleep(0.1)
            except Exception as e:
                print(e)
                print("page_1_update Error...")

                if not self._skip_sensor_wifi:
                    self.page1.message = "Disconnected to AIO, trying to reconnect..."
                    self.page1.start()

                    self.ini_wifi(self.spi, self.wifi_secrets)
                    self.connect_wifi(self.esp32_cs, self.esp32_ready, self.esp32_reset)
                    aio(self.wifi.socket, self.wifi.esp, aio_secrets, "temp")

                    time.sleep(0.1)
                continue
    def get_air(self):
        temperature = co2 = tvoc = press = humidity = 0

        temperature = self.air.temperature
        if self.isTouch():
            return temperature, co2, tvoc, press, humidity

        co2 = self.air.co2
        if self.isTouch():
            return temperature, co2, tvoc, press, humidity

        tvoc = self.air.tvoc
        if self.isTouch():
            return temperature, co2, tvoc, press, humidity

        press = round(self.air.pressure, 2)
        if self.isTouch():
            return temperature, co2, tvoc, press, humidity

        humidity = self.air.humidity
        return temperature, co2, tvoc, press, humidity

    def get_bio(self):
        self.bio.readBpm()
        is_finger = self.bio.finger_detected == 3
        hr = self.bio.heart_rate
        if self.isTouch():
            return is_finger, hr, 0
        ox = self.bio.oxygen
        return is_finger, hr, ox

    def get_time(self):
        for _ in range(10):
            try:
                url = self.aio.aio_url_hh_mm()
                print("time url = ", url)

                r = self.wifi.get(url)
                print(r)
                print(r.text)
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
                print("get_time Error...")
                print(e)
                continue

    def update_local(self, temperature, co2, tvoc, press, hr, oxy, humidity):
        try:
            self.page1.set_temp(temperature)
            self.page1.set_co2(co2)
            self.page1.set_tvoc(tvoc)
            self.page1.set_pressure(press)

            self.page1.push_humidity(humidity)

            self.page1.set_hr(hr)
            self.page1.set_oxygen(oxy)

            self.page1.start()
        except Exception as e:
            print("update_local failed")
            print(e)

    last_push = 0
    queue_push = 0
    turn_bio_count  = 0
    def update_aio(self, is_finger, temperature, co2, tvoc, press, hr, oxy, humidity):
        if self._skip_sensor_wifi:
            return
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
                    self.update_aio_bio(hr, oxy)

                    self.last_push =  time.monotonic_ns()
                    self.page1.message = "Push biometric data..."
            elif push_air == 1:
                self.queue_push = 2
                if time.monotonic_ns() - self.last_push > second_per_push * 1e9 * 5:
                    self.queue_push = 0
                    self.update_aio_air(temperature, co2, tvoc, press, humidity)

                    self.last_push =  time.monotonic_ns()
                    self.page1.message = "Push air data..."
        except Exception as e:
            print("update_aio Error...")
            print(e)

    def update_aio_bio(self, hr, oxy):
        if hr > 0:
            self.aio.get_oi().publish("heartrate", hr)
            if self.isTouch():
                return

        if oxy > 0:
            self.aio.get_oi().publish("oxygen", oxy)

    def update_aio_air(self, temperature, co2, tvoc, press, humidity):
        self.aio.get_oi().publish("co2", co2)
        if self.isTouch():
            return

        self.aio.get_oi().publish("tvoc", tvoc)
        if self.isTouch():
            return

        self.aio.get_oi().publish("humidity", humidity)
        if self.isTouch():
            return

        self.aio.get_oi().publish("pressure", press)
        if self.isTouch():
            return

        self.aio.get_oi().publish("temp", temperature)

    def isTouch(self):
        if self._last_touch != 0:
            return self._last_touch

        touch = self.eve.rd32(self.eve.REG_TOUCH_RAW_XY)
        is_touch = touch != 0xFFFFFFFF

        self._last_touch = is_touch

        return self._last_touch
