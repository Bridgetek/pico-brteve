import time
import math
#import random
from random import randint
import json
from .helper import helper
from .gesture import gesture
from .datetime import hh_mm, hh_mm_ss_ms, milis, now, print_weekday, random
from .layout import layout
from .LDSBus_Sensor import LDSBus_Sensor
from .ui_config import ui_config
from .tags import *
from . import datetime
from .widgets import widgets_box, widgets_point

import sys
if sys.implementation.name == "circuitpython":
    from brteve.brt_eve_bt817_8 import BrtEve
else:
    from ....lib.brteve.brt_eve_bt817_8 import BrtEve

class ui_common(ui_config):
    data_gui=1
    def __init__(self, eve: BrtEve, helper: helper, gesture: gesture, layout: layout,LDSBus_Sensor:LDSBus_Sensor):
        super().__init__()
        self.eve = eve
        self.helper = helper
        self.gesture = gesture
        self.layout = layout
        self.LDSBus_Sensor = LDSBus_Sensor
        self.title=""
        self._clearData=True

        self.start_time_ns = 0
        self.hh=0
        self.mm=0

        self.temp_value=0        
        self.hr_value=0
        self.last_hr=0
        self.useGradient=0
        self._COLOR_GREEN=[0x90, 0xC8, 0x3A]
        self._COLOR_GRAY=[0x33, 0x33, 0x33]
        self._COLOR_WARNING=[0xD4, 0x21, 0x33]
        self._COLOR_YELLOW=[0xFF, 0xFF, 0x00]
        self.hData=2
        self.hBk=3
        self.hBk512=4
        self.useBlend=1
        self.boxW=272
        self.boxH=180
        self.last_timeout =  time.monotonic_ns() / 1000_000

 
    def interrupt(self):
        return 0

    def rotate_str_up(self, sss, angle, fontsize, opt, x, y):
        e=self.eve

        tan_a = math.tan(angle)

        char_w = fontsize/3
        char_h = fontsize/3
        space_x = char_w * 3 / 2
        space_y = char_h
        if angle > 45:
            space_x=space_y / tan_a
        elif angle < 45:
            space_y=tan_a * space_x

        _x=x
        _y=y
        for s in sss:
            _x=(int)(_x)
            _y=(int)(_y)
            e.cmd_text(_x, _y, fontsize, opt, s)
            _x+=space_x
            _y-=space_y

#temperature
    def Progress_box(self, x, y, w, h, border, title, unit, vmin, vmax, warning, value,scale=1):
        e=self.eve

        color = self._COLOR_GREEN
        if value > warning:
            color = self._COLOR_WARNING
        percent = value / vmax * 100

        if vmin > value:
            return

        if scale==1:
            self.box(x, y, w+20, h, border,0,scale)

        _padding = 20
        _x=x+_padding
        #_y = (y+h)/2 + 40
        _y = y+(h/2 )  #new
        _w= w-_padding-_padding
        _h = 20
        _midpoint=_w * percent / 100
        # left progress
        e.ColorRGB(color[0], color[1], color[2])
        e.Begin(e.LINES)
        e.LineWidth(_h)
        e.Vertex2f(_x, _y)
        e.Vertex2f(_x + _midpoint, _y)

        # right progress
        e.Begin(e.LINES)
        e.ColorRGB(self._COLOR_GRAY[0], self._COLOR_GRAY[1], self._COLOR_GRAY[2])
        e.LineWidth(_h)
        e.Vertex2f(_x + _midpoint, _y)
        e.Vertex2f(_x + _w, _y)

        # Circle point
        e.ColorRGB(color[0], color[1], color[2])
        e.Begin(e.POINTS)
        e.PointSize(35)
        e.Vertex2f(_x + _midpoint, _y)

        # Center text
        char_w_1 = 18
        char_w_2 = char_w_1 + 25
        e.ColorRGB(255, 255, 255)
        if scale==1:e.cmd_text(x + 3, y + 3, 21, 0, title)
        else:e.cmd_text(x + 3, y + 3, 31, 0, title)
        e.cmd_text(x + (w / 2), _y - 70, 31, e.OPT_RIGHTX, str(round(value, 1)))
        #e.cmd_text(x + (w / 2) + char_w_1, _y - 60, 18, e.OPT_RIGHTX, "o")
        e.cmd_text(x + (w / 2) + char_w_2, _y - 70, 31, e.OPT_RIGHTX, unit)

    def box(self, x, y, w, h, border,blend=0,scale=1):
        e = self.eve
        #e.ColorA(128)                          # 50% opacity  new 
        if scale==1:
            # big border
            e.ColorRGB(255, 255, 255)
            e.Begin(e.RECTS)
            e.LineWidth(10)
            e.Vertex2f(x, y)
            e.Vertex2f(x + w, y + h)

        # inside box
        e.ColorRGB(0, 0, 0)
        e.Begin(e.RECTS)
        e.LineWidth(10)
        e.Vertex2f(x + border, y + border)
        e.Vertex2f(x + w - border, y + h - border)        


