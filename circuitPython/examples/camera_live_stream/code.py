import time
import busio
import board

from adafruit import adafruit_vc0706
from brteve.brt_eve_common import align4
from brteve.brt_eve_bt817_8 import BrtEve
from brteve.brt_eve_rp2040 import BrtEveRP2040

img_size=adafruit_vc0706.IMAGE_SIZE_320x240
class camera_live_stream:
    def __init__(self):
        self.TEST_DIR = ''
        self.eve=eve

    def start_load_image(self):
        # start drawing
        eve.ClearColorRGB(255, 255, 255)
        eve.Clear()
        eve.ColorRGB(255, 255, 255)
        eve.VertexFormat(4)

        self.eve.cmd_loadimage(0, 0)

    def write_image_buffer(self, buff):
        try:
            self.eve.cc(align4(buff))
        except Exception:
            self.reset_eve()
            return 0

        return 1

    def reset_eve(self):
        while 1:
            try:
                print("Reset...")
                RAM_ERR_REPORT=0x309800
                err=eve.transfer_read(RAM_ERR_REPORT, (int)(128))
                print("Err=", err.decode('utf-8'))

                REG_COPRO_PATCH_PTR=0x302000+0x7162
                original_patch = eve.rd32(REG_COPRO_PATCH_PTR)
                eve.wr32(eve.REG_CPURESET, 0xffffffff)
                eve.wr32(eve.REG_CMD_WRITE, 0)
                eve.wr32(eve.REG_CMD_READ, 0)
                eve.wr32(eve.REG_CMD_DL, 0)
                eve.wr32(eve.REG_PCLK, pclk)
                eve.wr32(eve.REG_CPURESET, 0)
                eve.wr32(REG_COPRO_PATCH_PTR, original_patch)

                eve.init(resolution="1280x800", touch="goodix")
                eve.ClearColorRGB(0, 0, 0)
                eve.Clear()
                eve.ColorRGB(255, 255, 255)
                self.eve.cmd_text(10, 10, 25, 0, "corrupted JPEG")
                self.eve.swap()

                break
            except Exception:
                pass
            time.sleep(0.5)

    def draw_1_image(self):
        try:
            eve.flush()
            self.eve.cmd_getprops()
            eve.flush()
        except Exception:
            self.reset_eve()
            return

        x=eve.lcd_width/2-w/2
        y=eve.lcd_height/2-h/2

        self.eve.Begin(self.eve.BITMAPS)
        self.eve.Vertex2f(x, y)

        self.eve.ColorRGB(0, 0, 0)
        # self.eve.cmd_text(100,500, 30, 0, "ptr="+str(ptr)+", width="+str(w)+", height="+str(h))

        self.eve.swap()
        eve.flush()

def init_VC0706Camera():
    # Setup VC0706 camera
    baudrate=115200
    uart = busio.UART(board.GP0, board.GP1, baudrate=baudrate, timeout=0.1)
    vc0706cam = adafruit_vc0706.VC0706(uart)
    time.sleep(3)

    # Print the version string from the camera.
    print("VC0706 version:")
    print(vc0706cam.version)

    # Set the baud rate to 115200 for fastest transfer (its the max speed)
    vc0706cam.baudrate = baudrate

    # Set the image size.
    vc0706cam.image_size = img_size  # Or set IMAGE_SIZE_320x240 or
    # IMAGE_SIZE_160x120
    # Note you can also read the property and compare against those values to
    # see the current size:
    size = vc0706cam.image_size
    if size == adafruit_vc0706.IMAGE_SIZE_640x480:
        width=640
        height=480
        print("Using 640x480 size image.")
    elif size == adafruit_vc0706.IMAGE_SIZE_320x240:
        width=320
        height=240
        print("Using 320x240 size image.")
    elif size == adafruit_vc0706.IMAGE_SIZE_160x120:
        width=160
        height=120
        print("Using 160x120 size image.")

    #  Motion detection system can alert you when the camera 'sees' motion!
    #vc0706.set_motion_detect(1)           # turn it on

    # You can also verify whether motion detection is active!
    print("Motion detection is ")
    if vc0706cam.get_motion_detect():
        print("ON")
    else :
        print("OFF")
    return vc0706cam, width, height

def get_buffer():
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

            if demo.write_image_buffer(copy_buffer) == 0:
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

host = BrtEveRP2040()
eve = BrtEve(host)
eve.init(resolution="1280x800", touch="goodix")

pclk=eve.rd32(eve.REG_PCLK)

demo=camera_live_stream()
vc0706, w, h = init_VC0706Camera()
get_buffer()
