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
    sIsSwipe = 0
    sMinMove = 15

    Read_tag = 0
    temp_tag = 0
    ret_tag = 0
    sk = 0

    def tag_release(self):
        newTag = 0
        tagRet = 0

        newTag = self.sTag

        if (newTag == 0 and self.oldtag != newTag):
            tagRet = self.oldtag
            self.oldtag = 0
        elif newTag != 0:
            self.oldtag = newTag
        return tagRet

    def renew(self, eve: BrtEve):
        self.sTag = eve.rd32(eve.REG_TOUCH_TAG) & 0xFF
        self.sIsTouch = eve.rd32(eve.REG_TOUCH_RAW_XY) != 0xFFFFFFFF
        self.sTouchX = eve.rd16(eve.REG_TOUCH_SCREEN_XY + 2)
        self.sTouchY = eve.rd16(eve.REG_TOUCH_SCREEN_XY + 4)

        self.sGesture.tagTrackTouched = eve.rd32(eve.REG_TRACKER)
        self.sGesture.isTouch = self.sIsTouch
        self.sGesture.tagPressed = self.sTag
        self.sGesture.isSwipe = self.sIsSwipe
        self.sGesture.tagReleased = self.tag_release()
        self.sGesture.touchX = self.sTouchX
        self.sGesture.touchY = self.sTouchY
