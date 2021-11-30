import time
from brteve.brt_eve_bt817_8 import BrtEve

MIN_TOUCH        = 14
MIN_MOVE         = 0.1 # pixel / milis
MAX_MOVE         = 2   # pixel / milis
MAX_DELAY        = 1000 # milisecond

# return value is the number of milliseconds that have elapsed since the system was started.
def milis():
    return round(time.monotonic_ns() / 1000_000)
    
class helper_scroller():
    def __init__(self, limit_top = 0, limit_bottom = 0, speed=1, friction=0.9) -> None:
        self._limit_t = limit_top
        self._limit_b = limit_bottom
        self._speed = speed
        self._friction = friction

        self.last_xy = 0
        self.last_milis_touch = 0
        self.last_milis = 0

        self._last_offset = 0
        self._last_velocity = 0
        self._back_to_top = 0
        self._back_to_bottom = 0
        self._last_no_touch = 0

    def set_limit(self, top, bottom):
        self._limit_t = top
        self._limit_b = bottom

    def set_speed(self, speed):
        self._speed = speed

    def set_friction(self, friction):
        self._friction = friction

    def stop(self):
        self._last_velocity = 0

    def _stop_and_run_back(self):
        padding = 100
        # back to top
        if self._last_offset > self._limit_t + padding:
            self._back_to_top = 1
            self._last_velocity = 0
        if self._back_to_top == 1:
            distance = round(abs(self._last_offset - self._limit_t) / 2)
            if distance < 3:
                self._back_to_top = 0
                self._last_offset = self._limit_t
            else:
                self._last_offset -= distance

         # back to bottom
        if self._last_offset < self._limit_b - padding:
            self._back_to_bottom = 1
            self._last_velocity = 0
        if self._back_to_bottom == 1:
            distance = round(abs(self._last_offset - self._limit_b) / 2)
            if distance < 3:
                self._back_to_bottom = 0
                self._last_offset = self._limit_b
            else:
                self._last_offset += distance

    def set_offset_vloc(self, offset, vloc):
        self._last_offset = offset
        self._last_velocity = vloc

    def get_offset_velocity(self, new_position_xy):
        time = milis()
        delay = max(1, time - self.last_milis_touch)

        distance = new_position_xy - self.last_xy

        if new_position_xy == 32768: # no touch, friction is in affect
            self._last_velocity = round(self._friction * self._last_velocity, 3)
            if abs(self._speed * self._last_velocity) < 0.02:
                self._last_velocity = 0

            self._last_offset += round(self._last_velocity * (time - self.last_milis))
            self._last_no_touch = 1
            self._stop_and_run_back()
        else:
            velocity = distance / delay # v = pixel / milisecond
            self.last_milis_touch = time

            if self._last_no_touch == 1 or delay > MAX_DELAY: # This is a single touch
                self.last_xy = new_position_xy
                self._back_to_top = 0
                self._back_to_bottom = 0
                self._last_velocity = 0
            else: # this is a swipe
                self._last_velocity = velocity
                self.last_xy = new_position_xy
                self._last_offset += round(self._last_velocity * delay)
            self._last_no_touch = 0

        self.last_milis = time
        return self._last_offset, self._last_velocity
