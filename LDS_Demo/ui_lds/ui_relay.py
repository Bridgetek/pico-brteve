import time
import json
from .helper import helper
from .gesture import gesture
from .layout import layout
from .LDSBus_Sensor import LDSBus_Sensor
from .ui_common import ui_common
from .ui_config import ui_config
from .tags import *
from .widgets import widgets_box, widgets_point
from brteve.brt_eve_bt817_8 import BrtEve

class ui_relay(ui_config):
    def __init__(self, eve: BrtEve, helper: helper, gesture: gesture, layout: layout,LDSBus_Sensor:LDSBus_Sensor):
        super().__init__()
        self.eve = eve
        self.helper = helper
        self.gesture = gesture
        self.layout = layout
        self.LDSBus_Sensor = LDSBus_Sensor
        self.title="LDSBus 2Ch Relay"
        self.ldsuid=-1
        self.lds_json=None
        self.relay_state=[ True, False ]       
        self._clearData=True
        self._histroy=[]
        self._maxLen=10
        self.hFanOn=5
        self.hFanOff=6
        self.relayStatus={'Relay - CH 1':'1','Relay - CH 2':'0' ,'Current - CH 1':'0.000','Current - CH 2':'0.000'}
        self.last_timeout =  time.monotonic_ns() / 1000_000
 

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

    def event(self):
        eve = self.eve
        layout = self.layout
        ges = self.gesture
        tag = ges.get().tagReleased
        if tag == tag_ui_lds_reset_data:
           self._histroy=[]
        elif tag == tag_ui_lds_relay_ch1:
          if self.lds_json is not None:
            #print ("=============ALTERNATE RELAYS STATES================")
            for said, sensor in enumerate(self.lds_json['SNS']):
                #print ("%20s :type:%s %d" %  (sensor['NAME'],  sensor['TYPE'],int(sensor['SAID'])) )
                if sensor['TYPE'] == 'OUTPUT' and sensor['NAME']=='Relay - CH 1':
                    data = b'\x01' if self.relay_state[said] == True else b'\x00'
                    self.relay_state[said] = not self.relay_state[said]
                    self.LDSBus_Sensor.LDSBus_SDK_WriteValue(self.ldsuid, sensor,  data)
                    time.sleep(0.1)
                    sns_value = self.LDSBus_Sensor.LDSBus_SDK_ReadValue(self.ldsuid,sensor)
        elif tag == tag_ui_lds_relay_ch2:
          if self.lds_json is not None:
            #print ("=============ALTERNATE RELAYS STATES================")
            for said, sensor in enumerate(self.lds_json['SNS']):
                if sensor['TYPE'] == 'OUTPUT' and sensor['NAME']=='Relay - CH 2':
                    data = b'\x01' if self.relay_state[said] == True else b'\x00'
                    self.relay_state[said] = not self.relay_state[said]
                    self.LDSBus_Sensor.LDSBus_SDK_WriteValue(self.ldsuid, sensor,  data)
                    time.sleep(0.1)
                    sns_value = self.LDSBus_Sensor.LDSBus_SDK_ReadValue(self.ldsuid,sensor)


    def draw_img(self, img_id, tag , x ,y,formatW=None):
        eve = self.eve
        img = self.layout.images[img_id]
        if formatW is None:
            formatW=self.eve.ASTC_4x4
        self.helper.image_draw_from_ram_g(
            img[0], x, y, img[2], img[3], formatW, 0, tag, self.eve.OPT_DITHER)

    def processOne(self,lds,x,y):
            eve = self.eve
            distance = 30
            y1=y+70
            y2=y+280
            x1=x+120
            x2=x+520
            x3=x+150
            x4=x+550
            ldsuid = int(lds['DID'])
            counter=0  
            for name in self.relayStatus:
                vv=self.relayStatus[name]
                if name=='Relay - CH 1':
                    eve.Tag(tag_ui_lds_relay_ch1)
                    if vv=='0':
                        self.layout.draw_asset4(tag_ui_lds_relay_ch1,"fanOff",x1,y1) 
                    elif vv=='1':
                        self.layout.draw_asset4(tag_ui_lds_relay_ch1,"fanOn",x1,y1)   
                    eve.Tag(0)
                elif name=='Relay - CH 2': 
                    eve.Tag(tag_ui_lds_relay_ch2)
                    if vv=='0':
                        self.layout.draw_asset4(tag_ui_lds_relay_ch2,"fanOff",x2,y1) 
                    elif vv=='1':
                        self.layout.draw_asset4(tag_ui_lds_relay_ch2,"fanOn",x2,y1) 
                    eve.Tag(0)
                elif name=='Current - CH 1' and lds['OBJ']=='49152':
                    if (float(vv)<0.06):vv="0.000"
                    eve.cmd_text(x3,y2, 31, 0, "%s A"%vv)
                elif name=='Current - CH 2' and lds['OBJ']=='49152':
                    if (float(vv)<0.06):vv="0.000"
                    eve.cmd_text(x4,y2, 31, 0, "%s A"%vv)                     

                
    def readOne(self,lds):
        eve = self.eve
        ldsuid = int(lds['DID'])
        counter=0
        error=1
        if self.firstTime:  
            print("ldsuid:",ldsuid, "OBJ:",lds['OBJ'] ," lds:",lds)
        self.ldsuid=ldsuid
        lds_object_file = self.LDSBus_Sensor.json_path + "/" + lds['OBJ'] + ".json"
        """
        Load and Parse the JSON File
        """
        with open(lds_object_file) as lds_json_file:
            lds_json = json.load(lds_json_file)
            self.lds_json=lds_json
            if self.LDSBus_Sensor.LDSBus_SDK_Process_LDSUID(ldsuid) >= 0:
                ss=""
                for said, sensor in enumerate(lds_json['SNS']):
                    time.sleep(0.002)
                    sns_value = self.LDSBus_Sensor.LDSBus_SDK_ReadValue(ldsuid,sensor)
                    if self.firstTime:print ("DID=%d %20s :type:%s %s "%  (ldsuid,sensor['NAME'],  sensor['TYPE'],sns_value) )
                    if sns_value is not None:
                        vv=sns_value['VALUE']
                        if self.relayStatus[sensor['NAME']] != vv:
                            self.relayStatus[sensor['NAME']]=vv
                            #print("%s changed  %s" % (sensor['NAME'],self.relayStatus[sensor['NAME']] ))
                        counter+=1
            else:rcode=-1
