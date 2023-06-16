# Build command:
#   $ cd pico-brteve\tools\uf2
#   $ mkdir build
#   $ cd build
#   $ set PICO_SDK_PATH=path-to--pico-sdk
#   $ set PICO_TOOLCHAIN_PATH=path-to--GNU Arm Embedded Toolchain\\10 2020-q4-major\\bin
#   $ "C:\Program Files\CMake\bin\cmake.exe" -G "NMake Makefiles" -DEVE_APPS_GRAPHICS=BT817 -DEVE_APPS_DISPLAY=WVGA  ../
#   $ nmake eve_flash_pico

cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PATH_EVEAPPS "./EveApps/common" CACHE PATH "Location of EveApps/common folder") 
set(EVE_APPS_PLATFORM "RP2040_PLATFORM")

MACRO(HELPER_SUBDIRLIST result curdir)
  FILE(GLOB children "${curdir}/*")
  LIST(APPEND ${result} ${curdir})
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${child})
      LIST(APPEND ${result} ${child})
      HELPER_SUBDIRLIST(${result} ${child})
    ENDIF()
  ENDFOREACH()
ENDMACRO()

# Include eve_hal framework folders
HELPER_SUBDIRLIST(SUBDIRS_EVEAPPS "${PATH_EVEAPPS}/")
list(FILTER SUBDIRS_EVEAPPS EXCLUDE REGEX ".*\\Arduino")
list(FILTER SUBDIRS_EVEAPPS EXCLUDE REGEX ".*\\esd_core")
include_directories(${SUBDIRS_EVEAPPS})

# A eve_hal framwwork source files
file(GLOB_RECURSE EVEAPPS_C "${PATH_EVEAPPS}/*.c" )
file(GLOB_RECURSE EVEAPPS_H "${PATH_EVEAPPS}/*.h" )

FILE(GLOB SRCS *.c ${EVEAPPS_C})
FILE(GLOB HDRS *.h ${EVEAPPS_H})

# exlude modified file
list(FILTER SRCS EXCLUDE REGEX ".*\\EVE_Util.c")
list(FILTER SRCS EXCLUDE REGEX ".*\\arduino\\.*")
list(FILTER SRCS EXCLUDE REGEX ".*\\esd_core\\.*")
MESSAGE(STATUS "SRCS = ${SRCS}")
MESSAGE(STATUS "SUBDIRS_EVEAPPS = ${SUBDIRS_EVEAPPS}")

ADD_DEFINITIONS(-DNDEBUG=1) # Prevent asserts fail in the TinyUSB in time.c : repeating_timer_callback()
ADD_DEFINITIONS(-DTINYUF2_DISPLAY=0)

# Add EveApps configuration
ADD_DEFINITIONS("-D${EVE_APPS_GRAPHICS}")
ADD_DEFINITIONS("-D${EVE_APPS_PLATFORM}")
ADD_DEFINITIONS("-D${EVE_APPS_DISPLAY}")
ADD_DEFINITIONS(-D_DEBUG=1) # eve debug flag

IF(EVE_APPS_PLATFORM STREQUAL "RP2040_PLATFORM")
  # Pull in Pico SDK (must be before project)
  INCLUDE("pico_sdk_import.cmake")
  PROJECT("eve_flash_pico" C CXX ASM)
  
  # Initialise the Pico SDK
  pico_sdk_init()

  # Set output
  ADD_EXECUTABLE(eve_flash_pico ${SRCS} ${HDRS})
  set_target_properties(eve_flash_pico PROPERTIES OUTPUT_NAME eve_flash_pico)
  pico_set_program_name(eve_flash_pico "eve_flash_pico")
  pico_add_extra_outputs(eve_flash_pico)
  #pico_set_binary_type(eve_flash_pico no_flash) # no_flash = program lost when reboot 
    
  TARGET_INCLUDE_DIRECTORIES(eve_flash_pico PRIVATE ${CMAKE_CURRENT_LIST_DIR})
  
  TARGET_LINK_LIBRARIES(eve_flash_pico PRIVATE
    pico_bootrom
    pico_unique_id
    tinyusb_device
    tinyusb_board
    pico_stdlib
    
    hardware_spi
    hardware_uart
  )
  
ENDIF()
