import time
from brteve.brt_eve_bt817_8 import BrtEve

MIN_MOVE         = 15
SWIPE_LIMIT      = 5

# return value is the number of milliseconds that have elapsed since the system was started.
def milis():
    return round(time.monotonic_ns() / 1000_000)

class Gesture_Touch():
    tagTrackTouched = 0
    tagPressed      = 0
    tagReleased     = 0
    isTouch         = 0
    isSwipe         = 0
    touchX          = 0
    touchY          = 0
    scroll_y        = 0
    scroll_x        = 0

class gesture():
    sTag     =0
    sIsTouch =0
    sTouchX  =0
    sTouchY  =0
    SWIPE_LEFT=1
    SWIPE_RIGHT=2
    SWIPE_DOWN=3
    SWIPE_UP=4   
    sIsSwipe = 0 # SWIPE_LEFT.SWIPE_RIGHT
    def __init__(self, eve: BrtEve):
        self.eve = eve
        self._gesture = Gesture_Touch()
        self._last_y = 0
        self._last_x = 0
        self._last_is_swipe = 0
        self._last_tag = 0
        self._count_swipe = 0
        self.prev_time = 0
        self._delay = 0

    def _tag_release(self, new_tag):
        tag_released = 0
        if (new_tag == 0 and self._last_tag != new_tag):
            tag_released = self._last_tag
            self._last_tag = 0
        elif new_tag != 0:
            self._last_tag = new_tag
        return tag_released

    def is_swipe(self, new_x, new_y):
        time = milis()
        if (self.prev_time != 0):
            self._delay += (time - self.prev_time)
        self.prev_time = time
        if self._delay < 30:
            return self._last_is_swipe
        self._delay = 0
        if new_x != 32768:
            distance_x = new_x - self._last_x
            self._last_x  = new_x
            if abs(distance_x) > MIN_MOVE:
                if self._count_swipe >= SWIPE_LIMIT:
                    if (distance_x>0):self._last_is_swipe=self.SWIPE_RIGHT  
                    else: self._last_is_swipe=self.SWIPE_LEFT
                    self.sIsSwipe=self._last_is_swipe
                    self._count_swipe = 0
                    return self._last_is_swipe
                else:
                    self._count_swipe +=1
            else:
                self._count_swipe = 0
        if new_y != 32768:
            distance_y = new_y - self._last_y
            self._last_y  = new_y
            if abs(distance_y) > MIN_MOVE:
                if self._count_swipe_y >= SWIPE_LIMIT:
                    if (distance_y>0):self._last_is_swipe=self.SWIPE_DOWN
                    else : self._last_is_swipe=self.SWIPE_UP
                    self.sIsSwipe=self._last_is_swipe
                    self._count_swipe_y = 0                  
                    return self._last_is_swipe
                else:
                    self._count_swipe_y +=1
            else:
                self._count_swipe_y = 0
        else:
            self._count_swipe_y = 0
        self._last_is_swipe = 0
        self.sIsSwipe=self._last_is_swipe
        return 0

    def renew(self):
        eve = self.eve
        tag = eve.rd32(eve.REG_TOUCH_TAG) & 0xFF
        is_touch = eve.rd32(eve.REG_TOUCH_RAW_XY) != 0xFFFFFFFF
        touch_x = eve.rd16(eve.REG_TOUCH_SCREEN_XY + 2)
        touch_y = eve.rd16(eve.REG_TOUCH_SCREEN_XY + 4)
        self._gesture.tagTrackTouched = eve.rd32(eve.REG_TRACKER)

        self._gesture.isTouch = is_touch
        self._gesture.isSwipe = self.is_swipe(touch_x, touch_y)

        self._gesture.tagPressed = tag
        self._gesture.tagReleased = self._tag_release(tag)
        self._gesture.touchX = touch_x
        self._gesture.touchY = touch_y
        return self._gesture

    def get(self):
        return self._gesture


