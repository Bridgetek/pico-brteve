from brteve.brt_eve_bt817_8 import BrtEve
from brteve.brt_eve_rp2040 import BrtEveRP2040
from ev_charge_point.main import demo_evchargepoint
from ev_charge_point.widgets import widgets_dialog_yes_no

host = BrtEveRP2040()
eve = BrtEve(host)
eve.init(resolution="1280x800", touch="goodix")
eve.calibrate()

yes = widgets_dialog_yes_no(eve, "Preparing flash", 
    "Write BT81X_Flash.bin from sdcard to EVE's connected flash at first?", 120, False) == True
if yes == True:
    eve.storage.write_flash_with_progressbar('/sd/EveApps/DemoEvChargePoint/Test/Flash/BT81X_Flash.bin', 0)

demo_evchargepoint(eve).start()
