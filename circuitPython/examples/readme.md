# CircuitPython examples for Raspberry Pi Pico with Eve module from Brdgetek Pte Ltd

## Examples

| File/Folder |  Description |
| ------ | ------ |
| alarm_clock                 | Alarm clock                                     |
| audio-playback              | An audio player                                 |
| blinka-rotate               | Image rotation                                  |
| camera-live-stream          | Live stream image from an camera                |
| dazzler-cube                | An 3D cube                                      |
| ev-charge-point             | An electric charging station                    |
| image-slide-viewer          | Image slider viewer                             |
| internet-data-display       | Download and display images from internet       |
| video2                      | Video playback from SDcard                      |
| bubble-code.py              | Simple bubble drawing                           |
| circle-progress-bar.py      | An circle progress bar                          |
| fizz-code.py                | Simple points                                   |
| flashinfo.py                | Fetch and display attached flash's information  |
| helloworld-code.py          | Hello world program                             |
| qix-code.py                 | Qix screen saver                                |
| temperature-code.py         | Display cpu temperature                         |
| video.py                    | Video playback from file                        |

## How to run

1. Install CircuitPython 7.0.0, follow this instruction:
https://learn.adafruit.com/intro-to-rp2040-pio-with-circuitpython/installing-circuitpython

2. Copy circuitPython/lib into CircuitPython drive

3. Copy example project into CircuitPython drive:
- .py file: rename it to code.py and copy
- folder: Copy every sub files/folder

## Wiring

```sh
PICO GP2 <--> Eve SCK
PICO GP3 <--> Eve MOSI
PICO GP4 <--> Eve MISO
PICO GP5 <--> Eve CS#
PICO GP7 <--> Eve PDN#
PICO Power <--> Eve Board power
PICO GND <--> Eve Board GND
```

SD card is accessed from Eve SPI but with different CS pin:
```sh
PICO GP13 <---> SD CS#
```

ILI9488:
```sh
PICO GP8 <---> D/CX pin of ILI9488
PICO GP9 <---> CSX pin of ILI9488
```

## Hardware
Below hardware are supported:
- MM2040EV
- ID2040

## Software

CircuitPython version: 7.0.0 Alpha 3
