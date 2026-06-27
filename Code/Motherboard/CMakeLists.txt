# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD Proteus CACHE STRING "Board type")
set(PICO_BOARD_HEADER_DIRS ${CMAKE_CURRENT_SOURCE_DIR})

if (NOT PICO_SDK_PATH)
    set(PICO_SDK_PATH "${USERHOME}/.pico-sdk/sdk/2.2.0" CACHE PATH "Path to the Raspberry Pi Pico SDK")
endif()

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(Motherboard C CXX ASM)

# Initialize the Raspberry Pi Pico SDK
pico_sdk_init()


add_subdirectory(
    third_party/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src
)
# Add executable. Default name is the project name, version 0.1

add_executable(Motherboard
    Motherboard.cpp
    hw_config.c)

pico_set_program_name(Motherboard "Motherboard")
pico_set_program_version(Motherboard "0.1")


# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(Motherboard 0)
pico_enable_stdio_usb(Motherboard 1)


# Add the standard library to the build
target_link_libraries(Motherboard
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(Motherboard PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(Motherboard 
        hardware_spi
        hardware_i2c
        hardware_pio
        no-OS-FatFS-SD-SDIO-SPI-RPi-Pico
        )

pico_add_extra_outputs(Motherboard)

