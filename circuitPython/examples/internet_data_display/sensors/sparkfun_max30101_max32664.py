# SPDX-FileCopyrightText: 2017 Dean Miller for Adafruit Industries
#
# SPDX-License-Identifier: MIT

"""
`sparkfun_pulse_oximeter_and_heart_rate_15219`
======================================================================
This library supports the use of the "SparkFun Pulse Oximeter and Heart Rate Sensor - MAX30101
    & MAX32664 (Qwiic)" in CircuitPython.

Author(s): tuan.nguyen@brtchip.com

**Hardware:**

* `SparkFun Pulse Oximeter and Heart Rate Senso
  <https://www.sparkfun.com/products/15219>`_

**Software and Dependencies:**

* Adafruit CircuitPython firmware for the supported boards:
  https://github.com/adafruit/circuitpython/releases

 * Adafruit's Bus Device library: https://github.com/adafruit/Adafruit_CircuitPython_BusDevice
 * Adafruit's Register library: https://github.com/adafruit/Adafruit_CircuitPython_Register

**Notes:**

#. `Datasheet
<https://cdn.sparkfun.com/assets/f/e/9/4/1/MAX30101.pdf>`_
<https://cdn.sparkfun.com/assets/4/3/c/2/b/MAX32664.pdf>`_
"""

import time
import sys
from micropython import const
from adafruit_bus_device.i2c_device import I2CDevice

sys.path.append("../lib/adafruit")
__version__ = "0.0.0-auto.0"

_WRITE_FIFO_INPUT_BYTE  = 0x04
_DISABLE                = 0x00
_ENABLE                 = 0x01

_APP_MODE               = 0x00
_BOOTLOADER_MODE        = 0x08
_NO_WRITE               = 0x00
INCORR_PARAM           = 0xEE

_CONFIGURATION_REGISTER = 0x0A
_PULSE_MASK             = 0xFC
_READ_PULSE_MASK        = 0x03
_SAMP_MASK              = 0xE3
_READ_SAMP_MASK         = 0x1C
_ADC_MASK               = 0x9F
_READ_ADC_MASK          = 0x60

_ENABLE_CMD_SLEEP       = 45 # Milliseconds
_CMD_SLEEP              = 0.06  # Milliseconds
_MAXFAST_ARRAY_SIZE     = 6  # Number of bytes....
_MAXFAST_EXTENDED_DATA  = 5
_MAX30101_LED_ARRAY     = 12 # 4 values of 24 bit (3 byte) LED values

_SET_FORMAT             = 0x00
_READ_FORMAT            = 0x01 # Index Byte under Family Byte: READ_OUTPUT_MODE (0x11)
_WRITE_SET_THRESHOLD    = 0x01 #Index Byte for WRITE_INPUT(0x14)
_WRITE_EXTERNAL_TO_FIFO = 0x00

_BIO_ADDRESS            = 0x55
_HW_ID_CODE             = const(0x15)

