
FILE(GLOB SRCS *.c)
FILE(GLOB HDRS *.h)
SOURCE_GROUP("" FILES ${SRCS} ${HDRS})

ADD_LIBRARY(fatfs
    ${SRCS}
    ${HDRS}
  )

IF(IS_PLATFORM_RP2040)
TARGET_LINK_LIBRARIES(
  fatfs 
  pico_stdlib
  hardware_gpio
  )
  
# Add any user requested libraries
TARGET_LINK_LIBRARIES(fatfs
    hardware_spi
    hardware_timer
    hardware_clocks
)
ENDIF()
