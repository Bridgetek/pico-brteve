
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
  pico_add_extra_outputs(eve_flash_pico)
  pico_set_binary_type(eve_flash_pico no_flash)
  
  TARGET_INCLUDE_DIRECTORIES(eve_flash_pico PRIVATE ${CMAKE_CURRENT_LIST_DIR})
  
  TARGET_LINK_LIBRARIES(eve_flash_pico PRIVATE
    pico_bootrom
	pico_unique_id
    tinyusb_device
	tinyusb_board
  )
  
ENDIF()
