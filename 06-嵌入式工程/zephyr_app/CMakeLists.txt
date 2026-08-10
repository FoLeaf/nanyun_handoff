cmake_minimum_required(VERSION 3.20.0)

set(BOARD_ROOT ${CMAKE_CURRENT_LIST_DIR})

# starm-clang's ld.lld.exe cannot be invoked standalone (DLL dependencies);
# it must be called via the starm-clang driver. Use STATIC_LIBRARY mode so
# Zephyr's toolchain sanity check uses compile-only, avoiding standalone ld.lld.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(zephyr_hello_world)

target_sources(app PRIVATE src/main.c)
