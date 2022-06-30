# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# Modify these to set the target GPU and display
set(FT8XX_TYPE BT817)
set(DISPLAY_RES WVGA)

#include build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Set name of project (as current working directory)
get_filename_component(ProjectId ${CMAKE_CURRENT_LIST_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
string(CONCAT ProjectId ${ProjectId} "-" )
string(CONCAT ProjectId ${ProjectId} ${FT8XX_TYPE} )
string(CONCAT ProjectId ${ProjectId} "-" )
string(CONCAT ProjectId ${ProjectId} ${DISPLAY_RES} )
project(${ProjectId} C CXX ASM)

# Set C Standard
set(CMAKE_C_STANDARD 11)
# Set C++ Standard
set(CMAKE_CXX_STANDARD 17)
# Set the target board to be a standard pico, you may change this for different targets
set(PICO_BOARD pico)

# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()

# point out the CMake, where to find the executable source file
add_executable(${PROJECT_NAME}
        main/main.c
        # Source code for EVE library
        lib/eve/source/EVE_API.c
        lib/eve/source/EVE_HAL.c
        lib/eve/ports/eve_arch_rpi/EVE_MCU_RP2040.c
        # Source code for EVE UI library (include files we need)
        lib/eve_ui/source/eve_ui_main.c
        #lib/eve_ui/source/eve_ui_images.c
        #lib/eve_ui/source/eve_ui_ext_font.c
        lib/eve_ui/ports/eve_arch_rpi/eve_ui_mcu_rp2040.c
        #lib/eve_ui/source/eve_ram_g.c
        # Source code for Invaders
        invaders/invaders.c
)

include_directories(
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/lib/eve/include
        ${CMAKE_CURRENT_LIST_DIR}/lib/eve_ui/include
        ${CMAKE_CURRENT_LIST_DIR}/invaders
)

# Tell EVE library to use RP2040 as base class
add_compile_definitions(PLATFORM_RP2040)

# Tell EVE library to use the correct EVE chip and display
add_compile_definitions(FT8XX_TYPE=${FT8XX_TYPE}
                        DISPLAY_RES=${DISPLAY_RES}
)

# Project dependencies
target_link_libraries(${PROJECT_NAME} 
        pico_stdlib # Commonly used features (gpio, timer-delay etc)
        hardware_spi
        hardware_timer
        hardware_flash # For non-volatile storage
        hardware_sync # For non-volatile storage
)
# Create map/bin/hex/uf2 files.
pico_add_extra_outputs(${PROJECT_NAME})

add_custom_command(TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND arm-none-eabi-size --format=berkeley ${PROJECT_NAME}.elf
    VERBATIM
)
# Enable stdio on UART
pico_enable_stdio_uart(${PROJECT_NAME} 1)
# Do NOT enable stdio on USB as we have our own USB device emulated.
#pico_enable_stdio_usb(${PROJECT_NAME} 1)
