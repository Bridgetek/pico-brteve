import time

from brteve.brt_eve_bt817_8 import BrtEve

from .eve_helper import eve_helper
from . import language

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
    EPAGE_CURRENT      =EPAGE_SCREENSAVER

    def __init__(self, eve: BrtEve) -> None:
        self.eve = eve

        helper=eve_helper(eve)
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

        self.EPAGE_CURRENT = self.EPAGE_SCREENSAVER
        self.page_active = self.pages[self.EPAGE_CURRENT]

        self.lan = language.language()
        self.lan.switch(self.lan.LANGUAGE_EN)

    def next_page(self):
        if self.EPAGE_CURRENT == self.EPAGE_REPORT:
            self.EPAGE_CURRENT = self.EPAGE_SCREENSAVER
        else:
            self.EPAGE_CURRENT += 1

        self.page_active=self.pages[self.EPAGE_CURRENT]
        self.page_active['page'].init(self.eve, self.helper, self.lan)

    def Get_SystemClock(self) :
        self.eve.rd32(self.eve.REG_CLOCK) # warm up

        c0 = self.eve.rd32(self.eve.REG_CLOCK)
        c1 = self.eve.rd32(self.eve.REG_CLOCK)

        overhead = c1 - c0

        c0 = self.eve.rd32(self.eve.REG_CLOCK)
        time.sleep(1)
        c1 = self.eve.rd32(self.eve.REG_CLOCK)

        freq = (c1 - c0) - overhead
        return freq

    def start(self):
        eve=self.eve
        helper = self.helper
        lan=self.lan
        lan.switch(lan.LANGUAGE_EN)

        helper.set_precision(3) # for 1280x800 big screen
        helper.flash_switch_fullmode()
        eve.cmd_flashread(0, 4096, helper.RAM_G_SIZE)

        print("System clock=", self.Get_SystemClock())

        while 1:
            self.page_active['page'].init(eve, helper, lan)
            cont=1
            while cont:
                self.helper.Display_Start()
                cont=self.page_active['page'].draw()
                eve.Display()
                eve.swap()

            self.page_active['page'].deinit()
            self.next_page()
