import time
from .tags import *
from .helper_gesture import helper_gesture
from brteve.brt_eve_rp2040 import BrtEveRP2040

_state_play = 0
_state_stop = 1
_state_pause = 2

EVE_CMD_FIFO_SIZE = ((4) * 1024)
EVE_CMD_FIFO_MASK = (EVE_CMD_FIFO_SIZE - 1)

class audio_eve():
    def __init__(self, eve, mediafifo_start, mediafifo_len) -> None:
        self._state = _state_stop
        self.mediafifo_start = mediafifo_start
        self.mediafifo_len = mediafifo_len
        self.vol = 0
        self.eve = eve
        self.eof = 0
        self.count=0
        self.start_ptr=0
        self.eve.storage.flash_state(eve.FLASH_STATUS_FULL)
        self.fifo_wp = 0    

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
            time.sleep(0.01)

        # Flush new data, from address self.fifo_wp
        count = 0
        remain = free_fifo
        while(count < free_fifo):
            read_num = chunk_size if chunk_size < remain else remain
            fifo_end_ptr = self.mediafifo_len - self.fifo_wp
            read_num = fifo_end_ptr if fifo_end_ptr < read_num else read_num

            buffer = file_handler.read(read_num)
            give_num = len(buffer)
            eve.write_mem(self.mediafifo_start + self.fifo_wp, buffer)
            count += give_num
            remain -= give_num
            self.fifo_wp += give_num
            self.fifo_wp %= self.mediafifo_len

            if give_num != read_num: #end of file
                self.eof = 1
                return 1
            callback(file_handler.tell())

        return 0


    def flash_flush_audio(self, addr, callback):
        eve = self.eve
        chunk_size = 1024 * 4

        remain = self.mediafifo_len - self.fifo_wp
        if (True):
            read_num = chunk_size if chunk_size < remain else remain
            fifo_end_ptr = self.mediafifo_len - self.fifo_wp
            read_num = fifo_end_ptr if fifo_end_ptr < read_num else read_num

            eve.cmd_flashread(self.mediafifo_start + self.fifo_wp,addr+self.count,read_num)
            give_num=read_num
            self.count += give_num
            remain -= give_num
            self.fifo_wp += give_num
            self.fifo_wp %= self.mediafifo_len
            self.wait_flush()
            read_ptr = eve.rd32(eve.REG_PLAYBACK_READPTR) & 0xFFFFF
            read_count=read_ptr-self.start_ptr
            #print('flush ', give_num, ' bytes to ', self.fifo_wp, self.count, read_count,self.mediafifo_len)
            if self.fifo_wp >= self.mediafifo_len or self.fifo_wp==0: #end of file
                self.eof = 1
                return 1
            if read_count<self.mediafifo_len:callback(read_count)

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
        
    def playfromFlash(self, callback, addr, wavlen,freq, sampling):
        eve = self.eve
        if self._state == _state_play:
            return self.pause()
        if self._state == _state_pause:
            return self.resume()

        self.mediafifo_start=1024*328
        self.mediafifo_len=wavlen
        """ Start playback a new song"""
        #self.fifo_wp = (eve.rd32(eve.REG_PLAYBACK_READPTR) + 4) % self.mediafifo_len
        self.fifo_wp = 0
        self.eof = 0
        print('play from flash' ,self.mediafifo_start,addr,wavlen,self.fifo_wp)
        self._state = _state_play

        started = 0
        eve.cmd_memset(self.mediafifo_start, 0xFF, self.mediafifo_len)
        READFLASH_FULL=False
        if READFLASH_FULL:
            eve.cmd_flashread(self.mediafifo_start,addr,wavlen)
            time.sleep(0.1) # how long it takes to transfer?
        if True:
            ret = 0
            self.count=0
            while ret == 0:
                while self._state == _state_pause: #callback function may modify this param
                    time.sleep(0.001)
                
                if self._state == _state_stop:
                    return

                if not READFLASH_FULL: ret |= self.flash_flush_audio(addr, callback)
                if started == 0:
                    started = 1
                    eve.wr32(eve.REG_VOL_PB, 0)
                    eve.wr32(eve.REG_PLAYBACK_START, self.mediafifo_start) #Audio playback start address
                    eve.wr32(eve.REG_PLAYBACK_LENGTH, self.mediafifo_len) #Length of raw data buffer in bytes
                    eve.wr32(eve.REG_PLAYBACK_FREQ, freq) #Frequency
                    eve.wr32(eve.REG_PLAYBACK_FORMAT, sampling) #Current sampling frequency
                    eve.wr32(eve.REG_PLAYBACK_LOOP, 1)
                    eve.wr32(eve.REG_PLAYBACK_PLAY, 1) 
                    time.sleep(0.3)
                    eve.wr8(eve.REG_GPIOX_DIR,0xfF)
                    eve.wr8(eve.REG_GPIOX,0xff) 
                    eve.wr32(eve.REG_VOL_PB, self.vol)
                    time.sleep(0.001)
                    self.start_ptr = eve.rd32(eve.REG_PLAYBACK_READPTR) & 0xFFFFF
                    #print('start_ptr' ,self.mediafifo_start,addr,wavlen,self.fifo_wp,self.start_ptr )
                    if READFLASH_FULL:break

        eve.Nop()
        eve.finish()
        self._state == _state_stop
        eve.wr32(eve.REG_PLAYBACK_LOOP, 0)
 
        i=0
        while  eve.rd32(eve.REG_PLAYBACK_PLAY) !=0  :
            read_ptr = eve.rd32(eve.REG_PLAYBACK_READPTR) & 0xFFFFF
            callback(read_ptr-self.start_ptr) 
            time.sleep(0.2)
            i+=1
            if self._state == _state_stop :
                break
        callback(wavlen)  

        print("end of file")
        self.eof=1

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

        eve.wr32(self.eve.REG_VOL_PB, 0)
        eve.wr32(eve.REG_PLAYBACK_LOOP, 0)
        eve.wr32(eve.REG_PLAYBACK_LENGTH, 0)
        eve.wr32(self.eve.REG_PLAYBACK_PAUSE, 0) #
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