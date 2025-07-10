cmake_minimum_required(VERSION 3.16)
project(protaudio-example)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(portaudio)

add_subdirectory(example)