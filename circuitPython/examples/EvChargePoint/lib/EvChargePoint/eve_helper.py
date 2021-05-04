import time


from bteve import registers  as reg
import bteve as eve

EVE_CMD_FIFO_SIZE = ((4) * 1024)
EVE_CMD_FIFO_MASK = (EVE_CMD_FIFO_SIZE - 1)

class eve_helper():
    RAM_G_SIZE = (1024*1024)
    def __init__(self, gd: Brt_PicoEve_Module, eve:eve):
        self.gd = gd

        self.VertextFormat=4
        self.VertextPrecision=16
        pass
    
    def set_precision(self, VertextFormat):
        lst={0:1, 1:2, 2:4, 3:8, 4:16}
        self.VertextFormat=VertextFormat
        self.VertextPrecision=lst[VertextFormat]

    def get_precision(self, v):
        return (int)(v*self.VertextPrecision)

    def cmd_(self):
        pass
    
    def cmd_rp(self):
        return self.gd.rd32 (reg.REG_CMD_READ) & EVE_CMD_FIFO_MASK
        
    def cmd_wp(self):
        return self.gd.rd32 (reg.REG_CMD_WRITE) & EVE_CMD_FIFO_MASK

    def wait_flush(self):
        rp = self.cmd_rp()
        wp = self.cmd_wp()
        while rp != wp:
            rp = self.cmd_rp(); wp = self.cmd_wp()
            time.sleep(0.01)
            
    def flash_switch_fullmode(self):
        self.gd.cmd_flashdetach()
        self.wait_flush()

        self.gd.cmd_flashattach()
        self.wait_flush()

        self.gd.cmd_flashfast()
        self.wait_flush()
        
    def Display_Start(self):
        self.gd.cmd_dlstart()
        self.gd.ClearColorRGB(255, 255, 255)
        self.gd.Clear(1, 1, 1)
        self.gd.ColorRGB(255, 255, 255)
        self.gd.VertexFormat(self.VertextFormat)

    def Display_End(self):
        self.gd.Display()
        self.gd.swap()
        self.wait_flush()

    
    def draw_image(self, img, addr, x, y):
        self.gd.BitmapHandle(1)
        self.gd.cmd_setbitmap(addr, img["format"], img["width"], img["height"]);    
        self.gd.Begin(eve.BITMAPS)
        self.gd.Vertex2f((x), (y))
        self.gd.End()
    
    def draw_image_with_tag(self, img, addr, x, y, tag):
        self.gd.BitmapHandle(1)
        self.gd.cmd_setbitmap(addr, img["format"], img["width"], img["height"])
        self.gd.Tag(tag); 
        self.gd.Begin(eve.BITMAPS)
        self.gd.Vertex2f((x),(y))
        self.gd.End()
        self.gd.Tag(0)
    
    def current_milli_time(self):
        return round(time.time() * 1000)

