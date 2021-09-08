# Description
   This is Pico example project to drive LCD display (800x480), use an EVE ME817EV/ME815EV module
   
   This project also should works on FT8xx board with compatible 800x480 LCD, althought we haven't tested them

   Note:
   1. The first screen is calibration and the second screen contains the examples of Eve built-in widgets
   
   3. UART is used to print out EVE's information, user can see these message via virtual serial port (with Putty/Tera term...etc)
   
# Connection
   - Pico and EVE:

    | EVE     |  Pico |
    | ------- | ----- |
    | SCK     | GP2   |
    | CS#     | GP5   |
    | MOSI    | GP3   |
    | MISO    | GP4   |
    | PD#     | GP7   |
    | GND     | GND   |
    | 5V      | VBUS  |
    
   - EVE and LCD:
   
    EVE   <----LCD_RGB_50P---->   LCD (800x480) 

    
# Compile 
Use cmake and nmake as per document (getting-started-with-pico.pdf, chapter 9.2):

   - Open the "Development command prompt for VS"
   
       Press window start menu and search for "Development command prompt"
       
   - Set up the correct PICO_SDK_PATH in the environment
   
       Example: set PICO_SDK_PATH=../pico-sdk
       
   - Build lcd example:
       $cd pico-examples
       
       $mkdir build
       
       $cd build
       
       $cmake -G "NMake Makefiles" ..
       
       $cd lcd
       
       $nmake
      
# run

Copy lcd_example.uf2 to pico's USB:

   $cp lcd_example.uf2 [Pico drive]

