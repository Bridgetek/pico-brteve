import time
import json
from random import randint
from LDSBus_SDK import LDSBus_SDK
from .ui_config import ui_config

G_LDSU_SENSOR_TYPE = None

class LDSBus_Sensor(ui_config):
    def __init__(self):  
        super().__init__() 
        self._lds_count = 0
        self.lds_list = {}
        self.lds_list_all ={1: {'TERM': 'off', 'PRV': '1.0', 'NAME': 'LDSBus 4in1 Sensor', 'MFG': '17092021', 'DID': '1', 'NICK': 'LDSBus 4in1 Sensor', 'UID': 'LS01010101272100039', 'SNO': '41363B88', 'OBJ': '32768'},
                             10: {'TERM': 'off', 'PRV': '1.0', 'NAME': 'LDSBus 2CH Relay', 'MFG': '19012022', 'DID': '10', 'NICK': 'LDSBus 2CH Relay', 'UID': 'LC01110101042200001', 'SNO': 'A6599D5F', 'OBJ': '16385'}, 
 #                            3: {'TERM': 'off', 'PRV': '1.0', 'NAME': 'LDSBus Thermocouple Sensor', 'MFG': '23062021', 'DID': '3', 'NICK': 'LDSBus Thermocouple Sensor', 'UID': 'LS01010105232113104', 'SNO': '000003EB', 'OBJ': '32769'},
                             16: {'TERM': 'off', 'PRV': '1.0', 'NAME': 'LDSBus CO2 Sensor SCD4x', 'MFG': '11032022', 'DID': '16', 'NICK': 'LDSBus CO2 Sensor SCD4x', 'UID': 'LS01010133112213873', 'SNO': '000003F8', 'OBJ': '49152'}}
 

        self.lds=None
        self.lds_bus=None
        self.json_path="json"
        self.sensorType=""
        self.dataIndex=-1
        #self.skipSensor=False # AttributeError: 'LDSBus_Sensor' object has no attribute 'skipSensor'
        self.relayStatus={'Relay - CH 1':1,'Relay - CH 2':0}
        self.relayCurrents={'Current - CH 1':0.000,'Current - CH 2':0.051}
        self.relayStatus['Relay - CH 1']=0
        self.relayStatus['Relay - CH 2']=1 
        self.turnOnLDS()

    def turnOnLDS(self):
        #print(LDSBus_Sensor.__dict__)
        if self.skipSensor: return
        """
        Setup the Options as provided in command line
        """
        port_type = 'ldsu'
        port = '1'
        ldsu_id = 0
        json_path = "json"
        """
        Instantiate the LDSBus SDK with the port_type provided by user
        """
        self.lds_bus = LDSBus_SDK(port_type)
        print ("%23s : %s" % ("LDSBus SDK Version", self.lds_bus.LDSBus_SDK_Version()))
        """
        Power ON the LDSBus Adapter
        """
        self.lds_bus.LDSBus_SDK_PowerPort('on')
        """
        Initialize the LDSBus SDK Stack
        """
        self.lds_bus.LDSBus_SDK_Init(port.encode('utf-8'))
        """
        Set Specific LDSUID  if user provided
        """
        if ldsu_id > 0:
            print ("%23s : %s" % ("Specified LDSUID", str(ldsu_id)))
            self.lds_bus.LDSBus_SDK_Set_Search_LDSUID(ldsu_id)
        print ("")
        """
        Set the Type of the Device to be discovered in LDSBus
        """
        print("G_LDSU_SENSOR_TYPE=", G_LDSU_SENSOR_TYPE)
        if G_LDSU_SENSOR_TYPE is not None:
            self.lds_busbus.LDSBus_SDK_Set_LDSUType(G_LDSU_SENSOR_TYPE)


    def scanLDS(self):
        startTime=time.time()
        if ui_config.skipSensor:
                            time.sleep(1)
                            self.lds_list ={1: {'TERM': 'off', 'PRV': '1.0', 'NAME': 'LDSBus 4in1 Sensor', 'MFG': '17092021', 'DID': '1', 'NICK': 'LDSBus 4in1 Sensor', 'UID': 'LS01010101272100039', 'SNO': '41363B88', 'OBJ': '32768'},
                             #10: {'TERM': 'off', 'PRV': '1.0', 'NAME': 'LDSBus 2CH Relay', 'MFG': '19012022', 'DID': '10', 'NICK': 'LDSBus 2CH Relay', 'UID': 'LC01110101042200001', 'SNO': 'A6599D5F', 'OBJ': '16385'}, 
                             99: {'TERM': 'off', 'PRV': '1.0', 'NAME': 'LDSBus 2CH Relay + iSENSE', 'MFG': '28042022', 'DID': '99', 'NICK': 'LDSBus 2CH Relay + iSENSE', 'UID': 'LC01010128042200004', 'SNO': '638EF554', 'OBJ': '49152'}, 
 #                            3: {'TERM': 'off', 'PRV': '1.0', 'NAME': 'LDSBus Thermocouple Sensor', 'MFG': '23062021', 'DID': '3', 'NICK': 'LDSBus Thermocouple Sensor', 'UID': 'LS01010105232113104', 'SNO': '000003EB', 'OBJ': '32769'},
                             16: {'TERM': 'off', 'PRV': '1.0', 'NAME': 'LDSBus CO2 Sensor SCD4x', 'MFG': '11032022', 'DID': '16', 'NICK': 'LDSBus CO2 Sensor SCD4x', 'UID': 'LS01010133112213873', 'SNO': '000003F8', 'OBJ': '32785'}}
                            return
        print ("Scan start:%d\n" %  (startTime))
        self._lds_count = self.lds_bus.LDSBus_SDK_Scan()
        print ("%20s : %s\n" %  ("No. of LDSUs Found", str(self._lds_count)))

        self.lds_list = self.lds_bus.LDSBus_SDK_GetLDSUs()
        print ("%s\n"%self.lds_list)
        print ("Scan Time used:%d\n" %  (time.time()-startTime))

    def simulator(self,sensor):
        sns='{"VALUE": "1"}'
        if  sensor['NAME'][0:1]=='T': sns='{"VALUE":"'+str(randint(20,27))+ '"}'
        if  sensor['NAME'][0:1]=='A': sns='{"VALUE":"'+str(randint(190,220))+ '"}'
        if  sensor['NAME'][0:1]=='H': sns='{"VALUE":"'+str(randint(55,70))+ '"}'
        if  sensor['NAME'][0:1]=='M': sns='{"VALUE":"'+str(randint(0,1))+ '"}'
        if  sensor['NAME']=='CO2': sns='{"VALUE":"'+str(randint(700,780))+ '"}'
        if  sensor['NAME'].startswith('Relay'):
             sns='{"VALUE":"'+str(self.relayStatus[sensor['NAME']])+ '"}'
        if  sensor['NAME'].startswith('Current'):
             sns='{"VALUE":"'+str(self.relayCurrents[sensor['NAME']])+ '"}'
        sns_value=json.loads(sns)
        return sns_value
    def simulatorWrite(self,sensor):
        if  sensor['NAME'].startswith('Relay'):self.relayStatus[sensor['NAME']]=0 if self.relayStatus[sensor['NAME']] ==1 else 1
        sns='{"VALUE":"'+str(self.relayStatus[sensor['NAME']])+ '"}'
        #print(sns)
        sns_value=json.loads(sns)
        return sns_value
    def  LDSBus_SDK_ReadValue(self,ldsuid,sensor):
        if ui_config.skipSensor:return self.simulator(sensor)
        else: return self.lds_bus.LDSBus_SDK_ReadValue(ldsuid, int(sensor['SAID']), int(sensor['CLS']))
    def  LDSBus_SDK_Process_LDSUID(self,ldsuid):
        if ui_config.skipSensor:return 1
        else: return self.lds_bus.LDSBus_SDK_Process_LDSUID(ldsuid)        
    def  LDSBus_SDK_WriteValue(self,ldsuid,sensor,data):
        if ui_config.skipSensor:return self.simulatorWrite(sensor)
        else: return self.lds_bus.LDSBus_SDK_WriteValue(ldsuid, int(sensor['SAID']), int(sensor['CLS']), data, len(data))
