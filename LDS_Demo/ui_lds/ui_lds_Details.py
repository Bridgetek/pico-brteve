import time
from .helper import helper
from .gesture import gesture
from .layout import layout
from .LDSBus_Sensor import LDSBus_Sensor
from .ui_common import ui_common
from .ui_config import ui_config
from .tags import *
from .widgets import widgets_box, widgets_point
from brteve.brt_eve_bt817_8 import BrtEve

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
 
            