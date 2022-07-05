from .ui_config import ui_config
from ui_lds.gesture import gesture
from ui_lds.layout import layout
from ui_lds.helper import helper
import time
from .tags import *
from .ui_lds_scan import ui_lds_scan
from .ui_4in1_sensor import ui_4in1_sensor
from .ui_4in1_sensor_t import ui_4in1_sensor_t
from .ui_4in1_sensor_h import ui_4in1_sensor_h
from .ui_4in1_sensor_a import ui_4in1_sensor_a
from .ui_4in1_sensor_m import ui_4in1_sensor_m
from .ui_co2_sensor import ui_co2_sensor
from .ui_co2_sensor_t import ui_co2_sensor_t
from .ui_co2_sensor_h import ui_co2_sensor_h
from .ui_co2_sensor_ambient import ui_co2_sensor_ambient
from .ui_co2_sensor_co2 import ui_co2_sensor_co2
from .ui_relay import ui_relay
from .ui_lds_Details import ui_lds_Details
from .LDSBus_Sensor import LDSBus_Sensor
from brteve.brt_eve_bt817_8 import BrtEve

# controler class
class ui_main():
    def __init__(self, eve: BrtEve, skipSensor=False,exit=0) -> None:
        self.eve = eve
        self.helper = helper(eve)
        self.gesture = gesture()
        self.layout = layout(eve, self.helper)
        ui_config.skipSensor=skipSensor
        self.LDSBus_Sensor=LDSBus_Sensor()
        self.ui_lds_scan=ui_lds_scan(eve, self.helper, self.gesture, self.layout,self.LDSBus_Sensor,exit)
        self.ui_4in1_sensor=ui_4in1_sensor(eve, self.helper, self.gesture, self.layout,self.LDSBus_Sensor)
        self.ui_4in1_sensor_t=ui_4in1_sensor_t(eve, self.helper, self.gesture, self.layout,self.LDSBus_Sensor)
        self.ui_4in1_sensor_h=ui_4in1_sensor_h(eve, self.helper, self.gesture, self.layout,self.LDSBus_Sensor)
        self.ui_4in1_sensor_a=ui_4in1_sensor_a(eve, self.helper, self.gesture, self.layout,self.LDSBus_Sensor)
        self.ui_4in1_sensor_m=ui_4in1_sensor_m(eve, self.helper, self.gesture, self.layout,self.LDSBus_Sensor)
        self.ui_co2_sensor=ui_co2_sensor(eve, self.helper, self.gesture, self.layout,self.LDSBus_Sensor)
        self.ui_co2_sensor_t=ui_co2_sensor_t(eve, self.helper, self.gesture, self.layout,self.LDSBus_Sensor)
        self.ui_co2_sensor_h=ui_co2_sensor_h(eve, self.helper, self.gesture, self.layout,self.LDSBus_Sensor)
        self.ui_co2_sensor_ambient=ui_co2_sensor_ambient(eve, self.helper, self.gesture, self.layout,self.LDSBus_Sensor)
        self.ui_co2_sensor_co2=ui_co2_sensor_co2(eve, self.helper, self.gesture, self.layout,self.LDSBus_Sensor)
        self.ui_relay=ui_relay(eve, self.helper, self.gesture, self.layout,self.LDSBus_Sensor)
        self.ui_lds_Details=ui_lds_Details(eve, self.helper, self.gesture, self.layout,self.LDSBus_Sensor)        
        self.ui_active = self.ui_lds_scan
        self.currType=''
        self.hBk512=4
        time.sleep(0.2)
        assetdir = "ui_lds/"
        eve.cmd_dlstart()  
        bmAdd=1024*750
        eve.BitmapHandle(self.hBk512)
        eve.cmd_loadimage(bmAdd, 0)
        eve.load(open(assetdir + "graph512.png", "rb"))
        if self.layout.portrait:
            self.eve.cmd_setrotate(2)
        else:
            self.eve.cmd_setrotate(0)
        self.flush()
        time.sleep(0.2)
        self.showScan()
        self.lastTouch=time.monotonic_ns() / 1000_000
        self.touchCounter=0
        self.longTouch=0 
        while 1:
            ev=self.loop()
            if (ev==tag_ui_back): break
         
    def start(self):
        self.eve.cmd_dlstart()
        self.eve.Clear(1, 1, 1)
        self.eve.VertexFormat(2)


    def flush(self):
        self.eve.Display()
        self.eve.cmd_swap()
        self.eve.flush()

    def draw(self,x,y,size,title):
        eve = self.eve
        eve.ColorRGB(0, 0, 0)
        self.eve.VertexFormat(3)
        eve.ColorRGB(255, 255, 255)
        eve.cmd_text(x, y, size, eve.OPT_CENTERX, title)

    def showScan(self):
        self.start()
        self.draw(50,300,30,"Scaning Sensor...")
        self.flush()
 
    def getType(self,cIndex):
        i=0
        currLDS =None
        for index, lds in self.LDSBus_Sensor.lds_list.items():
        #for index, lds in self.LDSBus_Sensor.lds_list_all.items():
            if ( cIndex==i): self.currType=lds["NAME"]; currLDS=lds; break
            i=i+1
        return self.currType ,currLDS
    def setActiveUI(self, name):
        #print("setActiveUI %s\n" %(name))
        if name=="scan" :self.ui_active = self.ui_lds_scan
        elif  name=="4in1" :self.ui_active = self.ui_4in1_sensor
        
  
    def get_event(self):
        eve = self.eve
        touch=self.gesture.renew(eve)
        if touch.isTouch:
            ms = time.monotonic_ns() / 1000_000
            if  (ms - self.lastTouch)>0 and ( ms - self.lastTouch < 250):
                self.touchCounter+=1
                if self.touchCounter>5:
                    self.touchCounter=0
                    self.longTouch=1
                    #print("longTouch " ,self.longTouch)
                    # from  main_menu.eve_snapshot import snapshot2
                    # snapshot2(eve,self.ui_active.title,60)
            else:
                self.touchCounter=0
                self.longTouch=0
            self.lastTouch=ms

        tag = touch.tagPressed
        tagReleased=touch.tagReleased

        if tagReleased==0: return
        tag=tagReleased
        
        if tag>0:
                self.touchCounter=0
                self.longTouch=0

        self.ui_previous=self.ui_active.title
        #print("*ui_previous",self.ui_previous)  

        if tag == tag_ui_lds_scan:
            self.showScan()
            self.ui_active = self.ui_lds_scan
            self.ui_lds_scan._rescan=True
            print("tag_ui_lds_scan", ui_4in1_sensor.temperature_sample_num)
            #print("temperature_data>", ui_4in1_sensor.temperature_data)
            ui_4in1_sensor.temperature_sample_num=0
            ui_4in1_sensor.temperature_data=[[0, 0]] *ui_4in1_sensor.temperature_MAX_SAMPLE
            ui_4in1_sensor.humidity_sample_num=0
            ui_4in1_sensor.humidity_data=[[0, 0]] *ui_4in1_sensor.temperature_MAX_SAMPLE
            ui_co2_sensor.temperature_sample_num=0
            ui_co2_sensor.temperature_data=[[0, 0]] *ui_co2_sensor.temperature_MAX_SAMPLE
            ui_co2_sensor.humidity_sample_num=0
            ui_co2_sensor.humidity_data=[[0, 0]] *ui_co2_sensor.temperature_MAX_SAMPLE
        elif tag == tag_ui_back:
            self.ui_active = self.ui_lds_scan
            print("tag_back")
        elif tag == tag_ui_lds_home:
            self.ui_active = self.ui_lds_scan
        elif tag == tag_ui_lds_info:
            self.ui_lds_Details.ui_previous=self.ui_active.title
            self.ui_active = self.ui_lds_Details  
        elif tag == tag_ui_lds_info_back:
            if ( self.ui_lds_Details.ui_previous.startswith("LDSBus 4in1 Sensor") ): 
                self.ui_lds_Details.ui_previous=self.ui_active.title 
                self.ui_active = self.ui_4in1_sensor
            elif ( self.ui_lds_Details.ui_previous.startswith("LDSBus CO2 Sensor")):   
                self.ui_lds_Details.ui_previous=self.ui_active.title
                self.ui_active = self.ui_co2_sensor
            elif ( self.ui_lds_Details.ui_previous.startswith("LDSBus 2Ch Relay")):   
                self.ui_lds_Details.ui_previous=self.ui_active.title
                self.ui_active = self.ui_relay
            #print("tag_ui_lds_info_back",self.ui_lds_Details.ui_previous)
        elif tag == tag_ui_lds_back:
 
            if ( self.ui_active.title=="LDSBus 4in1 Sensor"):   self.ui_active = self.ui_lds_scan
            elif ( self.ui_active.title=="LDSBus 4in1 Sensor(Temperature)"):   self.ui_active = self.ui_4in1_sensor
            if ( self.ui_active.title=="LDSBus 4in1 Sensor(Humidity)"):   self.ui_active = self.ui_4in1_sensor
            elif ( self.ui_active.title=="LDSBus 4in1 Sensor(Ambient)"):   self.ui_active = self.ui_4in1_sensor
            elif ( self.ui_active.title=="LDSBus 4in1 Sensor(Motion)"):   self.ui_active = self.ui_4in1_sensor
            elif ( self.ui_active.title=="LDSBus CO2 Sensor"):   self.ui_active = self.ui_lds_scan
            elif ( self.ui_active.title=="LDSBus CO2 Sensor(Temperature)"):   self.ui_active = self.ui_co2_sensor
            elif ( self.ui_active.title=="LDSBus CO2 Sensor(Humidity)"):   self.ui_active = self.ui_co2_sensor
            elif ( self.ui_active.title=="LDSBus CO2 Sensor(Ambient)"):   self.ui_active = self.ui_co2_sensor
            elif ( self.ui_active.title=="LDSBus CO2 Sensor(CO2)"):   self.ui_active = self.ui_co2_sensor
            elif ( self.ui_active.title=="LDSBus 2Ch Relay"):   self.ui_active = self.ui_lds_scan
            elif ( self.ui_lds_Details.ui_previous=="LDSBus 2Ch Relay"):   self.ui_active = self.ui_relay
            #print("tag_ui_lds_back",self.ui_active.title,self.ui_lds_Details.ui_previous)

        elif tag == tag_ui_lds_4in1_t:
            self.ui_active = self.ui_4in1_sensor_t
        elif tag == tag_ui_lds_4in1_h:
            self.ui_active = self.ui_4in1_sensor_h
        elif tag == tag_ui_lds_4in1_a:
            self.ui_active = self.ui_4in1_sensor_a
        elif tag == tag_ui_lds_4in1_m:
            self.ui_active = self.ui_4in1_sensor_m

        elif tag == tag_ui_lds_co2_t:
            self.ui_active = self.ui_co2_sensor_t
        elif tag == tag_ui_lds_co2_h:
            self.ui_active = self.ui_co2_sensor_h
        elif tag == tag_ui_lds_co2_a:
            self.ui_active = self.ui_co2_sensor_ambient
        elif tag == tag_ui_lds_co2_co2:
                self.ui_active = self.ui_co2_sensor_co2
        elif tag == tag_ui_lds_data_0:
            self.LDSBus_Sensor.dataIndex=0
            sensorType,lds=self.getType(0)
            self.LDSBus_Sensor.sensorType=sensorType
            self.LDSBus_Sensor.lds=lds
            #print("choose %d,  %s"% (self.LDSBus_Sensor.dataIndex,sensorType) )
            if sensorType=="LDSBus 4in1 Sensor" or sensorType=="LDSBus Thermocouple Sensor":
                self.ui_active = self.ui_4in1_sensor
            elif sensorType.startswith("LDSBus CO2 Sensor"): self.ui_active = self.ui_co2_sensor
            else:
                self.ui_active = self.ui_relay
            self.ui_active.firstTime=True
        elif tag == tag_ui_lds_data_1:
            self.LDSBus_Sensor.dataIndex=1
            sensorType,lds=self.getType(1)
            self.LDSBus_Sensor.sensorType=sensorType
            self.LDSBus_Sensor.lds=lds
            #print("choose %d,  %s"% (self.LDSBus_Sensor.dataIndex,sensorType) )
            if sensorType=="LDSBus 4in1 Sensor" or sensorType=="LDSBus Thermocouple Sensor":
                self.ui_active = self.ui_4in1_sensor
            elif sensorType.startswith("LDSBus CO2 Sensor"): self.ui_active = self.ui_co2_sensor
            else:
                self.ui_active = self.ui_relay
            self.ui_active.firstTime=True
        elif tag == tag_ui_lds_data_2:
            self.LDSBus_Sensor.dataIndex=2
            sensorType,lds=self.getType(2)
            self.LDSBus_Sensor.sensorType=sensorType
            self.LDSBus_Sensor.lds=lds
            if sensorType=="LDSBus 4in1 Sensor" or sensorType=="LDSBus Thermocouple Sensor":
                self.ui_active = self.ui_4in1_sensor
            elif sensorType.startswith("LDSBus CO2 Sensor"): self.ui_active = self.ui_co2_sensor
            else:
                self.ui_active = self.ui_relay
            self.ui_active.firstTime=True
        elif tag == tag_ui_lds_data_3:
            self.LDSBus_Sensor.dataIndex=3
            sensorType,lds=self.getType(3)
            self.LDSBus_Sensor.sensorType=sensorType
            self.LDSBus_Sensor.lds=lds
            #print("choose %d,  %s"% (self.LDSBus_Sensor.dataIndex,sensorType) )
            if sensorType=="LDSBus 4in1 Sensor" or sensorType=="LDSBus Thermocouple Sensor":
                self.ui_active = self.ui_4in1_sensor
            elif sensorType.startswith("LDSBus CO2 Sensor"): self.ui_active = self.ui_co2_sensor
            else:
                self.ui_active = self.ui_relay
            self.ui_active.firstTime=True
        elif tag == tag_ui_lds_data_4:
            self.LDSBus_Sensor.dataIndex=4
            sensorType,lds=self.getType(4)
            self.LDSBus_Sensor.sensorType=sensorType
            self.LDSBus_Sensor.lds=lds
            if sensorType=="LDSBus 4in1 Sensor" or sensorType=="LDSBus Thermocouple Sensor":
                self.ui_active = self.ui_4in1_sensor
            elif sensorType.startswith("LDSBus CO2 Sensor"): self.ui_active = self.ui_co2_sensor
            else:
                self.ui_active = self.ui_relay
            self.ui_active.firstTime=True
 
        return tag

    def loop(self):
        self.eve.cmd_dlstart() #   
        self.eve.ClearColorRGB(0, 0, 0) 
        self.eve.Clear(1, 1, 1)
        self.eve.BlendFunc(self.eve.SRC_ALPHA, self.eve.ONE_MINUS_SRC_ALPHA) #reset to  default
        self.eve.ColorRGB(0, 0, 0)  
        self.eve.VertexFormat(2)
        self.ui_active.draw()
        ev = self.get_event()
        try:
            self.eve.Display()
            self.eve.cmd_swap()  
            self.eve.flush() 
            self.eve.cmd_loadidentity() 
        except  Exception as e:
            print("exceprion:",e)
        time.sleep(0.005)
        return ev





