cmake_minimum_required(VERSION 3.16)
project(onnxruntime-utils)


set(CXX_STANDARD_REQUIRED ON)
set(CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(onnxruntime)

add_subdirectory(onnxruntime-utils)
add_subdirectory(test)