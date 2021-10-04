
""" Movie player for BridgeTek's EVE chips"""
from .brt_eve_common import const

_SOURCE_FILE = 0
_SOURCE_FLASH = 1
class BrtEveMoviePlayer:
    """ Helper class to play movie with EVE """
    def __init__(self):
        self.source = _SOURCE_FILE
        self.file = ''
        self.eve = None # this eve is set in BrtEveModule
        self.flag = 0
        self.mf_base = 0
        self.mf_size = 0
        self.write_pointer = 0

    def movie_player_from_flash(self, flash_address: int):
        """ Play video from flash"""
        eve = self.eve
        self.source = _SOURCE_FLASH
        if eve.storage.flash_state(eve.FLASH_STATUS_FULL) != 0:
            print("Cannot switch flash to full mode")
            return self

        self.flag = eve.OPT_FLASH | eve.OPT_FULLSCREEN | eve.OPT_NOTEAR | eve.OPT_SOUND
        eve.cmd_flashsource(flash_address)
        return self

    def movie_player(self, file: str, mf_base=const(0xF0000), mf_size=const(0x8000)):
        """ Play video from a file handler via media fifo"""
        eve = self.eve
        self.file = file
        self.mf_base = mf_base
        self.mf_size = mf_size
        self.source = _SOURCE_FILE

        eve.cmd_mediafifo(mf_base, mf_size)
        self.write_pointer = 0
        eve.cmd_regwrite(eve.REG_MEDIAFIFO_WRITE, 0)

        self.flag = eve.OPT_MEDIAFIFO | eve.OPT_FULLSCREEN | eve.OPT_NOTEAR | eve.OPT_SOUND
        return self

    def set_flag(self, flag):
        """ Set cmd_playvideo flag"""
        self.flag = flag

    def add_flag(self, flag):
        """ Append cmd_playvideo flag"""
        self.flag |= flag

    def set_volume(self, vol):
        """ Set system volume"""
        eve = self.eve
        eve.wr8(eve.REG_VOL_PB, vol)
        
    def play(self):
        """ Start playback a movie"""
        eve = self.eve
        eve.wr8(eve.REG_GPIOX_DIR,0xFF)
        eve.wr8(eve.REG_GPIOX,0xFF)

        self.set_volume(0xFF)
        eve.cmd_playvideo(self.flag)

        if self.source == _SOURCE_FILE:
            eve.Nop()
            eve.flush()
            with open(self.file, "rb") as file_handler:
                while not eve.is_finished():
                    self._file_service(file_handler)

        eve.Nop()
        eve.finish()

    def _file_service(self, file_handler):
        """ Send movie data chunk by chunk"""
        eve = self.eve

        read_pointer = eve.rd32(eve.REG_MEDIAFIFO_READ)
        fullness = (self.write_pointer - read_pointer) % self.mf_size
        chunk_size = 2048
        while fullness < (self.mf_size - chunk_size):
            chunk = file_handler.read(chunk_size)
            if not chunk:
                return
            eve.write_mem(self.mf_base + self.write_pointer, chunk)
            self.write_pointer = (self.write_pointer + len(chunk)) % self.mf_size
            eve.wr32(eve.REG_MEDIAFIFO_WRITE, self.write_pointer)
            fullness += len(chunk)
