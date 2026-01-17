#----------------------------------------------
# CMakeLists.txt for Rocket Avionics Flight Computer
# Adafruit Feather RP2040 with RFM95 LoRa Radio
#
# Hardware:
#   - Adafruit Feather RP2040 with RFM95 LoRa 915MHz (5714)
#   - Adafruit BMP390 Barometric Sensor (4816)
#   - Adafruit LSM6DSOX + LIS3MDL 9-DoF IMU FeatherWing (4565)
#   - Adafruit FeatherWing OLED 128x64 (4650)
#   - Quad Side-By-Side FeatherWing Kit (4254)
#
# Build:
#   mkdir build && cd build
#   cmake ..
#   make -j4
#----------------------------------------------

cmake_minimum_required(VERSION 3.13)

# Set the board type - Feather RP2040
set(PICO_BOARD adafruit_feather_rp2040)

# Include Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(rocket_avionics_flight C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the Pico SDK
pico_sdk_init()

# Main executable
add_executable(rocket_avionics_flight
    src/main.c
    src/version.c
    src/flight_control.c
    src/lora_radio.c
    src/bmp390.c
    src/imu.c
    src/ssd1306.c
    src/status_display.c
    src/storage.c
    src/flight_storage.c
    src/heartbeat_led.c
    src/base64.c
    src/gps.c
)

# Force version.c to rebuild every time (updates build date/time)
add_custom_command(
    TARGET rocket_avionics_flight PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_SOURCE_DIR}/src/version.c
    COMMENT "Touching version.c to update build timestamp"
)

# Generate PIO header for WS2812 NeoPixel
pico_generate_pio_header(rocket_avionics_flight
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ws2812.pio
)

# Include directories
target_include_directories(rocket_avionics_flight PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link libraries
target_link_libraries(rocket_avionics_flight
    pico_stdlib
    hardware_gpio
    hardware_adc
    hardware_i2c
    hardware_spi
    hardware_uart
    hardware_flash
    hardware_sync
    hardware_timer
    hardware_pio
    hardware_clocks
    pico_unique_id
)

# Enable USB output, disable UART
pico_enable_stdio_usb(rocket_avionics_flight 1)
pico_enable_stdio_uart(rocket_avionics_flight 0)

# Generate UF2 file for flashing
pico_add_extra_outputs(rocket_avionics_flight)

# Compiler warnings
target_compile_options(rocket_avionics_flight PRIVATE
    -Wall
    -Wextra
    -Wno-unused-parameter
)

# Define for flight computer hardware
target_compile_definitions(rocket_avionics_flight PRIVATE
    HARDWARE_FLIGHT=1
)
