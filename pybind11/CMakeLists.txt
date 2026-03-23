cmake_minimum_required(VERSION 3.15...4.2)
project(example LANGUAGES CXX)

set(PYBIND11_EXAMPLE_VERSION "0.1.0")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

option(ENABLE_PYTHON "Enable Python bindings" OFF)

set(PYBIND11_FINDPYTHON ON)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(pybind11)

add_subdirectory(csrc)

if(${ENABLE_PYTHON})
  add_subdirectory(python)
endif()

