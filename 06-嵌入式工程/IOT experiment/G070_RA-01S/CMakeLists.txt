cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME G070_RA-01S)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME}
        BSP/oled/oled.c
        BSP/oled/oledFont.c
        BSP/myiic/myiic.c
        SYS/delay/delay.c
        APP/oled_test/oled_test.c
        BSP/aht30/aht30.c
        BSP/aht30/aht30_port.c
        APP/aht30_test/aht30_test.c
        BSP/led/led.c
        APP/scheduler/scheduler.c
        BSP/key/key.c
        BSP/key/multi_button.c
        APP/volt_measure/voltmeasure.c
        APP/monitor/monitor.c
        APP/input_capture/input_capture.c
        APP/callback_manager/callback_manager.c
        APP/lora_link/lora_link.c
)

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    BSP/ra01s/radio.c
    BSP/ra01s/sx126x.c
    Core/Src/ra01s_board/sx126x_board.c
    APP/ra01s_test/ra01s_test.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
        BSP/oled
        BSP/myiic
        SYS/delay
        APP/oled_test
        BSP/aht30
        APP/aht30_test
        BSP/led
        BSP/key
        APP/scheduler
        APP/volt_measure
        APP/monitor
        APP/input_capture
        APP/callback_manager
    APP/lora_link
    BSP/ra01s
        Core/Inc/ra01s_board
        APP/ra01s_test
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)

# Enable float formatting for printf/sprintf
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE -u _printf_float)
