""" Alarm clock example """

from brteve.brt_eve_bt817_8 import BrtEve
from brteve.brt_eve_rp2040 import BrtEveRP2040
import alarm_clock as alarm_clock

host = BrtEveRP2040()
eve = BrtEve(host)
eve.init(resolution="1280x800", touch="goodix")
eve.calibrate()
# eve.wr32(eve.REG_TOUCH_TRANSFORM_A, 0xfffefefc) # pre setting for 1280x800 lcd
# eve.wr32(eve.REG_TOUCH_TRANSFORM_B, 0xfffffcbf)
# eve.wr32(eve.REG_TOUCH_TRANSFORM_C, 0x506adb4)
# eve.wr32(eve.REG_TOUCH_TRANSFORM_D, 0xfffffed1)
# eve.wr32(eve.REG_TOUCH_TRANSFORM_E, 0xfffefc79)
# eve.wr32(eve.REG_TOUCH_TRANSFORM_F, 0x32c3211)
alarm_clock.alarm_clock(eve)
