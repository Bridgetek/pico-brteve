set(project_name "lcd_example")

# Add executable. Default name is the project name, version 0.1
file(GLOB SOURCES_ ./**.c)
file(GLOB HEADERS_ ./**.h)

add_executable(${project_name} ${SOURCES_} ${HEADERS_} )

pico_enable_stdio_uart(${project_name} 1)
pico_enable_stdio_usb(${project_name} 1)

# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(${project_name} pico_stdlib hardware_spi)

pico_add_extra_outputs(${project_name})


