# Telemetrix examples for Raspberry Pi Pico with Eve module from Brdgetek Pte Ltd

## Examples


| File/Folder |  Description |
| ------ | ------ |
| bubble-code.py              | Simple bubble drawing                           |
| circle-progress-bar.py      | An circle progress bar                          |
| fizz-code.py                | Simple points                                   |
| flashinfo.py                | Fetch and display attached flash's information  |
| helloworld-code.py          | Hello world program                             |
| qix-code.py                 | Qix screen saver                                |

## How to run

1. Start Telemetrix server on Pico with this uf2: lib/brteve/telemetrix_rpi_pico/Telemetrix4RpiPico.uf2

2. Run client example on a PC:

  ```sh
  cd telemetrix_examples
  py -3 [example].py
  ```

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

Telemetrix server: https://github.com/BRTSG-FOSS/Telemetrix4RpiPico commit 79b9ef5

Telemetrix client: https://github.com/BRTSG-FOSS/telemetrix-rpi-pico commit 05d646e