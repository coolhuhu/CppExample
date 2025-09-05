cmake_minimum_required(VERSION 3.10)
project(UDPSimulator VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)      # .exe, .dll
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)      # .dll (Windows)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)      # .lib

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(asio)

set(SOURCES
    main.cpp
    network_simulator.cpp
    config_manager.cpp
)

add_executable(udp_simulator ${SOURCES})
target_include_directories(udp_simulator PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

if(WIN32)
    target_link_libraries(udp_simulator PRIVATE
        ws2_32
    )
endif()

target_compile_features(udp_simulator PRIVATE cxx_std_17)

set_target_properties(udp_simulator PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(udp_simulator PRIVATE DEBUG)
    target_compile_options(udp_simulator PRIVATE -g -O0)
else()
    target_compile_options(udp_simulator PRIVATE -O3)
endif()

install(TARGETS udp_simulator
    RUNTIME DESTINATION bin
)

install(FILES ${HEADERS}
    DESTINATION include/udp_simulator
)