import time
import json
from .helper import helper
from .gesture import gesture
from .layout import layout
from .LDSBus_Sensor import LDSBus_Sensor
from .ui_common import ui_common
from .ui_common import ui_config
from .tags import *
from .widgets import widgets_box, widgets_point
from brteve.brt_eve_bt817_8 import BrtEve

class ui_4in1_sensor(ui_common):
    data_gui=1
    HUMIDITY_MAX_SAMPLE=120
    humidity_sample_num = 0
    humidity_data=[[0, 0]] * HUMIDITY_MAX_SAMPLE
    temperature_MAX_SAMPLE=HUMIDITY_MAX_SAMPLE
    temperature_sample_num = 0
    temperature_data=[[0, 0]] *temperature_MAX_SAMPLE
    def __init__(self, eve: BrtEve, helper: helper, gesture: gesture, layout: layout,LDSBus_Sensor:LDSBus_Sensor):
        ui_common.__init__(self,eve,helper,gesture,layout,LDSBus_Sensor)

        self.title="LDSBus 4in1 Sensor"
        self.value_t=0 
        self.value_h=0     
        self.value_a=0
        self.value_m=0

    last_push_humidity = 0
    def push_humidity(self, value):
        now = time.monotonic_ns()
        TIME_PATTERN= 1 #second
        if now - self.last_push_humidity < TIME_PATTERN *1e9:
            return
        self.last_push_humidity = now

        timestamp = time.monotonic_ns() / 1e9
        data = [timestamp, value]

        try:
            # new data on top
            temp=ui_4in1_sensor.humidity_data[0:ui_4in1_sensor.HUMIDITY_MAX_SAMPLE] # get index from 0 to max-1
            ui_4in1_sensor.humidity_data = [data] + temp                  # add value to top

            if ui_4in1_sensor.humidity_sample_num < ui_4in1_sensor.HUMIDITY_MAX_SAMPLE:
                ui_4in1_sensor.humidity_sample_num += 1
            else:
                ui_4in1_sensor.humidity_sample_num=1
                ui_4in1_sensor.humidity_data=[data]
        except  Exception as e:
            print("exceprion:",e)
            print("len:%d humidity_sample_num %d"%(len(ui_4in1_sensor.humidity_data),ui_4in1_sensor.humidity_sample_num) )

    last_push_temperature  = 0
    def push_temperature (self, value):
        now = time.monotonic_ns()
        TIME_PATTERN= 1 #second
        if now - self.last_push_temperature < TIME_PATTERN *1e9:
            return

        self.last_push_temperature = now

        timestamp = time.monotonic_ns() / 1e9
        data = [timestamp, value]

        try:
            # new data on top
            temp=ui_4in1_sensor.temperature_data[0:ui_4in1_sensor.temperature_MAX_SAMPLE] # get index from 0 to max-1
            ui_4in1_sensor.temperature_data = [data] + temp                  # add value to top

            if ui_4in1_sensor.temperature_sample_num < ui_4in1_sensor.temperature_MAX_SAMPLE:
                ui_4in1_sensor.temperature_sample_num += 1
            else:
                ui_4in1_sensor.temperature_sample_num=1
                ui_4in1_sensor.temperature_data=[data]
            #print("ui_4in1_sensor.temperature_sample_num",ui_4in1_sensor.temperature_sample_num)
        except  Exception as e:
            print("exceprion:",e)
            print("len:%d humidity_sample_num %d"%(len(ui_4in1_sensor.temperature_data),ui_4in1_sensor.temperature_sample_num) )

  
    def drawBtn(self):
        eve = self.eve
        eve.ColorRGB(0xff, 0xff, 0xff)
        y = self.layout.APP_Y 
        btn_w = self.btn_w
        btn_h = self.btn_h
        x1 = self.xStart
        x5 = x1 + 310
        eve.Tag(tag_ui_lds_back)
        eve.cmd_button(x5, y, btn_w, btn_h, 31, 0, "Back")
        eve.Tag(0)




    def processOne(self,lds,eve,x,y):
        xHalf=410
        yHalf=205
        boxW=self.boxW   
        boxH=self.boxH
        distance = 30
        ldsuid = int(lds['DID'])      
        eve.TagMask(1)
        eve.Tag(tag_ui_lds_4in1_t)
        eve.SaveContext() 
        self.barGraphHis(x = x, y=y, w = boxW, h = boxH, border=1,  data=ui_4in1_sensor.temperature_data ,scale=1,blend=1)
        self.blendBk(x=x,y=y,w=boxW,h = boxH, border=1 ,scale=1,blend=1) 
        eve.RestoreContext()
        self.coordinateMarker(x,y,boxW,boxH,1,1,0,tvalue=self.value_t)            
        self.eve.Tag(tag_ui_lds_4in1_a)
        self.circle_box(x =x+xHalf, y=y, w = boxW, h = boxH, border=1, title="Ambient",unit="Lux", vmin=0, vmax=1000, lwarning=70, hwarning=900, value=self.value_a)
        eve.BlendFunc(eve.SRC_ALPHA, eve.ONE_MINUS_SRC_ALPHA) #reset to  default 

        eve.Tag(tag_ui_lds_4in1_h)
        self.statitics_box(x = x+xHalf, y=y+yHalf, w = boxW, h = boxH, border=1,data=ui_4in1_sensor.humidity_data,tvalue=self.value_h)
        eve.Tag(tag_ui_lds_4in1_m)
        self.boxMotion(x = x, y=y+yHalf, w = boxW, h = boxH, border=1)  
        if self.value_m>=1:
            self.layout.draw_asset_MCU(tag_ui_lds_4in1_m,"m_active",x =x+60, y=y+yHalf+10,fm=self.eve.ASTC_4x4)
        else:
            self.layout.draw_asset_MCU(tag_ui_lds_4in1_m,"m_inactive",x =x+60, y=y+yHalf+10,fm=self.eve.ASTC_4x4)

  
    def readOne(self,lds):
        ldsuid = int(lds['DID'])
        lds_object_file = self.LDSBus_Sensor.json_path + "/" + lds['OBJ'] + ".json"
        """
        Load and Parse the JSON File
        """
        with open(lds_object_file) as lds_json_file:
            lds_json = json.load(lds_json_file)
            if self.LDSBus_Sensor.LDSBus_SDK_Process_LDSUID(ldsuid) >= 0:
                ss=""
                for said, sensor in enumerate(lds_json['SNS']):
                    sns_value=self.LDSBus_Sensor.LDSBus_SDK_ReadValue(ldsuid,sensor)
                    #sns_value = self.LDSBus_Sensor.lds_bus.LDSBus_SDK_ReadValue(ldsuid, int(sensor['SAID']), int(sensor['CLS']))
                    if sns_value is not None:
                        if len(ss) == 0:
                            ss="%s:%-5.2f %s "% (sensor['NAME'][0:1],  float(sns_value['VALUE']), sensor['UNIT'][0:1])
                        else: ss=ss+","+"%s:%5.2f %s "%(sensor['NAME'][0:1], float( sns_value['VALUE']), sensor['UNIT'][0:1])
                        if  sensor['NAME'][0:1]=='T' and ui_4in1_sensor.data_gui==1:
                            self.value_t=float( sns_value['VALUE'])
                            self.push_temperature(self.value_t)
                        elif  sensor['NAME'][0:1]=='A' and ui_4in1_sensor.data_gui==1:
                            self.value_a=float( sns_value['VALUE'])
                        elif  sensor['NAME'][0:1]=='H' and ui_4in1_sensor.data_gui==1:
                            self.value_h=float( sns_value['VALUE'])
                            self.push_humidity(self.value_h)
                        elif  sensor['NAME'][0:1]=='M' and ui_4in1_sensor.data_gui==1: #Motion
                            self.value_m=float( sns_value['VALUE'])
                return 1
            else:
                print ("%20s : %s ,ldsuid=%d" %  ("4in1 SENSOR PROCESS", "FAILED" ,ldsuid))
                return -1

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
        eve.Tag(tag_ui_lds_info)
        eve.cmd_button(x+len(self.title)*FONTSIZE, y, self.btn_w, self.btn_h, 31, 0, "Info")
        if self.skipSensor: eve.cmd_text(x+70+len(self.title)*FONTSIZE, y, 28, 0, self.simulatorTitle)        
        self.drawBtn()
        ymargin = 50
        y +=  ymargin
        widgets_box(eve,x,y-1,800,1, 1, [0x00, 0xff, 0xff])      
        x+=50
        y+=20
        self.processOne(self.LDSBus_Sensor.lds,eve,x,y) 
        ms = time.monotonic_ns() / 1000_000
        if ms - self.last_timeout < self.readingInterval: return
        self.last_timeout =  time.monotonic_ns() / 1000_000
        if self.readOne(self.LDSBus_Sensor.lds)>0:
            self.last_timeout =  time.monotonic_ns() / 1000_000