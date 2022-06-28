# pico Virtual Keyboard Demo

This is a virtual touchscreen keyboard. It is rendered on the EVE display using the pico MCU to present a HID device on the USB interface to a host. The host treats the output of the pico virtual keyboard as it would a real physical keyboard.

It is straight forward to add multiple screens of keys. An example page showing an additional numeric keypad is enabled. This can be extended in code for other specialist inputs or even macros.

The default layout is UK, although US and DE layouts can be selected from the options screen (cog icon).

![Keyboard Screenshot](./keyboard.jpg)

## Options

By default the project will build for the BT817 as used in the IDM2040-7A module. To change this alter the FT8XX_TYPE in the CMakeLists.txt file.

```
# Modify these to set the target GPU and display
set(FT8XX_TYPE BT817)
set(DISPLAY_RES WVGA)
```
