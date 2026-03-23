pybind11_add_module(_pymodule pymath.cc)
target_include_directories(_pymodule PRIVATE ${CMAKE_SOURCE_DIR}/csrc)

if(NOT WIN32)
  target_link_libraries(_pymodule PRIVATE "-Wl,-rpath,$ORIGIN")
endif()

target_link_libraries(_pymodule PRIVATE pymodule)

install(TARGETS _pymodule DESTINATION lib)