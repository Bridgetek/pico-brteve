import time
_now = 0
_milis_start = 0
_utc_time_hh = 0
_utc_time_mm = 0

def init_datetime(millis_realtime = 0, utc_init = '00:00'):
    global _milis_start
    global _utc_time_hh
    global _utc_time_mm
    tm = utc_init.split(":")
    _utc_time_hh = (int)(tm[0])
    _utc_time_hh = (int)(tm[1])
    _milis_start = milis() - millis_realtime # start milis from system

def set_timnezone(hh, mm):
    global _utc_time_hh
    global _utc_time_mm
    _utc_time_hh = (int)(hh)
    _utc_time_mm = (int)(mm)

# return number of milliseconds in realtime since 1970 Jan 1
def now():
    global _now
    utc_milis = _utc_time_hh*60*60+_utc_time_mm*60
    _now = milis() - _milis_start + utc_milis * 1000
    return _now

def now_hh_mm():
    return hh_mm(now() / 1000)

def now_hh_mm_ss():
    second = int(str(_now)[0:10])
    return hh_mm_ss(second)

def hh_mm(second):
    tt=time.localtime(second)
    hh=tt.tm_hour
    mm=tt.tm_min
    return hh, mm

def hh_mm_ss(second):
    tt=time.localtime(second)
    hh=tt.tm_hour
    mm=tt.tm_min
    ss=tt.tm_sec
    return hh, mm, ss

def hh_mm_ss_ms(milis):
    ms = int(str(milis)[-3::])
    second = int(str(milis)[0:10])
    tt=time.localtime(second)
    hh=tt.tm_hour
    mm=tt.tm_min
    ss=tt.tm_sec
    return hh, mm, ss, ms

# return value is the number of milliseconds that have elapsed since the system was started.
def milis():
    return round(time.monotonic_ns() / 1000_000)

def print_weekday(weekday, fmt='short'):
    f1 = ["Mon","Tue","Wed","Thu","Fri","Sat","Sun"]
    f2 = ["Monday","Tuesday","Wednessday","Thursday","Friday","Saturday","Sunday"]

    if fmt == 'short':
        return f1[weekday]

    elif fmt == 'long':
        return f2[weekday]

    return f1[weekday]

def _print_month(mon, fmt='short'):
    f1 = ["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"]
    f2 = ["January", "February", "March", "April", "May", "June", "July", "August",
          "September", "October", "November", "December"]

    mon -=1

    if fmt == 'short':
        return f1[mon]

    elif fmt == 'long':
        return f2[mon]

    return f1[mon]

def yyyy_mt_dd_hh_mm_ss_ms_weekday_weeks_weekl_mons_monl(milis):
    duration = milis
    ms = duration % 1000
    second = int(str(milis)[0:10]) # duration = num of second
    tt=time.localtime(second)
    yyyy = tt.tm_year - 30
    mt = tt.tm_mon
    dd = tt.tm_mday
    hh=tt.tm_hour
    mm=tt.tm_min
    ss=tt.tm_sec
    weekday = tt.tm_wday

    weeks =print_weekday(weekday, 'short')
    weekl =print_weekday(weekday, 'long')
    mons =_print_month(mt, 'short')
    monl =_print_month(mt, 'long')
    return yyyy, mt, dd, hh, mm, ss, ms, weekday, weeks, weekl, mons, monl

_last_random = 0
def random(minv, maxv):
    global _last_random
    num = minv - 1
    while num < minv or num == _last_random:
        num=time.monotonic_ns()
        nums=str(num)
        num3 = nums[len(nums) - 3:]

        for i in num3:
            num += (int)(i)

        num = num % maxv

    _last_random = num
    return num
