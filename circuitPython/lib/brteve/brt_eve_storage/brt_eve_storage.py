""" EVE's connected flash and EVE's RAM_G data transfer library
    This library support to read/write and update blob binary to EVE
    Progress bar can show up while reading/writing flash

    Usage:
        - Reading flash with progress bar:
            read_flash_with_progressbar

        - Writing flash with progress bar:
            write_flash_with_progressbar

        - Reading flash:
            read_flash_to_file
            read_flash_via_ramg

        - Writing flash:
            flash_update_flash_from_ramg
            write_flash_via_ramg
            write_flash_via_fifo

        - Reading ram_g:
            read_ramg_to_file

        - Writing ram_g:
            write_ramg_n_bytes
            write_file_to_ramg

        Blod update:
            flash_write_blob_file
            flash_write_blob_default
            flash_update_blob_file

        Other:
            flash_size
            flash_state
            flash_erase
            flash_clearcache

"""

from ..brt_eve_common import const

def _align_mask(value, mask):
    """ Alignment mask """
    return ((value)+(mask))&~(mask)

def align_to(value, align):
    """ Alignment """
    align=round(align)
    value=round(value)
    return _align_mask(value, align - 1)

FREAD_BLOCK              = const(8 * 1024)
BLOBSIZE                 = const(4096)

FILE_BLOB_BT815          = "/lib/brteve/brt_eve_storage/BT815-unified.blob"
FILE_BLOB_BT817          = "/lib/brteve/brt_eve_storage/BT817-unified.blob"

EVE_CMD_FIFO_SIZE        = const(4 * 1024) # 4kB coprocessor FIFO size
FIFO_SIZE_MASK           = const(4*1024-1)
FIFO_BYTE_ALIGNMENT_MASK = const(4*1024-4+1)
FLASH_WRITE_ALIGN_BYTE   = const(256)
FLASH_UPDATE_ALIGN_BYTE  = const(4096)
FLASH_READ_ALIGN_BYTE    = const(64)

FLASH_CMD_SUCCESS     = 0
FLASH_CMD_UNSUCCESS   = 1
FLASH_CMD_ALIGNED_ERR = 2

SEEK_SET = 0
SEEK_CUR = 1
SEEK_END = 2

PROGESS_BAR_READ = 1
PROGESS_BAR_WRITE = 2

