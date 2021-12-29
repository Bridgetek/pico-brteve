# Build command:
#   git clone https://github.com/BRTSG-FOSS/EveApps
#   git clone https://github.com/hathach/tinyusb
#   mkdir build
#   cd build
#   cmake -G "NMake Makefiles" -DEVE_APPS_PLATFORM=RP2040  ../
#   nmake
# 

FILE(GLOB SRCS *.c)
FILE(GLOB HDRS *.h)
SOURCE_GROUP("" FILES ${SRCS} ${HDRS})
SOURCE_GROUP("tinyuf2" FILES usb_descriptors.c uf2.h screen.c msc.c images.c ghostfat.c compile_date.h board_api.h main.c)

ADD_EXECUTABLE(eve_flash_pico ${SRCS} ${HDRS})

#ADD_DEFINITIONS(-DTINYUF2_DISPLAY=0)

IF(CMAKE_BUILD_TYPE MATCHES DEBUG)
  ADD_DEFINITIONS(-DCFG_TUSB_DEBUG=1)
ENDIF(CMAKE_BUILD_TYPE MATCHES DEBUG)

IF(EVE_APPS_PLATFORM STREQUAL "RP2040")
SET(IS_PLATFORM_RP2040   "RP2040"   CACHE STRING "Platform Target RP2040  ")
mark_as_advanced(IS_PLATFORM_RP2040  )

ADD_DEFINITIONS("-DEVE_PLATFORM_RP2040=1")
ADD_DEFINITIONS("-DRP2040_PLATFORM=1")


# Pull in Pico SDK (must be before project)
  INCLUDE("EveApps/pico_sdk_import.cmake")
  PROJECT("eve_flash_pico" C CXX ASM)

  # Initialise the Pico SDK
  pico_sdk_init()
  
  pico_add_extra_outputs(eve_flash_pico)
  pico_set_binary_type(eve_flash_pico no_flash)
  
  pico_enable_stdio_usb(eve_flash_pico 1)
  pico_enable_stdio_uart(eve_flash_pico 1)
  
  ADD_SUBDIRECTORY("EveApps/common")
  INCLUDE_DIRECTORIES("EveApps/common/eve_hal")
  INCLUDE_DIRECTORIES("EveApps/common/eve_hal/Hdr")
  INCLUDE_DIRECTORIES("EveApps/common/application/fatfs/source")
  INCLUDE_DIRECTORIES("EveApps/common/application")

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