#enum READ_STATUS_BYTE_VALUE():
SUCCESS                  = 0x00
ERR_UNAVAIL_CMD          = 0x01
ERR_UNAVAIL_FUNC         = 0x02
ERR_DATA_FORMAT          = 0x03
ERR_INPUT_VALUE          = 0x04
ERR_TRY_AGAIN            = 0x05
ERR_BTLDR_GENERAL        = 0x80
ERR_BTLDR_CHECKSUM       = 0x81
ERR_BTLDR_AUTH           = 0x82
ERR_BTLDR_INVALID_APP    = 0x83
ERR_UNKNOWN              = 0xFF
#enum FAMILY_REGISTER_BYTES():
HUB_STATUS               = 0x00
SET_DEVICE_MODE          = 0x01
READ_DEVICE_MODE         = 0x02
OUTPUT_MODE              = 0x10
READ_OUTPUT_MODE         = 0x11
READ_DATA_OUTPUT         = 0x12
READ_DATA_INPUT          = 0x13
WRITE_INPUT              = 0x14
WRITE_REGISTER           = 0x40
READ_REGISTER            = 0x41
READ_ATTRIBUTES_AFE      = 0x42
DUMP_REGISTERS           = 0x43
ENABLE_SENSOR            = 0x44
READ_SENSOR_MODE         = 0x45
CHANGE_ALGORITHM_CONFIG  = 0x50
READ_ALGORITHM_CONFIG    = 0x51
ENABLE_ALGORITHM         = 0x52
BOOTLOADER_FLASH         = 0x80
BOOTLOADER_INFO          = 0x81
IDENTITY                 = 0xFF
#enum DEVICE_MODE_WRITE_BYTES():
EXIT_BOOTLOADER          = 0x00
RESET                    = 0x02
ENTER_BOOTLOADER         = 0x08
# Write Bytes under Family Byte: OUTPUT_MODE (0x10) and Index byte: SET_FORMAT
# (0x00)
#enum OUTPUT_MODE_WRITE_BYTE():
PAUSE                    = 0x00
SENSOR_DATA              = 0x01
ALGO_DATA                = 0x02
SENSOR_AND_ALGORITHM     = 0x03
PAUSE_TWO                = 0x04
SENSOR_COUNTER_BYTE      = 0x05
ALGO_COUNTER_BYTE        = 0x06
SENSOR_ALGO_COUNTER      = 0x07
# Index Byte under the Family Byte: READ_DATA_OUTPUT (0x12)
#enum FIFO_OUTPUT_INDEX_BYTE():
NUM_SAMPLES              =0
READ_DATA                =1
# Index Byte under the Family Byte: READ_DATA_INPUT (0x13)
#enum FIFO_EXTERNAL_INDEX_BYTE():
SAMPLE_SIZE              =0
READ_INPUT_DATA          =1
READ_SENSOR_DATA         =2     # For external accelerometer
READ_NUM_SAMPLES_INPUT   =3 # For external accelerometer
READ_NUM_SAMPLES_SENSOR  =4
# Index Byte under the Family Registry Byte: WRITE_REGISTER (0x40)
#enum WRITE_REGISTER_INDEX_BYTE():
WRITE_MAX30101           = 0x03
WRITE_ACCELEROMETER      = 0x04
# Index Byte under the Family Registry Byte: READ_REGISTER (0x41)
#enum READ_REGISTER_INDEX_BYTE():
READ_MAX30101            = 0x03
READ_ACCELEROMETER       = 0x04
# Index Byte under the Family Registry Byte: READ_ATTRIBUTES_AFE (0x42)
#enum GET_AFE_INDEX_BYTE():
RETRIEVE_AFE_MAX30101      = 0x03
RETRIEVE_AFE_ACCELEROMETER = 0x04
# Index Byte under the Family Byte: DUMP_REGISTERS (0x43)
#enum DUMP_REGISTER_INDEX_BYTE():
DUMP_REGISTER_MAX30101      = 0x03
DUMP_REGISTER_ACCELEROMETER = 0x04
# Index Byte under the Family Byte: ENABLE_SENSOR (0x44)
#enum SENSOR_ENABLE_INDEX_BYTE():
ENABLE_MAX30101           = 0x03
ENABLE_ACCELEROMETER      = 0x04
# Index Byte for the Family Byte: READ_SENSOR_MODE (0x45)
#enum READ_SENSOR_ENABLE_INDEX_BYTE():
READ_ENABLE_MAX30101      = 0x03
READ_ENABLE_ACCELEROMETER = 0x04
# Index Byte under the Family Byte: CHANGE_ALGORITHM_CONFIG (0x50)
#enum ALGORITHM_CONFIG_INDEX_BYTE():
SET_TARG_PERC            = 0x00
SET_STEP_SIZE            = 0x00
SET_SENSITIVITY          = 0x00
SET_AVG_SAMPLES          = 0x00
SET_PULSE_OX_COEF        = 0x02
BPT_CONFIG               = 0x04
# Write Bytes under the Family Byte: CHANGE_ALGORITHM_CONFIG (0x50) and the
# Index Byte: ALGORITHM_CONFIG_INDEX_BYTE - SET_TARG_PERC
#enum ALGO_AGC_WRITE_BYTE():
AGC_GAIN_ID              = 0x00
AGC_STEP_SIZE_ID         = 0x01
AGC_SENSITIVITY_ID       = 0x02
AGC_NUM_SAMP_ID          = 0x03
MAXIMFAST_COEF_ID        = 0x0B
#enum ALGO_BPT_WRITE_BYTE():
BPT_MEDICATION           = 0x00
SYSTOLIC_VALUE           = 0x01
DIASTOLIC_VALUE          = 0x02
BPT_CALIB_DATA           = 0x03 #Index + 824 bytes of calibration data
PATIENT_RESTING          = 0x05
AGC_SP02_COEFS           = 0x0B
# Index Bytes under the Family Byte: READ_ALGORITHM_CONFIG (0x51)
#enum READ_ALGORITHM_INDEX_BYTE():
READ_AGC_PERCENTAGE      = 0x00
READ_AGC_STEP_SIZE       = 0x00
READ_AGC_SENSITIVITY     = 0x00
READ_AGC_NUM_SAMPLES     = 0x00
READ_MAX_FAST_COEF       = 0x02
# Write Bytes under the Family Byte: READ_ALGORITHM_CONFIG (0x51) and Index Byte:
# READ_ALGORITHM_INDEX_BYTE - AGC
#enum READ_AGC_ALGO_WRITE_BYTE():
READ_AGC_PERC_ID           = 0x00
READ_AGC_STEP_SIZE_ID      = 0x01
READ_AGC_SENSITIVITY_ID    = 0x02
READ_AGC_NUM_SAMPLES_ID    = 0x03
READ_MAX_FAST_COEF_ID      = 0x0B
# Index Byte under the Family Byte: ENABLE_ALGORITHM (0x52).
#enum ALGORITHM_MODE_ENABLE_INDEX_BYTE():
ENABLE_AGC_ALGO            = 0x00
ENABLE_WHRM_ALGO           = 0x02
# Index Byte under the Family Byte: BOOTLOADER_FLASH (0x80).
#enum BOOTLOADER_FLASH_INDEX_BYTE():
SET_INIT_VECTOR_BYTES    = 0x00
SET_AUTH_BYTES           = 0x01
SET_NUM_PAGES            = 0x02
ERASE_FLASH              = 0x03
SEND_PAGE_VALUE          = 0x04
# Index Byte under the Family Byte: BOOTLOADER_INFO (0x81).
#enum BOOTLOADER_INFO_INDEX_BYTE():
BOOTLOADER_VERS          = 0x00
PAGE_SIZE                = 0x01
# Index Byte under the Family Byte: IDENTITY (0xFF).
#enum IDENTITY_INDEX_BYTES():
READ_MCU_TYPE            = 0x00
READ_SENSOR_HUB_VERS     = 0x03
READ_ALGO_VERS           = 0x07

