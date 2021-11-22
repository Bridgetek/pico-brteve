import time
from brteve.brt_eve_bt817_8 import BrtEve

EVE_CMD_FIFO_SIZE = ((4) * 1024)
EVE_CMD_FIFO_MASK = (EVE_CMD_FIFO_SIZE - 1)

class eve_helper():
    RAM_G_SIZE = (1024*1024)
    def __init__(self, eve: BrtEve):
        self.eve = eve

        self.VertextFormat=4
        self.VertextPrecision=16

    def set_precision(self, VertextFormat):
        lst={0:1, 1:2, 2:4, 3:8, 4:16}
        self.VertextFormat=VertextFormat
        self.VertextPrecision=lst[VertextFormat]

    def get_precision(self, v):
        return (int)(v*self.VertextPrecision)

    def cmd_(self):
        pass

    def cmd_rp(self):
        return self.eve.rd32 (self.eve.REG_CMD_READ) & EVE_CMD_FIFO_MASK

    def cmd_wp(self):
        return self.eve.rd32 (self.eve.REG_CMD_WRITE) & EVE_CMD_FIFO_MASK

    def wait_flush(self):
        rp = self.cmd_rp()
        wp = self.cmd_wp()
        while rp != wp:
            rp = self.cmd_rp()
            wp = self.cmd_wp()
            time.sleep(0.01)

    def flash_switch_fullmode(self):
        self.eve.cmd_flashdetach()
        self.wait_flush()

        self.eve.cmd_flashattach()
        self.wait_flush()

        self.eve.cmd_flashfast()
        self.wait_flush()

    def Display_Start(self):
        self.eve.cmd_dlstart()
        self.eve.ClearColorRGB(255, 255, 255)
        self.eve.Clear(1, 1, 1)
        self.eve.ColorRGB(255, 255, 255)
        self.eve.VertexFormat(self.VertextFormat)

    def Display_End(self):
        self.eve.Display()
        self.eve.swap()
        self.wait_flush()


    def draw_image(self, img, addr, x, y):
        self.eve.BitmapHandle(1)
        self.eve.cmd_setbitmap(addr, img["format"], img["width"], img["height"])
        self.eve.Begin(self.eve.BITMAPS)
        self.eve.Vertex2f((x), (y))
        self.eve.End()

    def draw_image_with_tag(self, img, addr, x, y, tag):
        self.eve.BitmapHandle(1)
        self.eve.cmd_setbitmap(addr, img["format"], img["width"], img["height"])
        self.eve.Tag(tag)
        self.eve.Begin(self.eve.BITMAPS)
        self.eve.Vertex2f((x),(y))
        self.eve.End()
        self.eve.Tag(0)

    def current_milli_time(self):
        return round(time.time() * 1000)
