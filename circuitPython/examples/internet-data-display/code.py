from brteve.brt_eve_bt817_8 import BrtEve
from brteve.brt_eve_rp2040 import BrtEveRP2040
from internet_data_display import internet_display

host = BrtEveRP2040()
eve = BrtEve(host)
eve.init(resolution="1280x800", touch="goodix")

spi1 = eve.spi_sdcard()
eve.calibrate()
eve.finish()
app=internet_display(eve, spi1)
