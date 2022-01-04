# INTRODUCTION
    This folder contains python library support for Raspberry Pi Pico with Eve module from Bridgetek Pte Ltd

# FOLDER STRUCTURE
    ├───brteve
    │   ├───readme.md                     | This file
    │   ├───brt_eve_storage               | Manage EVE's flash memory
    │   ├───telemetrix_rpi_pico           | Telemetric client source code
    │   ├───_eve_.py                      | Define DL commands to deal with EVE ic
    │   ├───brt_eve_bt81x_ft81x_common.py | FT81X and BT81X common functions
    │   ├───brt_eve_bt815_6.py            | BT815's and BT816's registers and commands definition
    │   ├───brt_eve_bt817_8.py            | BT817's and BT818's registers and commands definition
    │   ├───brt_eve_common.py             | Common registers and commands definition
    │   ├───brt_eve_ft80x.py              | FT80X's registers and commands definition
    │   ├───brt_eve_ft81x.py              | FT81X's registers and commands definition
    │   ├───brt_eve_module.py             | Initialize EVE ic and setup LCD
    │   ├───brt_eve_movie_player.py       | EVE's movie player
    │   ├───brt_eve_rp2040.py             | Raspberry Pi Pico host platform library
    │   ├───brt_eve_telemetrix.py         | Telemetrix host platform library

# USAGE
User need to import one of host platform class:
- BrtEveRP2040 from brt_eve_rp2040.py,
- BrtEveTelemetrix from brt_eve_telemetrix.py

And import class BrtEve from one of following file:
- brt_eve_ft81x.py
- brt_eve_ft80x.py
- brt_eve_bt817_8.py
- brt_eve_bt815_6.py

Then call init() function.

## Example:
    
- Start EVE with Pico host platform:

```sh
from brteve.brt_eve_bt817_8 import BrtEve
from brteve.brt_eve_rp2040 import BrtEveRP2040
host = BrtEveRP2040()
eve = BrtEve(host)
eve.init(resolution="1280x800", touch="goodix")
```

- Start EVE with Telemetrix host platform:

```sh
from brteve.brt_eve_bt817_8 import BrtEve
from lib.brteve.brt_eve_telemetrix import BrtEveTelemetrix
host = BrtEveTelemetrix()
eve = BrtEve(host)
eve.init(resolution="1280x800", touch="goodix")
```















