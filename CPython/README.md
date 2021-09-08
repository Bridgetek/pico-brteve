# Eve Support Library and Examples for standard CPython environment   

Users can refer to this folder on how to write a standard python3 application to work with Eve. 

**Instructions**
- **Don't** install CircuitPython, instead install the [Adafruit Blinka](https://github.com/adafruit/Adafruit_Blinka/) library
- Blinka  provides a CircuitPython like wrapper API over the Raspberry Pi hardware & Linux API

```
pip3 install Adafruit-Blinka
```
- The code assumes default SPI on the Raspberry Pi is used and CE1 used as chip select. CE0 does **not** work (see https://github.com/adafruit/Adafruit_Blinka/issues/329)

- Overwrite the files in circuitPython/lib/bteve dierctory with the ones here, and everything else should work as normal

About Bridgetek Eve, check https://brtchip.com/eve/
