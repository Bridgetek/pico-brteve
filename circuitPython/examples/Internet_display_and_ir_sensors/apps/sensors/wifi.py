import random
import math
import busio
import board
import sdcardio
import storage
import os
import sys

sys.path.append("../lib/adafruit")
sys.path.append("../lib/adafruit/adafruit_esp32spi")

# ESP 32 air lift
from digitalio import DigitalInOut
import adafruit_requests as requests
import adafruit_esp32spi.adafruit_esp32spi_socket as socket
from adafruit_esp32spi import adafruit_esp32spi

class wifi():
    def __init__(self, spi, secrets):
        self.secrets=secrets
        self.spi=spi

    def connect(self, cs=board.GP22, rd=board.GP0, rs=board.GP1, clk=board.GP10, MOSI=board.GP11, MISO=board.GP12):
        print("ESP32 SPI webclient test")

        esp32_cs   = DigitalInOut(cs)
        esp32_ready = DigitalInOut(rd)
        esp32_reset = DigitalInOut(rs)

        spi = self.spi#busio.SPI(clk, MOSI, MISO)
        esp = adafruit_esp32spi.ESP_SPIcontrol(spi, esp32_cs, esp32_ready, esp32_reset)

        requests.set_socket(socket, esp)

        if esp.status == adafruit_esp32spi.WL_IDLE_STATUS:
            print("ESP32 found and in idle mode")
        print("Firmware vers.", esp.firmware_version)
        print("MAC addr:", [hex(i) for i in esp.MAC_address])

        for ap in esp.scan_networks():
            print("\t%s\t\tRSSI: %d" % (str(ap["ssid"], "utf-8"), ap["rssi"]))

        print("Connecting to AP...")
        i=0
        while not esp.is_connected:
            secrets=self.secrets[i]
            i+=1
            if i >= len(self.secrets):
                i=0
            try:
                print("ap=%s, pass=%s" % (secrets["ssid"], secrets["password"]))
                esp.connect_AP(secrets["ssid"], secrets["password"], timeout_s=10)
            except RuntimeError as e:
                print("could not connect to AP, retrying: ", e)
                continue

        print("Connected to", str(esp.ssid, "utf-8"), "\tRSSI:", esp.rssi)
        print("My IP address is", esp.pretty_ip(esp.ip_address))

        self.socket=socket
        self.esp=esp
    
    @property
    def get_socket(self):
        return self.socket

    @property
    def get_esp(self):
        return self.esp

    def get(self, url):
        return requests.get(url)
        
    def request(self, url):
        return requests.request(url)

    def get_raw(self, url):
        print("Fetching text from", url)
        r = requests.get(url)
        print('-'*40)
        print(r.text)
        print('-'*40)
        r.close()
    
    def get_json(self, url):
        print()
        print("Fetching json from", url)
        r = requests.get(url)
        print('-'*40)
        print(r.json())
        print('-'*40)
        r.close()


        print("Raspberry Pi RP2040 - ESP32 SPI webclient test")

        TEXT_URL = "http://wifitest.adafruit.com/testwifi/index.html"
        JSON_URL = "http://api.coindesk.com/v1/bpi/currentprice/USD.json"

        wf = wifi()
        wf.connect()
        wf.get_raw(TEXT_URL)

    
