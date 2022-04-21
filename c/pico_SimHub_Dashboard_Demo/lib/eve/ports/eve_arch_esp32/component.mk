#
# "eve" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)
COMPONENT_OBJS := EVE_MCU_ESP32.o ../source/EVE_HAL.o ../source/EVE_API.o 
COMPONENT_SRCDIRS := . ../source 
COMPONENT_ADD_INCLUDEDIRS := . ../include 
