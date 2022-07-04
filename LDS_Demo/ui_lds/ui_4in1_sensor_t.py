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

class ui_4in1_sensor_t(ui_4in1_sensor):
    
    def __init__(self, eve: BrtEve, helper: helper, gesture: gesture, layout: layout,LDSBus_Sensor:LDSBus_Sensor):
        super().__init__(eve , helper, gesture, layout,LDSBus_Sensor)
        self.title="LDSBus 4in1 Sensor(Temperature)"        


    def boxE(self, x, y, w, h, border=1):
        e = self.eve
        # big border
        e.ColorRGB(255, 255, 255)
        e.Begin(e.RECTS)
        e.LineWidth(1)
        e.Vertex2f(x, y)
        e.Vertex2f(x + w, y + h)
        
    def processOne(self,lds,x,y):
        boxW=self.boxW 
        boxH=self.boxH
        self.eve.SaveContext() 
        self.barGraphHis(x = x, y=y, w = boxW, h = boxH, border=1,data=ui_4in1_sensor.temperature_data,scale=2, blend=1) 
        self.blendBk(x=x,y=y,w=boxW,h = boxH, border=1,scale=2  ,blend=1) 
        self.eve.RestoreContext()
        self.coordinateMarker(x,y,2*boxW,2*boxH,0,2,0 ,tvalue=self.value_t)
        PADDING_X=30                       
                 
    def draw(self):
        eve = self.eve
        layout = self.layout
        helper=self.helper
        eve.ColorRGB(0xff, 0xff, 0xff)
        eve.BlendFunc(eve.SRC_ALPHA, eve.ONE_MINUS_SRC_ALPHA) #reset to  default

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
        if self.firstTime:  self.firstTime=False; print("lds:",self.LDSBus_Sensor.lds)
        ms = time.monotonic_ns() / 1000_000
        if ms - self.last_timeout < self.readingInterval: return
        self.last_timeout =  time.monotonic_ns() / 1000_000
        if self.readOne(self.LDSBus_Sensor.lds)>0:
            self.last_timeout =  time.monotonic_ns() / 1000_000
 