import bteve as eve_module
import busio
import board
import time
import sys
sys.path.append("lib/adafruit")

import adafruit_vc0706

w=160
h=120
img_size=adafruit_vc0706.IMAGE_SIZE_320x240
pclk=0
class camera_live_stream:
    def __init__(self, eve:eve_module.Brt_PicoEve_Module) -> None:
        self.TEST_DIR = ''
        self.eve=eve
    
    def start_load_image(self):
        eve=self.eve

        # start drawing
        eve.ClearColorRGB(255, 255, 255)
        eve.Clear()
        eve.ColorRGB(255, 255, 255)
        eve.VertexFormat(4)

        self.eve.cmd_loadimage(0, 0)

    def write_image_buffer(self, buff):
        try:
            self.eve.cc(eve_module.align4(buff))
        except:
            self.reset_eve()
            return 0
            
        return 1

    def reset_eve(self):
        global pclk

        while 1:
            try:
                print("Reset...")
                RAM_ERR_REPORT=0x309800
                err=eve.rd(RAM_ERR_REPORT, (int)(128))
                print("Err=", err.decode('utf-8'))

                REG_COPRO_PATCH_PTR=0x302000+0x7162
                original_patch = eve.rd32(REG_COPRO_PATCH_PTR); 
                eve.wr32(eve.REG_CPURESET, 0xffffffff);
                eve.wr32(eve.REG_CMD_WRITE, 0);
                eve.wr32(eve.REG_CMD_READ, 0);
                eve.wr32(eve.REG_CMD_DL, 0);
                eve.wr32(eve.REG_PCLK, pclk);
                eve.wr32(eve.REG_CPURESET, 0);
                eve.wr32(REG_COPRO_PATCH_PTR, original_patch);
                
                eve.init()

                eve.ClearColorRGB(0, 0, 0)
                eve.Clear()
                eve.ColorRGB(255, 255, 255)
                self.eve.cmd_text(10, 10, 25, 0, "corrupted JPEG")
                self.eve.swap()

                break
            except:
                pass
            time.sleep(0.5)

    def draw_1_image(self):
        eve=self.eve
        try:
            eve.flush()
            rp=eve.EVE_Cmd_wp(); 
            self.eve.cmd_getprops(0, 0, 0)
            eve.flush()
        except:
            self.reset_eve()
            return

        x=eve.w/2-w/2
        y=eve.h/2-h/2

        self.eve.Begin(self.eve.BITMAPS)
        self.eve.Vertex2f(x, y)

        self.eve.ColorRGB(0, 0, 0)
        # self.eve.cmd_text(100,500, 30, 0, "ptr="+str(ptr)+", width="+str(w)+", height="+str(h))

        self.eve.swap()
        eve.flush()

def init_VC0706Camera():
    global w, h
    # Setup VC0706 camera
    baudrate=115200 
    uart = busio.UART(board.GP0, board.GP1, baudrate=baudrate, timeout=0.1)
    vc0706 = adafruit_vc0706.VC0706(uart)
    time.sleep(3)

    # Print the version string from the camera.
    print("VC0706 version:")
    print(vc0706.version)

    # Set the baud rate to 115200 for fastest transfer (its the max speed)
    vc0706.baudrate = baudrate

    # Set the image size.
    vc0706.image_size = img_size  # Or set IMAGE_SIZE_320x240 or
    # IMAGE_SIZE_160x120
    # Note you can also read the property and compare against those values to
    # see the current size:
    size = vc0706.image_size
    if size == adafruit_vc0706.IMAGE_SIZE_640x480:
        w=640
        h=480
        print("Using 640x480 size image.")
    elif size == adafruit_vc0706.IMAGE_SIZE_320x240:
        w=320
        h=240
        print("Using 320x240 size image.")
    elif size == adafruit_vc0706.IMAGE_SIZE_160x120:
        w=160
        h=120
        print("Using 160x120 size image.")

    #  Motion detection system can alert you when the camera 'sees' motion!
    #vc0706.set_motion_detect(1);           # turn it on

    # You can also verify whether motion detection is active!
    print("Motion detection is ");
    if (vc0706.get_motion_detect()) :
        print("ON");
    else :
        print("OFF");
    return vc0706

def get_buffer(vc0706, demo:camera_live_stream):
    while 1:
        print("SNAP!")
        if not vc0706.take_picture():
            raise RuntimeError("Failed to take picture!")
        frame_length = vc0706.frame_length
        print("Picture size (bytes): {}".format(frame_length))
        stamp = time.monotonic()
        # pylint: disable=invalid-name
        wcount = 0
        
        demo.start_load_image()
        while frame_length > 0:
            # Compute how much data is left to read as the lesser of remaining bytes
            # or the copy buffer size (32 bytes at a time).  Buffer size MUST be
            # a multiple of 4 and under 100.  Stick with 32!
            to_read = min(frame_length, 32)
            copy_buffer = bytearray(to_read)
            # Read picture data into the copy buffer.
            if vc0706.read_picture_into(copy_buffer) == 0:
                pass #raise RuntimeError("Failed to read picture frame data!")
            # Write the data to SD card file and decrement remaining bytes.
            
            if 0 == demo.write_image_buffer(copy_buffer):
                print("Write buffer error... skip frame")
                break
            
            frame_length -= 32
            # Print a dot every 2k bytes to show progress.
            wcount += 1
            if wcount >= 64:
                print(".", end="")
                wcount = 0
        print("Finished in %0.1f seconds!" % (time.monotonic() - stamp))
        demo.draw_1_image()

        # Turn the camera back into video mode.
        vc0706.resume_video()

eve = eve_module.Brt_PicoEve_Module()  # Default is MM2040 with LCD 1280x800 capacity touch
eve.set_lcd(eve.LCD_WSVGA_1024X600())
eve.init()

pclk=eve.rd32(eve.REG_PCLK)

demo=camera_live_stream(eve)
vc0706 = init_VC0706Camera()
get_buffer(vc0706, demo)
