from .eve_helper import eve_helper

import bteve as eve
import time
from . import language

import sys
from .page_screensaver  import page_screensaver
from .page_language     import page_language  
from .page_authenticate import page_authenticate  
from .page_plugin       import page_plugin    
from .page_charge       import page_charge        
from .page_transition   import page_transition
from .page_report       import page_report       

class demo_evchargepoint:
    #enum
    EPAGE_SCREENSAVER  =1 
    EPAGE_LANGUAGE     =2 
    EPAGE_AUTHENTICATE =3 
    EPAGE_PLUGIN       =4 
    EPAGE_TRANSITION   =5 
    EPAGE_CHARGE       =6 
    EPAGE_REPORT       =7 
    EPAGE_MAX          =7 
    EPAGE_CURRENT      =EPAGE_SCREENSAVER

    def __init__(self, eve: eve, gd: Brt_PicoEve_Module) -> None:
        self.eve =eve
        self.gd = gd

        helper=eve_helper(gd, eve)
        self.helper = helper

        self.pages = {
            self.EPAGE_SCREENSAVER : {'page':page_screensaver() , 'page_name':'page_screensaver '},
            self.EPAGE_LANGUAGE    : {'page':page_language()    , 'page_name':'page_language    '},
            self.EPAGE_AUTHENTICATE: {'page':page_authenticate(), 'page_name':'page_authenticate'},
            self.EPAGE_PLUGIN      : {'page':page_plugin()      , 'page_name':'page_plugin      '},
            self.EPAGE_TRANSITION  : {'page':page_transition()  , 'page_name':'page_transition  '},
            self.EPAGE_CHARGE      : {'page':page_charge()      , 'page_name':'page_charge      '},
            self.EPAGE_REPORT      : {'page':page_report()      , 'page_name':'page_report      '},
        }
        
        self.EPAGE_CURRENT      =self.EPAGE_SCREENSAVER
        self.page_active=self.pages[self.EPAGE_CURRENT]

        self.lan = language.language()
        self.lan.switch(self.lan.LANGUAGE_EN)

    def next_page(self):
        if self.EPAGE_CURRENT == self.EPAGE_MAX:
            self.EPAGE_CURRENT=self.EPAGE_SCREENSAVER
        else:
            self.EPAGE_CURRENT += 1
        
        self.page_active=self.pages[self.EPAGE_CURRENT]
        self.page_active['page'].init(self.eve, self.gd, self.helper, self.lan)

    def Get_SystemClock(self) :
        self.gd.rd32(self.eve.REG_CLOCK); # warm up

        c0 = self.gd.rd32(self.eve.REG_CLOCK)
        c1 = self.gd.rd32(self.eve.REG_CLOCK)

        overhead = c1 - c0

        c0 = self.gd.rd32(self.eve.REG_CLOCK)
        time.sleep(1)
        c1 = self.gd.rd32(self.eve.REG_CLOCK)

        freq = (c1 - c0) - overhead
        return freq
    
    def start(self):
        gd=self.gd
        eve=self.eve
        helper = self.helper
        lan=self.lan
        lan.switch(lan.LANGUAGE_EN)

        gd.wr32(eve.REG_TOUCH_TRANSFORM_A, 0xfffefefc)
        gd.wr32(eve.REG_TOUCH_TRANSFORM_B, 0xfffffcbf)
        gd.wr32(eve.REG_TOUCH_TRANSFORM_C, 0x506adb4)
        gd.wr32(eve.REG_TOUCH_TRANSFORM_D, 0xfffffed1)
        gd.wr32(eve.REG_TOUCH_TRANSFORM_E, 0xfffefc79)
        gd.wr32(eve.REG_TOUCH_TRANSFORM_F, 0x32c3211)

        print("Setup touch for Goodix\n")
        gd.wr8(eve.REG_CPURESET, 2)
        gd.wr16(eve.REG_TOUCH_CONFIG, 0x05d0)
        gd.wr8(eve.REG_CPURESET, 0)

        helper.set_precision(3) # for 1280x800 big screen
        helper.flash_switch_fullmode()
        gd.cmd_flashread(eve.RAM_G, 4096, helper.RAM_G_SIZE)
        
        gd.calibrate()

        while(1):
            self.page_active['page'].init(eve, gd, helper, lan)
            cont=1
            while (cont):
                self.helper.Display_Start()
                cont=self.page_active['page'].draw()
                gd.Display()
                gd.swap()

            self.page_active['page'].deinit()
            self.next_page()
