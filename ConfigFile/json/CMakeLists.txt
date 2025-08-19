cmake_minimum_required(VERSION 3.10)
project(json)

set(CXX_STANDARD_REQUIRED ON)
set(CXX_STANDARD 11)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(json)

add_executable(example example.cc)
target_link_libraries(example PRIVATE json)