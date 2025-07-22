cmake_minimum_required(VERSION 3.16)
project(net)


list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(asio)

add_subdirectory(example)