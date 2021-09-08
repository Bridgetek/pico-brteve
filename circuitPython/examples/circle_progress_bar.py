# Referenced from https://www.programmersought.com/article/48894138782/
import time
import math
from brteve.brt_eve_bt817_8 import BrtEve
from brteve.brt_eve_rp2040 import BrtEveRP2040

host = BrtEveRP2040()
eve = BrtEve(host)
eve.init(resolution="1280x800", touch="goodix")

color_yellow=[0xF5, 0x90, 0x1F]
color_white=[255, 255, 255]

c1=color_white
c2=color_yellow

degree=1
while 1:
    eve.VertexFormat(4)
    eve.ClearColorRGB(255, 255, 255)
    eve.Clear()
    eve.ColorRGB(c1[0], c1[1], c1[2])
    eve.cmd_gradient (0, (int)(eve.lcd_height/2), 0x00FC0003,
        eve.lcd_width, (int)(eve.lcd_height/2), 0x00860078)

    eve.StencilFunc(eve.NEVER, 0x00, 0x00)
    eve.StencilOp(eve.INCR, eve.INCR)
    eve.Begin(eve.POINTS)
    eve.PointSize(80 * 2)
    eve.Vertex2f((eve.lcd_width / 2), (eve.lcd_height / 2))

    eve.StencilFunc(eve.NOTEQUAL, 0x01, 0x01)
    eve.PointSize(90 * 2)
    eve.Vertex2f((eve.lcd_width / 2), (eve.lcd_height / 2))

    #1) The effect of the first STENCIL_FUNC and STENCIL_OP on the back drawn
    #   circle (blue code part) is: The first parameter NEVER of STENCIL_FUNC
    #   is to make all pixels of the circle not display because the template
    #   value does not pass the test, because it is NEVER, So the last 2
    #   parameters are meaningless (Start modeEdition valueAll0). If the
    #   parameter of STENCIL_OP is set to INCR, it means that the template
    #   value corresponding to the template position of all the pixel positions
    #   of the circle is increased by 1 to 1, and the template value
    #   corresponding to the template position of other places without pixels
    #   remains at the original value of 0.

    #If there is no next statement, the running program can not see any circle
    #   display, so at present, just make a template.

    #2) The effect of the second STENCIL_FUNC on the following circle is: The
    #   three parameters of STENCIL_FUNC indicate that the template value in
    #   the template is ANDed with the third parameter 0x01 and the result is
    #   compared with the second parameter. That is to become transparent), the
    #   same does not pass (that is, opaque). Therefore, if the template value
    #   of the pixel position corresponding to the template position is 1, it
    #   will not be displayed. If it is 0, it will be displayed.

    #If we want to display the progress bar on the ring, we can set STENCIL_FUNC
    #   and STENCIL_OP as follows:

    eve.StencilFunc(eve.EQUAL, 0x01, 0x01)
    eve.StencilOp(eve.KEEP, eve.KEEP)

    # When the displayed picture is equal to 1 in the green circle part of the
    #   above figure, it is displayed, and it is not displayed if it is not
    #   equal to 1 in other positions. The parameters of STENCIL_OP are KEEP,
    #   that is, the template is not changed. The following code is as follows,
    #   draw POINT on the ring:

    eve.ColorRGB(c2[0], c2[1], c2[2])
    eve.PointSize(10 * 2)

    r=85
    for num in range(0, degree, 2):
        num=math.radians(num)
        posX = (eve.lcd_width / 2) + (r) * math.cos(num)
        posY = (eve.lcd_height / 2) + (r) * math.sin(num)
        eve.Vertex2f(posX, posY)
    degree+=2
    if degree > 360:
        degree=1
        if c1==color_white:
            c1=color_yellow
            c2=color_white
        else:
            c1=color_white
            c2=color_yellow

    eve.swap()
    time.sleep(.001)