#circle_box
    def circle_box(self,
            x, y, w, h, border, title, unit, vmin, vmax, lwarning, hwarning, value,tsize=29,scale=1):
        e=self.eve

        color = self._COLOR_GREEN
        if value > hwarning or value < lwarning:
            color = self._COLOR_WARNING
        percent = value / vmax * 100

        if scale==1:
            self.box(x, y, w+20, h, border,0,scale)

        # circle view
        _skip_range = 50 # degree
        _skip_low = (int)( _skip_range / 2) # degree
        _skip_high = (int)(360 - _skip_range / 2) # degree
        _max_degree = 360 - _skip_range
        degree = min(_max_degree * percent / 100, _max_degree)
        _padding = 25
        _size = 14
        _ROTATE = 90
        _paddingX=20
        x=x+_paddingX
        r = (h - _padding * 2) / 2

        _x = x + r + (w -r*2) / 2
        _y = y + r + _padding + 10

        c1=self._COLOR_GRAY
        c2=color

        e.ColorRGB(c1[0], c1[1], c1[2])
        e.StencilFunc(e.NEVER, 0x00, 0x00)
        e.StencilOp(e.INCR, e.INCR)
        e.Begin(e.POINTS)
        e.PointSize((r - _size) * 2)
        e.Vertex2f(_x, _y)

        e.StencilFunc(e.NOTEQUAL, 0x01, 0x01)
        e.PointSize( (r) * 2)
        e.Vertex2f(_x, _y)

        e.StencilFunc(e.EQUAL, 0x01, 0x01)
        e.StencilOp(e.KEEP, e.KEEP)

        # Black Circle part
        e.ColorRGB(0, 0, 0)
        e.PointSize(_size * 2)
        for num in list(range(0, _skip_low, 5)) + list(range(_skip_high, 360, 5)):
            num +=_ROTATE  #rotate circle by 90 degree and center down
            num %=360
            num=math.radians(num)
            posX = _x + (r- _size/2) * math.cos(num)
            posY = _y + (r- _size/2) * math.sin(num)
            e.Vertex2f(posX, posY)

        # last gray part
        e.ColorRGB(c1[0], c1[1], c1[2])
        e.PointSize(_size)
        for num in list(range(0, 20, 5)) + list(range(_max_degree - 20, _max_degree, 2)):
            num +=_ROTATE + _skip_low  + _size / 4 #rotate circle by 90 degree and center down
            num %=360
            num=math.radians(num)
            posX = _x + (r- _size/2) * math.cos(num)
            posY = _y + (r- _size/2) * math.sin(num)
            e.Vertex2f(posX, posY)

        # Green circle part
        e.ColorRGB(c2[0], c2[1], c2[2])
        e.PointSize(_size)
        #for num in range(0, degree, 5):
        for num in range(0, degree, 2): # new
            num +=_ROTATE + _skip_low  + _size / 4 #rotate circle by 90 degree and center down
            num %=360
            num=math.radians(num)
            posX = _x + (r - _size/2) * math.cos(num)
            posY = _y + (r - _size/2) * math.sin(num)
            e.Vertex2f(posX, posY)
        e.End()


        e.StencilFunc(e.ALWAYS, 0x00, 0xFF)
        e.StencilOp(e.KEEP, e.KEEP)

        e.ColorRGB(255, 255, 255)
        if scale==1:e.cmd_text(x + 3, y + 3, 21, 0, title)
        else:e.cmd_text(x + 3, y + 3, 31, 0, title)
        e.cmd_text(x + (w / 2), _y - 5, tsize, e.OPT_CENTER, str(round(value, 1)))
        e.cmd_text(x + (w / 2), _y + 25, 23, e.OPT_CENTER, unit)

        e.cmd_text(x + (w / 2) - (r/3), _y + r, 21, e.OPT_RIGHTX, str(vmin))
        e.cmd_text(x + (w / 2) + (r/3), _y + r, 21, 0           , str(vmax))


        # Warning line
        for i in lwarning, hwarning:
            if i in (0, vmax):
                continue # no warning
            percent = i / vmax * 100
            degree = min(_max_degree * percent / 100, _max_degree)
            num = degree + _ROTATE + _skip_low #rotate circle by 90 degree and center down
            num %=360
            num=math.radians(num)

            posX = _x + (r) * math.cos(num)
            posY = _y + (r) * math.sin(num)
            posX2 = _x + (r - _size) * math.cos(num)
            posY2 = _y + (r - _size) * math.sin(num)

            margin = _size/2
            if len(str(i)) > 3:
                margin /=4
                if posX > (x + w/2):
                    margin += 10
            textX = _x + (r + margin) * math.cos(num)
            textY = _y + (r + margin) * math.sin(num)

            opt = e.OPT_RIGHTX
            if posX> (x + w/2):
                opt = 0

            e.Begin(e.LINES)
            e.ColorRGB(0, 0, 0)
            e.LineWidth(1)  #new 3 -->1
            e.Vertex2f(posX, posY)
            e.Vertex2f(posX2, posY2)

            e.ColorRGB(255, 255, 255)
            e.cmd_text(textX, textY, 21, opt, str(round(i, 1)))

    def _time_str(self, hh, mm, isnext = 0) :
        """ isnext != 0: return hour:minute before hh:mm
        """

        prefix0='0'
        prefix_h=''
        prefix_m=''

        if isnext != 0:
            if mm == 0:
                hh -=1
                mm = 60

            mm = (mm - 1) %60
            hh = hh % 24

        hh_short = hh%12
        if hh == 12:
            hh_short = 12

        am="am"
        if hh >= 12:
            am="pm"

        if hh_short < 10:
            prefix_h=prefix0
        if mm < 10:
            prefix_m=prefix0

        if isnext == 0:
            return prefix_h+str(hh_short)+":"+prefix_m+str(mm) + "" + am
        return hh, mm, prefix_h+str(hh_short)+":"+prefix_m+str(mm) + "" + am

    def second_gaps(self):
        duration_ns = time.monotonic_ns() - self.start_time_ns
        duration_ss = duration_ns / 1e9
        return (int)(duration_ss % 60)

    def last_min(self, vmin):
        # duration from start time
        duration_ns = time.monotonic_ns() - self.start_time_ns
        duration_ss = duration_ns / 1e9
        duration_mm = duration_ss / 60
        duration_hh = duration_mm / 60

        vmin = min(vmin, duration_mm)

        rounded_vmin = (int)(vmin%60)
        rounded_vmin = max(rounded_vmin, 1) #atleast 1min humidity displayed on LCD

        # hour clock and minute clock until now
        now_hh = (int)( self.hh + duration_hh ) % 24
        now_mm = (int)( self.mm + duration_mm ) % 60

        return rounded_vmin, now_hh, now_mm


    def set_temp(self, value):
        self.temp_value = value
    def set_hr(self, value):
        self.hr_value = value
        self.last_hr = time.monotonic_ns()

    def statitics_box(self, x, y, w, h, border, data=bytearray(),scale=1,tvalue=0,MaxMin=2):
        """ data = [[0=(timestamp) time), 1=(int) value)], ...], data from last 10 minute
        """
        e = self.eve
        MAX_VALUE=100  #40-80
        MEASURE_MINUTE_MAX = MaxMin # QF 3-->6
        ROW_NUM =5 #6-->5
        ROW_LINE = ROW_NUM + 1
        PADDING_Y = PADDING_X = 30
        ANGLE = 35
        FONTSIZE = 16
        LINE_INTERVAL=20 # 10-->20

        num_mins, now_hh, now_mm =  MEASURE_MINUTE_MAX,0,0
        _second_gap=0
        ''' 
        num_mins, now_hh, now_mm = self.last_min(MEASURE_MINUTE_MAX)
        _second_gap = self.second_gaps() # no needed to be updated
        if _second_gap >= 0:
            num_mins +=1
        '''

        # box
        if scale==1:
            self.box(x, y, w+20, h, border,0,scale)
        else: self.box(x, y, w, h, border,0,scale)

        e.ColorRGB(255, 255, 255)
        if scale==1:e.cmd_text(x + 3, y + 3, 21, 0, "Humidity(%)")
        else: e.cmd_text(x + 3, y + 3, 28, 0, "Humidity(%)")

        # Rows
        row_offsetx = x + PADDING_X+5
        row_offsety = y + PADDING_Y
        #ROW_HEIGHT = (h/2) / ROW_NUM
        ROW_HEIGHT =(h-PADDING_Y*2) / ROW_NUM
        e.LineWidth(1)
        minY=0
        for i in range(ROW_NUM+1):
            e.Begin(e.LINES)
            _y=row_offsety + i*ROW_HEIGHT
            e.Vertex2f(row_offsetx     , _y)
            e.Vertex2f(x+w    , _y)
            if scale==1:e.cmd_text(x + 30 , _y - 5, 16, e.OPT_RIGHTX, str(MAX_VALUE - i*LINE_INTERVAL))
            else:e.cmd_text(x + 30 , _y - 5, 18, e.OPT_RIGHTX, str(MAX_VALUE - i*LINE_INTERVAL))
            minY=_y

        # Columns
        col_offset = row_offsetx 
        row_len = x + w - col_offset
        col_num = num_mins
        col_w = row_len / num_mins
        hh, mm = now_hh, now_mm
        _x_gaps = col_w * _second_gap / 60 # how much second passed from last minute?
        for i in range(col_num+1):
            e.Begin(e.LINES)
            _x= x + w - i*col_w - _x_gaps

            e.Vertex2f(_x, row_offsety)
            e.Vertex2f(_x, row_offsety + ROW_LINE * ROW_HEIGHT - ROW_HEIGHT)

            # _time_str = "19:20 am"
            #tm = self._time_str(hh, mm, 0)
            #if i != 0: hh, mm, tm = self._time_str(hh, mm, 1)
            #self.rotate_str_up(tm, ANGLE, FONTSIZE, 0, _x - 50, row_offsety + ROW_LINE * ROW_HEIGHT + 25)
            tm="00:%02d"%(MEASURE_MINUTE_MAX-i)
            if scale==1:e.cmd_text (_x - 20, row_offsety + (ROW_LINE) * ROW_HEIGHT -ROW_HEIGHT/2,FONTSIZE,0,tm)
            else:e.cmd_text (_x - 20, row_offsety + (ROW_LINE) * ROW_HEIGHT -ROW_HEIGHT*0.7,FONTSIZE+2,0,tm)

        # line strips
        #MAX_TIME = num_mins * 60 #second
        MAX_TIME = 120 #  euuql to 2HUMIDITY_MAX_SAMPLE
        W_PER_S = row_len / MAX_TIME # width pixel per second
        #H_PER_V = (ROW_HEIGHT * ROW_NUM) / (100-ROW_NUM*LINE_INTERVAL) # height pixel per second, range from 50 to 100
        H_PER_V = (ROW_HEIGHT * ROW_NUM) / (ROW_NUM*LINE_INTERVAL) # height pixel per second, range from 50 to 100
 
        if self.humidity_sample_num > 0:
            t0 = data[0][0]
            e.ColorRGB(self._COLOR_GREEN[0], self._COLOR_GREEN[1], self._COLOR_GREEN[2])
            e.Begin(e.LINE_STRIP)
            e.LineWidth(2)   # newValue  4-->2
            #print("humidity_sample_num",self.humidity_sample_num)
            for i in range(self.humidity_sample_num):
                d = data[i]
                t=d[0]
                v=d[1] - 0 # start from 40  -->0

                # skip if invalid data
                if v < 0 or v > 100:
                    continue

                #gap = t0 - t # gaps in second,but not accurate 
                gap = i   #now
                if gap > ((MEASURE_MINUTE_MAX + 1) * 60 + 10):
                    break

                _x = x + w - W_PER_S * gap
                #_y = y + PADDING_Y + ROW_HEIGHT*ROW_NUM - H_PER_V*v
                _y = y + PADDING_Y + ROW_HEIGHT*ROW_NUM - H_PER_V*v

                #print(minY,ROW_HEIGHT,H_PER_V,v,_y)
                if _x < row_offsetx:
                    break
                e.Vertex2f(_x, _y)
            e.End()
        if tvalue!=0:
            e.ColorRGB(self._COLOR_GREEN[0],self._COLOR_GREEN[1],self._COLOR_GREEN[2])
            if scale==1:e.cmd_text(x + w/2, y + 3, 21, 0, "%3.1f"%( tvalue) )
            else:e.cmd_text(x + w/2, y + 3, 28, 0, "%3.1f"%( tvalue) )
    def gradien_Scissor(self,x, y, w, h, border):
        eve = self.eve
        #print("gradien_Scissor",x,y,w,h)
        rounded = 1
        #boundary
        eve.SaveContext()
        eve.StencilFunc(eve.NEVER, 0x00, 0x00)
        eve.StencilOp(eve.INCR, eve.INCR)


        eve.ColorRGB(0x80, 0xFF, 0x40)
        eve.Begin(eve.RECTS)

        eve.LineWidth(rounded)
        eve.Vertex2f(x , y )
        eve.Vertex2f(x + w , y-h )

        
        eve.StencilFunc(eve.EQUAL, 0x01, 0x01)
        eve.StencilOp(eve.KEEP, eve.KEEP)
        eve.ScissorXY((int)(x), (int)(y-h))

        eve.ScissorSize(w, h)


        #eve.cmd_gradient(x, y, 0xaef078, x+w, y-h, 0xa83e32) #good
        eve.cmd_gradient(x, y, 0x100994, x+w, y-h, 0x6e3642)
        
        eve.RestoreContext()

        ''' 
        self.gradients = [
                    [0xa83e32, 0xaef078],
                    [0x09ad2f, 0x10170b],
                    [0x100994, 0x6e3642],
                    [0x6e0eeb, 0xfa6182],
                    [0xcc06c9, 0xcfde5f],
                    [0x3b8201, 0x27751b],
                ]
        '''
    def coordinateMarker(self,x,y,w,h,border,scale=1,blend=0,tvalue=0,MaxMin=2):
        eve = self.eve

        MEASURE_MINUTE_MAX = MaxMin # QF 3-->1
        MAX_VALUE=80 # 40-->80
        ROW_NUM = 5  #QF 5-->9
        ROW_LINE = ROW_NUM     #  ROW_NUM  1 ->ROW_NUM
        PADDING_Y = PADDING_X = 30
        ANGLE = 35
        FONTSIZE = 16

        #num_mins, now_hh, now_mm = self.last_min(MEASURE_MINUTE_MAX)
        num_mins, now_hh, now_mm = MEASURE_MINUTE_MAX,0,0

        _second_gap=0
        '''
        _second_gap = self.second_gaps() # no needed to be updated
        if _second_gap >= 0:
            num_mins +=1
        '''

        if border==1:
            # big border
            m=20
            eve.ColorRGB(255, 255, 255)
            eve.Begin(eve.LINES)
            eve.LineWidth(1)
            eve.Vertex2f(x , y )
            eve.Vertex2f(x+w+m, y )
            eve.Vertex2f(x+w+m , y )
            eve.Vertex2f(x+w+m , y+h )
            eve.Vertex2f(x+w+m , y+h )
            eve.Vertex2f(x , y+h )
            eve.Vertex2f(x , y+h )
            eve.Vertex2f(x , y  )    



        if (blend==1) and (self.useBlend==1 ):
            eve.BlendFunc(eve.SRC_ALPHA, eve.ZERO) # Reset blend ,new 


        eve.ColorRGB(255, 255, 255)
        if scale==1:
            eve.cmd_text(x + 3, y + 3, 21, 0, "Temperature(")
            eve.Vertex2ii(x + 90, y + 3,17,248) #dot
            eve.cmd_text(x + 95, y + 3, 21, 0, "C)")
