from brteve.brt_eve_bt817_8 import BrtEve

class Gesture_Touch():
    tagTrackTouched =0
    tagPressed      =0
    tagReleased     =0
    isTouch         =0
    isSwipe         =0
    touchX          =0
    touchY          =0

class gesture():
    oldtag = 0
    sGesture = Gesture_Touch
    sTag     =0
    sIsTouch =0
    sTouchX  =0
    sTouchY  =0
    SWIPE_LEFT=1
    SWIPE_RIGHT=2
    sIsSwipe = 0 # SWIPE_LEFT.SWIPE_RIGHT

    sMinMove = 15

    Read_tag = 0
    temp_tag = 0
    ret_tag = 0
    sk = 0

    _delay = 0
    lastTouchX = 0
    prev_time = 0
    distance = 0
    dragprev = 0
    count1Touch = 0

    def __init__(self, eve: BrtEve) -> None:
        self.eve=eve

    def tag_release(self):
        newTag = 0
        tagRet = 0

        newTag = gesture.sTag

        if (newTag == 0 and gesture.oldtag != newTag):
            tagRet = gesture.oldtag
            gesture.oldtag = 0
        elif newTag != 0:
            gesture.oldtag = newTag
        return tagRet

    def renew(self):
        e=self.eve
        self.sTag = e.rd32(e.REG_TOUCH_TAG) & 0xFF
        self.sIsTouch = e.rd32(e.REG_TOUCH_RAW_XY) != 0xFFFFFFFF
        self.sTouchX = e.rd16(e.REG_TOUCH_SCREEN_XY + 2)
        self.sTouchY = e.rd16(e.REG_TOUCH_SCREEN_XY + 4)
        self.sGesture.tagTrackTouched = e.rd32(e.REG_TRACKER)

        self.sGesture.isTouch = self.sIsTouch
        self.sGesture.tagPressed = self.sTag
        self.sGesture.isSwipe = self.sIsSwipe
        self.sGesture.tagReleased = self.tag_release()
        self.sGesture.touchX = self.sTouchX
        self.sGesture.touchY = self.sTouchY

    def get(self):
        return self.sGesture
