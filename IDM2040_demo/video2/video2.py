import sys
from .widgets import widgets_dialog_yes_no

import sys
if sys.implementation.name == "circuitpython":
    from brteve.brt_eve_bt817_8 import BrtEve
else:
    from ....lib.brteve.brt_eve_bt817_8 import BrtEve

def flash_video(eve: BrtEve, file, blob=''):
    eve.storage.write_flash_with_progressbar(file, 4096)
    pass


def play(eve):
    """ Start playback a movie"""
    eve.wr8(eve.REG_GPIOX_DIR,0xFF)
    eve.wr8(eve.REG_GPIOX,0xFF)

    eve.wr8(eve.REG_VOL_PB, 0x20)
    eve.wr8(eve.REG_PLAY_CONTROL, 0x01)  # reset play
    print("play  " )

    flag = eve.OPT_FLASH | eve.OPT_FULLSCREEN | eve.OPT_NOTEAR | eve.OPT_SOUND
    eve.cmd_playvideo(flag)

    eve.Nop()
    finish(eve)
    eve.wr8(eve.REG_VOL_PB, 0x00)
    eve.wr8(eve.REG_VOL_SOUND, 0x00)
    eve.wr8(eve.REG_PLAY_CONTROL, 0xFF)  
    #-1  exit playback
    #0  pause playback
    #1  play normally

    eve.wr8(eve.REG_GPIOX_DIR,0xf0)
    eve.wr8(eve.REG_GPIOX,0xf0) 
    print("exit play ,eve.space", eve.space)


def finish(eve):
    """Flush command queue and wait until EVE is idle"""
    eve.flush()
    reserve(eve,eve.FIFO_MAX)

def reserve(eve, num):
    """Wait until command fifo have enough space or touch any place"""
    i=0
    while eve.space < num:
        #add some delay to detect touch event
        i+=1
        if (i>300):
            touch_x = eve.rd16(eve.REG_TOUCH_SCREEN_XY + 2)
            touch_y = eve.rd16(eve.REG_TOUCH_SCREEN_XY + 4)
            if touch_x != 32768:
                print("break touch_x",touch_x,i)
                break
            if touch_y != 32768:
                print("break touch_y",touch_y,i)
                break
        eve.getspace()

def play_video_from_eve_flash(eve: BrtEve, file, blob=''):
    # yes = widgets_dialog_yes_no(eve, "Preparing video", 
    #     "Write video from sdcard to EVE's connected flash at first?", 120, False) == True
    # if yes == True:
    #     flash_video(eve, file, blob)
    print("play_video_from_eve_flash")
    mp = eve.movie_player_from_flash(1149568)
    #mp = eve.movie_player_from_flash(1242432)
    #mp.play()
    play(eve)
    
def play_video_from_pico_sdcard(eve: BrtEve, file):
    mp = eve.movie_player(file)
    mp.play()
        
def start(sdcard, eve):
    file_16mb = sdcard + 'pico-brteve/circuitPython/examples/video2/trailer_400p_20s.avi'
    #file_16mb = sdcard + 'pico-brteve/circuitPython/examples/video2/351x272.avi'

    play_video_from_eve_flash(eve, file_16mb)
        # yes = widgets_dialog_yes_no(eve, "Select source video", 
        #     "Play video from EVE's connected flash?", 10) == True
        # if yes == True:
        #     play_video_from_eve_flash(eve, file_16mb)
        #     continue
# 
#         yes = widgets_dialog_yes_no(eve, "Select source video", 
#             "Play video from PICO's connected sdcard?", 10) == True
#         if yes == True:
#             play_video_from_pico_sdcard(eve, file_16mb)
#             continue