RESULT_OK=0x01
RESULT_NOT_OK=0x02
RETURN_RESULT_OK = b"".join([bytes([RESULT_OK])])
RETURN_RESULT_NOT_OK = b"".join([bytes([RESULT_NOT_OK])])

class SparkFun_MAX30101_And_MAX32664:
    """SparkFun Pulse Oximeter and Heart Rate sensor driver.

    :param ~busio.I2C i2c_bus: The I2C bus the BME280 is connected to
    :param int address: The I2C address of the SparkFun_MAX30101_And_MAX32664.
                     Defaults to :const:`0x5A`

    **Quickstart: Importing and using the SparkFun_MAX30101_And_MAX32664**

        Here is an example of using the :class:`SparkFun_MAX30101_And_MAX32664` class.
        First you will need to import the libraries to use the sensor

        .. code-block:: python

            import board
            import sparkfun_max30101_max32664

        Once this is done you can define your `board.I2C` object and define your sensor object

        .. code-block:: python

            i2c = board.I2C()   # uses board.SCL and board.SDA
            ccs811 = sparkfun_max30101_max32664.SparkFun_MAX30101_And_MAX32664(i2c)

        Now you have access to the attributes :
            attr:`bpm`
            attr:`heartrate`
            attr:`confidence`
            attr:`oxygen`
            attr:`status`

        .. code-block:: python

            eco2 = ccs811.eco2
            tvoc = ccs811.tvoc

    """

    MODE_ONE               = 0x01
    MODE_TWO               = 0x02

    _BUFFER = bytearray(20)
    def __init__(self, i2c_bus, address=0x55):
        # while not i2c_bus.try_lock():
        #     pass
        # [print("Found I2C device:", hex(x)) for x in i2c_bus.scan()]
        # result = bytearray(2)
        # i2c_bus.writeto(address, bytes([READ_REGISTER, READ_MAX30101, 0xFF]))

        # for i in range(3):
        #     i2c_bus.readfrom_into(address, result)
        #     print("result=", result)
        # time.sleep(9999999)

        self.i2c = i2c_bus
        self.i2c_device = 0# I2CDevice(i2c_bus, address)
        self._device = I2CDevice(i2c_bus, address)

        # check that the HW id is correct
        self.hw_id = self._read_MAX30101_register(0xFF)
        if self.hw_id != _HW_ID_CODE:
            raise RuntimeError(
                "Device ID returned is not correct! Please check your wiring."
            )

        self._heart_ate =0

        # Confidence formatting
        self._confidence = 0

        #Blood oxygen level formatting
        self._oxygen = 0
        #"Machine State" - has a finger been detected?
        self._finger_detected = 0
        #Sp02 r Value formatting
        self._Sp02 = 0
        #Extended Machine State formatting
        self._extStatus = 0
        # for i in range(0xff):
        #     print(i, "=", self._read_MAX30101_register(i))
        # print(0xff, "=", self._read_MAX30101_register(0xff))

    def configBpm(self, mode):
        if mode in (self.MODE_ONE, self.MODE_TWO):
            pass
        else: return INCORR_PARAM

        statusChauf = self.setOutputMode(ALGO_DATA) # Just the data
        if statusChauf != SUCCESS:
            return statusChauf

        statusChauf = self.setFifoThreshold(0x01) # One sample before interrupt is fired.
        if statusChauf != SUCCESS:
            return statusChauf

        statusChauf = self.agcAlgoControl(_ENABLE) # One sample before interrupt is fired.
        if statusChauf != SUCCESS:
            return statusChauf

        statusChauf = self.max30101Control(_ENABLE)
        if statusChauf != SUCCESS:
            return statusChauf

        statusChauf = self.maximFastAlgoControl(mode)
        if statusChauf != SUCCESS:
            return statusChauf

        _userSelectedMode = mode
        _sampleRate = self.readAlgoSamples()

        time.sleep(1)
        return SUCCESS

    # Family Byte: OUTPUT_MODE (0x10), Index Byte: SET_FORMAT (0x00),
    # Write Byte : outputType (Parameter values in OUTPUT_MODE_WRITE_BYTE)
    def setOutputMode(self, outputType):
        if outputType > SENSOR_ALGO_COUNTER: # Bytes between 0x00 and 0x07
            return INCORR_PARAM

        # Check that communication was successful, not that the IC is outputting
        # correct format.
        statusByte = self._write_u8(OUTPUT_MODE, _SET_FORMAT, outputType)
        if statusByte != SUCCESS:
            return statusByte
        else:
            return SUCCESS

    # Family Byte: OUTPUT_MODE(0x10), Index Byte: WRITE_SET_THRESHOLD (0x01), Write byte: intThres
    # (parameter - value betwen 0 and 0xFF).
    # This function changes the threshold for the FIFO interrupt bit/pin. The
    # interrupt pin is the MFIO pin which is set to INPUT after IC initialization
    # (begin).
    def setFifoThreshold(self, intThresh):
        # Checks that there was succesful communcation, not that the threshold was
        # set correctly.
        statusByte = self._write_u8(OUTPUT_MODE, _WRITE_SET_THRESHOLD, intThresh)
        if statusByte != SUCCESS:
            return statusByte
        else:
            return SUCCESS


    # Family Byte: ENABLE_ALGORITHM (0x52), Index Byte:
    # ENABLE_AGC_ALGO (0x00)
    # This function enables (one) or disables (zero) the automatic gain control algorithm.
    def agcAlgoControl(self, enable):
        if enable in (0, 1):
            pass
        else:
            return INCORR_PARAM

        statusByte = self._write_u8(ENABLE_ALGORITHM, ENABLE_AGC_ALGO, enable)
        if statusByte != SUCCESS:
            return statusByte
        else:
            return SUCCESS


        # Family Byte: ENABLE_SENSOR (0x44), Index Byte: ENABLE_MAX30101 (0x03), Write
        # Byte: senSwitch  (parameter - 0x00 or 0x01).
        # This function enables the MAX30101.
    def max30101Control(self, senSwitch):
        if senSwitch in (0, 1):
            pass
        else:
            return INCORR_PARAM

        # Check that communication was successful, not that the sensor is enabled.
        statusByte = self._write_u8(ENABLE_SENSOR, ENABLE_MAX30101, senSwitch)
        if statusByte != SUCCESS :
            return statusByte
        else:
            return SUCCESS

        # Family Byte: ENABLE_ALGORITHM (0x52), Index Byte:
        # ENABLE_WHRM_ALGO (0x02)
        # This function enables (one) or disables (zero) the wrist heart rate monitor
        # algorithm.
    def maximFastAlgoControl(self, mode):
        if mode in (0, 1, 2):
            pass
        else:
            return INCORR_PARAM

        statusByte = self._write_u8(ENABLE_ALGORITHM, ENABLE_WHRM_ALGO, mode)
        if statusByte != SUCCESS:
            return statusByte
        else:
            return SUCCESS

        # Family Byte: READ_ALGORITHM_CONFIG (0x51), Index Byte:
        # READ_AGC_NUM_SAMPLES (0x00), Write Byte: READ_AGC_NUM_SAMPLES_ID (0x03)
        # This function changes the number of samples that are averaged.
        # It takes a paramater of zero to 255.
    def readAlgoSamples(self):
        samples = self._read_u8(
            READ_ALGORITHM_CONFIG,
            READ_AGC_NUM_SAMPLES,
            READ_AGC_NUM_SAMPLES_ID )
        return samples

    def readBpm(self):
        self._read_data_MODE_TWO(self._BUFFER)

    def _write_MAX30101_register_u8(self, address, value):
        # Read an 8-bit unsigned value from the specified 8-bit address.
        arr_value = bytes([value])
        return self._write_bytes_array(self, WRITE_REGISTER, WRITE_MAX30101, address, arr_value)

    def _write_MAX30101_register_array(self, address, arr_value):
        # Read an 8-bit unsigned value from the specified 8-bit address.
        return self._write_bytes_array(self, WRITE_REGISTER, WRITE_MAX30101, address, arr_value)

    def _write_address(self, i2c, family_byte, index_byte, address):
        self._BUFFER[0] = family_byte & 0xFF
        self._BUFFER[1] = index_byte  & 0xFF
        self._BUFFER[2] = address     & 0xFF
        i2c.write(self._BUFFER, end=3)
        time.sleep(_CMD_SLEEP)

    def _write_command(self, i2c, family_byte, index_byte):
        self._BUFFER[0] = family_byte & 0xFF
        self._BUFFER[1] = index_byte  & 0xFF
        i2c.write(self._BUFFER, end=2)
        time.sleep(_CMD_SLEEP)

    def _write_bytes_array(self, family_byte, index_byte, address, value):
        # Write an byte array
        self._device.write(b"".join(
            [bytes([family_byte]),
            bytes([index_byte]),
            bytes([address]),
            bytes(value)]))

    def _read_MAX30101_register(self, register):
        # Read an 8-bit unsigned value from the specified 8-bit address.
        return self._read_u8(READ_REGISTER, READ_MAX30101, register)

    def _read_data_MODE_TWO(self, buffer):
        _len=12
        if len(buffer) < _len:
            print("buffer len must >= ", _len)
            return RETURN_RESULT_NOT_OK

        with self._device as i2c:
            self._write_command(i2c, READ_DATA_OUTPUT,READ_DATA)
            i2c.readinto(buffer, end=_len)
            if buffer[0] != 0:
                print("Fail to read from I2C")
                return  RETURN_RESULT_NOT_OK

        buffer=buffer[1:]
        # Heart Rate formatting
        self._heart_ate = (buffer[0])
        self._heart_ate = self._heart_ate << 8
        self._heart_ate |= buffer[1]
        self._heart_ate /= 10

        # Confidence formatting
        self._confidence = buffer[2]

        #Blood oxygen level formatting
        self._oxygen = (buffer[3]) << 8
        self._oxygen |= buffer[4]
        self._oxygen /= 10.0

        #"Machine State" - has a finger been detected?
        self._finger_detected = buffer[5]

        #Sp02 r Value formatting
        self._Sp02 = (buffer[6]) << 8
        self._Sp02 |= buffer[7]
        self._Sp02 /= 10.0

        #Extended Machine State formatting
        self._extStatus = buffer[8]
        return RETURN_RESULT_OK

    @property
    def heart_rate(self):
        return self._heart_ate
    @property
    def oxygen(self):
        return self._oxygen
    @property
    def finger_detected(self):
        return self._finger_detected
    @property
    def Sp02(self):
        return self._Sp02
    @property
    def confidence(self):
        return self._confidence

    def _write_u8(self, family_byte, index_byte, address):
        # Read an 8-bit unsigned value from the specified 8-bit address.
        with self._device as i2c:
            self._write_address(i2c, family_byte,index_byte, address)
            i2c.readinto(self._BUFFER, end=2)
        return self._BUFFER[0]

    def _read_u8(self, family_byte, index_byte, address):
        # Read an 8-bit unsigned value from the specified 8-bit address.
        with self._device as i2c:
            self._write_address(i2c, family_byte,index_byte, address)
            i2c.readinto(self._BUFFER)
            if self._BUFFER[0] != 0:
                print("_read_register failed")
                return -1

        return self._BUFFER[1]

    def _read_u16(self, family_byte, index_byte, address):
        # Read a 16-bit BE unsigned value from the specified 8-bit address.
        with self._device as i2c:
            self._write_address(i2c, family_byte,index_byte, address)
            i2c.readinto(self._BUFFER)
            if self._BUFFER[0] != 0:
                print("_read_register failed")
                return -1

        return (self._BUFFER[0] << 8) | self._BUFFER[1]

    #     # try to start the app
    #     buf = bytearray(1)
    #     buf[0] = 0xF4
    #     with self.i2c_device as i2c:
    #         i2c.write(buf, end=1)
    #     time.sleep(0.1)

    #     # make sure there are no errors and we have entered application mode
    #     if self.error:
    #         raise RuntimeError(
    #             "Device returned a error! Try removing and reapplying power to "
    #             "the device and running the code again."
    #         )
    #     if not self.fw_mode:
    #         raise RuntimeError(
    #             "Device did not enter application mode! If you got here, there may "
    #             "be a problem with the firmware on your sensor."
    #         )

    #     self.interrupt_enabled = False

    #     # default to read every second
    #     self.drive_mode = DRIVE_MODE_1SEC

    #     self._eco2 = None  # pylint: disable=invalid-name
    #     self._tvoc = None  # pylint: disable=invalid-name

    # @property
    # def error_code(self):
    #     """Error code"""
    #     buf = bytearray(2)
    #     buf[0] = 0xE0
    #     with self.i2c_device as i2c:
    #         i2c.write_then_readinto(buf, buf, out_end=1, in_start=1)
    #     return buf[1]

    # def _update_data(self):
    #     if self.data_ready:
    #         buf = bytearray(9)
    #         buf[0] = _ALG_RESULT_DATA
    #         with self.i2c_device as i2c:
    #             i2c.write_then_readinto(buf, buf, out_end=1, in_start=1)

    #         self._eco2 = (buf[1] << 8) | (buf[2])
    #         self._tvoc = (buf[3] << 8) | (buf[4])

    #         if self.error:
    #             raise RuntimeError("Error:" + str(self.error_code))

    # @property
    # def baseline(self):
    #     """
    #     The property reads and returns the current baseline value.
    #     The returned value is packed into an integer.
    #     Later the same integer can be used in order
    #     to set a new baseline.
    #     """
    #     buf = bytearray(3)
    #     buf[0] = _BASELINE
    #     with self.i2c_device as i2c:
    #         i2c.write_then_readinto(buf, buf, out_end=1, in_start=1)
    #     return struct.unpack("<H", buf[1:])[0]

    # @baseline.setter
    # def baseline(self, baseline_int):
    #     """
    #     The property lets you set a new baseline. As a value accepts
    #     integer which represents packed baseline 2 bytes value.
    #     """
    #     buf = bytearray(3)
    #     buf[0] = _BASELINE
    #     struct.pack_into("<H", buf, 1, baseline_int)
    #     with self.i2c_device as i2c:
    #         i2c.write(buf)

    # @property
    # def tvoc(self):  # pylint: disable=invalid-name
    #     """Total Volatile Organic Compound in parts per billion."""
    #     self._update_data()
    #     return self._tvoc

    # @property
    # def eco2(self):  # pylint: disable=invalid-name
    #     """Equivalent Carbon Dioxide in parts per million. Clipped to 400 to 8192ppm."""
    #     self._update_data()
    #     return self._eco2

    # @property
    # def temperature(self):
    #     """
    #     .. deprecated:: 1.1.5
    #        Hardware support removed by vendor

    #     Temperature based on optional thermistor in Celsius."""
    #     buf = bytearray(5)
    #     buf[0] = _NTC
    #     with self.i2c_device as i2c:
    #         i2c.write_then_readinto(buf, buf, out_end=1, in_start=1)

    #     vref = (buf[1] << 8) | buf[2]
    #     vntc = (buf[3] << 8) | buf[4]

    #     # From ams ccs811 app note 000925
    #     # https://download.ams.com/.../SparkFun_MAX30101_And_MAX32664_Doc_cAppNote-Connectin..pdf
    #     rntc = float(vntc) * _REF_RESISTOR / float(vref)
    #     print("rntc=", rntc)
    #     rntc = rntc / 10000.0
    #     if rntc <=0: rntc = 1
    #     ntc_temp = math.log(rntc / 10000.0)
    #     ntc_temp /= 3380.0
    #     ntc_temp += 1.0 / (25 + 273.15)
    #     ntc_temp = 1.0 / ntc_temp
    #     ntc_temp -= 273.15
    #     return ntc_temp - self.temp_offset

    # def set_environmental_data(self, humidity, temperature):
    #     """Set the temperature and humidity used when computing eCO2 and TVOC values.

    #     :param int humidity: The current relative humidity in percent.
    #     :param float temperature: The current temperature in Celsius."""
    #     # Humidity is stored as an unsigned 16 bits in 1/512%RH. The default
    #     # value is 50% = 0x64, 0x00. As an example 48.5% humidity would be 0x61,
    #     # 0x00.
    #     humidity = int(humidity * 512)

    #     # Temperature is stored as an unsigned 16 bits integer in 1/512 degrees
    #     # there is an offset: 0 maps to -25C. The default value is 25C = 0x64,
    #     # 0x00. As an example 23.5% temperature would be 0x61, 0x00.
    #     temperature = int((temperature + 25) * 512)

    #     buf = bytearray(5)
    #     buf[0] = _ENV_DATA
    #     struct.pack_into(">HH", buf, 1, humidity, temperature)

    #     with self.i2c_device as i2c:
    #         i2c.write(buf)

    # def set_interrupt_thresholds(self, low_med, med_high, hysteresis):
    #     """Set the thresholds used for triggering the interrupt based on eCO2.
    #     The interrupt is triggered when the value crossed a boundary value by the
    #     minimum hysteresis value.

    #     :param int low_med: Boundary between low and medium ranges
    #     :param int med_high: Boundary between medium and high ranges
    #     :param int hysteresis: Minimum difference between reads"""
    #     buf = bytearray(
    #         [
    #             _THRESHOLDS,
    #             ((low_med >> 8) & 0xF),
    #             (low_med & 0xF),
    #             ((med_high >> 8) & 0xF),
    #             (med_high & 0xF),
    #             hysteresis,
    #         ]
    #     )
    #     with self.i2c_device as i2c:
    #         i2c.write(buf)

    # def reset(self):
    #     """Initiate a software reset."""
    #     # reset sequence from the datasheet
    #     seq = bytearray([_SW_RESET, 0x11, 0xE5, 0x72, 0x8A])
    #     with self.i2c_device as i2c:
    #         i2c.write(seq)
