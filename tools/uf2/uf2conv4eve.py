#!/usr/bin/env python3
import sys
import struct
import subprocess
import re
import os
import time
import os.path
import argparse
from tqdm import tqdm

# Derived from https://raw.githubusercontent.com/microsoft/uf2/master/utils/uf2conv.py under MIT license
# Modifications:
# - Support EVE only
# - Combine BT815 and BT817 firmware alongside data in the same file

UF2_MAGIC_START0 = 0x0A324655 # "UF2\n"
UF2_MAGIC_START1 = 0x9E5D5157 # Randomly selected
UF2_MAGIC_END    = 0x0AB16F30 # Ditto

INFO_FILE = "/INFO_UF2.TXT"

UF2_FAMILY_ID_EVE3 = 0x1594d309 # BT815, BT816
UF2_FAMILY_ID_EVE4 = 0x9c217c82 # BT817, BT818

EVE_FLASH_FIRMWARE_SIZE = 4096

def is_uf2(buf):
    if len(buf) < 512:
        return False
    w = struct.unpack("<II", buf[0:8])
    return w[0] == UF2_MAGIC_START0 and w[1] == UF2_MAGIC_START1

def convert_from_uf2(buf):
    numblocks = len(buf) // 512
    curraddr = None
    outp = []
    for blockno in range(numblocks):
        ptr = blockno * 512
        block = buf[ptr:ptr + 512]
        hd = struct.unpack(b"<IIIIIIII", block[0:32])
        if hd[0] != UF2_MAGIC_START0 or hd[1] != UF2_MAGIC_START1:
            print("Skipping block at " + ptr + "; bad magic")
            continue
        if hd[2] & 1:
            # NO-flash flag set; skip block
            continue
        datalen = hd[4]
        if datalen > 476:
            assert False, "Invalid UF2 data size at " + ptr
        newaddr = hd[3]
        if curraddr == None:
            curraddr = newaddr
        padding = newaddr - curraddr
        if padding < 0:
            assert False, "Block out of order at " + ptr
        if padding > 10*1024*1024:
            assert False, "More than 10M of padding needed at " + ptr
        if padding % 4 != 0:
            assert False, "Non-word padding size at " + ptr
        while padding > 0:
            padding -= 4
            outp += b"\x00\x00\x00\x00"
        outp.append(block[32 : 32 + datalen])
        curraddr = newaddr + datalen
    return b"".join(outp)

def convert_to_carray(file_content):
    outp = "const unsigned long bindata_len = %d;\n" % len(file_content)
    outp += "const unsigned char bindata[] __attribute__((aligned(16))) = {"
    for i in range(len(file_content)):
        if i % 16 == 0:
            outp += "\n"
        outp += "0x%02x, " % file_content[i]
    outp += "\n};\n"
    return bytes(outp, "utf-8")

