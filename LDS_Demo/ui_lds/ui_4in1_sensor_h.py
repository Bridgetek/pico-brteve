import time
import json
from .helper import helper
from .gesture import gesture
from .layout import layout
from .LDSBus_Sensor import LDSBus_Sensor
from .ui_common import ui_common
from .ui_4in1_sensor import ui_4in1_sensor
from .tags import *
from .widgets import widgets_box, widgets_point
from brteve.brt_eve_bt817_8 import BrtEve

class ui_4in1_sensor_h(ui_4in1_sensor):
    data_gui=1
    def __init__(self, eve: BrtEve, helper: helper, gesture: gesture, layout: layout,LDSBus_Sensor:LDSBus_Sensor):
        super().__init__(eve , helper, gesture, layout,LDSBus_Sensor)
        self.title="LDSBus 4in1 Sensor(Humidity)"

    def processOne(self,lds,x,y):        
        self.statitics_box(x = x, y=y, w = self.boxW*2, h = self.boxH*2, border=1,data=ui_4in1_sensor.humidity_data,scale=2,tvalue=self.value_h)
                                          
    def draw(self):
        eve = self.eve
        layout = self.layout
        helper=self.helper
        eve.ColorRGB(0xff, 0xff, 0xff)
        
        x = self.x0
        y = self.y0
        FONTSIZE = 29
        eve.cmd_text(x, y, 31, 0, self.title)
        if self.skipSensor: eve.cmd_text(x+len(self.title)*FONTSIZE, y, 28, 0, self.simulatorTitle)        
        self.drawBtn()
        ymargin = 50
        y +=  ymargin
        widgets_box(eve,x,y-1,800,1, 1, [0x00, 0xff, 0xff])           
        eve.ColorRGB(255, 255, 255)
        x+=100
        y+=20
        self.processOne(self.LDSBus_Sensor.lds,x,y) 
        ms = time.monotonic_ns() / 1000_000
        if ms - self.last_timeout < self.readingInterval: return
        self.last_timeout =  time.monotonic_ns() / 1000_000
        if self.readOne(self.LDSBus_Sensor.lds)>0:
            self.last_timeout =  time.monotonic_ns() / 1000_000
 
           