#----------------------------------------------
# CMakeLists.txt for Rocket Avionics Flight Computer
# Adafruit Feather RP2040 with RFM95 LoRa Radio
#
# Hardware:
#   - Adafruit Feather RP2040 with RFM95 LoRa 915MHz (5714)
#   - Adafruit BMP390 Barometric Sensor (4816)
#   - Adafruit Adalogger FeatherWing (SD + RTC) (2922)
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

# Add FatFs library for SD card
add_library(fatfs STATIC
    ${PICO_SDK_PATH}/lib/tinyusb/lib/fatfs/source/ff.c
    ${PICO_SDK_PATH}/lib/tinyusb/lib/fatfs/source/ffsystem.c
    ${PICO_SDK_PATH}/lib/tinyusb/lib/fatfs/source/ffunicode.c
)
target_include_directories(fatfs PUBLIC
    ${PICO_SDK_PATH}/lib/tinyusb/lib/fatfs/source
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Add cJSON library
add_library(cjson STATIC
    lib/cJSON/cJSON.c
)
target_include_directories(cjson PUBLIC
    lib/cJSON
)

# Main executable
add_executable(rocket_avionics_flight
    src/main.c
    src/flight_control.c
    src/lora_radio.c
    src/bmp390.c
    src/ssd1306.c
    src/status_display.c
    src/rtc_pcf8523.c
    src/sd_logger.c
    src/diskio.c
    src/storage.c
    src/heartbeat_led.c
    src/base64.c
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
    hardware_flash
    hardware_sync
    hardware_timer
    hardware_pio
    hardware_clocks
    pico_unique_id
    fatfs
    cjson
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
