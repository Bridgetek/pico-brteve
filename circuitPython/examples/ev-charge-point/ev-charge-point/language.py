s_languageEnglish = b'ENGLISH'
s_languageGerman = b'DEUTSCH'
s_languageChinese = b'\x01\x02'

class lan_en():
    s_chooseLanguage = b'\x09\x2a\x31\x31\x35\x27\x01\x2e\x23\x30\x29\x37\x23\x29\x27'
    s_tapYourCard = b'\x17\x2e\x27\x23\x35\x27\x01\x36\x23\x32\x01\x3b\x31\x37\x34\x01\x25\x23\x34\
\x26\x01\x36\x31\x01\x25\x31\x30\x36\x2b\x30\x37\x27'
    s_checkingTheCard = b'\x09\x2a\x27\x25\x2d\x2b\x30\x29\x01\x36\x2a\x27\x01\x25\x23\x34\x26\x05\
\x05\x05'
    s_authenticated = b'\x07\x37\x36\x2a\x27\x30\x36\x2b\x25\x23\x36\x27\x26\x02'
    s_plugIn = b'\x17\x13\x1c\x0e\x01\x10\x15'
    s_pleaseTouch = b'\x17\x2e\x27\x23\x35\x27\x01\x36\x31\x37\x25\x2a\x01\x36\x2a\x27\x01\x35\x25\
\x34\x27\x27\x30\x01\x36\x31\x01\x25\x31\x30\x36\x2b\x30\x37\x27'
    s_payWith = b'\x17\x23\x3b\x01\x39\x2b\x36\x2a'
    s_checkingConnection = b'\x09\x2a\x27\x25\x2d\x2b\x30\x29\x01\x25\x31\x30\x30\x27\x25\x36\x2b\
\x31\x30\x05\x05\x05'
    s_connected = b'\x09\x31\x30\x30\x27\x25\x36\x27\x26\x02'
    s_transactionEstimation = b'\x1b\x34\x23\x30\x35\x23\x25\x36\x2b\x31\x30\x01\x0c\x35\x36\x2b\
\x2f\x23\x36\x2b\x31\x30'
    s_energy = b'\x0c\x30\x27\x34\x29\x3b'
    s_time = b'\x1b\x2b\x2f\x27'
    s_cost = b'\x09\x31\x35\x36'
    s_kWh = b'\x2d\x1e\x2a'
    s_currency = b'\x03'
    s_startCharging = b"START\nCHARGING"
    s_charging = b'\x09\x2a\x23\x34\x29\x2b\x30\x29\x05\x05\x05'
    s_pleaseUnplug = b'\x17\x2e\x27\x23\x35\x27\x01\x37\x30\x32\x2e\x37\x29\x01\x36\x2a\x27\x01\
\x25\x2a\x23\x34\x29\x27\x34\x01\x36\x31\x01\x35\x36\x31\x32'
    s_report = b'\x19\x27\x32\x31\x34\x36'

    s_minutes = b'\x2f\x2b\x30'
    s_battery = b'Battery:'
    s_batteryHealth = b'Battery health :'
    s_chargingTime = b'Charging time :'
    s_totalCost = b'Total cost :'
    s_minutes_report = b'min'
    s_healthy = b'\x0f\x27\x23\x2e\x36\x2a\x3b'
    s_exit = b'\x0c\x1f\x10\x1b'

