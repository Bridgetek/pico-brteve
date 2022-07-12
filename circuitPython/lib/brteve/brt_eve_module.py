""" BridgeTek EVE module """
import time
import struct
from collections import namedtuple

from .brt_eve_movie_player import BrtEveMoviePlayer
from .brt_eve_common import BrtEveCommon, align4

# Order matches the register layout, so can fill with a single block read
_Touch = namedtuple(
    "TouchInputs",
    (
    "rawy",
    "rawx",
    "rz",
    "y",
    "x",
    "tag_y",
    "tag_x",
    "tag",
    ))
_State = namedtuple(
    "State",
    (
    "touching",
    "press",
    "release"
    ))
_Tracker = namedtuple(
    "Tracker",
    (
    "tag",
    "val"
    ))
_Inputs = namedtuple(
    "Inputs",
    (
    "touch",
    "tracker",
    "state",
    ))

class CoprocessorException(Exception):
    """Raise exception on faulty"""

def is_eve_faulty(read_pointer):
    """Check if EVE read pointer is faulty or not"""
    return read_pointer & 0x3

def get_transfer_addess(address):
    """Pack an address"""
    return struct.pack(">I", address)[1:]

class BrtEveModule(BrtEveCommon, BrtEveMoviePlayer): # pylint: disable=too-many-instance-attributes,too-many-public-methods
    """EVE management, including boot up and transfer data, via SPI port"""

    FIFO_MAX = (0xffc) # Maximum reported free space in the EVE command FIFO
    command_write_pointer = 0 # internal command_write_pointer position
    EVE_CMD_FIFO_SIZE =((4) * 1024) # 4kB coprocessor FIFO size
    EVE_CMD_FIFO_MASK =(EVE_CMD_FIFO_SIZE - 1)

    def __init__(self):
        self.host = None # This is set in brt_eve_[chip id].py
        self.eve = None # This is set in brt_eve_[chip id].py

        self.lcd_width=1280
        self.lcd_height=800

        self.space = 0
        self.prev_touching = 0
        self.inputs = 0

    def init(self, resolution = "", touch = ""):
        """Start up EVE and light up LCD"""

        print("Initialing for MCU " + self.eve.eve_type)
        self.eve.register(self)
        self.coldstart()

        time_start = time.monotonic()
        while self.rd32(self.eve.REG_ID) != 0x7c:
            assert (time.monotonic() - time_start) < 1.0, "No response - is device attached?"

        self.getspace()

        print("ID %x  %x %x %x" % (
            self.rd32(self.eve.REG_ID),
            self.rd32(0xc0000),
            self.rd32(self.eve.REG_HSIZE),
            self.rd32(self.eve.REG_VSIZE)))

        self.standard_startup()

        if resolution == "800x480":
            self.setup_800x480()
        if resolution == "800x480_NoSquare":
            self.setup_800x480_NoSquare()
        if resolution == "1280x720":
            self.setup_1280x720()
        if resolution == "1280x800":
            self.setup_1280x800()
        if resolution == "1024x600":
            self.setup_1024x600()
        if resolution == "480x272":
            self.setup_480x272()
        if resolution == "640x480":
            self.setup_640x480()
        if resolution == "320x480":
            self.init_ili9488()
            self.setup_320x480()

        if touch != "":
            self.setup_touch(touch)

        self.lcd_width = self.rd32(self.eve.REG_HSIZE)
        self.lcd_height = self.rd32(self.eve.REG_VSIZE)

    def spi_sdcard(self):
        """ Return SPI sdcard object"""
        return self.host.spi_sdcard

    def eve_system_clk(self, freq):
        """Setting EVE's system clock"""
        if self.eve.eve_type == "ft80x":
            if self.eve.EVE_SYSCLK_24M == freq:
                self.host_cmd(self.eve.EVE_PLL_24M)
            elif self.eve.EVE_SYSCLK_36M == freq:
                self.host_cmd(self.eve.EVE_PLL_36M)
            elif self.eve.EVE_SYSCLK_48M == freq:
                self.host_cmd(self.eve.EVE_PLL_48M)
            else:
                print("Invalid sys clk frequency selected (%i)\n", freq)
        else:
            if self.eve.EVE_SYSCLK_24M == freq:
                self.host_cmd(0x61, 0x02)
            elif self.eve.EVE_SYSCLK_36M == freq:
                self.host_cmd(0x61, 0x03)
            elif self.eve.EVE_SYSCLK_48M == freq:
                self.host_cmd(0x61, 0x44)
            elif self.eve.EVE_SYSCLK_60M == freq:
                self.host_cmd(0x61, 0x45)
            elif self.eve.EVE_SYSCLK_72M == freq:
                self.host_cmd(0x61, 0x46)
            elif self.eve.EVE_SYSCLK_DEFAULT == freq:
                self.host_cmd(0x61)

    def coldstart(self):
        """Start up EVE"""
        freq = self.eve.EVE_SYSCLK_60M #60Mhz is default for FT8xx
        if ( self.eve.eve_type == 'bt815_6' or
             self.eve.eve_type == 'bt817_8' ):
            freq = self.eve.EVE_SYSCLK_72M #72Mhz is default for BT8xx

        self.eve_system_clk(freq)
        self.host_cmd(0x44)         # Select PLL input from external clock source
        self.host_cmd(0x00)         # Wake up
        self.host_cmd(0x68)         # Core reset

    def host_cmd(self, byte_a, byte_b = 0, byte_c = 0):
        """Send a host command"""
        self.host.transfer(bytes([byte_a, byte_b, byte_c]))

    def standard_startup(self):
        """Clean up command fifo at start up"""
        self.Clear(1,1,1)
        self.swap()
        self.finish()

        self.wr32(self.eve.REG_GPIO_DIR, 0xff)
        self.wr32(self.eve.REG_GPIO, 0xff)

        time.sleep(.1)

    def cmd_regwrite(self, reg, value):
        """Write value to a register"""
        self.wr32(reg, value)

    def pad_drive_strength(self, strength, group):
        """ Set the drive strength for various pins
        :param strength: Drive strength
        :param group: Pin group to set
        :return: none
        """
        self.eve.host_cmd(0x70, group, strength)

    def transfer_read(self, address, number):
        """Transfer data to SPI in read mode"""
        dummy_bytes = 1
        return self.host.transfer(
            get_transfer_addess(address), dummy_bytes + number)[dummy_bytes:]

    def transfer_write(self, address, value):
        """Transfer data to SPI in write mode"""
        self.host.transfer(get_transfer_addess(0x800000 | address) + value)

    def rd8(self, address):
        """Get write pointer address"""
        return struct.unpack("<B", self.transfer_read(address, 1))[0]

    def rd16(self, address):
        """Read a number 16 bits"""
        return struct.unpack("<H", self.transfer_read(address, 2))[0]

    def rd32(self, address):
        """Read a number 32 bits"""
        return struct.unpack("<I", self.transfer_read(address, 4))[0]

    def wr8(self, address, value):
        """Write a number 8 bits """
        self.command_write_pointer += 1
        self.command_write_pointer &= self.EVE_CMD_FIFO_MASK
        self.transfer_write(address, struct.pack("B", value))

    def wr16(self, address, value):
        """Write a number 16 bits """
        self.command_write_pointer += 2
        self.command_write_pointer &= self.EVE_CMD_FIFO_MASK
        self.transfer_write(address, struct.pack("H", value))

    def wr32(self, address, value):
        """Write a number 32 bits """
        self.command_write_pointer += 4
        self.command_write_pointer &= self.EVE_CMD_FIFO_MASK
        self.transfer_write(address, struct.pack("I", value))

    def write_mem(self, address, buff):
        """Write a buffer to EVE"""
        self.transfer_write(address, buff)

    def read_mem(self, address, size):
        """Write a buffer to EVE"""
        return self.transfer_read(address, size)

    def write_file(self, address, file):
        """Write a buffer to EVE's RAM_G"""
        chunksize = 1000
        with open(file, 'rb') as file_handle:
            while True:
                buff = file_handle.read(chunksize)
                if not buff:
                    break # done
                self.transfer_write(address, buff)
                address += len(buff)
        return address

    def eve_write_pointer(self):
        """Get write pointer value"""
        return self.rd32(self.eve.REG_CMD_WRITE) & self.EVE_CMD_FIFO_MASK

    def eve_read_pointer(self):
        """Get read pointer value"""
        return self.rd16(self.eve.REG_CMD_READ) & self.EVE_CMD_FIFO_MASK

    def getspace(self):
        """Query space of command fifo"""
        if ( self.eve.eve_type == "bt815_6" or
             self.eve.eve_type == "bt817_8" ):
            self.space = self.rd16(self.eve.REG_CMDB_SPACE) & self.EVE_CMD_FIFO_MASK
            if is_eve_faulty(self.space):
                print("Co-processor faulty")
                raise CoprocessorException
        else:
            write_pointer = self.eve_write_pointer()
            read_pointer = self.eve_read_pointer()
            self.space = (read_pointer - write_pointer - 4) & self.EVE_CMD_FIFO_MASK

        if self.space & 1:
            print("Co-processor faulty")
            raise CoprocessorException

    def reserve(self, num):
        """Wait until command fifo have enough space"""
        while self.space < num:
            self.getspace()

    def is_finished(self):
        """Query if EVE is idle"""
        self.getspace()
        return self.space == self.FIFO_MAX

    def write(self, buffer):
        """Write a buffer to EVE's command fifo"""
        self.reserve(len(buffer))
        if ( self.eve.eve_type == "bt815_6" or
             self.eve.eve_type == "bt817_8" ):
            self.transfer_write(self.eve.REG_CMDB_WRITE, buffer)
        else:
            self.transfer_write(self.eve.RAM_CMD + self.command_write_pointer, buffer)
            self.command_write_pointer += len(buffer)
            self.command_write_pointer &= self.EVE_CMD_FIFO_MASK
            self.wr32(self.eve.REG_CMD_WRITE, self.command_write_pointer)

        self.getspace()

    def finish(self):
        """Flush command queue and wait until EVE is idle"""
        self.flush()
        self.reserve(self.FIFO_MAX)

    def VertexFormat(self, fmt):  # pylint: disable=invalid-name
        """Overwride function VertexFormat of _EVE class, do nothing if ft80x is in use"""
        if self.eve.eve_type == "ft80x":
            pass
        else:
            super().VertexFormat(fmt)

    def result(self, num=1):
        """Return the result field of the preceding command"""
        self.finish()
        write_pointer = self.rd32(self.eve.REG_CMD_READ)
        return self.rd32(self.eve.RAM_CMD + (4095 & (write_pointer - 4 * num)))

    def setup_touch(self, touch = ""):
        """Setting touch"""
        if touch ==  "goodix":
            print("Setup touch for Goodix\n")
            self.wr8(self.eve.REG_ADAPTIVE_FRAMERATE, 0);
            self.wr8(self.eve.REG_CPURESET, 2)
            self.wr16(self.eve.REG_TOUCH_CONFIG, 0x05d0)
            self.wr8(self.eve.REG_CPURESET, 0)
            time.sleep(0.3)

    def init_ili9488(self):
        """Init for ili9488 LCD"""
        #Toggle RESX pin of ILI9488 to complete power-on reset process
        self.wr32(self.eve.REG_GPIO, 0x0)
        time.sleep(0.002)
        self.wr32(self.eve.REG_GPIO, 0x83)

        ili9488_cmd_software_reset = b'\x01'
        ili9488_cmd_colomnaddr = b'\x2a'
        ili9488_cmd_rowaddr = b'\x2b'

        ili9488_cmd_interface_mode_control = b'\xb0'
        ili9488_cmd_frame_rate_control = b'\xb1'
        ili9488_cmd_interface_pixel_format = b'\x3a'
        ili9488_interface_pixel_format_18bit_dpi = b'\x66'
        ili9488_cmd_imagefunction = b'\xe9'
        ili9488_cmd_write_control_display = b'\x53'
        ili9488_cmd_madctrl = b'\x36'

        ili9488_cmd_display_function_control = b'\xb6'
        ili9488_cmd_sleep_out = b'\x11'
        ili9488_cmd_displayon = b'\x29'


        self.host.write_ili9488_cmd(ili9488_cmd_software_reset)
        time.sleep(0.00012)

        #colomn address set - 0 to 319
        self.host.write_ili9488(ili9488_cmd_colomnaddr,bytes([0x00,0x00,0x01, 0x3f]))
        #row address set - 0 to 479
        self.host.write_ili9488(ili9488_cmd_rowaddr,bytes([0x00,0x00,0x01, 0xdf]))

        #frame rate 70hz
        self.host.write_ili9488(ili9488_cmd_frame_rate_control,b'\xb0')

        #adjust control 3
        self.host.write_ili9488(b'\xf7', bytes([0xa9,0x51,0x2c,0x82]))

        self.host.write_ili9488(ili9488_cmd_interface_mode_control, b'\x02')
        self.host.write_ili9488(
            ili9488_cmd_interface_pixel_format,
            ili9488_interface_pixel_format_18bit_dpi)
        self.host.write_ili9488(ili9488_cmd_imagefunction, b'\x00')
        self.host.write_ili9488(ili9488_cmd_write_control_display, b'\x2c')

        #bgr connection and colomn address order
        self.host.write_ili9488(ili9488_cmd_madctrl,b'\x48')

        self.host.write_ili9488(ili9488_cmd_display_function_control, bytes([0x30,0x02,0x3b]))

        self.host.write_ili9488_cmd(ili9488_cmd_sleep_out)
        time.sleep(0.02)

        self.host.write_ili9488_cmd(ili9488_cmd_displayon)

    def setup_1280x720(self):
        """Default setting for LCD 1280x720"""
        self.Clear()
        self.swap()
        setup = [
            (self.eve.REG_OUTBITS, 0),
            (self.eve.REG_DITHER, 0),
            (self.eve.REG_CSPREAD, 0),
            (self.eve.REG_PCLK_POL, 0),
            (self.eve.REG_HCYCLE, 1650),
            (self.eve.REG_HOFFSET, 260),
            (self.eve.REG_HSIZE, 1280),
            (self.eve.REG_VCYCLE, 750),
            (self.eve.REG_VOFFSET, 225),
            (self.eve.REG_VSIZE, 720),
            (self.eve.REG_HSYNC1, 0),
            (self.eve.REG_HSYNC0, 40),
            (self.eve.REG_VSYNC1, 0),
            (self.eve.REG_VSYNC0, 5),
            (self.eve.REG_ADAPTIVE_FRAMERATE, 0),
            (self.eve.REG_PCLK, 1),
        ]
        for (adress, value) in setup:
            self.cmd_regwrite(adress, value)

    def setup_320x240(self):
        """Default setting for LCD QVGA 320x240"""
        self.Clear()
        self.swap()
        setup = [
            (self.eve.REG_DITHER, 1),
            (self.eve.REG_CSPREAD, 1),
            (self.eve.REG_PCLK_POL, 0),
            (self.eve.REG_SWIZZLE, 2),

            (self.eve.REG_HCYCLE, 408),
            (self.eve.REG_HOFFSET, 70),
            (self.eve.REG_HSIZE, 320),

            (self.eve.REG_HSYNC1, 10),
            (self.eve.REG_HSYNC0, 0),

            (self.eve.REG_VCYCLE, 263),
            (self.eve.REG_VOFFSET, 13),
            (self.eve.REG_VSIZE, 240),

            (self.eve.REG_VSYNC1, 2),
            (self.eve.REG_VSYNC0, 0),
            (self.eve.REG_PCLK, 8),
        ]
        for (adress, value) in setup:
            self.cmd_regwrite(adress, value)

    def setup_320x480(self):
        """Default setting for LCD HVGA 320x480"""
        self.Clear()
        self.swap()
        setup = [
            (self.eve.REG_DITHER, 1),
            (self.eve.REG_CSPREAD, 1),
            (self.eve.REG_PCLK_POL, 1),
            (self.eve.REG_SWIZZLE, 2),

            (self.eve.REG_HCYCLE, 400),
            (self.eve.REG_HOFFSET, 40),
            (self.eve.REG_HSIZE, 320),

            (self.eve.REG_HSYNC1, 10),
            (self.eve.REG_HSYNC0, 0),

            (self.eve.REG_VCYCLE, 500),
            (self.eve.REG_VOFFSET, 10),
            (self.eve.REG_VSIZE, 480),

            (self.eve.REG_VSYNC1, 5),
            (self.eve.REG_VSYNC0, 0),
            (self.eve.REG_PCLK, 5),
        ]
        for (adress, value) in setup:
            self.cmd_regwrite(adress, value)

    def setup_640x480(self):
        """Default setting for LCD 640x480"""
        self.Clear()
        self.swap()
        setup = [
            # (self.eve.REG_OUTBITS, 0),
            (self.eve.REG_DITHER, 0),
            (self.eve.REG_CSPREAD, 0),
            (self.eve.REG_PCLK_POL, 0),
            (self.eve.REG_ADAPTIVE_FRAMERATE, 0),

            (self.eve.REG_HCYCLE, 800),
            (self.eve.REG_HOFFSET, 16 + 96),
            (self.eve.REG_HSIZE, 640),

            (self.eve.REG_HSYNC1, 0),
            (self.eve.REG_HSYNC0, 96),

            (self.eve.REG_VCYCLE, 525),
            (self.eve.REG_VOFFSET, 12),
            (self.eve.REG_VSIZE, 480),

            (self.eve.REG_VSYNC1, 0),
            (self.eve.REG_VSYNC0, 10),
            (self.eve.REG_PCLK, 3),
        ]
        for (adress, value) in setup:
            self.cmd_regwrite(adress, value)
    def setup_800x480(self):
        """Default setting for LCD WVGA 800x480"""
        self.Clear()
        self.swap()
        setup = [
            # (self.eve.REG_OUTBITS, 0),
            (self.eve.REG_DITHER, 1),
            (self.eve.REG_CSPREAD, 0),
            (self.eve.REG_PCLK_POL, 1),
#            (self.eve.REG_ADAPTIVE_FRAMERATE, 0),

            (self.eve.REG_HCYCLE, 928),
            (self.eve.REG_HOFFSET, 88),
            (self.eve.REG_HSIZE, 800),

            (self.eve.REG_HSYNC1, 48),
            (self.eve.REG_HSYNC0, 0),

            (self.eve.REG_VCYCLE, 525),
            (self.eve.REG_VOFFSET, 32),
            (self.eve.REG_VSIZE, 480),

            (self.eve.REG_VSYNC1, 3),
            (self.eve.REG_VSYNC0, 0),
            (self.eve.REG_PCLK, 2),
        ]
        for (adress, value) in setup:
            self.cmd_regwrite(adress, value)
    def setup_800x480_NoSquare(self):
        """Default setting for LCD WVGA 800x480"""
        print("setup_800x480_NoSquare ")
        self.Clear()
        self.swap()
        setup = [
            # (self.eve.REG_OUTBITS, 0),
            (self.eve.REG_DITHER, 1),
            (self.eve.REG_CSPREAD, 0),
            (self.eve.REG_PCLK_POL, 1),
#            (self.eve.REG_ADAPTIVE_FRAMERATE, 0),

            (self.eve.REG_HCYCLE, 928),
            (self.eve.REG_HOFFSET, 88),
            (self.eve.REG_HSIZE, 861),
            (self.eve.REG_HSYNC1, 48),
            (self.eve.REG_HSYNC0, 0),

            (self.eve.REG_VCYCLE, 525),
            (self.eve.REG_VOFFSET, 32),
            (self.eve.REG_VSIZE, 480),

            (self.eve.REG_VSYNC1, 3),
            (self.eve.REG_VSYNC0, 0),
            #(self.eve.REG_PCLK, 2),
            (self.eve.REG_PCLK, 1), #When REG_PCLK is set to 1, the display output will be in EXTSYNC mode
            (self.eve.REG_PCLK_FREQ, 0x8A1), #60M
            #(self.eve.REG_PCLK_FREQ, 0x8B2), #33M
            #(self.eve.REG_PCLK_FREQ, 443),  #8M blink
        ]
        for (adress, value) in setup:
            self.cmd_regwrite(adress, value)

    def setup_1024x600(self):
        """Default setting for LCD WSVGA 1024x600"""
        self.Clear()
        self.swap()
        setup = [
            (self.eve.REG_DITHER, 1),
            (self.eve.REG_CSPREAD, 0),
            (self.eve.REG_PCLK_POL, 1),
            (self.eve.REG_ADAPTIVE_FRAMERATE, 0),

            (self.eve.REG_HCYCLE, 1344),
            (self.eve.REG_HOFFSET, 160),
            (self.eve.REG_HSIZE, 1024),

            (self.eve.REG_HSYNC1, 100),
            (self.eve.REG_HSYNC0, 0),

            (self.eve.REG_VCYCLE, 635),
            (self.eve.REG_VOFFSET, 23),
            (self.eve.REG_VSIZE, 600),

            (self.eve.REG_VSYNC1, 10),
            (self.eve.REG_VSYNC0, 0),
            (self.eve.REG_PCLK, 1),
            (self.eve.REG_PCLK_FREQ, 0xD12),
        ]
        for (adress, value) in setup:
            self.cmd_regwrite(adress, value)
    def setup_480x272(self):
        """Default setting for LCD WQVGA 480x272"""
        self.Clear()
        self.swap()
        setup = [
            (self.eve.REG_DITHER, 0),
            (self.eve.REG_CSPREAD, 0),
            (self.eve.REG_PCLK_POL, 1),

            (self.eve.REG_HCYCLE, 548),
            (self.eve.REG_HOFFSET, 43),
            (self.eve.REG_HSIZE, 480),

            (self.eve.REG_HSYNC1, 41),
            (self.eve.REG_HSYNC0, 0),

            (self.eve.REG_VCYCLE, 292),
            (self.eve.REG_VOFFSET, 12),
            (self.eve.REG_VSIZE, 272),

            (self.eve.REG_VSYNC1, 10),
            (self.eve.REG_VSYNC0, 0),
            (self.eve.REG_PCLK, 5),
        ]
        for (adress, value) in setup:
            self.cmd_regwrite(adress, value)

    def setup_1280x800(self):
        """Default setting for LCD WXGA 1280x800"""
        self.Clear()
        self.swap()
        setup = [
            (self.eve.REG_OUTBITS, 0),
            (self.eve.REG_PCLK, 0),
            (self.eve.REG_DITHER, 0),
            (self.eve.REG_CSPREAD, 0),
            (self.eve.REG_PCLK_POL, 0),
            (self.eve.REG_PCLK_2X, 0),

            (self.eve.REG_HCYCLE, 1411),
            (self.eve.REG_HOFFSET, 120),
            (self.eve.REG_HSIZE, 1280),

            (self.eve.REG_HSYNC1, 100),
            (self.eve.REG_HSYNC0, 0),

            (self.eve.REG_VCYCLE, 815),
            (self.eve.REG_VOFFSET, 14),
            (self.eve.REG_VSIZE, 800),

            (self.eve.REG_VSYNC1, 10),
            (self.eve.REG_VSYNC0, 0),
            (self.eve.REG_PCLK_FREQ, 0x8B1),
            (self.eve.REG_PCLK, 1),
        ]
        for (adress, value) in setup:
            self.cmd_regwrite(adress, value)

    # Some higher-level functions
    def get_inputs(self):
        """Get user inputs"""
        self.finish()
        touch = _Touch(*struct.unpack("HHIhhhhB",
            self.transfer_read(self.eve.REG_TOUCH_RAW_XY, 17)))

        tracker = _Tracker(*struct.unpack("HH", self.transfer_read(self.eve.REG_TRACKER, 4)))

        if not hasattr(self, "prev_touching"):
            self.prev_touching = False
        touching = (touch.x != -32768)
        press = touching and not self.prev_touching
        release = (not touching) and self.prev_touching
        state = _State(touching, press, release)
        self.prev_touching = touching

        self.inputs = _Inputs(touch, tracker, state)
        return self.inputs

    def swap(self):
        """Flush command queue and swap display list"""
        self.Display()
        self.eve.cmd_swap()
        self.flush()
        self.eve.cmd_dlstart()
        self.eve.cmd_loadidentity()

    def calibrate(self):
        """Start calibration screen"""
        self.ClearColorRGB(64, 64, 64)
        self.Clear(1, 1, 1)
        self.ColorRGB(0xff, 0xff, 0xff)
        self.eve.cmd_text(self.lcd_width // 2, self.lcd_height // 2, 29, 0x0600, "Tap the dot")

        self.eve.cmd_calibrate()
        self.eve.cmd_dlstart()

    def screenshot_ft800(self, dest):
        """Take screen shot, this function is only available on FT800"""
        self.finish()

        pclk = self.rd32(self.eve.REG_PCLK)
        self.wr32(self.eve.REG_PCLK, 0)
        time.sleep(0.001)
        self.wr32(self.eve.REG_SCREENSHOT_EN, 1)
        self.wr32(0x0030201c, 32)

        for i in range(self.lcd_height):
            print(i, "/", self.lcd_height)
            self.wr32(self.eve.REG_SCREENSHOT_Y, i)
            self.wr32(self.eve.REG_SCREENSHOT_START, 1)
            time.sleep(.002)

            while self.transfer_read(self.eve.REG_SCREENSHOT_BUSY, 8) != bytes(8):
                pass

            self.wr32(self.eve.REG_SCREENSHOT_READ, 1)
            bgra = self.transfer_read(self.eve.RAM_SCREENSHOT, 4 * self.lcd_width)
            (color_b, color_g, color_r) = [bgra[i::4] for i in range(3)]
            line = bytes(sum(zip(color_r, color_g, color_b), ()))
            dest(line)
            self.wr32(self.eve.REG_SCREENSHOT_READ, 0)
        self.wr32(self.eve.REG_SCREENSHOT_EN, 0)
        self.wr32(self.eve.REG_PCLK, pclk)

    def load(self, file_handler):
        """Load a file to command fifo"""
        while True:
            chunk = file_handler.read(512)
            if not chunk:
                return
            self.cc(align4(chunk))
