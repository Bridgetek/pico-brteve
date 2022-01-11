# Build command:
#   git clone https://github.com/BRTSG-FOSS/EveApps
#   git clone https://github.com/hathach/tinyusb
#   mkdir build
#   cd build
#   cmake -G "NMake Makefiles" -DEVE_APPS_PLATFORM=RP2040  ../
#   nmake
# 

cmake_minimum_required(VERSION 3.13)

FILE(GLOB SRCS *.c EveApps/common/application/*.c)
FILE(GLOB HDRS *.h EveApps/common/application/*.h)
SOURCE_GROUP("" FILES ${SRCS} ${HDRS})
SOURCE_GROUP("tinyuf2" FILES usb_descriptors.c uf2.h screen.c msc.c images.c ghostfat.c compile_date.h board_api.h main.c)

ADD_EXECUTABLE(eve_flash_pico ${SRCS} ${HDRS})

#ADD_DEFINITIONS(-DTINYUF2_DISPLAY=0)

IF(CMAKE_BUILD_TYPE MATCHES DEBUG)
  ADD_DEFINITIONS(-DCFG_TUSB_DEBUG=1)
ENDIF(CMAKE_BUILD_TYPE MATCHES DEBUG)

IF(EVE_APPS_PLATFORM STREQUAL "RP2040")
  # Pull in Pico SDK (must be before project)
  INCLUDE("EveApps/pico_sdk_import.cmake")
  PROJECT("eve_flash_pico" C CXX ASM)
  

  # Initialise the Pico SDK
  pico_sdk_init()

  # Set output
  pico_add_extra_outputs(eve_flash_pico)
  pico_set_binary_type(eve_flash_pico no_flash)
    
  # Add EveApps configuration
  ADD_SUBDIRECTORY("EveApps")
  INCLUDE_DIRECTORIES("EveApps/common")
  INCLUDE_DIRECTORIES("EveApps/common/eve_hal")
  INCLUDE_DIRECTORIES("EveApps/common/eve_hal/Hdr")
  INCLUDE_DIRECTORIES("EveApps/common/application/fatfs/source")
  INCLUDE_DIRECTORIES("EveApps/common/application")

  ADD_DEFINITIONS("-D${EVE_APPS_GRAPHICS}")
  ADD_DEFINITIONS("-D${EVE_APPS_PLATFORM}")
  ADD_DEFINITIONS("-D${EVE_APPS_DISPLAY}")
  add_definitions(-DNDEBUG) # Assert fail in time.c : repeating_timer_callback()

  TARGET_INCLUDE_DIRECTORIES(eve_flash_pico PRIVATE ${CMAKE_CURRENT_LIST_DIR})
  
  TARGET_LINK_LIBRARIES(eve_flash_pico PRIVATE
    pico_bootrom
    pico_unique_id
    tinyusb_device
    tinyusb_board
    pico_stdlib
    eve_hal
    fatfs
  )
  
ENDIF()
