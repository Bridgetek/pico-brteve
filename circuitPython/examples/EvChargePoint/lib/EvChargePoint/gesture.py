from bteve.eve import EVE



import sys




import bteve as eve
from .eve_helper import eve_helper


class Gesture_Touch():
	tagTrackTouched =0
	tagPressed      =0
	tagReleased     =0
	isTouch         =0
	isSwipe         =0
	touchX          =0
	touchY          =0
	velocityX       =0

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

    def tag_release():
        newTag = 0
        tagRet = 0

        newTag = gesture.sTag

        if (newTag == 0 and gesture.oldtag != newTag):
            tagRet = gesture.oldtag
            gesture.oldtag = 0
        elif (newTag != 0):
            gesture.oldtag = newTag
        return tagRet
        
    def renew(gd: Brt_PicoEve_Module, eve: eve):
        gesture.sTag = gd.rd32(eve.REG_TOUCH_TAG) & 0xFF
        gesture.sIsTouch = gd.rd32(eve.REG_TOUCH_RAW_XY) != 0xFFFFFFFF
        gesture.sTouchX = gd.rd16(eve.REG_TOUCH_SCREEN_XY + 2)
        gesture.sTouchY = gd.rd16(eve.REG_TOUCH_SCREEN_XY + 4)

        gesture.sGesture.tagTrackTouched = gd.rd32(eve.REG_TRACKER)
        gesture.sGesture.isTouch = gesture.sIsTouch
        gesture.sGesture.tagPressed = gesture.sTag
        gesture.sGesture.isSwipe = gesture.sIsSwipe
        gesture.sGesture.tagReleased = gesture.tag_release()
        gesture.sGesture.velocityX = 0
        gesture.sGesture.touchX = gesture.sTouchX
        gesture.sGesture.touchY = gesture.sTouchY
        
    def get():
        return gesture.sGesture