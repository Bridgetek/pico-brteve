"""The Raspberry Pi Pico is a low-cost, high-performance microcontroller
   board with flexible digital interfaces"""
import os
import board
import busio
import digitalio
import sdcardio # pylint: disable=import-error
import storage # pylint: disable=import-error

def spilock(func):
    """ Delegate function, to lock and unlocki SPI"""
    def wrapper(*args):
        spi = args[0].spi_eve
        while not spi.try_lock():
            pass
        ret = func(*args)
        spi.unlock()
        return ret
    return wrapper

def pin(pin_num):
    """Init a pin"""
    ret = digitalio.DigitalInOut(pin_num)
    ret.direction = digitalio.Direction.OUTPUT
    ret.value = True
    return ret

class BrtEveRP2040():
    """ Host platform RP2040 to control EVE, this class initialize,
    and set up SPI connection on RP2040, also set up the SDcard

    A host platform class must have below APIs:
     - transfer()
     - write_ili9488()
     - write_ili9488_cmd()
     - write_ili9488_data()
     - spi_sdcard -- SPI object of SDcard interface
    """

    def __init__(self):
        mach = os.uname().machine # pylint: disable=no-member
        if mach == 'Raspberry Pi Pico with rp2040':
            #SPI for Eve
            self.spi_eve = busio.SPI(board.GP2, MOSI=board.GP3, MISO=board.GP4)

            #SPI for SD card
            self.spi_sdcard = busio.SPI(board.GP10, MOSI=board.GP11, MISO=board.GP12)

        self.pin_cs = pin(board.GP5) #cs of SPI for Eve
        self.pin_pdn = pin(board.GP7) #power down pin of Eve

        self.pin_cs_eve_ili9488 = pin(board.GP9) #CSX pin of ILI9488
        self.pin_dcx_eve_ili9488 = pin(board.GP8) #D/CX pin of ILI9488

        self.pin_cs_sdcard = board.GP13 #cs of SPI for SD card

        if not self._setup_sd(self.pin_cs_sdcard):
            pin(self.pin_cs_sdcard)

		#configure SPI for Eve
        self._setup_spi()

    def _setup_sd(self, sdcs):
        """ Setup sdcard"""
        try:
            self.sdcard = sdcardio.SDCard(self.spi_sdcard, sdcs)
        except OSError:
            return False
        self.vfs = storage.VfsFat(self.sdcard)
        storage.mount(self.vfs, "/sd")
        return True

    @spilock
    def _setup_spi(self):
        """ Setup SPI interface"""
        self.spi_eve.configure(baudrate=30000000, phase=0, polarity=0)

    @spilock
    def transfer(self, write_data, bytes_to_read = 0):
        """ Transfer data via SPI"""
        self.pin_cs.value = False
        self.spi_eve.write(write_data)
        read_buffer = None
        if bytes_to_read != 0:
            read_buffer = bytearray(bytes_to_read)
            self.spi_eve.readinto(read_buffer)
        self.pin_cs.value = True
        return read_buffer

    def write_ili9488(self,cmd,data):
        """ Write command and data to ili9488 LCD"""
        self.write_ili9488_cmd(cmd)
        self.write_ili9488_data(data)

    @spilock
    def write_ili9488_cmd(self, cmd):
        """ Write command to ili9488 LCD"""
        self.pin_cs_eve_ili9488.value = False
        self.pin_dcx_eve_ili9488.value = False

        self.spi_eve.write(cmd)
        self.pin_cs_eve_ili9488.value = True

    @spilock
    def write_ili9488_data(self, data):
        """ Write data to ili9488 LCD"""
        self.pin_cs_eve_ili9488.value = False
        self.pin_dcx_eve_ili9488.value = True

        self.spi_eve.write(data)
        self.pin_cs_eve_ili9488.value = True
