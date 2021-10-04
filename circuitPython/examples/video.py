import sys

if __name__ == "__main__":
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

    while 1:
        for fn in ("car-1500.avi",  "fun-1500.avi", "spa-1500.avi", "tra-1500.avi", "tub-1500.avi"):
            with open(D + fn, "rb") as f:
                mp = eve.movie_player(gd, f)
                mp.play()
