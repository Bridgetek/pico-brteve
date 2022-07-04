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

class ui_co2_sensor_t(ui_co2_sensor):

    def __init__(self, eve: BrtEve, helper: helper, gesture: gesture, layout: layout,LDSBus_Sensor:LDSBus_Sensor):
        super().__init__(eve , helper, gesture, layout,LDSBus_Sensor)
        self.title="LDSBus CO2 Sensor(Temperature)"     
 
    def processOne(self,lds,x,y):
        boxW=self.boxW   
        boxH=self.boxH
        self.eve.SaveContext() 
        self.barGraphHis(x = x, y=y, w = boxW, h = boxH, border=1,data=ui_co2_sensor.temperature_data,scale=2)     
        self.blendBk(x=x,y=y,w=boxW,h = boxH, border=1,scale=2,blend=1) 
        self.eve.RestoreContext()
        self.coordinateMarker(x,y,2*boxW,2*boxH,0,2,0,tvalue=self.value_t,MaxMin=2) 

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
        if self.skipSensor: eve.cmd_text(x+70+len(self.title)*FONTSIZE, y, 28, 0, self.simulatorTitle)        
        self.drawBtn()
        ymargin = 50
        y +=  ymargin
        widgets_box(eve,x,y-1,800,1, 1, [0x00, 0xff, 0xff])      
        x+=100
        y+=20
        self.processOne(self.LDSBus_Sensor.lds,x,y) 
        self.preNext()