#            if counter<4:print ("Error when reading Relay Data:%d"%  ( counter) )
        if self.firstTime:  
            self.firstTime=False
        return error
                   
    def draw(self):
        eve = self.eve

        layout = self.layout
        helper=self.helper
        eve.ColorRGB(0xff, 0xff, 0xff)       
        x = self.x0
        y = self.y0
        FONTSIZE = 29
        eve.cmd_text(x, y, 31, 0, self.title)
        eve.Tag(tag_ui_lds_info)
        eve.cmd_button(x+len(self.title)*FONTSIZE, y, self.btn_w, self.btn_h, 31, 0, "Info")
        if self.skipSensor: eve.cmd_text(x+70+len(self.title)*FONTSIZE, y, 28, 0, self.simulatorTitle)        
        self.drawBtn()
        self.event()
        ymargin = 50
        y +=  ymargin
        widgets_box(eve,x,y-1,800,1, 1, [0x00, 0xff, 0xff])        
        eve.ColorRGB(255, 255, 255)
        y +=  ymargin
        eve.cmd_text(x+100, y, 31, 0, "Channel 1")
        eve.cmd_text(x+500, y, 31, 0, "Channel 2")
        self.processOne(self.LDSBus_Sensor.lds,x,y) 
        ms = time.monotonic_ns() / 1000_000
        if ms - self.last_timeout < self.readingInterval: return
        self.last_timeout =  time.monotonic_ns() / 1000_000
        if self.readOne(self.LDSBus_Sensor.lds)>0:self.last_timeout =  time.monotonic_ns() / 1000_000