class BrtEveStorage(): # pylint: disable=too-many-public-methods
    """ EVE storage read/write helper class """
    def __init__(self, eve) -> None:
        self.eve = eve

    class _FlashProgressbar(): # pylint: disable=too-few-public-methods
        """ Internal Progress bar's global data """
        file=''
        file_name=''
        message=''
        read_or_write=''
        file_size=''
        byte_per_1percent=''
        addr=''
        sent=''
        file_handler = ''

    def flash_update_flash_from_ramg(self, dest_flash, src_ram, num):
        """ Writes the given data to flash.
            If the data matches the existing contents of flash, nothing is done.
            Otherwise the flash is erased in 4K units, and the data is written.

            :param dest_flash: Destination in flash memory. Must be 4096-byte aligned
            :param src_ram: source data in main memory. Must be 4-byte aligned
            :param num: number of bytes to write, should be multiple of 4096,
                        otherwise, dummy data will be padded
            :return: FLASH_CMD_SUCCESS on success
        """
        eve = self.eve
        last_chunk = (num % 4096) # must be multiple of 4096. Cut off the extended data */

        if (dest_flash % FLASH_UPDATE_ALIGN_BYTE != 0  or
            (src_ram % 4) != 0): # Check aligned address
            return FLASH_CMD_ALIGNED_ERR
        if num < FLASH_UPDATE_ALIGN_BYTE:
            eve.cmd_flashupdate(dest_flash, src_ram, FLASH_UPDATE_ALIGN_BYTE)
            eve.finish()
        elif last_chunk == 0: # num is multiple of 4k
            eve.cmd_flashupdate(dest_flash, src_ram, num)
            eve.finish()

        else: # num is not fit in multiple of 4k
            eve.cmd_flashupdate(dest_flash, src_ram, num - last_chunk)
            eve.finish()

            # 4k is quite big for allocating new stack/heap data.
            # So reuse the pointer and write dummy data to flash */
            eve.cmd_flashupdate(dest_flash + num - last_chunk,
                src_ram + num - last_chunk, FLASH_UPDATE_ALIGN_BYTE)
            eve.finish()
        return FLASH_CMD_SUCCESS

    def read_flash_via_ramg(self, dest_ram, src_flash, num):
        """ Read data from flash via RAM_G, the data size should not exceed RAM_G size (1mb)

            :param dest_flash: Destination in flash memory. Must be 4096-byte aligned
            :param src_ram: source data in main memory. Must be 4-byte aligned
            :param num: number of bytes to read
            :return: data buffer
        """
        eve = self.eve
        num = num - (num % 4) # Only read lesser or equal aligned bytes */
        # Check aligned address
        if (src_flash % FLASH_READ_ALIGN_BYTE != 0)  or  ((dest_ram % 4) != 0):
            return FLASH_CMD_ALIGNED_ERR

        eve.cmd_flashread(dest_ram, src_flash, num)
        eve.finish()

        return eve.read_mem(dest_ram, num)

    def flash_state(self, state = ''):
        """ Read EVE's connected flash state
            If state is set: write EVE's conencted flash state

            :param state: If state is set: write EVE's conencted flash state
            :return: FLASH_CMD_SUCCESS on success
        """
        eve = self.eve

        ret = FLASH_CMD_SUCCESS
        curr_flash_state = eve.rd8(eve.REG_FLASH_STATUS)
        ret_addr = 0
        if curr_flash_state == state:
            return ret
        if state == '':
            return curr_flash_state

        #Only handle if state is diff
        if eve.FLASH_STATUS_DETACHED == state:
            eve.cmd_flashdetach()
            eve.finish()
        elif eve.FLASH_STATUS_BASIC == state:
            if eve.FLASH_STATUS_FULL == curr_flash_state:
                while eve.FLASH_STATUS_DETACHED != eve.rd8(eve.REG_FLASH_STATUS):
                    eve.cmd_flashdetach()
                    eve.finish()
            eve.cmd_flashattach()
            eve.finish()
        elif eve.FLASH_STATUS_FULL == state:
            if eve.FLASH_STATUS_BASIC != curr_flash_state:
                while eve.FLASH_STATUS_BASIC != eve.rd8(eve.REG_FLASH_STATUS):
                    eve.cmd_flashattach()
                    eve.finish()
            eve.cmd_flashfast()
            eve.finish()

            # Read the return code in CMD_BUFFER */
            ret_addr = (eve.eve_write_pointer() - 4) & FIFO_SIZE_MASK
            ret = eve.rd32(eve.eve.RAM_CMD + ret_addr)
        else:
            ret = 0xffff
        return ret

    def flash_erase(self):
        """ Erase EVE's connected flash
            :return: FLASH_CMD_SUCCESS on successful
        """
        # Try switch full mode
        eve = self.eve
        eve.flash_state(eve.FLASH_STATUS_FULL)
        if eve.FLASH_STATUS_FULL != eve.rd8(eve.REG_FLASH_STATUS):
            # Try update blob and switch full mode again
            eve.flash_write_blob_default()
            eve.flash_state(eve.FLASH_STATUS_FULL)
            if eve.FLASH_STATUS_FULL != eve.rd8(eve.REG_FLASH_STATUS):
                return FLASH_CMD_UNSUCCESS
        # Erase the flash """
        eve.cmd_flasherase()
        eve.finish()
        return FLASH_CMD_SUCCESS

    def flash_clearcache(self):
        """ Clear EVE's connected flash cache
            :return: FLASH_CMD_SUCCESS on successful
        """
        eve = self.eve
        eve.cmd_dlstart()
        eve.cmd_swap()
        eve.finish()

        eve.cmd_dlstart()
        eve.cmd_swap()
        eve.finish()

        eve.cmd_clearcache()
        eve.finish()
        return FLASH_CMD_SUCCESS

    def flash_update_blob_file(self, blob_file) :
        """ Write blob file to EVE's connected flash
            When updating a flash image, the following steps are to be expected:
                - Reading the first 4096 bytes in flash image into RAMG and update the blob first
                    using cmd_flashupdate in Basic mode.
                - Send the command cmd_flashfast to enter into fast mode.
                - If success, program the rest data of flash image by cmd_flashupdate
                - If not success, write a default blob. param blob data buffer

            :param blob_file: file blob to update
            :return: FLASH_CMD_SUCCESS on successful
        """
        eve = self.eve

        with open(blob_file, "rb") as file_hanfler:
            self.flash_state(eve.FLASH_STATUS_BASIC) # basic mode
            eve.write_mem(eve.RAM_G, file_hanfler.read(4096))
            eve.finish()

        self.flash_update_flash_from_ramg(0, eve.RAM_G, BLOBSIZE)
        eve.finish()

        ret = self.flash_state(eve.FLASH_STATUS_FULL) # fast mode
        if ret == FLASH_CMD_SUCCESS:
            print("Blob updated successful\n")
            return FLASH_CMD_SUCCESS

        print("Failed to update Blob, error code: ", hex(ret))
        return FLASH_CMD_UNSUCCESS

    def flash_write_blob_file(self, blob_file) :
        """ Try to write blob file to EVE's connected flash, if unccessful, write default blob file
            :param blob_file: Blob file address
            :return: FLASH_CMD_SUCCESS on successful
        """
        print("Writing blob from file", blob_file)
        ret = self.flash_update_blob_file(blob_file)

        #/ fail to default Blob
        if FLASH_CMD_UNSUCCESS == ret:
            return self.flash_write_blob_default()

        return FLASH_CMD_SUCCESS

    def flash_write_blob_default(self):
        """ Write default blob file to flash
            :return: FLASH_CMD_SUCCESS on successful
        """
        file_blob = FILE_BLOB_BT815
        if self.eve.eve_type == 'bt817_8':
            file_blob = FILE_BLOB_BT817
        print("Writing default blob file", file_blob)

        return self.flash_update_blob_file(file_blob)

    def progress_bar_init(self, file, addr, read_or_write) :
        """ Progress bar initialization for EVE's connected flash reading/writing

            :param file: Path to the file
            :param addr: Address on flash
            :param read_or_write: PROGESS_BAR_READ or PROGESS_BAR_WRITE
            :return: new _FlashProgressbar object
        """
        progress = self._FlashProgressbar()
        file_size = 0

        progress.sent = 0
        progress.addr = addr

        progress.read_or_write = read_or_write
        progress.file = file
        progress.file_name = file.split("/")[-1].split("\\")[-1]

        progress.file_handler = open(file, "rb") # pylint: disable=consider-using-with
        progress.file_handler.seek(0, SEEK_END)
        file_size = progress.file_handler.tell()
        progress.file_handler.seek(0, SEEK_SET)
        progress.file_handler.close()

        if file_size > 16*1024*1024:
            print("File size is too big, max 16Mb")
            return 0

        if read_or_write == PROGESS_BAR_READ:
            if file_size <= 0:
                print("Unable to open file: ", file)
                return 0

            progress.message = "Reading " + progress.file_name + " from flash"
        else :
            # update blob from file first
            status = 0
            if addr == 0:
                status = self.flash_write_blob_file(file)
            else :
                status = self.flash_write_blob_default()

            if status == FLASH_CMD_UNSUCCESS:
                print("Blob update failed, stop file transfer")
                return 0

            if file_size <= 0:
                print("Unable to open file:", file)
                return 0

            # Jump to real data
            if addr == 0:
                progress.sent = progress.addr = BLOBSIZE
            elif addr >= 4096:
                progress.sent = progress.addr = 0
            else:
                print("Invalid flash address, must be 0 (blob update), or >=4096 (data raw)")
                return 0

            progress.file_handler = open(file, "rb") # pylint: disable=consider-using-with
            progress.file_handler.seek(progress.addr)
            progress.message = "Writing " + progress.file_name + " to flash"
            progress.file_size = file_size

        #/ Destination address in flash memory must be 4096-byte aligned
        progress.byte_per_1percent = align_to(file_size / 100, 4096)

        progress.byte_per_1percent = max(progress.byte_per_1percent, FREAD_BLOCK)
        return progress

    def progress_bar_write_chunk(self, progress: _FlashProgressbar) :
        """ Write a block data of file to flash
            :param progress: _FlashProgressbar struct
            :return: Percent of data transfered, 100 mean file transfer is done
        """
        eve = self.eve
        sent = 0
        ramg_sent = 0
        block_size = 0

        # Tranfer 1 percent of file
        while (progress.sent < progress.file_size and sent < progress.byte_per_1percent) :
            block_size = progress.byte_per_1percent\
                if FREAD_BLOCK > progress.byte_per_1percent else FREAD_BLOCK

            # Tranfer to eve.RAM_g
            while (ramg_sent < eve.RAM_G_SIZE
                   and progress.sent < progress.file_size
                   and sent < progress.byte_per_1percent) :
                data = progress.file_handler.read(block_size)
                byte_num = len(data)
                if byte_num == 0:
                    print("Error on reading file:", progress.file)
                    return 0

                eve.write_mem(ramg_sent, data)
                eve.finish()
                ramg_sent += byte_num
                sent += byte_num
                progress.sent += byte_num

            # Update flash from eve.RAM_g
            ramg_sent = (ramg_sent + 4095) & (~4095)#to ensure 4KB alignment
            self.flash_update_flash_from_ramg(progress.addr, 0, ramg_sent)
            progress.addr += ramg_sent

        return progress.sent * 100 / progress.file_size # Percent """

    def progress_bar_read_chunk(self, progress: _FlashProgressbar) :
        """ Read a block data from flash to file
            :param progress: _FlashProgressbar struct
            :return: Percent of data received, 100 mean data transfer is done
        """
        eve = self.eve
        buffer_size = FREAD_BLOCK
        sent = 0
        block_size = 0
        gramaddr = eve.RAM_G

        while (progress.sent < progress.file_size and sent < progress.byte_per_1percent) :
            chunk_size = progress.byte_per_1percent\
                if buffer_size > progress.byte_per_1percent else buffer_size

            if (progress.file_size - progress.sent) < chunk_size:
                chunk_size = progress.file_size - progress.sent

            # source address in flash memory Must be 64-byte aligned.
            block_size = align_to(chunk_size, 64)

            fret = self.read_flash_via_ramg(gramaddr, progress.addr, block_size)

            if len(fret) != block_size:
                print("Error when reading flash\n")
                return 0

            sent += block_size
            progress.sent += block_size
            progress.addr += block_size
            try:
                progress.file_handler.write(fret)
            except IOError as exception:
                print("Unable to write file: ", progress.file)
                print(exception)

        return (int)(progress.sent * 100 / progress.file_size) # Percent

    def progress_bar_ui_default(self, progress: _FlashProgressbar) :
        """ Default UI for the progress bar
            User may construct their own UI for the progress bar with
            - progress_bar_init and progress_bar_write_chunk
            or:
            - progress_bar_init and progress_bar_read_chunk

            :param progress: _FlashProgressbar struct
            :return: FLASH_CMD_SUCCESS on successful
        """
        eve = self.eve
        font = 29
        bar_range = 1000
        sent64 = progress.sent

        if progress.file_size == 0:
            return FLASH_CMD_UNSUCCESS

        opt = 0
        val = (int)(sent64 * 1000 / progress.file_size)

        bar_w = (int)(eve.lcd_width * 8 / 10)
        bar_h = (int)(eve.lcd_height * 1 / 10)
        bar_x = (int)((eve.lcd_width - bar_w) / 2)
        bar_y = (int)((eve.lcd_height - bar_h) / 2)

        eve.cmd_dlstart()
        eve.Clear(1, 1, 1)
        eve.ClearColorRGB(0, 0, 0)

        eve.cmd_text(bar_x, bar_y - 50, font, opt, progress.message)
        eve.cmd_progress(bar_x, bar_y, bar_w, bar_h, opt, val, bar_range)

        text = str(round(val * 100 / bar_range, 2)) + '%'
        eve.ColorRGB(0, 200, 0)
        eve.cmd_text(bar_x + bar_w / 2, bar_y + 5, font, opt, text)
        eve.ColorRGB(255, 255, 255)

        eve.Display()
        eve.cmd_swap()
        eve.finish()

        return FLASH_CMD_SUCCESS

    def write_flash_with_progressbar(self, file, address) :
        """ Write file to flash and show a default progress bar on LCD

            :param file: File to transfer
            :param address: Address on flash
            :return: Number of bytes transfered
        """
        progress = self.progress_bar_init(file, address, PROGESS_BAR_WRITE)
        if progress == 0:
            return 0 # Error

        while 1:
            percent = self.progress_bar_write_chunk(progress)
            self.progress_bar_ui_default(progress)

            if percent >= 100:
                break

        progress.file_handler.close()
        return progress.file_size

    def read_flash_with_progressbar(self, file, address, size) :
        """ Read file from flash and show default progress bar on LCD
            :param file: File to write
            :param address: Address on flash
            :return: Number of bytes received
        """
        progress = self.progress_bar_init(file, address, PROGESS_BAR_READ)
        progress.file_size = size

        while 1:
            percent = self.progress_bar_read_chunk(progress)
            self.progress_bar_ui_default(progress)
            if percent >= 100:
                break

        progress.file_handler.close()
        return progress.file_size


    def write_flash_via_fifo(self, file, addr, is_erase) :
        """ Write file to flash via CMD_FLASHWRITE
            :param file: File to write
            :param addr: Address on flash
            :param is_erase: set to True to erase flash before write data
            :return: Number of bytes transfered on successful
        """
        eve=self.eve
        file_size = 0
        blocklen = 0

        #Erase Flash
        if is_erase and FLASH_CMD_UNSUCCESS == self.flash_erase():
            return 0 # Error

        # Check flash status
        self.flash_state(eve.FLASH_STATUS_FULL)
        if eve.FLASH_STATUS_FULL != eve.rd8(eve.REG_FLASH_STATUS):
            print("Switch flash to fullmode failed")
            return 0 # Error

        try:
            with open(file, "rb") as file_handler:
                eve.wr32(0x45) # send CMD_FLASHWRITE
                eve.wr32(addr)
                eve.wr32(align_to(file_size, FLASH_WRITE_ALIGN_BYTE))

                #/ Download data to Coprocessor
                data = file_handler.read(EVE_CMD_FIFO_SIZE)
                blocklen = len(data)
                while blocklen > 0:
                    byte_num = align_to(blocklen, FLASH_WRITE_ALIGN_BYTE)
                    eve.write_mem(data, byte_num)
                    data = file_handler.read(EVE_CMD_FIFO_SIZE)
                    blocklen = len(data)
        except OSError as exception:
            print("Unable to open file: ", file)
            print(exception)
            return 0 # Error

        return file_size # File size

    def write_flash_via_ramg(self, file, addr):
        """ Write file to flash via RAM_G
            :param file: File to write
            :param addr: Address on flash
            :return: Number of bytes transfered on successful
        """
        eve = self.eve
        buffer_size = FREAD_BLOCK
        sent = 0
        ramg_sent = 0

        # update blob from file first
        if addr < BLOBSIZE:
            self.flash_write_blob_file(file)
        else :
            #/ check and write blob
            ret = self.flash_state(eve.FLASH_STATUS_FULL) # full mode
            if ret != 0:
                self.flash_write_blob_default()

                ret = self.flash_state(eve.FLASH_STATUS_FULL) # full mode
                if ret != 0:
                    print("Cannot switch flash to fullmode\n")
                    return 0 # Error

        try:
            with open(file, "rb") as file_handler:
                file_handler.seek(0, SEEK_END)
                file_size = file_handler.tell()
                file_handler.seek(0, SEEK_SET)

                # Ignore Blob data part of file
                if addr < BLOBSIZE:
                    sent = addr = BLOBSIZE
                    file_handler.seek(addr)

                #/ Transfer rest of file to EVE (in fast mode)
                while sent < file_size:
                    # Fill up RAMG
                    while (ramg_sent < eve.RAM_G_SIZE and sent < file_size) :
                        data = file_handler.read(buffer_size)
                        blocklen = len(data)
                        if blocklen == 0:
                            print("Error on reading file:", file)
                            return 0 # Error

                        eve.write_mem(ramg_sent, data, blocklen)

                        ramg_sent += blocklen
                        sent += blocklen

                    # Send RAMG to Flash
                    ramg_sent = (ramg_sent + 4095) & (~4095)#to ensure 4KB alignment
                    self.flash_update_flash_from_ramg(addr, 0, ramg_sent)
                    addr += ramg_sent
        except OSError as exception:
            print("Unable to open file: ", file)
            print(exception)
            return 0 # Error

        return sent # File size """

    def read_flash_to_file(self, file, address, size) :
        """ Read data on RAMG into a file
            :param file: Filename output
            :param address: Address on RAM_G
            :param size: Size to read
            :return: FLASH_CMD_SUCCESS on successful
        """
        gramaddr = 0
        block_size = 4 * 1024 #/ block_size must be less than RAMG size
        address = align_to(address, 64)
        size = align_to(size, 4)

        try:
            with open(file, "rb") as file_handler:
                sent = 0
                while size > 0:
                    blocklen = size if block_size > size else block_size
                    buff = self.read_flash_via_ramg(gramaddr, address, blocklen)
                    size -= blocklen
                    sent += blocklen
                    address += blocklen

                    try:
                        file_handler.write(buff, blocklen)
                    except IOError as exception:
                        print("Unable to write file: ", file)
                        print(exception)
                        return FLASH_CMD_UNSUCCESS
        except OSError as exception:
            print("Unable to open file: ", file)
            print(exception)
            return FLASH_CMD_UNSUCCESS

        return FLASH_CMD_SUCCESS

    def flash_size(self):
        """ Get flash size
            :return" Flash size in Mb
        """
        eve = self.eve
        self.flash_state(eve.FLASH_STATUS_BASIC)
        size = eve.rd32(eve.REG_FLASH_SIZE)
        return size

    def write_ramg_n_bytes(self, file, addr, nbytes, offset) :
        """ Transfer a file to RAMG
            :param file: File to transfer
            :param addr: Address on RAMG
            :param nbytes: number of byte to write
            :param offset: file offset
            :return: Number of bytes transfered
        """
        eve = self.eve
        buffer_size = FREAD_BLOCK
        sent = 0
        file_size = 0

        try:
            with open(file, "rb") as file_handler:
                file_handler.seek(0, SEEK_END)
                file_size = file_handler.tell()
                file_handler.seek(0, SEEK_SET)

                if nbytes == 0:
                    nbytes = file_size
                    offset = 0

                if offset:
                    file_handler.seek(offset)

                remain = nbytes
                while (file_size > 0 and sent < nbytes) :
                    blocklen = remain if buffer_size>remain else buffer_size
                    if (sent + blocklen) > nbytes:
                        blocklen = nbytes - sent
                    data = file_handler.read(blocklen)

                    eve.write_mem(addr, data)
                    file_size -= blocklen
                    remain -= blocklen
                    sent += blocklen
                    addr += blocklen
        except OSError as exception:
            print("Unable to open file: ", file)
            print(exception)
            return 0  # Error

        return sent

    def write_file_to_ramg(self, file, addr):
        """ Transfer a whole file to RAM_G
            :param file: File to transfer
            :param addr: Address on RAMG
            :return: Number of bytes transfered on successful
        """
        return self.write_ramg_n_bytes(file, addr, 0, 0)

    def read_ramg_to_file(self, file, address, size):
        """ Read data on RAMG into a file
            :param file: Filename output
            :param address: Address on RAMG
            :param size: Size to read
            :return: FLASH_CMD_SUCCESS on successful
        """
        eve = self.eve
        try:
            with open(file, "wb") as file_handler:
                block = 0
                offset = 0
                while size > 0:
                    block = FREAD_BLOCK if size > FREAD_BLOCK else size
                    data = eve.read_mem(address + offset, block)
                    offset += block
                    size -= block

                    try:
                        file_handler.write(data)
                    except IOError as exception:
                        print("Unable to write file: ", file)
                        print(exception)
                        return 0 # Error

        except OSError as exception:
            print("Unable to open file: ", file)
            print(exception)
            return 0 # Error

        return FLASH_CMD_SUCCESS