class lan_cn:
    s_chooseLanguage = b'\x46\x22\x40\x3e'
    s_tapYourCard = b'\x41\x33\x23\x1e\x38\x12\x34\x3a\x3b'
    s_checkingTheCard = b'\x2e\x2d\x12\x03\x03\x03'
    s_authenticated = b'\x47\x43\x42\x08\x4b\x3f\x4c'
    s_plugIn = b'\x26\x0e'
    s_pleaseTouch = b'\x41\x3d\x27\x16\x18\x3a\x3b'
    s_payWith = b'\x06\x2f\x29\x1b'
    s_checkingConnection = b'\x2e\x2d\x44\x25\x03\x03\x03'
    s_connected = b'\x17\x44\x25'
    s_transactionEstimation = b'\x05\x2b\x09\x39'
    s_energy = b'\x3c\x32'
    s_time = b'\x2a\x4a'
    s_cost = b'\x1f\x2c'
    s_kWh = b'\x11\x36\x2a'
    s_minutes = b'\x10\x49'
    s_minutes_report = b'\x10\x49'
    s_currency = b'\x02'
    s_startCharging = b'\x1a\x15\n\x0d\x37'
    s_charging = b'\x0d\x37\x04\x03\x03\x03'
    s_pleaseUnplug = b'\x41\x21\x24\x0d\x37\x14\x07\x0b\x30'
    s_report = b'\x20\x13'
    s_battery = b'\x37\x31\x37\x48\x4d'
    s_batteryHealth = b'\x37\x31\x35\x1c\x4d'
    s_chargingTime = b'\x0d\x37\x2a\x4a\x4d'
    s_totalCost = b'\x1d\x1f\x2c\x4d'
    s_healthy = b'\x0c\x19'
    s_exit = b'\x45\x0f'

class lan_gm:
    s_chooseLanguage = b'\x1a\x32\x34\x23\x25\x2a\x27\x01\x39\x41\x2a\x2e\x27\x30'
    s_tapYourCard = b'\x08\x2b\x36\x36\x27\x01\x36\x2b\x32\x32\x27\x30\x01\x1a\x2b\x27\x01\x23\
\x37\x28\x01\x10\x2a\x34\x27\x01\x12\x23\x34\x36\x27,\x01\x37\x2f\x01\x28\x31\x34\x36\
\x3c\x37\x28\x23\x2a\x34\x27\x30'
    s_checkingTheCard = b'\x3f\x24\x27\x34\x32\x34\x43\x28\x27\x30\x01\x26\x27\x34\x01\x12\x23\
\x34\x36\x27\x01\x05\x05\x05'
    s_authenticated = b'\x07\x37\x36\x2a\x27\x30\x36\x2b\x28\x2b\x3c\x2b\x27\x34\x36\x02'
    s_plugIn = b'\x07\x15\x1a\x09\x0f\x13\x10\x0c\x40\x0c\x15'
    s_pleaseTouch = b'\x08\x2b\x36\x36\x27\x01\x24\x27\x34\x43\x2a\x34\x27\x30\x01\x1a\x2b\x27\
\x01\x26\x27\x30\x01\x08\x2b\x2e\x26\x35\x25\x2a\x2b\x34\x2f,\x01\x37\x2f\x01\x28\x31\
\x34\x36\x3c\x37\x28\x23\x2a\x34\x27\x30'
    s_payWith = b'Bezahlen mit'
    s_checkingConnection = b'\x1d\x27\x34\x24\x2b\x30\x26\x37\x30\x29\x01\x32\x34\x43\x28\x27\
\x30\x01\x05\x05\x05'
    s_connected = b'\x1d\x27\x34\x24\x2b\x30\x26\x37\x30\x29\x01\x2a\x27\x34\x29\x27\x35\x36\
\x27\x2e\x2e\x36\x02'
    s_transactionEstimation = b'\x1b\x34\x23\x30\x35\x23\x2d\x36\x2b\x31\x30\x35\x35\x25\x2a\
\x41\x36\x3c\x37\x30\x29'
    s_energy = b'\x0c\x30\x27\x34\x29\x2b\x27'
    s_time = b'\x21\x27\x2b\x36'
    s_cost = b'\x12\x31\x35\x36\x27\x30'
    s_kWh = b'\x2d\x1e\x2a'

    s_currency = b'\x03'
    s_startCharging = b'LADEVORGANG\nBEGINNEN'
    s_charging = b'\x13\x23\x26\x27\x30\x01\x05\x05\x05'
    s_pleaseUnplug = b'\x08\x2b\x36\x36\x27\x01\x36\x34\x27\x30\x30\x27\x30\x01\x1a\x2b\x27\x01\
\x26\x23\x35\x01\x13\x23\x26\x27\x29\x27\x34\x41\x36\x01\x37\x2f\x01\x23\x30\x3c\x37\x2a\
\x23\x2e\x36\x27\x30'
    s_report = b'\x08\x27\x34\x2b\x25\x2a\x36'

    s_minutes = b'\x2f\x2b\x30'
    s_battery = b'Batterie:'
    s_batteryHealth = b'Batterie Gesundheit:'
    s_chargingTime = b'Ladezeit:'
    s_totalCost = b'Gesamtkosten:'
    s_minutes_report = b'min'
    s_healthy = b'\x0e\x27\x35\x37\x30\x26'
    s_exit = b'\x08\x0c\x0c\x15\x0b\x0c\x15'

