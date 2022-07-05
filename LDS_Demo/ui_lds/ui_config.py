class ui_config():
    skipSensor=False
    def __init__(self):
        self.aa=240/96
        self.bb=256 *(1/self.aa)
        self.firstTime=True
        self.simulatorTitle="."
        self.btn_w = 110
        self.btn_h = 40
        self.x0 = 10
        self.y0 = 10
        self.xmargin =10
        self.xStart = 380
        self.readingInterval=1000 # reading sensor interval
