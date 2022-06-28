import time

from .helper import helper
from .gesture import gesture
from .datetime import hh_mm, hh_mm_ss_ms, milis, now, print_weekday, random
from .layout import layout
from .LDSBus_Sensor import LDSBus_Sensor
from .ui_common import ui_common
from .ui_config import ui_config
from .tags import *
from . import datetime
from .widgets import widgets_box, widgets_point

import sys
if sys.implementation.name == "circuitpython":
    from brteve.brt_eve_bt817_8 import BrtEve
else:
    from ....lib.brteve.brt_eve_bt817_8 import BrtEve

class ui_lds_Details(ui_config):
    def __init__(self, eve: BrtEve, helper: helper, gesture: gesture, layout: layout,LDSBus_Sensor: LDSBus_Sensor):
        super().__init__()
        self.eve = eve
        self.helper = helper
        self.gesture = gesture
        self.layout = layout
        self.LDSBus_Sensor = LDSBus_Sensor
        self.title="LDSBus Details"
        self._rescan=True
        self.ui_previous=""

    def event(self):
        eve = self.eve
        layout = self.layout
        ges = self.gesture

        tag = ges.get().tagReleased
        if ( tag>0 ): print("lds_scan tag", tag, self.gesture.get().tagReleased, self.gesture.get().tagPressed)

 
    def interrupt(self):
        return 0

    def drawBtn(self):
        eve = self.eve
        eve.ColorRGB(0xff, 0xff, 0xff)
        y = self.layout.APP_Y 
        btn_w = self.btn_w
        btn_h = self.btn_h
        x1 = self.xStart
        x5 = x1 + 310
        eve.Tag(tag_ui_lds_info_back)
        eve.cmd_button(x5, y, btn_w, btn_h, 31, 0, "Back")
        eve.Tag(0)
 

    def draw(self):
        eve = self.eve
        layout = self.layout
        helper=self.helper

        eve.ColorRGB(0xff, 0xff, 0xff)
 
        x = self.x0
        y = self.y0
        FONTSIZE = 29
        eve.cmd_text(x, y, 31, 0, self.title)
        self.drawBtn()
        self.event()
        
        xdistance=300
        ymargin = 70
        y +=  ymargin
       
        distance = 40
        eve.ColorRGB(255, 255, 255)     

        eve.ColorRGB(170, 85, 0)
        eve.cmd_text(x, y, FONTSIZE, 0, "LDS Name")
        eve.cmd_text(x+xdistance, y, FONTSIZE, 0, self.LDSBus_Sensor.sensorType)

        widgets_box(eve,x,y-1,800,1, 1, [0x00, 0xff, 0xff])
        eve.ColorRGB(170, 85, 0)
        y +=  distance
        eve.cmd_text(x, y, 28, 0, "DID")
        eve.cmd_text(x+xdistance, y, FONTSIZE, 0, self.LDSBus_Sensor.lds["DID"])
        widgets_box(eve,x,y-1,800,1, 1, [0x00, 0xff, 0xff])
        eve.ColorRGB(170, 85, 0)
        y +=  distance
        eve.cmd_text(x, y, FONTSIZE, 0, "UID")
        eve.cmd_text(x+xdistance, y, FONTSIZE, 0, self.LDSBus_Sensor.lds["UID"])
        widgets_box(eve,x,y-1,800,1, 1, [0x00, 0xff, 0xff])
        eve.ColorRGB(170, 85, 0)
        y +=  distance
        eve.cmd_text(x, y, FONTSIZE, 0, "OBJ")
        eve.cmd_text(x+xdistance, y, FONTSIZE, 0, self.LDSBus_Sensor.lds["OBJ"])
        widgets_box(eve,x,y-1,800,1, 1, [0x00, 0xff, 0xff])
        eve.ColorRGB(170, 85, 0)
        y +=  distance
        eve.cmd_text(x, y, FONTSIZE, 0, "SNO")
        eve.cmd_text(x+xdistance, y, FONTSIZE, 0, self.LDSBus_Sensor.lds["SNO"])
        widgets_box(eve,x,y-1,800,1, 1, [0x00, 0xff, 0xff])
        eve.ColorRGB(170, 85, 0)
        y +=  distance
        eve.cmd_text(x, y, FONTSIZE, 0, "MFG")
        eve.cmd_text(x+xdistance, y, FONTSIZE, 0, self.LDSBus_Sensor.lds["MFG"])
        widgets_box(eve,x,y-1,800,1, 1, [0x00, 0xff, 0xff])
        eve.ColorRGB(170, 85, 0)
        y +=  distance
        eve.cmd_text(x, y, FONTSIZE, 0, "PRV")
        eve.cmd_text(x+xdistance, y, FONTSIZE, 0, self.LDSBus_Sensor.lds["PRV"])
        widgets_box(eve,x,y-1,800,1, 1, [0x00, 0xff, 0xff])


        eve.ColorRGB(255, 255, 255)

""" 
        i=0
        for index, lds in self.LDSBus_Sensor._lds_list.items():
            if (i %2)==0: eve.ColorRGB(170, 85, 0)
            esle:eve.ColorRGB(85, 85, 0)
            eve.Tag(tag_ui_lds_data_0+i) # start from 1
            eve.cmd_text(x, y, 28, 0, lds['NAME'])
            eve.cmd_text(x+310, y, 28, 0, lds['DID'])
            eve.cmd_text(x+365, y, 28, 0, lds['UID'])
            eve.cmd_text(x+640, y, 28, 0, lds['OBJ'])
            y+=distance
            i=i+1
            widgets_box(eve,x,y-1,800,1, 1, [0x00, 0xff, 0xff])
            """
"""
   {3: {'TERM': 'off', 'PRV': '1.0', 'NAME': 'LDSBus Thermocouple Sensor', 'MFG': '23062021', 'DID': '3', 'NICK': 'LDSBus Thermocouple Sensor', 'UID': 'LS01010105232113104', 'SNO': '000003EB', 'OBJ': '32769'}, 
   1: {'TERM': 'off', 'PRV': '1.0', 'NAME': 'LDSBus 4in1 Sensor', 'MFG': '17092021', 'DID': '1', 'NICK': 'LDSBus 4in1 Sensor', 'UID': 'LS01010101272100039', 'SNO': '41363B88', 'OBJ': '32768'}}
"""
            