class language():
    def __init__(self):
        self.lan_cn = lan_cn()
        self.lan_en = lan_en()
        self.lan_gm = lan_gm()
        self.lan_active = self.lan_en

        self.LANGUAGE_CN=1
        self.LANGUAGE_EN=2
        self.LANGUAGE_GM=3

        self.FontBegin   = {'gl_addr': 4096  , 'gl_size': 0    , 'xf_addr': 4096  , 'xf_size': 0  }
        self.FontNumber  = {'gl_addr': 661056, 'gl_size': 40960, 'xf_addr': 702016, 'xf_size': 192}
        self.FontTitleEN = {'gl_addr': 576768, 'gl_size': 39744, 'xf_addr': 616512, 'xf_size': 128}
        self.FontBottomEN= {'gl_addr': 616640, 'gl_size': 22080, 'xf_addr': 638720, 'xf_size': 128}
        self.FontSmallEN = {'gl_addr': 638848, 'gl_size': 22080, 'xf_addr': 660928, 'xf_size': 128}
        self.FontTitleCH = {'gl_addr': 702208, 'gl_size': 44928, 'xf_addr': 747136, 'xf_size': 128}
        self.FontBottomCH= {'gl_addr': 747264, 'gl_size': 44928, 'xf_addr': 792192, 'xf_size': 128}
        self.FontSmallCH = {'gl_addr': 792320, 'gl_size': 19968, 'xf_addr': 812288, 'xf_size': 128}
        self.FontLangCH  = {'gl_addr': 812416, 'gl_size': 1728 , 'xf_addr': 814144, 'xf_size': 64 }

        self.FontTitle={'gl_addr': 0, 'gl_size': 0, 'xf_addr': 0, 'xf_size': 0}
        self.FontSmall={'gl_addr': 0, 'gl_size': 0, 'xf_addr': 0, 'xf_size': 0}
        self.FontBottom={'gl_addr': 0, 'gl_size': 0, 'xf_addr': 0, 'xf_size': 0}

    def switch(self, lang):
        if lang == self.LANGUAGE_CN:
            self.lan_active = self.lan_cn
            self.FontTitle['xf_addr'] = self.FontTitleCH['xf_addr']
            self.FontSmall['xf_addr'] = self.FontSmallCH['xf_addr']
            self.FontBottom['xf_addr'] = self.FontBottomCH['xf_addr']

        elif  lang == self.LANGUAGE_EN:
            self.lan_active = self.lan_en
            self.FontTitle['xf_addr'] = self.FontTitleEN['xf_addr']
            self.FontSmall['xf_addr'] = self.FontSmallEN['xf_addr']
            self.FontBottom['xf_addr'] = self.FontBottomEN['xf_addr']

        elif  lang == self.LANGUAGE_GM:
            self.lan_active = self.lan_gm

            #Use same font with EN, no change
            self.FontTitle['xf_addr'] = self.FontTitleEN['xf_addr']
            self.FontSmall['xf_addr'] = self.FontSmallEN['xf_addr']
            self.FontBottom['xf_addr'] = self.FontBottomEN['xf_addr']

        else:
            print("Swith language failed")
            self.lan_active = self.lan_en
            self.FontTitle['xf_addr'] = self.FontTitleEN['xf_addr']
            self.FontSmall['xf_addr'] = self.FontSmallEN['xf_addr']
            self.FontBottom['xf_addr'] = self.FontBottomEN['xf_addr']
