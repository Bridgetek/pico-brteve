import bteve as eve
import EvChargePoint as evchargepoint

gd = eve.Brt_PicoEve_Module()
gd.init()
gd.setup_1280x800()

evchargepoint.demo_evchargepoint(eve, gd).start()