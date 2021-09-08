# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(project_name "lcd_example")

# Pull in Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(${project_name} C CXX ASM)

set(PICO_CXX_ENABLE_EXCEPTIONS 1)

set(PICO_CXX_ENABLE_RTTI 1)

# Initialise the Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
file(GLOB SOURCES_ ./**.cpp ./**.c)
file(GLOB HEADERS_ ./**.h)

include_directories(spi)
include_directories(eve)

add_executable(${project_name} ${SOURCES_} ${HEADERS_} )

pico_set_program_name(${project_name} "${project_name}")
pico_set_program_version(${project_name} "0.1")
pico_enable_stdio_uart(${project_name} 1)
pico_enable_stdio_usb(${project_name} 1)

# Add the standard library to the build
target_link_libraries(${project_name} pico_stdlib)

# Add any user requested libraries
target_link_libraries(${project_name}
        hardware_spi
        hardware_dma
        hardware_pio
        )

pico_add_extra_outputs(${project_name})
