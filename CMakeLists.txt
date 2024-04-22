cmake_minimum_required(VERSION 3.29.2)

include(pico-sdk/pico_sdk_init.cmake)

project(pico-test)

pico_sdk_init()

add_subdirectory(src)
