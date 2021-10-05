import sys
import time

if sys.implementation.name != "circuitpython":
    from ...lib.brteve.brt_eve_bt817_8 import BrtEve
    from ...lib.brteve.brt_eve_rp2040 import BrtEveRP2040

_last_tag = 0
def _tag_release(new_tag):
    global _last_tag
    tag_released = 0
    if (new_tag == 0 and _last_tag != new_tag):
        tag_released = _last_tag
        _last_tag = 0
    elif new_tag != 0:
        _last_tag = new_tag
    return tag_released

# return value is the number of milliseconds that have elapsed since the system was started.
def milis():
    return round(time.monotonic_ns() / 1000_000)

def widgets_dialog_yes_no(eve, title, question, timeout = 120, default_tag = 1, x=0, y=0, w=0, h=0, lcd_w=0, lcd_h=0, tag_yes=1, tag_no=2):
    # lcd_w and lcd_h may different with eve.lcd_w and eve.lcd_h, when cmd_setrotate is used
    global _last_tag
    _last_tag = 0
    if lcd_w == 0 or lcd_h == 0: # w and h are not set
        lcd_w = eve.lcd_width
        lcd_h = eve.lcd_height

    # calculate dialog size and position
    border = 2 # pixel
    base_lcd_w = 800
    base_lcd_h = 480
    dialog_w = 400
    dialog_h = 200
    dialog_x = (base_lcd_w - dialog_w) / 2
    dialog_y = (base_lcd_h - dialog_h) / 2
    htop = 50 #widget header height
    hbottom = 50 #widget footer height
    font = 20

    btn_h = 30
    btn_w = 80

    ratio = base_lcd_w * 100 / lcd_w

    if (ratio != 100):
        dialog_w = dialog_w * 100 / ratio
        dialog_h = dialog_h * 100 / ratio
        btn_w = btn_w * 100 / ratio
        btn_h = btn_h * 100 / ratio
        dialog_x = (lcd_w - dialog_w) / 2
        dialog_y = (lcd_h - dialog_h) / 2
        htop = htop * 100 / ratio
        hbottom = hbottom * 100 / ratio

        ratio += (100 - ratio) / 2
        font = round(font * 100 / ratio)
        if font > 31:
            font = 31
        if font < 16:
            font = 16
        if font == 17 or font == 19: # 17 and 19 are special unicode font
            font = 18

    if x != 0 or y != 0: # x and y are set
        dialog_x = x
        dialog_y = y
    if w != 0 or h != 0: # w and h are set
        dialog_w = w
        dialog_h = h
    
    time_start=milis() / 1000
    while 1:
        time_end=milis() / 1000
        duration = time_end - time_start
        if duration > timeout:
            return default_tag == tag_yes
        
        eve.cmd_dlstart()
        eve.ClearColorRGB(255, 255, 255)
        eve.Clear(1, 1, 1)
        eve.VertexFormat(3)

        #/ fade the whole LCD
        eve.ColorRGB(0x3F, 0x3F, 0x3F)
        eve.Begin(eve.RECTS)
        eve.Vertex2f(0, 0)
        eve.Vertex2f(lcd_w, lcd_h)

        #/ diag border
        eve.ColorRGB(0xE1, 0xE1, 0xE1)
        eve.Begin(eve.RECTS)
        eve.Vertex2f(dialog_x, dialog_y)
        eve.Vertex2f(dialog_x + dialog_w, dialog_y + dialog_h)

        #/ center part
        eve.ColorRGB(0xFF, 0xFF, 0xFF)
        eve.Begin(eve.RECTS)
        eve.Vertex2f(dialog_x + border, dialog_y + border)
        eve.Vertex2f(dialog_x + dialog_w - border, dialog_y + dialog_h - border)

        #/ top part
        eve.ColorRGB(0x46, 0x50, 0x52)
        eve.Begin(eve.RECTS)
        eve.Vertex2f(dialog_x + border, dialog_y + border)
        eve.Vertex2f(dialog_x + dialog_w - border, dialog_y + htop)

        #/ bottom part
        eve.ColorRGB(0xF3, 0xF3, 0xF3)
        eve.Begin(eve.RECTS)
        eve.Vertex2f(dialog_x + border, dialog_y + dialog_h - hbottom + border)
        eve.Vertex2f(dialog_x + dialog_w - border, dialog_y + dialog_h - border)

        #/ diag message
        eve.ColorRGB(0xFF, 0xFF, 0xFF)
        eve.cmd_text(dialog_x + border + 10, dialog_y + border + 10, font, 0, title + ' (' + str(timeout - round(duration)) + 's)')

        eve.ColorRGB(0x78, 0x78, 0x78)
        eve.cmd_fillwidth(dialog_w - border - 30)
        eve.cmd_text(dialog_x + border + 30, dialog_y + dialog_h / 2 - 20, font, eve.OPT_FILL, question)

        #/ diag button yes/no
        if default_tag == True:
            eve.ColorRGB(0xFF, 0x00, 0x00)
        else:
            eve.ColorRGB(0xFF, 0xFF, 0xFF)
        btn_margin = 25
        eve.Tag(tag_yes)
        eve.cmd_button(dialog_x + dialog_w / 2 - btn_w - btn_margin,
                dialog_y + dialog_h - hbottom + border + (hbottom - btn_h) / 2, btn_w, btn_h,
                font, 0, "Yes")

        if default_tag == False:
            eve.ColorRGB(0xFF, 0x00, 0x00)
        else:
            eve.ColorRGB(0xFF, 0xFF, 0xFF)
        eve.Tag(tag_no)
        eve.cmd_button(dialog_x + dialog_w / 2 + btn_margin,
                dialog_y + dialog_h - hbottom + border + (hbottom - btn_h) / 2, btn_w, btn_h,
                font, 0, "No")

        eve.Display()
        eve.cmd_swap()
        eve.finish()

        tag = eve.rd32(eve.REG_TOUCH_TAG) & 0xFF
        tag_released=_tag_release(tag)
        if tag_released == tag_yes:
            return True
        if tag_released == tag_no:
            return False


        