#             eve.cmd_setfont2(1,0,32)
#             eve.cmd_text(x + 185, y + 3, 1, 0, "\x20")
#             eve.cmd_text(x + 200, y + 3, 21, 0, "()")
#             eve.Begin(eve.BITMAPS)

        else:
            eve.cmd_text(x + 3, y + 3, 28, 0, "Temperature(")
            eve.Begin(eve.BITMAPS)
            eve.Vertex2ii(x + 130, y + 3,17,248) #dot
            eve.cmd_text(x + 135, y + 3, 28, 0, "C)")

        # Rows
        #row_offsetx = x + PADDING_X
        if scale==1:row_offsetx = x + PADDING_X+5 
        else:row_offsetx = x + PADDING_X+37  

        row_offsety = y + PADDING_Y
        ROW_HEIGHT = (h-PADDING_Y*2) / ROW_NUM
        eve.LineWidth(1)

        row_offsetyMax=row_offsetx
        for i in range(ROW_NUM):   #ROW_NUM-1 -->ROW_NUM
            eve.Begin(eve.LINES)
            _y=row_offsety + i*ROW_HEIGHT
            eve.Vertex2f(row_offsetx     , _y)
            eve.Vertex2f(x+w    , _y)
            #eve.cmd_text(x + 10 , _y - 5, 16, 0, str(MAX_VALUE - i*10))   #100-->45  QF
            if scale==1:eve.cmd_text(x + 30 , _y - 5, 16, eve.OPT_RIGHTX, str(MAX_VALUE - i*20))   #100-->45  QF
            else:eve.cmd_text(x + 60 , _y - 5, 18, eve.OPT_RIGHTX, str(MAX_VALUE - i*20))   #100-->45  QF
            row_offsetyMax=_y
    
        # Columns
        if scale==1:col_offset = row_offsetx  
        else:col_offset = row_offsetx  
        row_len = x + w - col_offset
        col_num = num_mins
        col_w = row_len / num_mins
 
        #hh, mm = now_hh, now_mm

        hh, mm = 00, MEASURE_MINUTE_MAX      

        _x_gaps = col_w * _second_gap / 60 # how much second passed from last minute?
        for i in range(col_num+1):
            eve.Begin(eve.LINES)
            _x= x + w - i*col_w - _x_gaps

            eve.Vertex2f(_x, row_offsety)
            #eve.Vertex2f(_x, row_offsety + ROW_LINE * ROW_HEIGHT - ROW_HEIGHT/2)
            eve.Vertex2f(_x, row_offsety + (ROW_LINE -1)* ROW_HEIGHT )

            # _time_str = "19:20 am"
            ''' 
            tm = self._time_str(hh, mm, 0)
            if i != 0: hh, mm, tm = self._time_str(hh, mm, 1)
            #self.rotate_str_up(tm, ANGLE, FONTSIZE, 0, _x - 50, row_offsety + (ROW_LINE-1) * ROW_HEIGHT + ROW_HEIGHT)
            '''
            tm="00:%02d"%(MEASURE_MINUTE_MAX-i) 
            if scale==1: eve.cmd_text(_x - 20, row_offsety + (ROW_LINE) * ROW_HEIGHT-ROW_HEIGHT/4 ,FONTSIZE,0,tm)
            else: eve.cmd_text(_x - 20, row_offsety + (ROW_LINE-1) * ROW_HEIGHT+ROW_HEIGHT/2 ,21,0,tm)
            #print("row_len,i,_x",row_len,i,_x)
        if tvalue!=0:
            eve.ColorRGB(self._COLOR_GREEN[0],self._COLOR_GREEN[1],self._COLOR_GREEN[2])
            if scale==1:eve.cmd_text(x +5+ w/2, y + 3, 21, 0, "%3.1f"%( tvalue) )
            else:eve.cmd_text(x + 5+w/2, y + 3, 28, 0, "%3.1f"%( tvalue) )

    def boxMotion(self,x,y,w,h,border,scale=1,blend=0,tvalue=0):
        eve = self.eve
        if border==1:
            m=20
            eve.ColorRGB(255, 255, 255)
            eve.Begin(eve.LINES)
            eve.LineWidth(1)
            eve.Vertex2f(x , y )
            eve.Vertex2f(x+w+m, y )
            eve.Vertex2f(x+w+m , y )
            eve.Vertex2f(x+w+m , y+h )
            eve.Vertex2f(x+w+m , y+h )
            eve.Vertex2f(x , y+h )
            eve.Vertex2f(x , y+h )
            eve.Vertex2f(x , y  )    
        eve.ColorRGB(255, 255, 255)
        if scale==1:eve.cmd_text(x + 3, y + 3, 21, 0, "Motion")
        else:eve.cmd_text(x + 3, y + 3, 28, 0, "Motion")


    def barGraphHis(self,x,y,w,h,border,data=bytearray(),scale=1,blend=0):
        eve = self.eve
        SAMAPP_BARGRAPH_ARRAY_SIZE=240
        lensTotal=360
        #numchunks=4
        numchunks=1
        bmWidth=96
        bmAdd=eve.RAM_G+256

 
        PADDING_Y = PADDING_X = 30
        MAX_VALUE=80  #40-80
       
        #self.coordinateMarker(x,y,w*scale,h*scale,1,scale,blend)
        if (blend==1) and (self.useBlend==1 ):
            eve.BlendFunc(eve.SRC_ALPHA, eve.ZERO) # Reset blend ,new     

        eve.BitmapHandle(0)
        eve.BitmapSource(eve.RAM_G) 
        eve.ColorRGB(109,145,145)
        #eve.ColorRGB(170,255,127)
        eve.BitmapLayout(eve.BARGRAPH,SAMAPP_BARGRAPH_ARRAY_SIZE,1)
        if scale==1:
            eve.cmd_setbitmap(bmAdd, eve.BARGRAPH, SAMAPP_BARGRAPH_ARRAY_SIZE, bmWidth)
            eve.BitmapSize(eve.NEAREST,eve.BORDER,eve.BORDER,SAMAPP_BARGRAPH_ARRAY_SIZE,bmWidth)
            eve.BitmapSizeH(0,0)
        else:
            eve.cmd_setbitmap(bmAdd, eve.BARGRAPH, SAMAPP_BARGRAPH_ARRAY_SIZE, bmWidth)
            eve.BitmapTransformA(0,128)  #double
            #eve.BitmapTransformE(0,128)
            eve.BitmapTransformE(0,int(self.bb ))
