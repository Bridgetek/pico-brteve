SCREEN_WIDTH           = (1280)
SCREEN_HEIGHT          = (800)

SS_START               = 0

def ADDR(X, Y)      :
    return X["addr"] - Y["addr"]

def H_CENTER(x)     :
    return (int)((SCREEN_WIDTH - (x)) * 0.5)

def V_CENTER(x)     :
    return (int)((SCREEN_HEIGHT - (x)) * 0.5)

def Y_CENTER(IMG, y):
    return (y) - IMG['height'] * 0.5
def X_CENTER(IMG, x):
    return (x) - IMG['width']* 0.5

X_HEADER               = (50)
Y_HEADER               = (50)
Y_BATTERY              = (60)
Y_FOOTER               = (700)

SLIDER_WIDTH           = (570)
SLIDER_HEIGHT          = (10)# * 16)
SLIDER_Y               = (350)
SLIDER_GAP             = (145)

# Screen saver
#SS_FRAME_COUNT       = (377) # 64Mb Flash
SS_FRAME_COUNT         = (97) # 16Mb Flash
SS_FRAME_SIZE          = (163840)
SS_FLASH_ADDR_FRAME_0  = (814208)

# Choose language
CIRCLE_NUM             = (3)

# Authenticate
PAY_NUM                = (5)

ATH_PAYWITH_X          = (X_HEADER)
ATH_PAYWITH_Y          = (670)
ATH_PAY_VERTICAL_CENTER= (750)
ATH_PAY_ROOM           = (SCREEN_WIDTH / PAY_NUM)

# Transaction         =
TR_RIGHT_MARGIN        = (150)
TR_RIGHT_BUTTON_MARGIN = (50)

HF_TITLE               = (14)    # font handle
HF_SMALL               = (13)
HF_BOTTOM              = (12)
HF_NUMBER              = (11)
HF_LANG_CN             = (10)


SS_TAG               = 1
CL_TAG_CIRCLE_EN     = 2
CL_TAG_CIRCLE_DE     = 3
CL_TAG_CIRCLE_CN     = 4
ATH_TAG_TAP_POINT    = 5
TR_TAG_CHARGE_BUTTON = 6
RE_TAG_EXIT_BUTTON   = 7
