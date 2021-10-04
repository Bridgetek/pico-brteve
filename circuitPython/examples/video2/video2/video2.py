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

def play_video_from_eve_flash(eve: BrtEve, file, blob=''):
    yes = widgets_dialog_yes_no(eve, "Preparing video", 
        "Write video from sdcard to EVE's connected flash at first?", 120, False) == True
    if yes == True:
        flash_video(eve, file, blob)

    mp = eve.movie_player_from_flash(4096)
    mp.play()
    
def play_video_from_pico_sdcard(eve: BrtEve, file):
    mp = eve.movie_player(file)
    mp.play()
        
def start(sdcard, eve):
    file_16mb = sdcard + 'pico-brteve/circuitPython/examples/video2/trailer_400p_20s.avi'

    while 1:
        yes = widgets_dialog_yes_no(eve, "Select source video", 
            "Play video from EVE's connected flash?", 10) == True
        if yes == True:
            play_video_from_eve_flash(eve, file_16mb)
            continue

        yes = widgets_dialog_yes_no(eve, "Select source video", 
            "Play video from PICO's connected sdcard?", 10) == True
        if yes == True:
            play_video_from_pico_sdcard(eve, file_16mb)
            continue
