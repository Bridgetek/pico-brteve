from brteve.brt_eve_bt817_8 import BrtEve
from brteve.brt_eve_rp2040 import BrtEveRP2040
from ir_sensors_and_gestures import ir_sensors_and_gestures

host = BrtEveRP2040()
eve = BrtEve(host)
eve.init(resolution="1280x800", touch="goodix")

spi1 = eve.spi_sdcard()
eve.calibrate()
eve.finish()
app=ir_sensors_and_gestures(eve)
