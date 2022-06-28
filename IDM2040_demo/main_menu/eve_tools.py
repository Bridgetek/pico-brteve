# Changed to use self lib brt_eve_rp2040_dmx.py instead of changing original   brt_eve_rp2040.py
import os
import board
import busio
import digitalio
import sdcardio # pylint: disable=import-error
import storage # pylint: disable=import-error

def snapshot2( eve,title):
    block=60   #  -- 96000
    #block=480 # --- 768000
    file="/sd/Snap565_"+title+"_"+str(block)+".raw"
    total=480/block
    #chunk_size=800*block*4  #RGBA
    block_size=800*block*2  #RGB565
    chunk_size=2048
    print("total" ,file,total ,chunk_size)
    with open(file, 'wb') as f:
        address = eve.RAM_G+(1024-128)*1024
        for i in range(0,total):
            #print("snapshotOne" ,i,block*i ,block_size)
            eve.cmd_snapshot2(eve.RGB565, address, 0, block*i, 800, block)  #RGB565
            eve.finish()
            readAdd=0
            while readAdd<block_size:
                leftSize=block_size-readAdd
                if (leftSize)>chunk_size:
                    buf=eve.read_mem(address+readAdd,chunk_size)
                else:
                    buf=eve.read_mem(address+readAdd,leftSize)
                readAdd+=chunk_size
                if not buf:
                    print("error snapshotOne" ,i,address)
                    return -1
                f.write(buf)
#         print("f.tell=", f.tell())
    print("snapshot2 finish",total*block_size)
