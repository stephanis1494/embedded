cmake_minimum_required(VERSION 3.13)

# Must be set before pico_sdk_import.cmake / project()
set(PICO_SDK_PATH ${CMAKE_CURRENT_LIST_DIR}/../external/pico-sdk)
set(PICO_BOARD pico)

include(pico_sdk_import.cmake)

project(pico_exercises C CXX ASM)
set(CMAKE_C_STANDARD 11)

pico_sdk_init()

# One subdirectory per exercise
add_subdirectory(01-blink)
