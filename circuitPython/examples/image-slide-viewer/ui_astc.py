import time
from brteve.brt_eve_bt817_8 import BrtEve

class ui_astc:
    def __init__(self, eve: BrtEve) -> None:
        self.eve=eve
        self.x = 0
        self.y = 0
        self.w = 0
        self.h = 0
        self.index = 0
        self.image_num = 0
        self.images=[]
        self.names=[]
        self.sizes=[]
        self.name=""

    def set_images(self, images, names, size):
        self.images=images
        self.names=names
        self.sizes=size
        print(self.sizes)
        print(self.sizes[self.index])
        print(self.sizes[self.index][0])
        self.image_num=len(images)
        self.draw_1_image(images[self.index], self.names[self.index], self.sizes[self.index])

    def swipe_left(self):
        self.swipe_image('left')
        self.index+=1
        if self.index >= self.image_num:
            self.index=0
        self.draw_1_image(self.images[self.index], self.names[self.index], self.sizes[self.index])

    def swipe_right(self):
        self.swipe_image('right')
        self.index -=1
        if self.index < 0:
            self.index=self.image_num-1

        self.draw_1_image(self.images[self.index], self.names[self.index], self.sizes[self.index])

    def swipe_image(self, direc):
        eve=self.eve
        offset = 0

        x = self.x
        y = self.y
        w = self.w
        h = self.h
        name = self.name

        if direc=='right':
            direc=-1
        else:
            direc=1

        while offset < (x + w):
            eve.ClearColorRGB(255, 255, 255)
            eve.Clear()
            eve.ColorRGB(255, 255, 255)
            eve.VertexFormat(4)

            eve.Begin(eve.BITMAPS)
            eve.Vertex2f(x-offset*direc, y)

            tx = x - offset*direc + w/2 - len(name) * 5
            ty = y + h + 10

            eve.ColorRGB(0, 0, 0)
            eve.cmd_text(tx, ty, 30, 0, name)
            eve.swap()
            offset += 10 + offset/5
            time.sleep(0.01)

        eve.ClearColorRGB(255, 255, 255)
        eve.Clear()
        eve.ColorRGB(255, 255, 255)
        eve.swap()

    def draw_1_image(self,image, name, size):
        eve=self.eve

        print(size)
        print(size[0])
        w=(int)(size[0])
        h=(int)(size[1])

        addr=0
        wp=addr
        CHUNK = 32
        with open(image, 'rb') as file:
            buff = file.read(CHUNK)
            while buff != b"":
                eve.wr(wp, buff)
                wp += CHUNK
                buff = file.read(CHUNK)

        eve.ClearColorRGB(255, 255, 255)
        eve.Clear()
        eve.ColorRGB(255, 255, 255)
        eve.VertexFormat(4)

        eve.cmd_setbitmap(addr, eve.ASTC_8x8, w, h)
        # eve.cmd_setbitmap(addr, eve.ASTC_4x4, w, h)

        x=eve.lcd_width/2-w/2
        y=eve.lcd_height/2-h/2

        eve.Begin(eve.BITMAPS)
        eve.Vertex2f(x, y)

        tx = x + w/2 - len(name) * 5
        ty = y+ h + 10

        eve.ColorRGB(0, 0, 0)
        eve.cmd_text(tx, ty, 30, 0, name)
        eve.swap()
        eve.flush()

        self.x = x
        self.y = y
        self.w = w
        self.h = h
        self.name = name
