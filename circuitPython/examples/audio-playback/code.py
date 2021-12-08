from brteve.brt_eve_bt817_8 import BrtEve
from brteve.brt_eve_rp2040 import BrtEveRP2040

from audio_playback.audio_playback import audio_playback
from audio_playback.widgets import widgets_dialog_yes_no

host = BrtEveRP2040()
eve = BrtEve(host)
eve.init(resolution="1280x800", touch="goodix")

# Store calibration setting
eve.calibrate()
#eve.wr32(eve.REG_TOUCH_TRANSFORM_A, 0xfffefefc);
#eve.wr32(eve.REG_TOUCH_TRANSFORM_B, 0xfffffcbf);
#eve.wr32(eve.REG_TOUCH_TRANSFORM_C, 0x506adb4);
#eve.wr32(eve.REG_TOUCH_TRANSFORM_D, 0xfffffed1);
#eve.wr32(eve.REG_TOUCH_TRANSFORM_E, 0xfffefc79);
#eve.wr32(eve.REG_TOUCH_TRANSFORM_F, 0x32c3211);
#audio_playback(eve)

yes = widgets_dialog_yes_no(eve, "Preparing flash", 
    "Write BT81X_Flash.bin from sdcard to EVE's connected flash at first?", 120, False) == True
if yes == True:
    eve.storage.write_flash_with_progressbar('/sd/pico-brteve/circuitPython/examples/audio-playback/BT81X_Flash.bin', 0)
audio_playback(eve)

