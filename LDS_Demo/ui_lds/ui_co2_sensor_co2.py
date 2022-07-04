import time
import json
from .helper import helper
from .gesture import gesture
from .layout import layout
from .LDSBus_Sensor import LDSBus_Sensor
from .ui_common import ui_common
from .ui_co2_sensor import ui_co2_sensor
from .tags import *
from .widgets import widgets_box, widgets_point
from brteve.brt_eve_bt817_8 import BrtEve

class ui_co2_sensor_co2(ui_co2_sensor):
    data_gui=1
    def __init__(self, eve: BrtEve, helper: helper, gesture: gesture, layout: layout,LDSBus_Sensor:LDSBus_Sensor):
        super().__init__(eve , helper, gesture, layout,LDSBus_Sensor)
        self.title="LDSBus CO2 Sensor(CO2)"
        

    def processOne(self,lds,x,y):
        self.circle_box(x =x, y=y, w = 290*2, h = 180*2, border=1, title="CO2",unit="ppm", vmin=0, vmax=30000, lwarning=400, hwarning=27000, value=self.value_co2,tsize=31,scale=2)               
                 
    def draw(self):
        eve = self.eve
        layout = self.layout
        helper=self.helper
        eve.ColorRGB(0xff, 0xff, 0xff)
        
        x = self.x0
        y = self.y0
        FONTSIZE = 29
        eve.cmd_text(x, y, 31, 0, self.title)
        if self.skipSensor: eve.cmd_text(x+70+len(self.title)*FONTSIZE, y, 28, 0, self.simulatorTitle)        
        self.drawBtn()
        ymargin = 50
        y +=  ymargin
        widgets_box(eve,x,y-1,800,1, 1, [0x00, 0xff, 0xff])      
        x+=100
        y+=20
        self.processOne(self.LDSBus_Sensor.lds,x,y) 
        self.preNext()
           