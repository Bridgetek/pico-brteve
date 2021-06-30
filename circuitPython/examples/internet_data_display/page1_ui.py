import sys
import random
import math
import bteve as eve_module
import busio
import board
import sdcardio
import storage
import os
import time
from random import randint

class Page1_UI:
    def __init__(self, eve:eve_module.Brt_PicoEve_Module) -> None:
        self.eve=eve

        self.start_time_ns = 0
        self.hh=0
        self.mm=0
        
        self.temp_value=0
        self.hr_value=0
        self.pressure_value=0
        self.co2_value=0
        self.tvoc_value=0
        self.oxygen_value=0

        self.humidity_sample_num = 0
        self.HUMIDITY_MAX_SAMPLE = 100
        self.humidity_data=[[0, 0]] * self.HUMIDITY_MAX_SAMPLE

        self._COLOR_GREEN=[0x90, 0xC8, 0x3A]
        self._COLOR_GRAY=[0x33, 0x33, 0x33]
        self._COLOR_WARNING=[0xD4, 0x21, 0x33]

        self.message=''
        self.message_disp_num = 0

    def rotate_str_up_start(self, angle, fontsize):
        return
        e=self.eve
        e.cmd_loadidentity()
        e.cmd_translate(fontsize, fontsize)
        e.cmd_rotate(angle)
        e.cmd_translate(-fontsize, -fontsize)
        e.cmd_setmatrix()

    def rotate_str_up(self, sss, angle, fontsize, opt, x, y):
        e=self.eve

        tan_a = math.tan(angle)

        char_w = fontsize/3
        char_h = fontsize/3
        space_x = char_w * 3 / 2
        space_y = char_h
        if (angle > 45):
            space_x=space_y / tan_a
        elif angle < 45:
            space_y=tan_a * space_x
        else:
            pass

        _x=x
        _y=y
        for s in sss:
            _x=(int)(_x)
            _y=(int)(_y)
            e.cmd_text(_x, _y, fontsize, opt, s)
            _x+=space_x
            _y-=space_y

    #temperature
    def Progress_box(self, x, y, w, h, border, title, unit, vmin, vmax, warning, value):
        e=self.eve

        color = self._COLOR_GREEN
        if value > warning:
            color = self._COLOR_WARNING
        percent = value / vmax * 100

        e.ColorRGB(255, 255, 255)
        e.Begin(e.RECTS)
        e.LineWidth(10)
        e.Vertex2f(x, y)
        e.Vertex2f(x + w, y + h)

        e.ColorRGB(0, 0, 0)
        e.Begin(e.RECTS)
        e.LineWidth(10)
        e.Vertex2f(x + border, y + border)
        e.Vertex2f(x + w - border, y + h - border)

        _padding = 20
        _x=x+_padding
        _y = (y+h)/2 + 40
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

        e.ColorRGB(self._COLOR_GREEN[0], self._COLOR_GREEN[1], self._COLOR_GREEN[2])
        e.cmd_text(x + 3, y + 3, 21, 0, title)
        e.ColorRGB(255, 255, 255)
        e.cmd_text((_x + _w) / 2 - 10, _y - 70, 25, 0, str(round(value, 2)))

    #circle_box
    def circle_box(self, x, y, w, h, border, title, unit, vmin, vmax, lwarning, hwarning, value):
        e=self.eve
        
        color = self._COLOR_GREEN
        if value > hwarning or value < lwarning:
            color = self._COLOR_WARNING
        percent = value / vmax * 100

        e.ColorRGB(255, 255, 255)
        e.Begin(e.RECTS)
        e.LineWidth(10)
        e.Vertex2f(x, y)
        e.Vertex2f(x + w, y + h)

        e.ColorRGB(0, 0, 0)
        e.Begin(e.RECTS)
        e.LineWidth(10)
        e.Vertex2f(x + border, y + border)
        e.Vertex2f(x + w - border, y + h - border)
        e.End()

        # circle view
        _skip_range = 50 # degree
        _skip_low = (int)( _skip_range / 2) # degree
        _skip_high = (int)(360 - _skip_range / 2) # degree
        _max_degree = 360 - _skip_range
        degree = min(_max_degree * percent / 100, _max_degree)
        _padding = 25
        _size = 14
        _ROTATE = 90
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
        for num in list(range(0, 20, 5)) + list(range(_max_degree - 20, _max_degree, 5)):
            num +=_ROTATE + _skip_low  + _size / 4 #rotate circle by 90 degree and center down
            num %=360
            num=math.radians(num)
            posX = _x + (r- _size/2) * math.cos(num)
            posY = _y + (r- _size/2) * math.sin(num)
            e.Vertex2f(posX, posY)

        # Green circle part
        e.ColorRGB(c2[0], c2[1], c2[2])
        e.PointSize(_size)
        for num in range(0, degree, 5):
            num +=_ROTATE + _skip_low  + _size / 4 #rotate circle by 90 degree and center down
            num %=360
            num=math.radians(num)
            posX = _x + (r - _size/2) * math.cos(num)
            posY = _y + (r - _size/2) * math.sin(num)
            e.Vertex2f(posX, posY)
        e.End()

        e.StencilFunc(e.ALWAYS, 0x00, 0xFF)
        e.StencilOp(e.KEEP, e.KEEP)

        e.cmd_text(x + 3, y + 3, 21, 0, title)
        e.cmd_text(_x - len(str(value)) * 5, _y - 15, 24, 0, str(round(value, 2)))
        e.cmd_text(_x - len(unit) * 5, _y + 15, 18, 0, unit)
    
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
        if hh == 12: hh_short = 12

        am="am"
        if hh >= 12:
            am="pm"
        
        if hh_short < 10: prefix_h=prefix0
        if mm < 10: prefix_m=prefix0
        
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

        if vmin > duration_mm:
            vmin = duration_mm
        
        rounded_vmin = (int)(vmin%60)
        if rounded_vmin < 1: #atleast 1min humidity displayed on LCD
            rounded_vmin = 1

        vhour = (int)(vmin / 60) % 24

        # hour clock and minute clock until now
        now_hh = (int)( self.hh + duration_hh ) % 24
        now_mm = (int)( self.mm + duration_mm ) % 60
        if self.mm + duration_mm%60 > 59:
            now_hh +=1        

        return rounded_vmin, now_hh, now_mm
 
    def statitics_box(self, x, y, w, h, border, data=bytearray()):
        """ data = [[0=(timestamp) time), 1=(int) value)], ...], data from last 10 minute
        """
        e = self.eve
        MEASURE_MINUTE_MAX = 4
        num_mins, now_hh, now_mm = self.last_min(MEASURE_MINUTE_MAX)
        _second_gap = self.second_gaps()
        if _second_gap > 0:
            num_mins +=1

        ROW_NUM = 10
        ROW_LINE = ROW_NUM + 1
        PADDING_Y = PADDING_X = 30

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

        e.ColorRGB(255, 255, 255)
        e.cmd_text(x + 3, y + 3, 21, 0, "Humidity")

        # Rows 
        row_offsetx = x + PADDING_X 
        row_offsety = y + PADDING_Y 
        ROW_HEIGHT= (h/2) / ROW_NUM
        e.LineWidth(1)
        for i in range(ROW_NUM+1):
            e.Begin(e.LINES)
            _y=row_offsety + i*ROW_HEIGHT
            e.Vertex2f(row_offsetx     , _y)
            e.Vertex2f(x+w    , _y)
            e.cmd_text(x + 10 , _y - 5, 16, 0, str(100 - i*5))
        
        # Columns
        col_offset = row_offsetx + 20
        row_len = x + w - col_offset
        col_num = num_mins
        col_w = row_len / num_mins
        angle = 35
        fontsize = 16
        self.rotate_str_up_start(angle, fontsize) 
        hh, mm = now_hh, now_mm

        # how much second passed from last minute?
        _x_gaps = col_w * _second_gap / 60
        
        for i in range(col_num):
            e.Begin(e.LINES)
            _x= x + w - i*col_w - _x_gaps
                
            e.Vertex2f(_x, row_offsety)
            e.Vertex2f(_x, row_offsety + ROW_LINE * ROW_HEIGHT)

            # _time_str = "19:20 am"
            tm = self._time_str(hh, mm, 0)
            if i != 0:
                hh, mm, tm = self._time_str(hh, mm, 1)
            self.rotate_str_up(tm, angle, fontsize, 0, _x - 50, row_offsety + ROW_LINE * ROW_HEIGHT + 30)
        
        # line strips
        MAX_TIME = num_mins * 60 #second
        W_PER_S = row_len / MAX_TIME # width pixel per second 
        H_PER_V = (ROW_HEIGHT * ROW_NUM) / (100-50) # # height pixel per second, range from 50 to 100
        if self.humidity_sample_num > 0:
            t0 = data[0][0]
            e.ColorRGB(self._COLOR_GREEN[0], self._COLOR_GREEN[1], self._COLOR_GREEN[2])
            e.Begin(e.LINE_STRIP)
            e.LineWidth(4)

            # print(self.humidity_sample_num)
            for i in range(self.humidity_sample_num):
                d = data[i]
                t=d[0]
                v=d[1] - 50
                
                gap = t0 - t # gaps in second
                if gap > (MEASURE_MINUTE_MAX * 60):
                    break

                # print('i=', i, 'data=', data[i], 't0=', t0, 'gap=', gap)
                _x = x + w - W_PER_S * gap
                _y = y + PADDING_Y + ROW_HEIGHT*ROW_NUM - H_PER_V*v
                e.Vertex2f(_x, _y)

            e.End()
            
    def start(self):
        e=self.eve
        eve=e

        eve.ClearColorRGB(0, 0, 0)
        eve.Clear()
        eve.ColorRGB(255, 255, 255)
        eve.VertexFormat(4)

        w=1024
        h=600

        if len(self.message) > 0:
            e.cmd_text(300, h - 50, 30, 0, self.message)

        PD = 20
        MG = 20
        NB = 4
        bw = (w - 2 * PD - 3 * MG) / NB
        bh=190
        y = 10
        self.Progress_box(x = PD + (bw + MG) * 0, y=y, w = bw, h = bh, border=1, title="Temperature", 
            unit=" C", vmin=20, vmax=50, warning=40, value=self.temp_value)

        self.circle_box(x = PD + (bw + MG) * 3, y=y, w = bw, h = bh, border=1, title="Heart Rate", 
            unit="bpm", vmin=0, vmax=200, lwarning=30, hwarning=180, value=self.hr_value)
        
        # center box
        cbw= (int)(bw *7/4)
        self.statitics_box(x = (w - cbw) / 2, y=y, w = cbw, h = bh, border=1, data=self.humidity_data)

        y += MG + bh
        self.circle_box(x = PD + (bw + MG) * 0, y=y, w = bw, h = bh, border=1, title="Barometric Pressure",
            unit="hPa", vmin=0, vmax=5000, lwarning=0, hwarning=5000, value=self.pressure_value)
        self.circle_box(x = PD + (bw + MG) * 1, y=y, w = bw, h = bh, border=1, title="CO2", 
            unit="ppm", vmin=0, vmax=100000, lwarning=0, hwarning=30000, value=self.co2_value)
        self.circle_box(x = PD + (bw + MG) * 2, y=y, w = bw, h = bh, border=1, title="TVOC", 
            unit="mg/m3", vmin=0, vmax=10, lwarning=0, hwarning=3, value=self.tvoc_value)
        self.circle_box(x = PD + (bw + MG) * 3, y=y, w = bw, h = bh, border=1, title="Blood Oxygen", 
            unit="%", vmin=30, vmax=100, lwarning=30, hwarning=100, value=self.oxygen_value)

        e.swap()
        pass

    last_push_humidity = 0
    def push_humidity(self, value):
        now = time.monotonic_ns()
        TIME_PATTERN= 5 #second
        if now - self.last_push_humidity < TIME_PATTERN *1e9:
            pass
            # return

        self.last_push_humidity = now

        timestamp = time.monotonic_ns() / 1e9
        data = [timestamp, value]

        # new data on top
        temp=self.humidity_data[0:self.HUMIDITY_MAX_SAMPLE] # get index from 0 to max-1
        self.humidity_data = [data] + temp                  # add value to top

        if self.humidity_sample_num < self.HUMIDITY_MAX_SAMPLE:
            self.humidity_sample_num += 1
        
    def set_temp(self, value):
        self.temp_value = value
    def set_hr(self, value):
        self.hr_value = value
    def set_pressure(self, value):
        self.pressure_value = value
    def set_co2(self, value):
        self.co2_value = value
    def set_tvoc(self, value):
        self.tvoc_value = value
    def set_oxygen(self, value):
        self.oxygen_value = value
