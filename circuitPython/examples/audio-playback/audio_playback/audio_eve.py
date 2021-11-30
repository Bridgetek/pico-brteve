import time

from .tags import *
from .helper_gesture import helper_gesture

import sys
if sys.implementation.name == "circuitpython":
    from brteve.brt_eve_bt817_8 import BrtEve
else:
    from ....lib.brteve.brt_eve_bt817_8 import BrtEve

_state_play = 0
_state_stop = 1
_state_pause = 2

class audio_eve():
    def __init__(self, eve, mediafifo_start, mediafifo_len) -> None:
        self._state = _state_stop
        self.mediafifo_start = mediafifo_start
        self.mediafifo_len = mediafifo_len
        self.vol = 0
        self.eve = eve
        self.eof = 0

        self.fifo_wp = 0      

    def fifo_free(self):
        eve = self.eve
        read_ptr = eve.rd32(eve.REG_PLAYBACK_READPTR) % self.mediafifo_len
        
        if read_ptr > self.fifo_wp:
            size =  read_ptr - self.fifo_wp
        else:
            size = self.mediafifo_len + (read_ptr - self.fifo_wp)
        
        return size

    def flush_audio(self, file_handler, callback):
        eve = self.eve
        chunk_size = 1024 * 4
        free_fifo = self.fifo_free()

        # wait ulti fifo free
        while free_fifo < chunk_size:
            free_fifo = self.fifo_free()
            #print('free_fifo=', free_fifo, 'ptr=', eve.rd32(eve.REG_PLAYBACK_READPTR) )
            time.sleep(0.01)

        # Flush new data, from address self.fifo_wp
        count = 0
        #print('flush ', free_fifo, ' bytes to ', self.fifo_wp, '---------------------------')
        remain = free_fifo
        while(count < free_fifo):
            read_num = chunk_size if chunk_size < remain else remain
            fifo_end_ptr = self.mediafifo_len - self.fifo_wp
            read_num = fifo_end_ptr if fifo_end_ptr < read_num else read_num

            buffer = file_handler.read(read_num)
            give_num = len(buffer)
            eve.write_mem(self.mediafifo_start + self.fifo_wp, buffer)
            #print('flush ', give_num, ' bytes to ', self.fifo_wp)

            count += give_num
            remain -= give_num
            self.fifo_wp += give_num
            self.fifo_wp %= self.mediafifo_len

            if give_num != read_num: #end of file
                self.eof = 1
                return 1
            callback(file_handler.tell())

        return 0

    def is_playing(self):
        return self._state == _state_play

    def is_ready(self):
        return self._state == _state_stop

    def is_eof(self):
        return self.eof == 1

    def play(self, callback, file, freq, sampling):
        eve = self.eve
        if self._state == _state_play:
            return self.pause()
        if self._state == _state_pause:
            return self.resume()

        """ Start playback a new song"""
        self.fifo_wp = (eve.rd32(eve.REG_PLAYBACK_READPTR) + 4) % self.mediafifo_len
        self.eof = 0
        print('play')
        self._state = _state_play

        started = 0
        with open(file, 'rb') as f:
            ret = 0
            while ret == 0:
                while self._state == _state_pause: #callback function may modify this param
                    callback(f.tell())
                    time.sleep(0.001)
                
                if self._state == _state_stop:
                    return

                ret |= self.flush_audio(f, callback)
                if started == 0:
                    started = 1
                    eve.wr32(eve.REG_VOL_PB, 0);
                    eve.wr32(eve.REG_PLAYBACK_START, self.mediafifo_start) #Audio playback start address
                    eve.wr32(eve.REG_PLAYBACK_LENGTH, self.mediafifo_len) #Length of raw data buffer in bytes
                    eve.wr32(eve.REG_PLAYBACK_FREQ, freq) #Frequency
                    eve.wr32(eve.REG_PLAYBACK_FORMAT, sampling) #Current sampling frequency
                    eve.wr32(eve.REG_PLAYBACK_LOOP, 1);
                    eve.wr32(eve.REG_PLAYBACK_PLAY, 1); 
                    time.sleep(0.3)
                    eve.wr8(eve.REG_GPIOX_DIR,0xfF)
                    eve.wr8(eve.REG_GPIOX,0xff) 
                    eve.wr32(eve.REG_VOL_PB, self.vol)

        eve.Nop()
        eve.finish()
        self._state == _state_stop
        eve.wr32(eve.REG_PLAYBACK_LOOP, 0);
        print("end of file")
        
    def pause(self):
        print("pause")
        if self._state == _state_play:
            self._state = _state_pause
            self.eve.wr32(self.eve.REG_PLAYBACK_PAUSE, 1);
            self.eve.flush()

    def resume(self):
        print("resume")
        if self._state == _state_pause:
            self._state = _state_play
            self.eve.wr32(self.eve.REG_PLAYBACK_PAUSE, 0);
            self.eve.flush()

    def stop(self):
        eve=self.eve
        self.eof = 0
        print("stop")
        vol = self.vol
        while vol > 0:
            vol-=1
            self.eve.wr32(self.eve.REG_VOL_PB, vol)
            time.sleep(0.001)
        eve.wr8(eve.REG_GPIOX_DIR,0xf0)
        eve.wr8(eve.REG_GPIOX,0xf0) 

        self.eve.wr32(self.eve.REG_VOL_PB, 0)
        eve.wr32(eve.REG_PLAYBACK_LOOP, 0);
        eve.wr32(eve.REG_PLAYBACK_LENGTH, 0);
        eve.cmd_memset(self.mediafifo_start, 0xFF, self.mediafifo_len)
        eve.Nop()
        eve.finish()
        
        self._state = _state_stop
        self.fifo_wp = 0
    
    @property
    def is_end(self):
        return self._state == _state_play and self.eve.rd32(self.eve.REG_PLAYBACK_PLAY) == 0

    def set_volume(self, vol):
        vol = vol % 256
        self.vol = vol
        self.eve.wr32(self.eve.REG_VOL_PB, vol)

    def jump_to(self, time):
        print("Function is not implemented")

    def fast_forward(self):
        print("Function is not implemented")

    def fast_backward(self):
        print("Function is not implemented")

        