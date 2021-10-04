""" Video playback from sdcard/pico flash/eve's connected flash example """

import sys
import video2 as demo

def eve_init():
    if sys.implementation.name == "circuitpython":
        from brteve.brt_eve_bt817_8 import BrtEve
        from brteve.brt_eve_rp2040 import BrtEveRP2040

        host = BrtEveRP2040()
        eve = BrtEve(host)
        eve.init(resolution="1280x800", touch="goodix")

        gd = eve
        D = "/sd/"
    else:
        import bteve as eve
        from spidriver import SPIDriver
        gd = eve.Gameduino(SPIDriver(sys.argv[1]))
        D = "sd/"
        gd.init()
    return D, gd
    
if __name__ == "__main__":
    sdcard, eve = eve_init()
    
    eve.calibrate()
    while 1:
        demo.start(sdcard, eve)