import bteve as eve
import EvChargePoint as evchargepoint

gd = eve.Brt_PicoEve_Module()
gd.init()

evchargepoint.demo_evchargepoint(eve, gd).start()