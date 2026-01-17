#----------------------------------------------
# Rocket Avionics Ground Gateway Firmware
# CMake build configuration
#
# Target: Adafruit Feather RP2040 + RFM95 LoRa
# Author: Mark Gavin
# Created: 2026-01-10
#----------------------------------------------

cmake_minimum_required(VERSION 3.13)

# Initialize Pico SDK from environment variable or relative path
if(DEFINED ENV{PICO_SDK_PATH})
  set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
else()
  # Fallback: assume SDK is symlinked in parent directory
  set(PICO_SDK_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../sdk)
endif()

# Include Pico SDK
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(rocket_gateway C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

#----------------------------------------------
# Gateway Executable
#----------------------------------------------
add_executable(rocket_gateway
  src/main.c
  src/lora_radio.c
  src/gateway_protocol.c
  src/ssd1306.c
  src/gateway_display.c
  src/bmp390.c
  src/gps.c
  src/wifi_nina.c
  src/wifi_config.c
  src/neopixel.c
)

# Include directories
target_include_directories(rocket_gateway PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/cJSON
)

# Link libraries
target_link_libraries(rocket_gateway
  pico_stdlib
  hardware_spi
  hardware_gpio
  hardware_i2c
  hardware_uart
  hardware_flash
  hardware_sync
  hardware_pio
  hardware_clocks
)

# Enable USB serial output (disable UART)
pico_enable_stdio_usb(rocket_gateway 1)
pico_enable_stdio_uart(rocket_gateway 0)

# Create UF2 file for deployment
pico_add_extra_outputs(rocket_gateway)

#----------------------------------------------
# Build Info
#----------------------------------------------
message(STATUS "Building Rocket Avionics Gateway Firmware")
message(STATUS "PICO_SDK_PATH: ${PICO_SDK_PATH}")
