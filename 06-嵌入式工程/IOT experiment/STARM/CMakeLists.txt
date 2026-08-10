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
set(CMAKE_PROJECT_NAME STARM)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME}
        BSP/key/key.c
        app/scheduler/scheduler.c
        BSP/led/led.c
        BSP/buzzer/buzzer.c
        BSP/oled/oled.c
        BSP/oled/oledFont.c
        BSP/delay/delay.c
        BSP/myiic/myiic.c
        BSP/ioExpansion/MCP23017.c
        BSP/lora/lora.c
        BSP/aht30/aht30.c
        BSP/rc522/rc522.c
        BSP/myspi/myspi.c
        app/aht30_test/aht30_test.c
        app/rc522_test/rc522_test.c
        app/monitor/monitor.c
        BSP/raindrop/raindrop.c
        BSP/ra01/sx127x_driver.c
        BSP/ra01/ra01_hal.c
        BSP/ra01/ra01.c
        app/ra01_test/ra01_test.c
        BSP/motor/motor.c
        BSP/ultrasonic/ultrasonic.c)

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    BSP/oled
    BSP/delay
    app/scheduler
    BSP/led
    BSP/key
    BSP/buzzer
    BSP/myiic
    BSP/ioExpansion
    BSP/lora
    BSP/aht30
    BSP/rc522
    BSP/myspi
    BSP/motor
    app/aht30_test
    app/rc522_test
    app/monitor
    BSP/raindrop
    BSP/ra01
    BSP/ultrasonic
    app/ra01_test
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

# Enable printf float formatting
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE -u _printf_float)