def convert_to_uf2(eve3_firmware, eve4_firmware, file_content):
    datapadding = b""
    while len(datapadding) < 512 - 256 - 32 - 4:
        datapadding += b"\x00\x00\x00\x00"
    numblocks_eve3 = (len(eve3_firmware) + 255) // 256
    numblocks_eve4 = (len(eve4_firmware) + 255) // 256
    numblocks_fw = max(numblocks_eve3, numblocks_eve4)
    numblocks_file = (((len(file_content)|4095)+1) - EVE_FLASH_FIRMWARE_SIZE + 255) // 256
    if numblocks_file < (EVE_FLASH_FIRMWARE_SIZE // 256):
        numblocks_file = 0
    numblocks = numblocks_fw + numblocks_file
    # print("Blocks: %i" % numblocks)
    outp = []
    interleave = numblocks_eve3 == numblocks_eve4
    for blockno in range(max(numblocks_eve3, numblocks_eve4)):
        if blockno < numblocks_eve3:
            ptr = 256 * blockno
            chunk = eve3_firmware[ptr:ptr + 256]
            flags = 0x2000
            hd = struct.pack(b"<IIIIIIII",
                UF2_MAGIC_START0, UF2_MAGIC_START1,
                flags, ptr, 256, blockno, numblocks, UF2_FAMILY_ID_EVE3)
            while len(chunk) < 256:
                chunk += b"\x00"
            block = hd + chunk + datapadding + struct.pack(b"<I", UF2_MAGIC_END)
            assert len(block) == 512
            outp.append(block)
        if blockno < numblocks_eve4:
            ptr = 256 * blockno
            chunk = eve4_firmware[ptr:ptr + 256]
            flags = 0x2000
            hd = struct.pack(b"<IIIIIIII",
                UF2_MAGIC_START0, UF2_MAGIC_START1,
                flags, ptr, 256, blockno, numblocks, UF2_FAMILY_ID_EVE4)
            while len(chunk) < 256:
                chunk += b"\x00"
            block = hd + chunk + datapadding + struct.pack(b"<I", UF2_MAGIC_END)
            assert len(block) == 512
            outp.append(block)
    for blockno in range(numblocks_file):
        ptr = (256 * blockno) + EVE_FLASH_FIRMWARE_SIZE
        chunk = file_content[ptr:ptr + 256]
        flags = 0x0
        hd = struct.pack(b"<IIIIIIII",
            UF2_MAGIC_START0, UF2_MAGIC_START1,
            flags, ptr, 256, numblocks_fw + blockno, numblocks, 0)
        while len(chunk) < 256:
            chunk += b"\xff"
        block = hd + chunk + datapadding + struct.pack(b"<I", UF2_MAGIC_END)
        assert len(block) == 512
        outp.append(block)
    return b"".join(outp)

def to_str(b):
    return b.decode("utf-8")

def get_drives_win32_wmic_portable():
    drives = []
    # command = wmic PATH Win32_LogicalDisk get DeviceID, VolumeName, FileSystem,  DriveType
    r = subprocess.check_output(["wmic\\wmic", "PATH", "Win32_LogicalDisk",
                                 "get", "DeviceID,", "VolumeName,",
                                 "FileSystem,", "DriveType"])
    for line in to_str(r).split('\n'):
        words = re.split('\s+', line)
        if len(words) >= 3 and words[1] == "2" and words[2] == "FAT":
            drives.append(words[0])
    return drives

def get_drives_win32_wmic_system():
    drives = []
    # command = wmic PATH Win32_LogicalDisk get DeviceID, VolumeName, FileSystem,  DriveType
    r = subprocess.check_output(["wmic", "PATH", "Win32_LogicalDisk",
                                 "get", "DeviceID,", "VolumeName,",
                                 "FileSystem,", "DriveType"])
    for line in to_str(r).split('\n'):
        words = re.split('\s+', line)
        if len(words) >= 3 and words[1] == "2" and words[2] == "FAT":
            drives.append(words[0])
    return drives
    
def get_drives_win32_powershell():
    drives = []
    r = subprocess.check_output(["PowerShell ", "Get-CimInstance -ClassName Win32_LogicalDisk | Format-Table",
                                "DeviceID,DriveType,FileSystem,VolumeNam"])
    for line in to_str(r).split('\n'):
        words = re.split('\s+', line)
        if len(words) >= 3 and words[1] == "2" and words[2] == "FAT":
            drives.append(words[0])
    return drives
    
def get_drives():
    drives = []
    if sys.platform == "win32":
        try:
          drives = get_drives_win32_powershell()
        except:
          pass
        
        try:
            if not drives:
                drives = get_drives_win32_wmic_portable()
        except:
          pass
        
        try:
            if not drives:
                drives = get_drives_win32_wmic_system()
        except:
          pass
    else:
        rootpath = "/media"
        if sys.platform == "darwin":
            rootpath = "/Volumes"
        elif sys.platform == "linux":
            tmp = rootpath + "/" + os.environ["USER"]
            if os.path.isdir(tmp):
                rootpath = tmp
        for d in os.listdir(rootpath):
            drives.append(os.path.join(rootpath, d))

    def has_info(d):
        try:
            return os.path.isfile(d + INFO_FILE)
        except:
            return False

    return list(filter(has_info, drives))


def board_id(path):
    with open(path + INFO_FILE, mode='r') as file:
        file_content = file.read()
    return re.search("Board-ID: ([^\r\n]*)", file_content).group(1)

def eve_flash_status(path):
    with open(path + INFO_FILE, mode='r') as file:
        file_content = file.read()
    return re.search("X-Eve-Flash-Status: ([^\r\n]*)", file_content).group(1)
    
def is_rpi_drive(d):
    return board_id(d) == "RPI-RP2"

def get_rpi_drives():
    drives = get_drives()
    return list(filter(board_id, drives))

def is_eve_drive(d):
    id = board_id(d)
    return id.startswith("BT815-") or id.startswith("BT816-") or id.startswith("BT817-") or id.startswith("BT818-")

def get_eve_drives():
    drives = get_drives()
    return list(filter(is_eve_drive, drives))

def list_drives():
    for d in get_drives():
        if is_rpi_drive(d):
            ident = "(RP2040)"
        elif is_eve_drive(d):
            ident = "(EVE)"
        else:
            ident = "(Unknown)"
        print(d, board_id(d), ident)


def write_file(name, buf):
    with open(name, "wb") as f:
        for s in tqdm(range(0, len(buf), 8192), unit='kB', unit_divisor=1024, unit_scale=8):
            f.write(buf[s:s + 8192])
    # with open(name, "wb") as f:
    #     f.write(buf)
    print("Wrote %d bytes to %s" % (len(buf), name))

def pin_select(args):
    pins=[
        ["MISO", 4 ],
        ["CS"  , 5 ],
        ["SCK" , 2 ],
        ["MOSI", 3 ],
        ["INT" , 6 ],
        ["PWD" , 7 ],
        ["IO2" , 14],
        ["IO3" , 15],
    ]
    spliter='-'
    for arg in args[0]:
        pin_nth = 0 
        for i, pin in enumerate(pins):
            print(pin)
            pin_name_pos=arg.find(spliter)
            if arg[0:pin_name_pos].upper() ==  pin[0]:
                pins[i][1] = arg[pin_name_pos+1::]
            
    print("Setting SPI pin:", pins)
    
    with open(name, "wb") as f:
        f.write(buf[s:s + 8192])
    
def main():
    def error(msg):
        print(msg)
        sys.exit(1)
    parser = argparse.ArgumentParser(description='Convert to UF2 or flash directly.')
    parser.add_argument('-i' , '--input', metavar="INPUT", dest='input', type=str,
                        help='input file for data (BIN or UF2)')
    parser.add_argument('-f' , '--firmware', metavar=('EVE_GEN','BLOB'), dest='firmware', action='append', nargs='*',
                        help="include flash firmware blob; pass without parameters to include default blobs for all supported EVE generations")
    parser.add_argument('-o' , '--output', metavar="FILE", dest='output', type=str,
                        help='write output to named file; defaults to "flash.uf2" or "flash.bin" where sensible')
    parser.add_argument('-d' , '--device', dest="device_path",
                        help='select a device path to flash')
    parser.add_argument('-l' , '--list', action='store_true',
                        help='list connected devices')
    parser.add_argument('-c' , '--convert', action='store_true',
                        help='do not flash, just convert')
    parser.add_argument('-D' , '--deploy', action='store_true',
                        help='just flash, do not convert')
    parser.add_argument('-C' , '--carray', action='store_true',
                        help='convert binary file to a C array, not UF2')
    parser.add_argument('-w' , '--wait', action='store_true',
                        help='wait for original drive to come back up')
    parser.add_argument('-k' , '--keep', action='store_true',
                        help='keep the original firmware if flash is already in full mode (does not work if input is already in uf2)')
    parser.add_argument('-p' , '--pin', dest='pin', type=str, action='append', nargs='*',
                        help='Set the pin assignment of Eve SPI interface')
    args = parser.parse_args()

    if args.list:
        list_drives()
    else:
        inpbuf = []
        if args.input:
            with open(args.input, mode='rb') as f:
                inpbuf = f.read()
        eve3blob = []
        eve4blob = []
        if args.pin:
            pin_select(args.pin)
            
        if args.firmware:
            for fw in args.firmware:
                if len(fw) == 0:
                    if len(eve3blob) == 0:
                        print("Including default EVE3 firmware blob")
                        with open(os.path.dirname(os.path.realpath(__file__)) + "/BT815-unified.blob", mode='rb') as f:
                            eve3blob = f.read()
                    if len(eve4blob) == 0:
                        print("Including default EVE4 firmware blob")
                        with open(os.path.dirname(os.path.realpath(__file__)) + "/BT817-unified.blob", mode='rb') as f:
                            eve4blob = f.read()
                else:
                    evegen = fw[0].upper()
                    if evegen == "EVE3" or evegen == "BT815" or evegen == "BT816":
                        if len(fw) >= 2:
                            print("Including EVE3 firmware blob %s" % fw[1])
                            blobfile = fw[1]
                        else:
                            print("Including default EVE3 firmware blob")
                            blobfile = os.path.dirname(os.path.realpath(__file__)) + "/BT815-unified.blob"
                        if blobfile == "input":
                            eve3blob = inpbuf
                        else:
                            with open(blobfile, mode='rb') as f:
                                eve3blob = f.read()
                    elif evegen == "EVE4" or evegen == "BT817" or evegen == "BT818":
                        if len(fw) >= 2:
                            print("Including EVE4 firmware blob %s" % fw[1])
                            blobfile = fw[1]
                        else:
                            print("Including default EVE3 firmware blob")
                            blobfile = os.path.dirname(os.path.realpath(__file__)) + "/BT817-unified.blob"
                        if blobfile == "input":
                            eve4blob = inpbuf
                        else:
                            with open(blobfile, mode='rb') as f:
                                eve4blob = f.read()
                    else:
                        error("Invalid EVE generation for firmware")
        if len(inpbuf) == 0 and len(eve3blob) == 0 and len(eve4blob) == 0:
            error("Need input file")
        
        from_uf2 = is_uf2(inpbuf)
        ext = "uf2"
        if args.deploy:
            outbuf = inpbuf
            if args.keep:
                outbuf_nofw = []
        elif from_uf2:
            outbuf = convert_from_uf2(inpbuf)
            ext = "bin"
        elif args.carray:
            outbuf = convert_to_carray(inpbuf)
            ext = "h"
        else:
            outbuf = convert_to_uf2(eve3blob[0:EVE_FLASH_FIRMWARE_SIZE], eve4blob[0:EVE_FLASH_FIRMWARE_SIZE], inpbuf)
            if args.keep:
                outbuf_nofw = convert_to_uf2([], [], inpbuf)
        print("Converting to %s, output size: %d" %
              (ext, len(outbuf)))
        if args.convert or ext != "uf2":
            eve_drives = []
            rpi_drives = []
            if args.output == None:
                args.output = "flash." + ext
        else:
            rpi_drives = get_rpi_drives()
            eve_drives = get_eve_drives()

        if args.output:
            print("Save as %s" % args.output)
            write_file(args.output, outbuf)
        else:
            if len(rpi_drives) == 0 and len(eve_drives) == 0:
                error("No drive to deploy.")
        
        if len(rpi_drives) != 0 or len(rpi_drives) != 0:
            if len(eve_drives) == 0:
                with open(os.path.dirname(os.path.realpath(__file__)) + "/eve_flash_pico.uf2", mode='rb') as f:
                    picofw = f.read()
                for d in rpi_drives:
                    print("Launching %s (%s)" % (d, board_id(d)))
                    write_file(d + "/NEW.UF2", picofw)
            
            tries = 0
            while len(eve_drives) < len(rpi_drives) and tries < 100:
                time.sleep(0.05)
                eve_drives = get_eve_drives()
                tries += 1
                
            if tries == 100:
                error("Timeout")
            
            for d in eve_drives:
                if args.keep and eve_flash_status(d) == "FULL":
                    print("Flashing %s (%s), firmware will be skipped" % (d, board_id(d)))
                    if len(outbuf_nofw):
                        write_file(d + "/NEW.UF2", outbuf_nofw)
                    else:
                        print("Nothing happens")
                else:
                    print("Flashing %s (%s)" % (d, board_id(d)))
                    write_file(d + "/NEW.UF2", outbuf)
            
            if args.wait:
                rpi_drives = get_rpi_drives()
                tries = 0
                while len(rpi_drives) < len(eve_drives) and tries < 100:
                    time.sleep(0.05)
                    rpi_drives = get_rpi_drives()
                    tries += 1


if __name__ == "__main__":
    main()
