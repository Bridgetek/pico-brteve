# Manage sensor SparkFun https://www.sparkfun.com/products/14348

import sys
import adafruit_ccs811
from adafruit_bme280 import basic as adafruit_bme280

sys.path.append("../lib/adafruit")

class CCS811_and_BME280_SparkFun():
    def __init__(self, i2c):
        self.ccs811 = adafruit_ccs811.CCS811(i2c, 0x5B)
        self.bme280 = adafruit_bme280.Adafruit_BME280_I2C(i2c)
        self.bme280.sea_level_pressure = 1013.25

        # Wait for the sensor to be ready
        while not self.ccs811.data_ready:
            pass

    @property
    def co2(self):
        # Wait for the sensor to be ready
        while not self.ccs811.data_ready:
            pass

        return self.ccs811.eco2

    @property
    def tvoc(self):
        # Wait for the sensor to be ready
        while not self.ccs811.data_ready:
            pass

        return self.ccs811.tvoc

    @property
    def temperature(self):
        return self.bme280.temperature

    @property
    def humidity(self):
        return self.bme280.relative_humidity

    @property
    def pressure(self):
        return self.bme280.pressure

    @property
    def altitude(self):
        return self.bme280.altitude
