#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""! @brief Defines the LDSBus Example Usage"""


from brteve.brt_eve_bt817_8 import BrtEve
from brteve.brt_eve_rp2040 import BrtEveRP2040
import ui_lds as ui_lds

import board
import digitalio
from digitalio import DigitalInOut, Direction
def initIO():     
    print("initIO")  
    PIN_22 = DigitalInOut(board.GP22)  # digital write to ESP_D_R_PIN
    PIN_22.direction = Direction.INPUT


initIO()
host = BrtEveRP2040()
eve = BrtEve(host)
#eve.init(resolution="1280x800", touch="goodix")
eve.init(resolution="800x480", touch="capacity")
#eve.calibrate()
# eve.wr32(eve.REG_TOUCH_TRANSFORM_A, 0xfffefefc) # pre setting for 1280x800 lcd
# eve.wr32(eve.REG_TOUCH_TRANSFORM_B, 0xfffffcbf)
# eve.wr32(eve.REG_TOUCH_TRANSFORM_C, 0x506adb4)
# eve.wr32(eve.REG_TOUCH_TRANSFORM_D, 0xfffffed1)
# eve.wr32(eve.REG_TOUCH_TRANSFORM_E, 0xfffefc79)
# eve.wr32(eve.REG_TOUCH_TRANSFORM_F, 0x32c3211)
 
 
#ui_lds.ui_main(eve,True)    # skip Sensor
ui_lds.ui_main(eve,False)  # use LDS Sensor

 