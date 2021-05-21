[Introduce]
    This is library support for Raspberry Pi Pico RP2040 with Eve module from Brdgetek Pte Ltd
    This library is derived from the following repo:  
    https://github.com/jamesbowman/CircuitPython_bteve 

[Note]
    The _eve_.py in this folder is just for refenrence and should not be imported into program.

[How to use]
    1. Import the bteve module:
        import bteve as eve
        
    2. Initialize new module:
        gd = eve.Brt_PicoEve_Module()
        By default, MM2040EV module is selected, with LCD 1280x800 capacity touch
        
    3. Optional: Configure the right hardware:
        gd.set_module(str): Setup a pre-defined module such as VM800P35A, ME817EV...
                             Usually, set_module is enough for configuration step, except
                             some module has no intergrated LCD, user have to call set_lcd
                             later, these module usually suffix by "_N", such as VM800C35A_N.
                        
        gd.set_ic(str)    : Setup for EVE ic to be used, such as IC_EVE_FT800.
        gd.set_lcd(str)   : Setup for LCD size to be used, such as LCD_WQVGA_480X272.
        gd.set_touch(str) : Setup for EVE ic to be used, such as TOUCH_GOODIX.
        gd.set_gen(str)   : Setup for EVE generation to be used, such as GENERATION_EVE4.
        
        The "str" arguments is an method of eve.Brt_PicoEve_Module() class, these attribute 
        are listed in brt_eve_module.py, user can call them via instance of class, for example:
            gd.set_module(gd.MODULE_ME817EV())
            gd.set_ic(gd.IC_EVE_BT817())
            gd.set_lcd(gd.LCD_WXGA_1280X800())        
        
    4. Start up EVE module:
        gd.init()
        After this display is visible on the LCD.
        
    See examples for more reference.
    
                                     [End]