#             eve.BitmapSize(eve.NEAREST,eve.BORDER,eve.BORDER,0,int(self.aa*bmWidth))
#             eve.BitmapSizeH(1,0)
            eve.BitmapSize(eve.NEAREST,eve.BORDER,eve.BORDER,SAMAPP_BARGRAPH_ARRAY_SIZE*2,int(self.aa*bmWidth))
            eve.BitmapSizeH(0,0)
 
        result=bytearray(b'\x0ff')*(SAMAPP_BARGRAPH_ARRAY_SIZE)

        ratio=(bmWidth)/MAX_VALUE
        #print("ratio", ratio )
        #print("temperature_sample_num",self.temperature_sample_num)
        for chunk in range(numchunks):
            for i in range(SAMAPP_BARGRAPH_ARRAY_SIZE/2):
                if (i <self.temperature_sample_num):
                    d = data[i]
                    t=d[0]
                    v=int(d[1] ) # start from  10   #QF
                    #v = int( v*ratio-PADDING_X)
                    v = int( bmWidth-v*ratio)
                    tmpidx =  (i*2 + chunk * SAMAPP_BARGRAPH_ARRAY_SIZE)
                    result[SAMAPP_BARGRAPH_ARRAY_SIZE-1-i*2]=v & 0xFF
                    result[SAMAPP_BARGRAPH_ARRAY_SIZE-1-(i*2+1)]=v & 0xFF
                else:
                    result[SAMAPP_BARGRAPH_ARRAY_SIZE-1-i*2]=0xFF
                    result[SAMAPP_BARGRAPH_ARRAY_SIZE-1-(i*2+1)]=0xFF
            #print("chunk", chunk,len(result) )
            eve.cmd_memwrite(bmAdd, len(result))
            eve.cc( result) 
        #eve.ColorA(128)                          # 50% opacity  new 
        eve.Begin(eve.BITMAPS)
        if scale==1: eve.Vertex2f(x+PADDING_X+5, y+PADDING_X) 
        else:        eve.Vertex2f(x+PADDING_X+37, y+PADDING_X) 
        eve.End() 
        
       
    def blendBk(self,x,y,w,h,border,scale=1,blend=0):
        eve = self.eve
        SAMAPP_BARGRAPH_ARRAY_SIZE=240
        PADDING_Y = PADDING_X = 30
        bmWidth=96   # 128->96
        bmAdd=1024*1024 - 200*1024
         

        if (blend==0) or (self.useBlend==0):return
         
        if (blend==1) and (self.useBlend==1 ):
           eve.BlendFunc(eve.DST_ALPHA,eve.ONE_MINUS_DST_ALPHA)

        if scale==1:
            eve.BitmapHandle(self.hBk512)
            eve.BitmapSize(eve.NEAREST,eve.BORDER,eve.BORDER,SAMAPP_BARGRAPH_ARRAY_SIZE,bmWidth)
            eve.BitmapSizeH(0,0)
        elif scale==2:
            eve.BitmapHandle(self.hBk512)
            #eve.BitmapTransformA(0,128)  #double
            #eve.BitmapTransformE(0,int(self.bb ))
