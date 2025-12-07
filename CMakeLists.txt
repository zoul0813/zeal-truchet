cmake_minimum_required(VERSION 3.16)

# Application Version
set(APP_VERSION_MAJOR 0)
set(APP_VERSION_MINOR 2)
set(APP_VERSION_PATCH 0)
set(APP_NAME "Truchet Demo")
set(target_name "truchet")

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

set(ZOS_TOOLCHAIN sdcc)
include($ENV{ZOS_PATH}/cmake/zos_init.cmake)

project(${target_name} C ASM)

add_executable(${target_name}
    src/main.c
)

# zos_use(zgdk)
zos_use(zvb)

find_package(ZVB_SDK REQUIRED)
target_link_libraries(${target_name} PUBLIC zvb_dma)

# find_package(ZGDK REQUIRED)
# target_link_libraries(${target_name} PUBLIC zgdk)

# gif2zeal(${target_name} FILES
#     "assets/letters.gif"
#     "assets/numbers.gif"
#     "assets/microbe.gif"
#     "assets/tiles.gif"
#     COMPRESSED)

# tiled2zeal(${target_name} FILES
#     "assets/splash.tmx"
#     "assets/microbe.tmx"
#     VERBOSE
#     LAYER 0)

zos_add_outputs(${target_name})
