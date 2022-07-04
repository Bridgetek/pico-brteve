from brteve.brt_eve_bt817_8 import BrtEve
from brteve.brt_eve_rp2040 import BrtEveRP2040
from ui_lds.ui_main import ui_main

host = BrtEveRP2040()
eve = BrtEve(host)
eve.init(resolution="800x480", touch="capacity") 
#ui_main(eve,True)    # skip Sensor
ui_main(eve,False)  # use LDS Sensor

 