#             eve.BitmapSize(eve.NEAREST,eve.BORDER,eve.BORDER,0,int(self.aa*bmWidth))
#             eve.BitmapSizeH(1,0)
            eve.BitmapSize(eve.NEAREST,eve.BORDER,eve.BORDER,SAMAPP_BARGRAPH_ARRAY_SIZE*2,int(self.aa*bmWidth))
            eve.BitmapSizeH(0,0)


        eve.Begin(eve.BITMAPS)
        if scale==1: eve.Vertex2f(x+PADDING_X+5, y+PADDING_X) 
        else:        eve.Vertex2f(x+PADDING_X+37, y+PADDING_X) 
        eve.End() 

    def snapshot2( self,title):
        eve = self.eve
        block=60   #  -- 96000
        #block=480 # --- 768000
        file="/sd/Snap565_"+title+"_"+str(block)+".raw"
        total=480/block
        #chunk_size=800*block*4  #RGBA
        block_size=800*block*2  #RGB565
        chunk_size=2048
        print("total" ,file,total ,block_size,chunk_size)
        with open(file, 'wb') as f:
            address = eve.RAM_G+(1024-256)*1024
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
        
    def drawBtn(self):
        eve = self.eve
        eve.ColorRGB(0xff, 0xff, 0xff)
        eve = self.eve
        eve.ColorRGB(0xff, 0xff, 0xff)
        y = self.layout.APP_Y 
        btn_w = self.btn_w
        btn_h = self.btn_h
        x1 = self.xStart
        x5 = x1 + 310
        eve.Tag(tag_ui_lds_back)
        eve.cmd_button(x5, y, btn_w, btn_h, 31, 0, "Back")
        eve.Tag(0)
                 
    