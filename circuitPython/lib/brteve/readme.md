# INTRODUCTION
    This folder contains python library support for Raspberry Pi Pico with Eve module from Bridgetek Pte Ltd

# FOLDER STRUCTURE
    ├───brteve
    │   ├───readme.md           | This file
    │   ├───_eve_.py            | Define DL commands to deal with EVE ic
    │   ├───brt_eve_rp2040.py   | Raspberry Pi Pico host platform library
    │   ├───brt_eve_module.py   | Initialize EVE ic and setup LCD
    │   ├───brt_eve_ft81x.py    | FT81X's registers and commands definition
    │   ├───brt_eve_ft80x.py    | FT80X's registers and commands definition
    │   ├───brt_eve_common.py   | Common registers and commands definition
    │   ├───brt_eve_bt817_8.py  | BT817's and BT818's registers and commands definition
    │   ├───brt_eve_bt815_6.py  | BT815's and BT816's registers and commands definition

# USAGE
    User need to import class BrtEveRP2040 from brt_eve_rp2040.py,
    and class BrtEve from one of following file:
    - brt_eve_ft81x.py
    - brt_eve_ft80x.py
    - brt_eve_bt817_8.py
    - brt_eve_bt815_6.py

    Then call init with LCD resolution as a parameter.

    Example:
        from brteve.brt_eve_bt817_8 import BrtEve
        from brteve.brt_eve_rp2040 import BrtEveRP2040
        host = BrtEveRP2040()
        eve = BrtEve(host)
        eve.init(resolution="1280x800", touch="goodix")
















